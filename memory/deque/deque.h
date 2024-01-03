#pragma once

#include <initializer_list>
#include <algorithm>
#include <deque>
#include <memory>
#include <array>
class Deque {
public:
    Deque() {
        head_ = {0, 0};
        tail_ = {0, 0};
        capacity_ = 0;
        size_ = 0;
    };
    Deque(Deque& rhs) {
        head_ = rhs.head_;
        tail_ = rhs.tail_;
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;
        if (rhs.size_ != 0) {
            buffer_ = new std::array<int, 128>*[capacity_];
            for (int i = 0; i < capacity_; ++i) {
                buffer_[i] = nullptr;
            }
            int it = rhs.head_.first;
            while (it != rhs.tail_.first) {
                buffer_[it] = new std::array<int, 128>(*rhs.buffer_[it]);
                ++it;
            }
            buffer_[rhs.tail_.first] = new std::array<int, 128>(*rhs.buffer_[rhs.tail_.first]);
        }
    };
    Deque(Deque&& rhs) {
        head_ = std::move(rhs.head_);
        tail_ = std::move(rhs.tail_);
        size_ = std::move(rhs.size_);
        capacity_ = std::move(rhs.capacity_);
        buffer_ = std::move(rhs.buffer_);
        rhs.buffer_ = nullptr;
    };
    explicit Deque(size_t size) {
        head_ = {0, 0};
        tail_ = {(size - 1) / 128, (size - 1) % 128};
        capacity_ = (size - 1) / 128 + 1;
        size_ = size;
        buffer_ = new std::array<int, 128>*[capacity_];
        for (int i = 0; i < capacity_; ++i) {
            buffer_[i] = nullptr;
        }
        for (int i = 0; i < size_; ++i) {
            if (i % 128 == 0) {
                buffer_[i / 128] = new (std::array<int, 128>);
            }
            (*buffer_[i / 128])[i % 128] = 0;
        }
    }

    Deque(std::initializer_list<int> list) {
        head_ = {0, 0};
        capacity_ = (list.size() - 1) / 128 + 1;
        tail_ = {(list.size() - 1) / 128, (list.size() - 1) % 128};
        size_ = list.size();
        buffer_ = new std::array<int, 128>*[capacity_];
        for (int i = 0; i < capacity_; ++i) {
            buffer_[i] = nullptr;
        }
        auto it = list.begin();
        for (int i = 0; i < size_; ++i) {
            if (i % 128 == 0) {
                buffer_[i / 128] = new (std::array<int, 128>);
            }
            (*buffer_[i / 128])[i % 128] = *it;
            ++it;
        }
    }

    Deque& operator=(const Deque& rhs) {
        if (this == &rhs) {
            return *this;
        }
        this->Clear();
        head_ = rhs.head_;
        tail_ = rhs.tail_;
        capacity_ = rhs.capacity_;
        size_ = rhs.size_;
        buffer_ = new std::array<int, 128>*[capacity_];
        int it = rhs.head_.first;
        for (int i = 0; i < capacity_; ++i) {
            buffer_[i] = nullptr;
        }
        while (it != rhs.tail_.first) {
            buffer_[it] = new std::array<int, 128>(*rhs.buffer_[it]);
            ++it;
        }
        buffer_[rhs.tail_.first] = new std::array<int, 128>(*rhs.buffer_[rhs.tail_.first]);
        return *this;
    }
    void Swap(Deque& rhs) {
        std::swap(head_, rhs.head_);
        std::swap(tail_, rhs.tail_);
        std::swap(capacity_, rhs.capacity_);
        std::swap(size_, rhs.size_);
        std::swap(buffer_, rhs.buffer_);
    }

