#pragma once

#include <unordered_map>
#include <mutex>
#include <functional>
#include "deque"
#include "list"
#include "stdexcept"
#include "atomic"

template <class K, class V, class Hash = std::hash<K>>
class ConcurrentHashMap {
public:
    ConcurrentHashMap(const Hash& hasher = Hash()) {
        hash_ = hasher;
        table_.resize(400);
        tabsize_ = 400;
        mutex_.resize(6);
        size_ = 0;
    }

    explicit ConcurrentHashMap(int expected_size, const Hash& hasher = Hash()) {
        table_.resize(std::max(400, expected_size));
        tabsize_ = std::max(400, expected_size);
        mutex_.resize(6);
        hash_ = hasher;
        size_ = 0;
    }

    ConcurrentHashMap(int expected_size, int expected_threads_count, const Hash& hasher = Hash()) {
        hash_ = hasher;
        table_.resize(std::max(400, expected_size));
        tabsize_ = std::max(400, expected_size);
        mutex_.resize(expected_threads_count);
        size_ = 0;
    }

    bool Insert(const K& key, const V& value) {
        if (size_ > tabsize_ * 2) {
            if (alphamut_.try_lock()) {
                for (int i = 0; i < mutex_.size(); ++i) {
                    mutex_[i].lock();
                }
                std::vector<std::list<std::pair<K, V>>> temptable;
                temptable.resize(tabsize_ * 10);
                for (int i = 0; i < table_.size(); ++i) {
                    for (auto& x : table_[i]) {
                        temptable[hash_(x.first) % (10 * tabsize_)].push_back({x.first, x.second});
                    }
                }
                table_ = std::move(temptable);
                size_t xt = tabsize_ * 9;
                tabsize_ += xt;
                for (int i = 0; i < mutex_.size(); ++i) {
                    mutex_[i].unlock();
                }
                alphamut_.unlock();
            }
        }
        std::lock_guard<std::mutex> lock(mutex_[(hash_(key) % tabsize_) % mutex_.size()]);
        int id = hash_(key) % tabsize_;
        if (!table_[id].empty()) {
            for (auto& x : table_[id]) {
                if (x.first == key) {
                    return false;
                }
            }
        }
        ++size_;
        table_[id].push_back({key, value});
        return true;
    }

    bool Erase(const K& key) {
        std::lock_guard<std::mutex> lock(mutex_[(hash_(key) % tabsize_) % mutex_.size()]);
        int id = hash_(key) % tabsize_;
        if (!table_[id].empty()) {
            for (auto it = table_[id].begin(); it != table_[id].end(); ++it) {
                if (it->first == key) {
                    --size_;
                    table_[id].erase(it);
                    return true;
                }
            }
        }
        return false;
    }

    void Clear() {
        for (int i = 0; i < mutex_.size(); ++i) {
            mutex_[i].lock();
        }
        for (int i = 0; i < table_.size(); ++i) {
            table_[i].clear();
        }
        for (int i = 0; i < mutex_.size(); ++i) {
            mutex_[i].unlock();
        }
        size_ = 0;
    }

    std::pair<bool, V> Find(const K& key) const {
        std::lock_guard<std::mutex> lock(mutex_[(hash_(key) % tabsize_) % mutex_.size()]);
        int id = hash_(key) % tabsize_;
        if (!table_[id].empty()) {
            for (auto& x : table_[id]) {
                if (x.first == key) {
                    return std::make_pair(true, x.second);
                }
            }
        }
        return std::make_pair(false, V());
    }

    const V At(const K& key) const {
        std::lock_guard<std::mutex> lock(mutex_[(hash_(key) % tabsize_) % mutex_.size()]);
        int id = hash_(key) % tabsize_;
        if (!table_[id].empty()) {
            for (auto& x : table_[id]) {
                if (x.first == key) {
                    return x.second;
                }
            }
        }
        throw std::out_of_range("Noval");
    }

    size_t Size() const {
        return size_;
    }

    static const int kDefaultConcurrencyLevel;
    static const int kUndefinedSize;

private:
    std::vector<std::list<std::pair<K, V>>> table_;
    mutable std::deque<std::mutex> mutex_;
    mutable std::atomic<size_t> size_;
    std::atomic<size_t> tabsize_;
    std::mutex alphamut_;
    Hash hash_;
};

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kDefaultConcurrencyLevel = 8;

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kUndefinedSize = -1;
