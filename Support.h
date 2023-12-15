#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

using namespace std;

int findMatch(string word, std::vector<std::string> words);
bool isfromthe(char a, string mas);
bool isfrom(string a, string* mas, int size);
bool isconst(string a);
bool issomeword(string a);
int get_num(string a, string* mas, int size);

class Node {
public:
    std::string type;
    std::string value;
    std::vector<std::shared_ptr<Node>> children;

    Node(const std::string& value) : value(value) {}
    Node(const std::string& value, const std::vector<std::shared_ptr<Node>>& children) : value(value), children(children) {}
};

struct Lexema {
    int value;
    int vid;
    string lexClass;
    Lexema() : value(0), vid(0), lexClass("") {}
};

struct lex_tables {
    std::vector <Lexema> lexStream;

    std::vector<std::string> idTable;
    std::vector<std::string> idTypeTable;
    std::vector<std::string> constTable;
    std::vector<std::string> constTypeTable;
    std::vector<std::string> LexClasses;
    vector<int> inn;

    bool error;
};