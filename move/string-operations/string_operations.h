#pragma once
#include <string>
#include <string_view>
#include <algorithm>
#include <memory>
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <type_traits>
#include <typeinfo>
#include <vector>
#include <numeric>
bool StartsWith(std::string_view string, std::string_view text);

bool EndsWith(std::string_view string, std::string_view text);

std::string_view StripPrefix(std::string_view string, std::string_view prefix);

std::string_view StripSuffix(std::string_view string, std::string_view suffix);

std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n = std::string_view::npos);

std::string_view StripAsciiWhitespace(std::string_view);

std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim);

std::string ReadN(const std::string& filename, size_t n);

std::string AddSlash(std::string_view path);

std::string_view RemoveSlash(std::string_view path);

std::string_view Dirname(std::string_view path);

std::string_view Basename(std::string_view path);

std::string CollapseSlashes(std::string_view path);

std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter);
inline std::string StrCatHelper(size_t x) {
    std::string str;
    str.reserve(x + 8);
    return str;
}
template <typename T, typename... Args>
std::string StrCatHelper(size_t x, const T& ft, const Args&... rest) {
    if constexpr (std::is_same_v<std::decay_t<decltype(ft)>, std::string> ||
                  std::is_same_v<std::decay_t<decltype(ft)>, std::string_view> ||
                  std::is_same_v<std::decay_t<decltype(ft)>, const char[]> ||
                  std::is_same_v<std::decay_t<decltype(ft)>, const char*> ||
                  std::is_same_v<std::decay_t<decltype(ft)>, const char>) {
        std::string_view st = static_cast<std::string_view>(ft);
        x += st.size();
        std::string cr(std::move(StrCatHelper(x, rest...)));
        x -= st.size();
        for (size_t i = 0; i < st.size(); ++i) {
            cr += st[st.size() - i - 1];
        }
        if (x == 0) {
            std::reverse(cr.begin(), cr.end());
        }
        return cr;
    } else {
        T first = ft;
        if constexpr (std::is_same_v<std::decay_t<decltype(ft)>, char> ||
                      std::is_same_v<std::decay_t<decltype(ft)>, const char>) {
            ++x;
            std::string cr(std::move(StrCatHelper(x, rest...)));
            ++x;
            cr += first;
            return cr;
        } else {
            T fst = first;
            size_t cnt = 1;
            while (fst / 10 != 0) {
                ++cnt;
                fst /= 10;
            }
            x += cnt;
            std::string cr(std::move(StrCatHelper(x, rest...)));
            x -= cnt;
            bool flg = true;
            if (first < 0) {
                flg = false;
            }
            while (first / 10 != 0) {
                cr += static_cast<char>(std::abs(static_cast<int>(first % 10)) + '0');
                first /= 10;
            }
            cr += static_cast<char>(std::abs(static_cast<int>(first)) + '0');
            if (!flg) {
                cr += '-';
            }
            if (x == 0) {
                std::reverse(cr.begin(), cr.end());
            }
            return cr;
        }
    }
}
inline std::string StrCat() {
    return "";
}
template <typename T, typename... Args>
std::string StrCat(const T& ft, const Args&... rest) {
    return StrCatHelper(size_t{0}, ft, rest...);
}
