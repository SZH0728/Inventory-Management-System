
#ifndef INDEX_H
#define INDEX_H

#include <string>
#include <unordered_map>

class Index {
private:
    std::unordered_map<std::wstring, int> name_to_code;

    static std::wstring str_to_w_str(const std::string& str);
    static std::string w_str_to_str(const std::wstring& w_str);
    static int levenshtein(const std::wstring &s1, const std::wstring &s2);
    static int levenshtein(const std::string &s1, const std::string &s2);
public:
    void insert(const std::string &name, int code);
    int select(const std::string &name);
    std::vector<std::string> del(int code);
    std::vector<int> find(const std::string &name, int max_distance = 2) const;
};

#endif //INDEX_H
