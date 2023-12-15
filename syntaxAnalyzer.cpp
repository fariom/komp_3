#include "syntaxAnalyzer.h"

/*
len работает как оператор, выдающий символьную длину string объекта в integer
[ ] скобки для группировки терминальных/нетерминальных символов
* рядом с нетерминальном, значит 0 или более раз

S             ::= PROGRAMM ; <declaration>* <statement>* <function> END .
declaration   ::= <type> <id> ;
statement     ::= [ <assignment> | <loop> | <ifelse> | EXITWHILE ] ;
assignment    ::= <identifier> = [ <expression> | ( <condition> ) ] ;
loop          ::= WHILE <condition> DO <statement> ENDWHILE ;
function      ::= FUNC <identifiers> ( <declaration> ) ; <statement>* <return> ENDF ;
return        ::= return ( <len> ) ;
function_call ::= <identifier> ( <identifier> )
ifelse        ::= if <condition> then <statement>* <else_block> endif ;
else_block    ::= else <statement> | ε
expression    ::= <function_call> | <id> [+|-|*] <expression> | <id> | <integer>
condition     ::= <expression> | <expression> <operator> <expression> ;
operator      ::= == | > | < | <= | >= | !=
id            ::= character [ character | digit ]*
integer       ::= [1-9] digit* | 0
string        ::= " [ character | digit ]* "
character     ::= [a-z] | [A-Z] | _
digit         ::= [0-9]
len           ::= len ( <expression> )

*/


void syntaxAnalyzer::analysis()
{
	ParseTree = analysisS();
}

std::shared_ptr<Node> syntaxAnalyzer::analysisS()
{
	std::vector<std::shared_ptr<Node>> children;
	if (IsMatch("PROGRAMM")) {
		children.push_back(Match("PROGRAMM"));
		children.push_back(Match(";"));
		children.push_back(analysisDeclarations());
		children.push_back(analysisStatements());
		children.push_back(analysisFunction());
		children.push_back(Match("END"));
		children.push_back(Match("."));
	}
	else {
		try {
			std::cout << "Unexpected lexema: " + CurrentLex() << '\n';
			throw std::runtime_error("Unexpected lexema: " + CurrentLex());
		}
		catch (...) {
			error();
			exit(-1);
		}
	}
	return std::make_shared<Node>("---Programm---", children);
}
std::shared_ptr<Node> syntaxAnalyzer::analysisFunction() {
	std::vector<std::shared_ptr<Node>> children;
	if (IsMatch("FUNC")) {
		children.push_back(Match("FUNC"));
		if (reprIdTable[reprIdTable.size()-2]==CurrentLex()) {
			children.push_back(analysisIdFunc());
		}
		else {
			try {
				std::cout << "Unexpected lexema: " + CurrentLex() << '\n';
				throw std::runtime_error("Unexpected lexema: " + CurrentLex());
			}
			catch (...) {
				error();
				exit(-1);
			}
		}
		children.push_back(Match("("));
		children.push_back(analysisDeclaration());
		children.push_back(Match(")"));
		children.push_back(Match(";"));
		children.push_back(analysisStatements());
		children.push_back(analysisReturn());
		children.push_back(Match("ENDF"));
		children.push_back(Match(";"));
	}
	else {
		try {
			std::cout << "Unexpected lexema: " + CurrentLex() << '\n';
			throw std::runtime_error("Unexpected lexema: " + CurrentLex());
		}
		catch (...) {
			error();
			exit(-1);
		}
	}
	return std::make_shared<Node>("---Function---", children);
}

std::shared_ptr<Node> syntaxAnalyzer::analysisReturn() {
	std::vector<std::shared_ptr<Node>> children;
	children.push_back(Match("return"));
	children.push_back(Match("("));
	children.push_back(analysisLen());
	children.push_back(Match(")"));
	children.push_back(Match(";"));
	return std::make_shared<Node>("----Return----", children);
}

std::shared_ptr<Node> syntaxAnalyzer::analysisDeclarations() {
	std::vector<std::shared_ptr<Node>> children;
	while (IsMatch({ "integer", "string", "bool" })) {
		children.push_back(analysisDeclaration());
		children.push_back(Match(";"));
	}
	return std::make_shared<Node>("-Declarations-", children); // может быть пустым
}

