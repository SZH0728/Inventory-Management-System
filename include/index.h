#ifndef INDEX_H
#define INDEX_H

#include <string>
#include <unordered_map>

class Index {
private:
    std::unordered_map<std::wstring, int> name_to_code;

    /**
     * @brief 将UTF-8字符串转换为宽字符串
     * @param str 输入的UTF-8编码字符串
     * @return 转换后的宽字符串
     * @note 使用std::codecvt_utf8进行编码转换
     */
    static std::wstring str_to_w_str(const std::string& str);

    /**
     * @brief 将宽字符串转换为UTF-8字符串
     * @param w_str 输入的宽字符串
     * @return 转换后的UTF-8编码字符串
     * @note 使用std::codecvt_utf8进行编码转换
     */
    static std::string w_str_to_str(const std::wstring& w_str);

    /**
     * @brief 计算两个宽字符串的编辑距离
     * @param s1 第一个宽字符串
     * @param s2 第二个宽字符串
     * @return 两个字符串的最小编辑操作次数
     * @note 替换操作权重为2，插入/删除权重为1
     */
    static int levenshtein(const std::wstring &s1, const std::wstring &s2);

    /**
     * @brief 计算两个字符串的编辑距离
     * @param s1 第一个UTF-8字符串
     * @param s2 第二个UTF-8字符串
     * @return 两个字符串的最小编辑操作次数
     * @note 先转换为宽字符再计算，替换操作权重为2，插入/删除权重为1
     */
    static int levenshtein(const std::string &s1, const std::string &s2);

public:
    /**
     * @brief 插入名称与编码的映射关系
     * @param name 要插入的名称（UTF-8编码）
     * @param code 对应的整型编码
     */
    void insert(const std::string &name, int code);

    /**
     * @brief 查询指定名称对应的编码
     * @param name 要查询的名称（UTF-8编码）
     * @return 对应的整型编码
     * @throw std::out_of_range 当名称不存在时抛出异常
     */
    int select(const std::string &name);

    /**
     * @brief 删除指定编码的所有映射
     * @param code 要删除的整型编码
     * @return 被删除的名称字符串列表（UTF-8编码）
     */
    std::vector<std::string> del(int code);

    /**
     * @brief 查找与指定名称编辑距离在阈值内的编码
     * @param name 查询名称（UTF-8编码）
     * @param max_distance 最大允许编辑距离（默认2）
     * @return 符合条件的所有编码列表
     * @note 空名称直接返回空列表
     */
    std::vector<int> find(const std::string &name, int max_distance = 2) const;
};

#endif //INDEX_H