    void PushBack(int value) {
        if (size_ == capacity_ * 128 or
            (tail_.second == 127 and head_.first == (tail_.first + 1) % capacity_)) {
            Resize(2 * (capacity_ + 1));
        }
        if (size_ == 0) {
            buffer_[head_.first] = new (std::array<int, 128>);
            (*buffer_[head_.first])[head_.second] = value;
            ++size_;
            return;
        }
        if (tail_.second == 127) {
            tail_.first = (tail_.first + 1) % capacity_;
            tail_.second = 0;
            buffer_[tail_.first] = new (std::array<int, 128>);
            (*buffer_[tail_.first])[0] = value;
        } else {
            ++tail_.second;
            (*buffer_[tail_.first])[tail_.second] = value;
        }
        ++size_;
    }

    void PopBack() {
        if (size_ == 0) {
            return;
        }
        if (size_ == 1) {
            delete buffer_[tail_.first];
            buffer_[tail_.first] = nullptr;
        } else {
            if (tail_.second == 0) {
                delete buffer_[tail_.first];
                buffer_[tail_.first] = nullptr;
                tail_.first = (tail_.first - 1 + capacity_) % capacity_;
                tail_.second = 127;
            } else {
                --tail_.second;
            }
        }
        --size_;
    }

    void PushFront(int value) {
        if (size_ == capacity_ * 128 or
            (head_.second == 0 and (head_.first - 1 + capacity_) % capacity_ == tail_.first)) {
            Resize(2 * (capacity_ + 1));
        }
        if (size_ == 0) {
            buffer_[head_.first] = new (std::array<int, 128>);
            (*buffer_[head_.first])[head_.second] = value;
            ++size_;
            return;
        }
        if (head_.second == 0) {
            head_.first = (head_.first - 1 + capacity_) % capacity_;
            head_.second = 127;
            buffer_[head_.first] = new (std::array<int, 128>);
            (*buffer_[head_.first])[head_.second] = value;
        } else {
            --head_.second;
            (*buffer_[head_.first])[head_.second] = value;
        }
        ++size_;
    }

    void PopFront() {
        if (size_ == 0) {
            return;
        }
        if (size_ == 1) {
            delete buffer_[head_.first];
            buffer_[head_.first] = nullptr;
        } else {
            if (head_.second == 127) {
                delete buffer_[head_.first];
                buffer_[head_.first] = nullptr;
                head_.first = (head_.first + 1 + capacity_) % capacity_;
                head_.second = 0;
            } else {
                ++head_.second;
            }
        }
        --size_;
    }

    int& operator[](size_t ind) {
        return (*buffer_[(head_.first + (head_.second + ind) / 128) %
                         capacity_])[(head_.second + ind) % 128];
    }

    int operator[](size_t ind) const {
        return (*buffer_[(head_.first + (head_.second + ind) / 128) %
                         capacity_])[(head_.second + ind) % 128];
    }
    size_t Size() const {
        return size_;
    }

    void Clear() {
        if (size_ != 0) {
            size_ = 0;
            head_ = {0, 0};
            tail_ = {0, 0};
            for (int i = 0; i < capacity_; ++i) {
                delete buffer_[i];
            }
            capacity_ = 0;
        }
        delete[] buffer_;
        buffer_ = nullptr;
    }
    ~Deque() {
        if (buffer_ != nullptr) {
            Clear();
        }
    }

private:
    std::pair<int, int> head_ = {0, 0};
    std::pair<int, int> tail_ = {0, 0};
    int capacity_ = 0;
    int size_ = 0;
    std::array<int, 128>** buffer_ = nullptr;

    void Resize(int new_size) {
        std::array<int, 128>** x;
        x = new std::array<int, 128>*[new_size];
        for (int i = 0; i < capacity_; ++i) {
            x[i] = buffer_[(head_.first + i) % capacity_];
        }
        for (int i = capacity_; i < new_size; ++i) {
            x[i] = nullptr;
        }
        delete[] buffer_;
        buffer_ = x;
        head_ = {0, head_.second};
        capacity_ = new_size;
        if (size_ != 0) {
            tail_ = {(size_ - 1 + head_.second) / 128, (size_ - 1 + head_.second + 128) % 128};
        } else {
            tail_ = {0, head_.second};
        }
    }
};
