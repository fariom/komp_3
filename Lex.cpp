#include "Lex.h"

string WORDS[17] = { "PROGRAMM","WHILE","DO","EXITWHILE","ENDWHILE","if","then","else","endif","FUNC","ENDF","END","return","string","bool","integer","len" };
char SOME[12] = { '=','-','>','<','(',')',';','.','"',' ','\n' };
string SKHL[10] = { "(",">","<",")","=","==","\"","-",";","." };
string digs[10] = { "0","1","2","3","4","5","6","7","8","9" };

lex_tables LexAnalysis() {
	ifstream inputFile("input_code.txt");
	ofstream outputFile("output_code.txt");
	vector<string> proga;
	lex_tables lexTable;
	lexTable.error = false;
	if (!inputFile) {
		cerr << "Не удалось открыть файл." << endl;
		lexTable.error = true;
		return lexTable;
	}

	string code((istreambuf_iterator<char>(inputFile)), (istreambuf_iterator<char>()));
	inputFile.close();

	//cout << code;
	int i = 0;
	int j = 0;
	string v;
	while (code[i] != '^') {
		if ((code[i] == '/' and code[i + 1] == '/')) {
			while (code[i] != '\n' and code[i] != '^') {
				i++;
			}
			j = i;
		}

		if (isfromthe(code[i], SOME) == false) {
			while (isfromthe(code[i], SOME) == false) {
				i++;
			}
			string seq;
			seq = code.substr(j, i - j);
			if (!seq.empty() and seq != " " and seq != "\n" and seq != "" and seq != "\r" and seq != "\0") {
				cout << seq;
				outputFile << seq << endl;
				proga.push_back(seq);
			}
			j = i;
		}
		if (code[i] == '"') {
			string seq;
			++i;
			while (code[i] != '"' and code[i++] != '^');
			if (code[i] == '"') i++;
			seq = code.substr(j, i - j);
			cout << seq;
			outputFile << seq << endl;
			proga.push_back(seq);
			continue;
		}
		string seq = code.substr(j, i - j);
		if (code[i] != ' ' and code[i] != '\n' and code[i] != '\0' and code[i] != '\r'
			and code[i] != NULL and code[i] != '\t') {
			outputFile << code[i] << endl;
			cout << code[i];
			char h = code[i];
			string r(1, h);
			proga.push_back(r);
		}

		v += seq;
		v += code[i];
		i++;
		j = i;
	}
	//outputFile <<v;
	cout << "\n";
	for (int k = 0; k < proga.size(); k++) {
		if (proga[k] == "=" and proga[k + 1] == "=") {
			proga[k + 1].erase();
			proga[k] = "==";
			for (int g = k + 1; g < proga.size() - 1; g++) {
				proga[g] = proga[g + 1];
			}
			proga.pop_back();
		}

		//тут должна быть добавление пробелов
	}
	string buf;
	for (int k = 0; k < proga.size(); k++) {
		Lexema lex;
		if (isfrom(proga[k], WORDS, 17) == true and isfrom(proga[k], SKHL, 10) == false and isconst(proga[k]) == false) {
			cout << " " << proga[k] << " KEYWORD\n";
			lex.vid = 1;
			lex.value = get_num(proga[k], WORDS, 17);
			lex.lexClass = proga[k];
			lexTable.lexStream.push_back(lex);
		}
		else if (isconst(proga[k]) /*or ((proga[k - 1]) == "\"" and (proga[k + 1]) == "\"")*/) {
			cout << " " << proga[k] << " CONSTANT\n";
			lex.vid = 2;
			lex.value = lexTable.constTable.size();
			lexTable.constTable.push_back(proga[k]);
			lex.lexClass = proga[k];
			if (isdigit(proga[k][0]) && proga[k-1] != "WHILE" && proga[k-1] != "if") {
				lexTable.constTypeTable.push_back("integer");
			}
			else if (isdigit(proga[k][0]) && (proga[k-1] == "WHILE" || proga[k-1] == "if")) {
				lexTable.constTypeTable.push_back("bool");
			}
			else {
				lexTable.constTypeTable.push_back("string");
			}
			lexTable.lexStream.push_back(lex);
		}
		else if (isfrom(proga[k], SKHL, 10)) {
			cout << " " << proga[k] << " LIMITER\n";
			lex.vid = 3;
			lex.value = get_num(proga[k], SKHL, 12);
			lex.lexClass = proga[k];
			lexTable.lexStream.push_back(lex);
		}
		else if (issomeword(proga[k])) {
			cout << " " << proga[k] << " IDENTIFICATOR\n";
			lex.vid = 4;
			int i;
			i = findMatch(proga[k], lexTable.idTable);
			if (i != lexTable.idTable.size()) {
				lex.value = i;
			}
			else if (i == lexTable.idTable.size()) {
				lex.value = i;
				lexTable.idTable.push_back(proga[k]);
				lexTable.inn.push_back(0);
			}
			lex.lexClass = proga[k];
			if (k > 0 && k<proga.size()-8 && findMatch(proga[k - 1], {"integer","bool","string"}) < 3
				&& findMatch(proga[k - 1], { "integer","bool","string" }) >= 0
				&& proga[k + 1] != "(" && proga[k + 8] != "len") {
				lexTable.idTypeTable.push_back(proga[k - 1]);
			}
			if (k < proga.size() - 8 && proga[k + 8] == "len") {
				lexTable.idTypeTable.push_back("integer");
				lexTable.inn[lexTable.idTypeTable.size()-1]++;
			}
			lexTable.lexStream.push_back(lex);
		}
		else {
			cout << " " << proga[k] << " <------------------------------------- UNIDENTIFIED TOKEN\n";
			lexTable.error = true;
		}
		//cout << k << " " << proga[k] << endl;
	}
	outputFile.close();
	return lexTable;
}