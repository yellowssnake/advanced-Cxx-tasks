#pragma once
#include "array"
#include "memory"
#include "deque"
template <class T>
struct Node {
    Node(const T& value) {
        bob_ = value;
    }
    T bob_;
    std::array<std::shared_ptr<Node<T>>, 32> arr_;
};
template <class T>
class ImmutableVector {
public:
    ImmutableVector() {
        size_ = 0;
    }
    ImmutableVector(std::shared_ptr<Node<T>> root, size_t size) {
        root_ = root;
        size_ = size;
    }

    explicit ImmutableVector(size_t count, const T& value = T()) {
        size_ = count;
        root_ = std::make_shared<Node<T>>(value);
        std::shared_ptr<Node<T>> node = root_;
        size_t cnt = 1;
        while (cnt < count) {
            node = root_;
            std::deque<size_t> index;
            size_t cop = cnt - 1;
            while (cop != 0) {
                index.push_front(cop % 32);
                cop /= 32;
            }
            if (index.empty()) {
                index.push_front(0);
            }
            for (int i = 0; i < index.size(); ++i) {
                if (i == index.size() - 1) {
                    node->arr_[index[i]] = std::make_shared<Node<T>>(value);
                } else {
                    node = node->arr_[index[i]];
                }
            }
            ++cnt;
        }
    }

    template <typename Iterator>
    ImmutableVector(Iterator first, Iterator last) {
        root_ = std::make_shared<Node<T>>(*first);
        ++first;
        std::shared_ptr<Node<T>> node = root_;
        size_t cnt = 1;
        while (first != last) {
            node = root_;
            std::deque<size_t> index;
            size_t cop = cnt - 1;
            while (cop != 0) {
                index.push_front(cop % 32);
                cop /= 32;
            }
            if (index.empty()) {
                index.push_front(0);
            }
            for (int i = 0; i < index.size(); ++i) {
                if (i == index.size() - 1) {
                    node->arr_[index[i]] = std::make_shared<Node<T>>(*first);
                } else {
                    node = node->arr_[index[i]];
                }
            }
            ++cnt;
            ++first;
        }
        size_ = cnt;
    }

    ImmutableVector(std::initializer_list<T> l) {
        auto x = l.begin();
        auto y = l.end();
        root_ = std::make_shared<Node<T>>(*x);
        ++x;
        std::shared_ptr<Node<T>> node = root_;
        size_t cnt = 1;
        while (x != y) {
            node = root_;
            std::deque<size_t> index;
            size_t cop = cnt - 1;
            while (cop != 0) {
                index.push_front(cop % 32);
                cop /= 32;
            }
            if (index.empty()) {
                index.push_front(0);
            }
            for (int i = 0; i < index.size(); ++i) {
                if (i == index.size() - 1) {
                    node->arr_[index[i]] = std::make_shared<Node<T>>(*x);
                } else {
                    node = node->arr_[index[i]];
                }
            }
            ++cnt;
            ++x;
        }
        size_ = cnt;
    }

    ImmutableVector Set(size_t index, const T& value) {
        std::shared_ptr<Node<T>> new_root = std::make_shared<Node<T>>(root_->bob_);
        if (index == 0) {
            new_root->bob_ = value;
            for (int i = 0; i < 32; ++i) {
                new_root->arr_[i] = root_->arr_[i];
            }
        } else {
            std::shared_ptr<Node<T>> node = new_root;
            std::shared_ptr<Node<T>> nd = root_;
            std::deque<size_t> ind;
            size_t cop = index - 1;
            while (cop != 0) {
                ind.push_front(cop % 32);
                cop /= 32;
            }
            if (ind.empty()) {
                ind.push_front(0);
            }
            for (int i = 0; i < ind.size(); ++i) {
                for (int j = 0; j < 32; ++j) {
                    node->arr_[j] = nd->arr_[j];
                }
                if (i == ind.size() - 1) {
                    node->arr_[ind[i]]->bob_ = value;
                } else {
                    node = node->arr_[ind[i]];
                    nd = nd->arr_[ind[i]];
                }
            }
        }
        return ImmutableVector<T>(new_root, size_);
    }

    const T& Get(size_t index) const {
        std::shared_ptr<Node<T>> node = root_;
        if (index == 0) {
            return node->bob_;
        } else {
            std::deque<size_t> ind;
            size_t cop = index - 1;
            while (cop != 0) {
                ind.push_front(cop % 32);
                cop /= 32;
            }
            if (ind.empty()) {
                ind.push_front(0);
            }
            for (int i = 0; i < ind.size(); ++i) {
                if (i == ind.size() - 1) {
                    return (node->arr_[ind[i]])->bob_;
                } else {
                    node = node->arr_[ind[i]];
                }
            }
        }
        static T dummy_value;
        return dummy_value;
    }

    ImmutableVector PushBack(const T& value) {
        if (size_ != 0) {
            std::shared_ptr<Node<T>> new_root = std::make_shared<Node<T>>(root_->bob_);
            std::shared_ptr<Node<T>> node = new_root;
            std::shared_ptr<Node<T>> nd = root_;
            std::deque<size_t> ind;
            size_t cop = size_ - 1;
            while (cop != 0) {
                ind.push_front(cop % 32);
                cop /= 32;
            }
            if (ind.empty()) {
                ind.push_front(0);
            }
            for (int i = 0; i < ind.size(); ++i) {
                if (i != ind.size() - 1) {
                    for (int j = 0; j < 32; ++j) {
                        node->arr_[j] = nd->arr_[j];
                    }
                } else if (ind[i] != 0) {
                    for (int j = 0; j < ind[i]; ++j) {
                        node->arr_[j] = nd->arr_[j];
                    }
                }
                if (i == ind.size() - 1) {
                    node->arr_[ind[i]] = std::make_shared<Node<T>>(value);
                } else {
                    node = node->arr_[ind[i]];
                    nd = nd->arr_[ind[i]];
                }
            }
            return ImmutableVector<T>(new_root, size_ + 1);
        } else {
            return ImmutableVector<T>(std::make_shared<Node<T>>(value), 1);
        }
    }

    ImmutableVector PopBack() {
        if (size_ != 0) {
            std::shared_ptr<Node<T>> new_root = std::make_shared<Node<T>>(root_->bob_);
            std::shared_ptr<Node<T>> node = new_root;
            std::shared_ptr<Node<T>> nd = root_;
            std::deque<size_t> ind;
            size_t cop = size_ - 1;
            while (cop != 0) {
                ind.push_front(cop % 32);
                cop /= 32;
            }
            if (ind.empty()) {
                ind.push_front(0);
            }
            for (int i = 0; i < ind.size(); ++i) {
                if (i != ind.size() - 1) {
                    for (int j = 0; j < 32; ++j) {
                        node->arr_[j] = nd->arr_[j];
                    }
                } else {
                    for (int j = 0; j < ind[i]; ++j) {
                        node->arr_[j] = nd->arr_[j];
                    }
                }
                if (i == ind.size() - 1) {
                    break;
                } else {
                    node = node->arr_[ind[i]];
                    nd = nd->arr_[ind[i]];
                }
            }
            return ImmutableVector<T>(new_root, size_ - 1);
        } else {
            return ImmutableVector();
        }
    }

    size_t Size() const {
        return size_;
    }

private:
    std::shared_ptr<Node<T>> root_;
    size_t size_ = 0;
};
