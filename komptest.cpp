#include <iostream>
#include <fstream>
#include <vector>
#include "Lex.h"
#include "syntaxAnalyzer.h"


using namespace std;


int main() {
    
    setlocale(LC_ALL, "Russian");

    // лексический анализ
    lex_tables lexTable = LexAnalysis();
    if (lexTable.error == true) {
        cout << "Ошибка при лексическом анализе" << endl;
    }

    syntaxAnalyzer* syntax = new syntaxAnalyzer(lexTable.lexStream, lexTable.idTable, lexTable.constTable,
        lexTable.LexClasses, lexTable.idTypeTable, lexTable.constTypeTable, lexTable.inn);
    //синтаксический анализ

    if (lexTable.error == false) {
        cout << "\n\n\n\nsyntax Analysis\n";

        syntax->analysis();
        cout << "Синтаксический анализ завершён успешно" << endl;
        syntax->PrintTable();
    }

    //перевод в ассемблерный код

    return 0;
}