std::shared_ptr<Node> syntaxAnalyzer::analysisDeclaration() {
	std::vector<std::shared_ptr<Node>> children;
	children.push_back(Match({ "integer","string","bool" }));
	try {
		for (int i = 0; i < reprIdTable.size() - 1;i++) {
			if (CurrentLex() == reprIdTable[i]) {
				inn[i]++;
				if (inn[i] > 1) {
					std::cout << CurrentLex() << " :---: ";
					std::cout << "Redeclaration\n";
					throw std::runtime_error("Redeclaration");
					break;
				}
			}
		}
	}
	catch (...) {
		error();
		exit(-1);
	}

	if (IsMatch(reprIdTable)) {
		children.push_back(Match(CurrentLex()));
	}
	else {
		try {
			std::cout << CurrentLex() << " :---: ";
			std::cout << "It's not ID\n";
			throw std::runtime_error("It's not ID");
		}
		catch (...) {
			error();
			exit(-1);
		}
	}
	return std::make_shared<Node>("-Declaration--", children);
}

std::shared_ptr<Node> syntaxAnalyzer::analysisStatements() {
	std::vector<std::shared_ptr<Node>> children;
	while (IsMatch("if") || IsMatch(reprIdTable) || IsMatch("EXITWHILE") || IsMatch("WHILE")) {
		children.push_back(analysisStatement());
	}
	return std::make_shared<Node>("--Statements--", children); // может быть пустым
}
std::shared_ptr<Node> syntaxAnalyzer::analysisStatement() {
	std::vector<std::shared_ptr<Node>> children;
	if (IsMatch("if")) {
		children.push_back(analysisIfElse());
	}
	else if (IsMatch(reprIdTable)) {
		children.push_back(analysisAssignment());
	}
	else if (IsMatch("EXITWHILE")) {
		if (inWhile!=0){
		children.push_back(Match("EXITWHILE"));
		}
		else {
			std::cout << "EXITWHILE error - not in WHILE" << endl;
			error();
			exit(-1);
		}
		children.push_back(Match(";"));
	}
	else if (IsMatch("WHILE")) {
		children.push_back(analysisLoop());
	}
	return std::make_shared<Node>("--Statement---", children);
}

std::shared_ptr<Node> syntaxAnalyzer::analysisIfElse() {
	std::vector<std::shared_ptr<Node>> children;
	children.push_back(Match("if"));
	children.push_back(analysisCondition());
	children.push_back(Match("then"));
	children.push_back(analysisStatements());
	children.push_back(analysisElseBlock());
	children.push_back(Match("endif"));
	children.push_back(Match(";"));
	return std::make_shared<Node>("----IfElse----", children);
}

std::shared_ptr<Node> syntaxAnalyzer::analysisElseBlock() {
	std::vector<std::shared_ptr<Node>> children;
	children.push_back(Match("else"));
	children.push_back(analysisStatement());
	return std::make_shared<Node>("---ElseBlock--", children); // может быть пустым
}

std::shared_ptr<Node> syntaxAnalyzer::analysisCondition() {
	std::vector<std::shared_ptr<Node>> children;
	if (IsMatch(reprIdTable) || IsMatch(constTable)) {
		if (lexStream[lexIndex + 1].lexClass == "(" && (
			lexStream[lexIndex + 4].lexClass == "==" || lexStream[lexIndex + 4].lexClass == "!=" ||
			lexStream[lexIndex + 4].lexClass == ">" || lexStream[lexIndex + 4].lexClass == "<" ||
			lexStream[lexIndex + 4].lexClass == ">=" || lexStream[lexIndex + 4].lexClass == "<=")) {
			children.push_back(analysisCallF());
			typesCondition.push_back(children[children.size() - 1]->children[0]->type);
			children.push_back(Match({ "==", "!=", ">", "<", ">=", "<=" }));
			children.push_back(analysisExpression());
			typesCondition.push_back(children[children.size() - 1]->children[0]->type);
			try {
				if (!IsSame(typesCondition)) {
					std::cout << typesCondition[0] << " and " << typesCondition[1] << " :---: ";
					std::cout << "Condition error - types don't match \n";
					throw std::runtime_error("Condition error - types don't match");
				}
			}
			catch (...) {
				error();
				exit(-1);
			}
		}
		else if (lexStream[lexIndex + 1].lexClass == "==" || lexStream[lexIndex + 1].lexClass == "!=" ||
			lexStream[lexIndex + 1].lexClass == ">" || lexStream[lexIndex + 1].lexClass == "<" ||
			lexStream[lexIndex + 1].lexClass == ">=" || lexStream[lexIndex + 1].lexClass == "<=") {
			children.push_back(analysisExpression());
			typesCondition.push_back(children[children.size() - 1]->children[0]->type);
			children.push_back(Match({ "==", "!=", ">", "<", ">=", "<=" }));
			children.push_back(analysisExpression());
			typesCondition.push_back(children[children.size() - 1]->children[0]->type);
			try {
				if (!IsSame(typesCondition)) {
					std::cout << typesCondition[0] << " and " << typesCondition[1] << " :---: ";
					std::cout << "Condition error - types don't match \n";
					throw std::runtime_error("Condition error - types don't match");
				}
			}
			catch (...) {
				error();
				exit(-1);
			}
		}
		else {
			children.push_back(analysisExpression());
			typesCondition.push_back(children[children.size() - 1]->children[0]->type);
			try {
				if (typesCondition[0] != "bool") {
					std::cout << typesCondition[0] << " :---: ";
					std::cout << "Condition has to be bool \n";
					throw std::runtime_error("Condition has to be bool ");
				}
			}
			catch (...) {
				error();
				exit(-1);
			}
		}
	}
	else {
		try {
			std::cout << "Condition cannot be empty\n";
			throw std::runtime_error("Condition cannot be empty");
		}
		catch (...) {
			error();
			exit(-1);
		}
	}
	children[0]->type = "bool";
	typesCondition.clear();
	return std::make_shared<Node>("---Condition--", children);
}

