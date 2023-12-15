#include "Support.h"

int findMatch(string word, std::vector<std::string> words) {
    for (int n = 0;n < words.size();n++) {
        if (word == words[n]) {
            return n;
        }
    }
    return words.size();
}
bool isfromthe(char a, string mas) {
    for (int p = 0; p < mas.length(); p++) {
        if (mas[p] == a) {
            return true;
        }
    }
    return false;
}
bool isfrom(string a, string* mas, int size) {
    for (int p = 0; p < size; p++) {
        if (mas[p] == a) {
            return true;
        }
    }
    return false;
}

bool isconst(string a) {
    if (a[0] == '\"' and a[a.length() - 1] == '\"')
        return true;
    for (char c : a) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool issomeword(string a) {
    string valid = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567890";
    if (isdigit(a[0])) {
        return false;
    }
    for (char d : a) {
        if (valid.find(d) == std::string::npos) {
            return false;
        }
    }
    return true;
}
int get_num(string a, string* mas, int size) {
    for (int p = 0; p < size; p++) {
        if (mas[p] == a) {
            return p;
        }
    }
    return 0;
}
