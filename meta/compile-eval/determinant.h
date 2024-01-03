#pragma once

#include <array>
template <int N, int I, int J>
constexpr int& GetElement(const std::array<std::array<int, N>, N>& a) {
    return std::get<J>(std::get<I>(a));
}
constexpr long long gcd(long long a, long long b) {
    long long x = a > b ? a : b;
    long long y = a > b ? b : a;
    if (y == 0) {
        return x;
    } else {
        return gcd(x % y, y);
    }
}
template <int N>
constexpr int determinant(const std::array<std::array<int, N>, N>& a) {
    long long int det = 1;

    if (N == 1) {
        return a[0][0];
    }
    long long matr[N][N] = {};
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matr[i][j] = a[i][j];
        }
    }
    long long int del = 1;
    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            if (matr[j][i] % matr[i][i] != 0) {
                del *= matr[i][i];
                for (int k = i; k < N; ++k) {
                    matr[j][k] *= matr[i][i];
                }
                long long int abs1 = del > 0 ? del : -del;
                long long int abs2 = det > 0 ? det : -det;
                long long int cmd = gcd(abs2, abs1);
                if (cmd != 0) {
                    det = det / cmd;
                    del = del / cmd;
                }
            }
            int factor = matr[j][i] / matr[i][i];
            for (int k = i; k < N; ++k) {
                matr[j][k] -= factor * matr[i][k];
            }
        }
        det *= matr[i][i];
    }

    return det / del;
}
