#include "string_operations.h"

bool StartsWith(std::string_view string, std::string_view text) {
    if (string.size() < text.size()) {
        return false;
    }
    for (size_t i = 0; i < text.size(); ++i) {
        if (string[i] != text[i]) {
            return false;
        }
    }
    return true;
}
bool EndsWith(std::string_view string, std::string_view text) {
    if (string.size() < text.size()) {
        return false;
    }
    for (size_t i = 0; i < text.size(); ++i) {
        if (string[string.size() - 1 - i] != text[text.size() - 1 - i]) {
            return false;
        }
    }
    return true;
}
std::string_view StripPrefix(std::string_view string, std::string_view prefix) {
    if (prefix.size() > string.size()) {
        return string;
    }
    bool chk = true;
    for (size_t i = 0; i < prefix.size(); ++i) {
        if (string[i] != prefix[i]) {
            chk = false;
            break;
        }
    }
    if (chk) {
        return std::string_view(string.begin() + prefix.size(), string.end());
    } else {
        return string;
    }
}
std::string_view StripSuffix(std::string_view string, std::string_view suffix) {
    if (suffix.size() > string.size()) {
        return string;
    }
    bool chk = true;
    for (size_t i = 0; i < suffix.size(); ++i) {
        if (string[string.size() - 1 - i] != suffix[suffix.size() - 1 - i]) {
            chk = false;
            break;
        }
    }
    if (chk) {
        return std::string_view(string.begin(), string.end() - suffix.size());
    } else {
        return string;
    }
}
std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n) {
    if (n > s.size()) {
        return s;
    } else if (pos + n > s.size() or n == std::string_view::npos) {
        return std::string_view(s.begin() + pos, s.size() - pos - 1);
    } else {
        return std::string_view(s.begin() + pos, n);
    }
}
std::string_view StripAsciiWhitespace(std::string_view s) {
    size_t start = 0;
    size_t end = s.size() - 1;
    while (isspace(s[start]) and start < s.size() - 1) {
        ++start;
    }
    while (isspace(s[end]) and end > start) {
        --end;
    }
    return std::string_view(s.begin() + start, end - start + 1 - (start == s.size() - 1));
}
std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim) {
    if (text.empty()) {
        std::vector<std::string_view> vec(1);
        return vec;
    }
    size_t del_count = 0;
    size_t cr = 0;
    while (cr < text.size()) {
        size_t it = 0;
        bool chk = true;
        while (it + cr < text.size() and it < delim.size()) {
            if (delim[it] != text[cr + it]) {
                chk = false;
                break;
            }
            ++it;
        }
        if (it < delim.size()) {
            chk = false;
        }
        ++cr;
        del_count += chk;
    }
    std::vector<std::string_view> vec(del_count + 1);
    size_t cur = 0;
    size_t new_start = 0;
    size_t vec_pos = 0;
    while (cur < text.size()) {
        size_t it = 0;
        bool chk = true;
        while (it + cur < text.size() and it < delim.size()) {
            if (delim[it] != text[cur + it]) {
                chk = false;
                break;
            }
            ++it;
        }
        if (it < delim.size()) {
            chk = false;
        }
        if (chk) {
            vec[vec_pos] = std::string_view(text.begin() + new_start, cur - new_start);
            cur = cur + it;
            new_start = cur;
            ++vec_pos;
        } else {
            ++cur;
        }
    }
    if (vec_pos < vec.size()) {
        vec[vec_pos] = std::string_view(text.begin() + new_start, text.size() - 1 - new_start + 1);
    }
    return vec;
}
std::string AddSlash(std::string_view path) {
    if (!path.empty() and path[path.size() - 1] == '/') {
        return std::string(path);
    }
    std::string x;
    x.reserve(path.size() + 1);
    for (size_t i = 0; i < path.size(); ++i) {
        x += path[i];
    }
    x += '/';
    return x;
}
std::string_view RemoveSlash(std::string_view path) {
    if (path.empty() or (path.size() == 1 and path[0] == '/')) {
        return path;
    }
    if (path[path.size() - 1] == '/') {
        return std::string_view(path.begin(), path.size() - 1);
    } else {
        return path;
    }
}
std::string_view Dirname(std::string_view path) {
    auto start = path.size() - 1;
    while (path[start] != '/') {
        --start;
    }
    if (start == 0) {
        return std::string_view(path.begin(), 1);
    } else {
        return std::string_view(path.begin(), start);
    }
}
std::string_view Basename(std::string_view path) {
    auto start = path.size() - 1;
    while (path[start] != '/') {
        --start;
    }
    return std::string_view(path.begin() + start + 1, path.size() - start - 1);
}
std::string CollapseSlashes(std::string_view path) {
    std::string str;
    size_t ct = 0;
    for (size_t i = 0; i < path.size(); ++i) {
        if (i != path.size() - 1 and path[i] == '/' and path[i + 1] == '/') {
            continue;
        }
        ++ct;
    }
    str.reserve(ct);
    for (size_t i = 0; i < path.size(); ++i) {
        if (i != path.size() - 1 and path[i] == '/' and path[i + 1] == '/') {
            continue;
        }
        str += path[i];
    }
    return str;
}
std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter) {
    if (!strings.empty()) {
        std::string str;
        size_t max_size = 0;
        for (size_t i = 0; i < strings.size(); ++i) {
            max_size = std::max(max_size, strings[i].size());
        }
        str.reserve(strings.size() * max_size + (strings.size() - 1) * delimiter.size());
        for (size_t i = 0; i < strings.size(); ++i) {
            for (size_t j = 0; j < strings[i].size(); ++j) {
                str += strings[i][j];
            }
            if (i != strings.size() - 1) {
                for (size_t h = 0; h < delimiter.size(); ++h) {
                    str += delimiter[h];
                }
            }
        }
        return str;
    } else {
        return "";
    }
}
std::string ReadN(const std::string& filename, size_t n) {
    int ff = open(filename.c_str(), O_RDONLY);
    if (ff == -1) {
        return "";
    }
    std::string str;
    str.resize(n);
    int bytes = read(ff, str.data(), n);
    if (bytes < n) {
        return "";
    }
    close(ff);
    return str;
}