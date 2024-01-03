#include "algorithm"

template <bool Condition, int a, int b, int N>
struct Alloh {
    enum {
        value = Alloh<(a + (a + b) / 2) / 2 >=
                          N / ((a + (a + b) / 2) / 2) + (N % ((a + (a + b) / 2) / 2) != 0),
                      a, (a + b) / 2, N*(b - a > 1)>::value
    };
};
template <int a, int b, int N>
struct Alloh<false, a, b, N> {
    enum {
        value = Alloh<((a + b) / 2 + b) / 2 >=
                          N / (((a + b) / 2 + b) / 2) + (N % (((a + b) / 2 + b) / 2) != 0),
                      (a + b) / 2, b, N*(b - a > 1)>::value
    };
};
template <int a, int b>
struct Alloh<false, a, b, 0> {
    enum { value = b };
};
template <int a, int b>
struct Alloh<true, a, b, 0> {
    enum { value = b };
};
template <int b, int N>
struct Alloh<false, b, b, N> {
    enum { value = b };
};
template <int b, int N>
struct Alloh<true, b, b, N> {
    enum { value = b };
};

template <int N>
struct Sqrt {
    enum { value = Alloh<(N / 2) >= N / ((N / 2)), 0, N, N>::value };
};
template <>
struct Sqrt<1> {
    enum { value = 1 };
};
