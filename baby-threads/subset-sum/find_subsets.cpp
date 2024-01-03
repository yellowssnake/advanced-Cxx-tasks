#include "find_subsets.h"
#include <vector>
#include "algorithm"
#include "thread"
#include "iostream"
#include "unordered_map"
#include "array"
#include "deque"
void GenerateSums(std::vector<std::pair<int64_t, std::pair<int64_t, int64_t>>>& sum, int sz,
                  int64_t cursum, int curi, int64_t allah, const std::vector<int64_t>& data,
                  int flag) {
    if (curi >= sz) {
        sum.push_back({cursum, {allah, flag}});
    } else {
        GenerateSums(sum, sz, cursum + data[curi], curi + 1, allah * 10 + 2, data,
                     std::min(3, 2 + (flag == 1)));
        GenerateSums(sum, sz, cursum - data[curi], curi + 1, allah * 10 + 1, data,
                     std::min(3, 1 + 2 * (flag == 2)));
        GenerateSums(sum, sz, cursum, curi + 1, allah * 10, data, flag);
    }
}
void GenerateSums2(std::unordered_map<int64_t, std::array<int64_t, 4>>& sum, int sz, int64_t cursum,
                   int curi, int64_t allah, const std::vector<int64_t>& data, int flag, int& val) {
    if (curi >= sz) {
        sum[cursum][flag] = allah;
    } else {
        allah *= 10;
        GenerateSums2(sum, sz, cursum + data[val + curi], curi + 1, allah + 2, data,
                      std::min(3, 2 + (flag == 1 or flag == 3)), val);
        GenerateSums2(sum, sz, cursum - data[val + curi], curi + 1, allah + 1, data,
                      std::min(3, 1 + 2 * (flag == 2 or flag == 3)), val);
        GenerateSums2(sum, sz, cursum, curi + 1, allah, data, flag, val);
    }
}
void OptimalSolve(std::atomic<bool>& bl, std::pair<std::atomic<int64_t>, std::atomic<int64_t>>& pr,
                  std::vector<std::pair<int64_t, std::pair<int64_t, int64_t>>>::iterator id1,
                  std::vector<std::pair<int64_t, std::pair<int64_t, int64_t>>>::iterator ind2,
                  const std::vector<std::pair<int64_t, std::pair<int64_t, int64_t>>>& sum1,
                  const std::unordered_map<int64_t, std::array<int64_t, 4>>& sum2) {
    for (; id1 != ind2; ++id1) {
        if (bl) {
            return;
        }
        int64_t i = -(id1->first);
        auto x = sum2.find(i);
        if (x != sum2.end()) {
            if (bl) {
                return;
            }
            int z = id1->second.second;
            if (z == 0) {
                if (x->second[3] != 0) {
                    pr.first = id1->second.first;
                    pr.second = x->second[3];
                    bl = true;
                }
            } else if (z == 1) {
                if (x->second[2] != 0) {
                    pr.first = id1->second.first;
                    pr.second = x->second[2];
                    bl = true;
                } else if (x->second[3] != 0) {
                    pr.first = id1->second.first;
                    pr.second = x->second[3];
                    bl = true;
                }
            } else if (z == 2) {
                if (x->second[1] != 0) {
                    pr.first = id1->second.first;
                    pr.second = x->second[1];
                    bl = true;
                } else if (x->second[3] != 0) {
                    pr.first = id1->second.first;
                    pr.second = x->second[3];
                    bl = true;
                }
            } else if (z == 3) {
                if (x->second[0] != 0) {
                    pr.first = id1->second.first;
                    pr.second = x->second[0];
                    bl = true;
                }
                if (x->second[1] != 0) {
                    pr.first = id1->second.first;
                    pr.second = x->second[1];
                    bl = true;
                } else if (x->second[2] != 0) {
                    pr.first = id1->second.first;
                    pr.second = x->second[2];
                    bl = true;
                } else if (x->second[3] != 0) {
                    pr.first = id1->second.first;
                    pr.second = x->second[3];
                    bl = true;
                }
            }
        }
    }
}
Subsets FindEqualSumSubsets(const std::vector<int64_t>& data) {
    std::vector<std::pair<int64_t, std::pair<int64_t, int64_t>>> sum1;
    std::unordered_map<int64_t, std::array<int64_t, 4>> sum2;
    int size1 = data.size() / 2 + 1;
    int size2 = data.size() - data.size() / 2 - 1;
    int vl = data.size() / 2 + 1;
    if (size2 < 1) {
        size1 -= 1;
        size2 += 1;
        vl = size1;
    }
    std::thread t1{GenerateSums, std::ref(sum1), size1, 0, 0, 1, data, 0};
    std::thread t2{GenerateSums2, std::ref(sum2), size2, 0, 0, 1, data, 0, std::ref(vl)};
    t1.join();
    t2.join();
    int val = std::min(sum1.size(), static_cast<size_t>(std::thread::hardware_concurrency()));
    std::vector<std::thread> conter;
    std::atomic<bool> bl = false;
    std::pair<std::atomic<int64_t>, std::atomic<int64_t>> pr;
    for (int i = 0; i < val; ++i) {
        conter.emplace_back(
            OptimalSolve, std::ref(bl), std::ref(pr), sum1.begin() + (i * sum1.size()) / val,
            sum1.begin() + ((i + 1) * sum1.size()) / val, std::ref(sum1), std::ref(sum2));
    }
    for (auto& x : conter) {
        x.join();
    }
    if (bl) {
        std::deque<int64_t> dq1;
        std::deque<int64_t> dq2;
        int counterrr = data.size();
        int64_t it1 = pr.second;
        int64_t it2 = pr.first;
        while (it1 > 1) {
            if (it1 % 10 == 1) {
                dq1.push_front(counterrr);
            } else if (it1 % 10 == 2) {
                dq2.push_front(counterrr);
            }
            it1 /= 10;
            --counterrr;
        }
        while (it2 > 1) {
            if (it2 % 10 == 1) {
                dq1.push_front(counterrr);
            } else if (it2 % 10 == 2) {
                dq2.push_front(counterrr);
            }
            it2 /= 10;
            --counterrr;
        }
        Subsets a;
        a.exists = true;
        for (auto& x : dq1) {
            a.first_indices.push_back(x - 1);
        }
        for (auto& x : dq2) {
            a.second_indices.push_back(x - 1);
        }
        return a;
    } else {
        Subsets a;
        a.exists = false;
        return a;
    }
}
