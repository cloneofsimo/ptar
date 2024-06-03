#include "ParallelTarReader.h"
#include <iostream>
#include <archive.h>
#include <archive_entry.h>

ParallelTarReader::ParallelTarReader(const std::vector<std::string>& shardlist, size_t buffer_limit, size_t N, const std::vector<std::string>& schema)
    : shardlist_(shardlist), buffer_limit_(buffer_limit), N_(N), schema_(schema), stop_(false) {
    startThreads();
}

ParallelTarReader::~ParallelTarReader() {
    stopAndJoinThreads();
}

void ParallelTarReader::reset() {
    stopAndJoinThreads();
    clearBuffer();
    stop_ = false;
    startThreads();
}

bool ParallelTarReader::is_valid_sequence(const std::vector<std::pair<std::string, std::string>>& data_sequence) const {
    if (data_sequence.size() != schema_.size()) {
        return false;
    }
    for (size_t i = 0; i < schema_.size(); ++i) {
        if (data_sequence[i].second.find(schema_[i]) == std::string::npos) {
            return false;
        }
    }
    return true;
}

std::vector<std::vector<std::pair<std::string, std::string>>> ParallelTarReader::get_next_n_data(size_t n) {
    std::vector<std::vector<std::pair<std::string, std::string>>> data;
    std::unique_lock<std::mutex> lock(mutex_);

    while (data.size() < n) {
        cond_var_.wait(lock, [this, n]() { return buffer_.size() >= n || stop_; });

        while (data.size() < n && !buffer_.empty()) {
            data.push_back(std::move(buffer_.front()));
            buffer_.pop();
        }

        if (stop_ && buffer_.empty()) {
            break;
        }
    }
    return data;
}

void ParallelTarReader::tarReaderThread(size_t index) {
    struct archive* a = archive_read_new();
    struct archive_entry* entry;

    archive_read_support_format_tar(a);
    archive_read_support_compression_gzip(a);

    if (archive_read_open_filename(a, shardlist_[index].c_str(), 10240) != ARCHIVE_OK) {
        std::cerr << "Could not open tar file: " << shardlist_[index] << std::endl;
        archive_read_free(a);
        return;
    }

    std::vector<std::pair<std::string, std::string>> sequence_buffer;

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        std::string filename = archive_entry_pathname(entry);
        size_t size = archive_entry_size(entry);
        std::string buffer(size, '\0');
        archive_read_data(a, &buffer[0], size);

        {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_var_.wait(lock, [this]() { return buffer_.size() < buffer_limit_; });

            sequence_buffer.emplace_back(std::move(buffer), filename);

            if (sequence_buffer.size() == schema_.size()) {
                if (is_valid_sequence(sequence_buffer)) {
                    buffer_.emplace(std::move(sequence_buffer));
                    cond_var_.notify_all();
                }
                sequence_buffer.clear();
            }
        }

        archive_read_data_skip(a);
    }

    archive_read_free(a);

    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
        cond_var_.notify_all();
    }
}

void ParallelTarReader::stopAndJoinThreads() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
        cond_var_.notify_all();
    }
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    threads_.clear();
}

void ParallelTarReader::clearBuffer() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (!buffer_.empty()) {
        buffer_.pop();
    }
}

void ParallelTarReader::startThreads() {
    for (size_t i = 0; i < shardlist_.size(); ++i) {
        threads_.emplace_back(&ParallelTarReader::tarReaderThread, this, i);
    }
}
