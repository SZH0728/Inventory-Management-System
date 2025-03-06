
#include "../include/index.h"

#include <codecvt>
#include <locale>


// 字符串转宽字符串（UTF-8 -> wstring）
std::wstring Index::str_to_w_str(const std::string &str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX> converterX;
    return converterX.from_bytes(str);
}



// 宽字符串转普通字符串（wstring -> UTF-8）
std::string Index::w_str_to_str(const std::wstring &w_str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX> converterX;
    return converterX.to_bytes(w_str);
}



// 计算宽字符串的编辑距离（动态规划实现）
int Index::levenshtein(const std::wstring &s1, const std::wstring &s2) {
    const size_t m = s1.size(), n = s2.size();
    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1, 0));

    // 计算宽字符串的编辑距离（动态规划实现）
    for(int i=0; i<=m; ++i) {
        dp[i][0] = i; // 全删除操作
    }

    for(int j=0; j<=n; ++j) {
        dp[0][j] = j;  // 全插入操作
    }

    for(int i=1; i<=m; ++i) {
        for(int j=1; j<=n; ++j) {
            dp[i][j] = std::min({
                // 三种操作取最小值：
                // 1. 删除s1字符（上边值+1）
                // 2. 插入s2字符（左边值+1）
                // 3. 替换操作（左上值+0或2）
                dp[i-1][j]+1,
                dp[i][j-1]+1,
                dp[i-1][j-1]+(s1[i-1]!=s2[j-1]) * 2
            });
        }
    }

    return dp[m][n];
}


// 重载版本：普通字符串的编辑距离计算
int Index::levenshtein(const std::string &s1, const std::string &s2) {
    // 转换为宽字符后调用宽字符版本
    return levenshtein(str_to_w_str(s1), str_to_w_str(s2));
}


// 插入键值对到哈希表
void Index::insert(const std::string &name, const int code) {
    name_to_code[str_to_w_str(name)] = code; // 存储宽字符串格式
}


// 根据名称查询编码
int Index::select(const std::string &name) {
    const std::wstring w_name = str_to_w_str(name);
    if (!name_to_code.count(w_name)) {
        throw std::out_of_range("name not found"); // 不存在时抛出异常
    }

    return name_to_code[w_name];
}


// 根据编辑距离查找匹配编码
std::vector<int> Index::find(const std::string &name, const int max_distance) const {
    if (name.empty()) {
        return {};  // 空输入直接返回
    }

    std::vector<int> match;

    // 遍历所有键值对
    for (std::pair<std::wstring, int> kv : name_to_code) {
        std::wstring w_str = str_to_w_str(name);
        const int distance = levenshtein(kv.first, w_str);
        if (distance <= max_distance && distance < std::max(kv.first.size(), w_str.size())) {
            match.push_back(kv.second); // 符合距离要求的加入结果
        }
    }

    return match;
}


// 删除指定编码的所有条目
std::vector<std::string> Index::del(const int code) {
    std::vector<std::string> result;

    // 遍历查找目标编码
    for (std::pair<std::wstring, int> kv: name_to_code) {
        if (kv.second == code) {
            result.push_back(w_str_to_str(kv.first));
            name_to_code.erase(kv.first);
        }
    }

    return result;
}

