#pragma once

constexpr long int hash(const char *s, long int p, long int mod, int cnt = 0) {
    return (*(s + cnt) == '\0' or *(s + cnt + 1) == '\0')
               ? *(s + cnt)
               : (*(s + cnt) + ((hash(s, p, mod, cnt + 1)) % mod) * p) % mod;
}