std::shared_ptr<Node> syntaxAnalyzer::analysisAssignment()
{
	std::vector<std::shared_ptr<Node>> children;
	children.push_back(analysisIdentif());
	typesAssignment.push_back(children[children.size() - 1]->children[0]->type);
	children.push_back(Match("="));
	if (IsMatch("(")) {
		children.push_back(Match("("));
		children.push_back(analysisCondition());
		typesAssignment.push_back(children[children.size() - 1]->children[0]->type);
		children.push_back(Match(")"));
	}
	else {
		children.push_back(analysisExpression());
		typesAssignment.push_back(children[children.size() - 1]->children[0]->type);
	}
	children.push_back(Match(";"));
	try {
		if (!IsSame(typesAssignment)) {
			std::cout << typesAssignment[0] << " and " << typesAssignment[1] << " :---: ";
			std::cout << "Assignment error - types don't match \n";
			throw std::runtime_error("Assignment error - types don't match");
		}
		children[0]->type = typesAssignment[0];
		typesAssignment.clear();
	}
	catch (...) {
		error();
		exit(-1);
	}
	return std::make_shared<Node>("--Assignment--", children);
}

std::shared_ptr<Node> syntaxAnalyzer::analysisExpression()
{
	std::vector<std::shared_ptr<Node>> children;
	bool flag1 = typesExpression.size();
	if (IsMatch(reprIdTable)) {
		if (lexStream[lexIndex + 1].lexClass == "(") {
			for (int i = 0; i < reprIdTable.size();i++) {
				if (CurrentLex() == reprIdTable[i]) {
					if (inn[i] == 1) {
						children.push_back(analysisCallF());
						typesExpression.push_back(children[children.size() - 1]->children[0]->type);
						if (IsMatch({ "+" , "-", "*" })) {
							children.push_back(Match({ "+" , "-", "*" }));
							children.push_back(analysisExpression());
							typesExpression.push_back(children[children.size() - 1]->children[0]->type);
						}
					}
					else {
						try {
							std::cout << CurrentLex() << " :---: ";
							std::cout << "This ID is not declared \n";
							throw std::runtime_error("This ID is not declared");
							break;
						}
						catch (...) {
							error();
							exit(-1);

						}
					}
					break;
				}
			}
		}
		else if (lexStream[lexIndex + 1].lexClass == "+" || lexStream[lexIndex + 1].lexClass == "-"
			|| lexStream[lexIndex + 1].lexClass == "*")
		{
			for (int i = 0; i < reprIdTable.size();i++) {
				if (CurrentLex() == reprIdTable[i]) {
					if (inn[i] != 1) {
						try {
							std::cout << CurrentLex() << " :---: ";
							std::cout << "This ID is not declared \n";
							throw std::runtime_error("This ID is not declared ");
							break;
						}
						catch (...) {
							error();
							exit(-1);
						}
					}
					children.push_back(analysisIdentif());
					typesExpression.push_back(children[children.size() - 1]->children[0]->type);
					if (IsMatch({ "+" , "-", "*" })) {
						children.push_back(Match({ "+" , "-", "*" }));
						children.push_back(analysisExpression());
						typesExpression.push_back(children[children.size() - 1]->children[0]->type);
					}
				}
			}
		}
		else {
			for (int i = 0; i < reprIdTable.size();i++) {
				if (CurrentLex() == reprIdTable[i]) {
					if (inn[i] != 1) {
						try {
							std::cout << "This ID is not declared \n";
							throw std::runtime_error("This ID is not declared ");
							break;
						}
						catch (...) {
							error();
							exit(-1);
						}
					}
					children.push_back(analysisIdentif());
					typesExpression.push_back(children[children.size() - 1]->children[0]->type);
				}
			}
		}
	}
	else if (IsMatch(constTable)) {
		children.push_back(analysisConst());
		typesExpression.push_back(children[children.size() - 1]->children[0]->type);
	}
	try
	{
		if (IsSame(typesExpression)) {
			children[0]->type = typesExpression[0];
			if (!flag1) {
				typesExpression.clear();
			}
		}
		else {
			std::cout << " Expression error - Types don't match \n";
			throw std::runtime_error(" Expression error - Types don't match ");
		}
	}
	catch (...) {
		error();
		exit(-1);
	}
	return std::make_shared<Node>("--Expression--", children);
}

