
#include "../include/index.h"

#include <codecvt>
#include <iostream>
#include <locale>


std::wstring Index::str_to_w_str(const std::string &str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.from_bytes(str);
}


std::string Index::w_str_to_str(const std::wstring &w_str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.to_bytes(w_str);
}


int Index::levenshtein(const std::wstring &s1, const std::wstring &s2) {
    const size_t m = s1.size(), n = s2.size();
    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1, 0));

    for(int i=0; i<=m; ++i) {
        dp[i][0] = i;
    }

    for(int j=0; j<=n; ++j) {
        dp[0][j] = j;
    }

    for(int i=1; i<=m; ++i) {
        for(int j=1; j<=n; ++j) {
            dp[i][j] = std::min({
                dp[i-1][j]+1,
                dp[i][j-1]+1,
                dp[i-1][j-1]+(s1[i-1]!=s2[j-1]) * 2
            });
        }
    }

    return dp[m][n];
}


int Index::levenshtein(const std::string &s1, const std::string &s2) {
    const size_t m = s1.size(), n = s2.size();
    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1, 0));

    for(int i=0; i<=m; ++i) {
        dp[i][0] = i;
    }

    for(int j=0; j<=n; ++j) {
        dp[0][j] = j;
    }

    for(int i=1; i<=m; ++i) {
        for(int j=1; j<=n; ++j) {
            dp[i][j] = std::min({
                dp[i-1][j]+1,
                dp[i][j-1]+1,
                dp[i-1][j-1]+(s1[i-1]!=s2[j-1]) * 2
            });
        }
    }

    return dp[m][n];
}



void Index::insert(const std::string &name, const int code) {
    name_to_code[str_to_w_str(name)] = code;
}


int Index::select(const std::string &name) {
    const std::wstring w_name = str_to_w_str(name);
    if (!name_to_code.count(w_name)) {
        throw std::out_of_range("name not found");
    }

    return name_to_code[w_name];
}


std::vector<int> Index::find(const std::string &name, const int max_distance) const {
    if (name.empty()) {
        return {};
    }

    std::vector<int> match;

    for (std::pair<std::wstring, int> kv : name_to_code) {
        if (levenshtein(kv.first, str_to_w_str(name)) <= max_distance) {
            match.push_back(kv.second);
        }
    }

    return match;
}


std::vector<std::string> Index::del(const int code) {
    std::vector<std::string> result;

    for (std::pair<std::wstring, int> kv: name_to_code) {
        if (kv.second == code) {
            result.push_back(w_str_to_str(kv.first));
            name_to_code.erase(kv.first);
        }
    }

    return result;
}

