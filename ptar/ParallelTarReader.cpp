#include "ParallelTarReader.h"
#include <iostream>

ParallelTarReader::ParallelTarReader(const std::vector<std::string>& shardlist, size_t buffer_limit, size_t N)
    : shardlist_(shardlist), buffer_limit_(buffer_limit), N_(N), stop_(false) {
    for (size_t i = 0; i < shardlist.size(); ++i) {
        threads_.emplace_back(&ParallelTarReader::tarReaderThread, this, i);
    }
}

ParallelTarReader::~ParallelTarReader() {
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
}

std::vector<std::pair<std::vector<char>, std::string>> ParallelTarReader::get_next_n_data(size_t n) {
    std::vector<std::pair<std::vector<char>, std::string>> data;
    std::unique_lock<std::mutex> lock(mutex_);
    cond_var_.wait(lock, [this, n]() { return buffer_.size() >= n || stop_; });

    for (size_t i = 0; i < n && !buffer_.empty(); ++i) {
        data.push_back(std::move(buffer_.front()));
        buffer_.pop();
    }

    cond_var_.notify_all();
    return data;
}

void ParallelTarReader::tarReaderThread(size_t index) {
    struct archive *a;
    struct archive_entry *entry;
    a = archive_read_new();
    archive_read_support_format_tar(a);
    archive_read_support_compression_gzip(a);

    if (archive_read_open_filename(a, shardlist_[index].c_str(), 10240) != ARCHIVE_OK) {
        std::cerr << "Could not open tar file: " << shardlist_[index] << std::endl;
        return;
    }

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char* filename = archive_entry_pathname(entry);
        size_t size = archive_entry_size(entry);
        std::vector<char> buffer(size);
        archive_read_data(a, buffer.data(), size);

        {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_var_.wait(lock, [this]() { return buffer_.size() < buffer_limit_ || stop_; });

            if (stop_) break;

            buffer_.emplace(std::move(buffer), filename);
            cond_var_.notify_all();
        }

        archive_read_data_skip(a);
    }

    archive_read_free(a);
}