std::shared_ptr<Node> syntaxAnalyzer::analysisCallF() {
	int arg1 = 0;
	std::vector<std::shared_ptr<Node>> children;
	children.push_back(analysisIdFunc());
	children[0]->type = children[children.size() - 1]->children[0]->type;
	children.push_back(Match("("));
	if (IsMatch(reprIdTable)) {
		children.push_back(analysisIdentif());
		try {
			if (children[children.size() - 1]->children[0]->type != idType[idType.size() - 1]) {
				std::cout << CurrentLex() << " :---: ";
				std::cout << " CallFunction error - Types don't match \n";
				throw std::runtime_error(" CallFunction error - Types don't match ");
			}
		}
		catch (...) {
			error();
			exit(-1);
		}
	}
	if (arg1 != countArg && IsMatch(")")) {
		try {
			std::cout << CurrentLex() << " :---: ";
			std::cout << "Unmatch count of arguments in callFunction\n";
			throw std::runtime_error("Unmatch count of arguments in callFunction");
		}
		catch (...) {
			error();
			exit(-1);
		}
	}
	children.push_back(Match(")"));
	return std::make_shared<Node>("-CallFunction-", children);
}
std::shared_ptr<Node> syntaxAnalyzer::analysisLoop()
{
	std::vector<std::shared_ptr<Node>> children;
	inWhile++;
	children.push_back(Match("WHILE"));
	children.push_back(analysisCondition());
	children.push_back(Match("DO"));
	children.push_back(analysisStatements());
	children.push_back(Match("ENDWHILE"));
	children.push_back(Match(";"));
	inWhile--;

	return std::make_shared<Node>("-----Loop-----", children);
}

std::shared_ptr<Node> syntaxAnalyzer::analysisConst()
{
	std::vector<std::shared_ptr<Node>> children;
	if (IsMatch(constTable)) {
		children.push_back(std::make_shared<Node>(constTable[lexStream[lexIndex].value]));
		children[0]->type = constType[lexStream[lexIndex].value];
		NextLexema();
	}
	else {
		try {
			std::cout << CurrentLex() << " :---: ";
			std::cout << "This is not CONST \n";
			throw std::runtime_error("This is not CONST");
		}
		catch (...) {
			error();
			exit(-1);
		}
	}
	return std::make_shared<Node>("-----CONST----", children);
}

std::shared_ptr<Node> syntaxAnalyzer::analysisIdentif()
{
	std::vector<std::shared_ptr<Node>> children;
	if ((str == 1) && !(idType[lexStream[lexIndex].value] == "string")) {
		try {
			std::cout << CurrentLex() << " :---: ";
			std::cout << "This is not string type \n";
			throw std::runtime_error("This is not string type");
		}
		catch (...) {
			error();
			exit(-1);
		}
	}
	children.push_back(std::make_shared<Node>(reprIdTable[lexStream[lexIndex].value]));
	children[0]->type = idType[lexStream[lexIndex].value]; //.push_back(std::make_shared<Node>(reprIdTable[lexStream[lexIndex].value]));
	NextLexema();
	return std::make_shared<Node>("------ID------", children);
}

