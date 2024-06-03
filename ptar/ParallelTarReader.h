#ifndef PARALLELTARREADER_H
#define PARALLELTARREADER_H

#include <vector>
#include <queue>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <archive.h>
#include <archive_entry.h>

class ParallelTarReader {
public:
    ParallelTarReader(const std::vector<std::string>& shardlist, size_t buffer_limit, size_t N, const std::vector<std::string>& schema);
    ~ParallelTarReader();

    std::vector<std::vector<std::pair<std::string, std::string>>> get_next_n_data(size_t n);
    void reset();

private:
    void tarReaderThread(size_t index);
    bool is_valid_sequence(const std::vector<std::pair<std::string, std::string>>& data_sequence) const;

    void stopAndJoinThreads();
    void clearBuffer();
    void startThreads();

    std::vector<std::string> shardlist_;
    std::vector<std::thread> threads_;
    std::queue<std::vector<std::pair<std::string, std::string>>> buffer_;
    std::vector<std::string> schema_;
    size_t buffer_limit_;
    size_t N_;
    bool stop_;
    std::mutex mutex_;
    std::condition_variable cond_var_;
};

#endif // PARALLELTARREADER_H
