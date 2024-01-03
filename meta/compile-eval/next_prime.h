#pragma once
constexpr bool is_prime(int x) {
    if (x == 1) {
        return false;
    }
    int d = 2;
    while (d * d <= x) {
        if (x % d == 0) {
            return false;
        }
        ++d;
    }
    return true;
}

constexpr int next_prime(int x) {
    if (is_prime(x)) {
        return x;
    } else {
        return next_prime(x + 1);
    }
}