std::shared_ptr<Node> syntaxAnalyzer::analysisIdFunc() {
	std::vector<std::shared_ptr<Node>> children;
	children.push_back(std::make_shared<Node>(reprIdTable[lexStream[lexIndex].value]));
	children[0]->type = idType[lexStream[lexIndex].value];
	NextLexema();
	return std::make_shared<Node>("----IdFunc----", children);
}

std::shared_ptr<Node> syntaxAnalyzer::analysisLen() {
	std::vector<std::shared_ptr<Node>> children;
	children.push_back(Match("len"));
	children.push_back(Match("("));
	str = 1;
	children.push_back(analysisIdentif());
	str = 0;
	children.push_back(Match(")"));
	children[0]->type = "integer";
	return std::make_shared<Node>("-----Len------", children);
}


std::string syntaxAnalyzer::CurrentLex() {
	return lexStream[lexIndex].lexClass;
}

std::shared_ptr<Node> syntaxAnalyzer::Match(const std::string& expectedLex)
{
	if (lexIndex < lexStream.size() && CurrentLex() == expectedLex) {
		std::shared_ptr<Node> node = std::make_shared<Node>(CurrentLex());
		NextLexema();
		return node;
	}
	else {
		try {
			std::cout << "Expected lexema: " + expectedLex + "  Current lexema: " + CurrentLex() << '\n';
			throw std::runtime_error("Unexpected lexema: " + CurrentLex());
		}
		catch (...) {
			error();
			exit(-1);
		}
	}
}

std::shared_ptr<Node> syntaxAnalyzer::Match(const std::vector<std::string>& expectedLexemes)
{
	try {
		if (lexIndex >= lexStream.size()) {
			std::cout << "No more lexema available.\n";
			throw std::runtime_error("No more lexema available.");
		}
	}
	catch (...) {
		error();
		exit(-1);
	}

	for (int i = 0; i < expectedLexemes.size(); i++) {
		if (CurrentLex() == expectedLexemes[i]) {
			std::shared_ptr<Node> node = std::make_shared<Node>(CurrentLex());
			NextLexema();
			return node;
		}
	}

	return nullptr;
}

bool syntaxAnalyzer::IsMatch(const std::vector<std::string>& expectedLexemes)
{
	for (int i = 0; i < expectedLexemes.size(); i++) {
		if (CurrentLex() == expectedLexemes[i]) {
			return true;
		}
	}
	return false;
}

bool syntaxAnalyzer::IsMatch(const std::string& expectedLex)
{
	return CurrentLex() == expectedLex;
}

void syntaxAnalyzer::NextLexema()
{
	try {
		if (lexIndex < lexStream.size()) {
			lexIndex++;
		}
		else if (lexStream[lexIndex - 1].vid != 3 && lexStream[lexIndex - 1].value != 9) {
			std::cout << "No more lexema available.\n";
			throw std::runtime_error("No more lexema available.");
		}
	}
	catch (...) {
		error();
		exit(-1);
	}
}

bool syntaxAnalyzer::IsSame(const std::vector<std::string>& list) {
	for (int i = 0; i < list.size(); i++) {
		if (list[0] != list[i]) {
			return false;
		}
	}
	return true;
}

void syntaxAnalyzer::error() {
	std::cout << "Error" << endl;
}

void syntaxAnalyzer::PrintTable()
{
	std::cout << '\n' << '#' << "\t" << "Name of Identifier" << "\t" << "Type" << std::endl;
	for (int i = 0; i < reprIdTable.size()-2 ; i++) {
		std::cout << i + 1 << "\t" << std::setw(20) << std::left << reprIdTable[i] << "\t" << idType[i] << std::endl;
	}

	std::cout << '\n' << '\n' << '#' << "\t" << "Name of IdentiFunc" << "\t" << "Type" << std::endl;
	std::cout << 1 << "\t" << std::setw(20) << std::left << reprIdTable[reprIdTable.size()-2]
		<< "\t" << idType[idType.size() - 2] << std::endl;

	std::cout << '\n' << '\n' << '#' << "\t" << "Name of IdentiArgFunc" << "\t" << "Type" << std::endl;
	std::cout << 1 << "\t" << std::setw(20) << std::left << reprIdTable[reprIdTable.size() - 1]
		<< "\t" << idType[idType.size() - 1] << std::endl;
}