#include "header.hpp"
#include "Utils.hpp"
#include <iostream>
#include <string>
#include <vector>

vector<string>	split(const string& str, const string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = str.find(delimiter, pos_start)) != string::npos) {
        token = str.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        if (token[token.size() -1] == '\r') token.erase(token.size() - 1);
        if (token.length() != 0) res.push_back(token);
    }
    token = str.substr(pos_start);
    if (token.length() != 0) res.push_back(token);
    return res;
}
