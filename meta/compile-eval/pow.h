#pragma once

template <unsigned a, unsigned b>
struct Pow {
    enum { value = a * Pow<a, b - 1>::value };
};
template <unsigned a>
struct Pow<a, 0> {
    enum { value = 1 };
};
