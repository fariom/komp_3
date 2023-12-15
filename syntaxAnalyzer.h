#pragma once
#include "Support.h"
/*
S - PROGRAMM ; <declarations> <statemnets> END .
declaration - <type> <id> ;
statement - <assigment> | <loop> | <function> | <ifelse> | EXITWHILE ;
assigment - <identifiers> = <expression> ;
loop - WHILE <condition> DO <statements> ENDWHILE ;
function -  FUNC <identifiers> ( <declaration> ) ; <statements> return ( <expression> ) ENDF ;
function call - <identifier> ( <identifier> )
ifelse - if <condition> then <statements> <else_block> endif ;
else_block - else <statements> | ε
condition - <value> | <comparison>
expression - <id> | <comparison> | <function_call> | <id> - <value>
value - <integer> | <string>
integer - [1-9] digit*
string - " [ character | digit ]* "
comparison - <id> <operator> <value> ;
operator - == | > | <
id - character [ character | digit ]*
character - a-z | A-Z | _
digit - 0-9
*/


class syntaxAnalyzer {
public:
	syntaxAnalyzer(std::vector<Lexema> lexStream) : lexStream(lexStream), lexIndex(0),
		ParseTree(nullptr), countArg(0), inWhile(0), str(0) {}

	syntaxAnalyzer(std::vector<Lexema> lexStream, std::vector<std::string> reprIdTable,
		std::vector<std::string> constTable, std::vector<std::string> lexemClass,
		std::vector<std::string> idType, std::vector<std::string> constType, std::vector<int> inn)
		: lexStream(lexStream), lexIndex(0), ParseTree(nullptr), reprIdTable(reprIdTable),
		constTable(constTable), lexemClass(lexemClass), inn(inn), countArg(0), inWhile(0), str(0),
		idType(idType), constType(constType) {}

	void analysis();
	std::string CurrentLex();
	std::shared_ptr<Node> GetParseTree() { return ParseTree; }
	std::vector<std::string> GetidType() { return std::move(idType); }
	std::vector<std::string> GetidTable() { return std::move(IdTable); }
	std::vector<std::string> GetidFuncTable() { return std::move(IdFuncTable); }
	int GetInWhile() { return inWhile; }

	std::shared_ptr<Node> analysisS();
	std::shared_ptr<Node> analysisFunction();
	std::shared_ptr<Node> analysisReturn();
	std::shared_ptr<Node> analysisDeclarations();
	std::shared_ptr<Node> analysisDeclaration();
	std::shared_ptr<Node> analysisStatements();
	std::shared_ptr<Node> analysisStatement();
	std::shared_ptr<Node> analysisIfElse();
	std::shared_ptr<Node> analysisElseBlock();
	std::shared_ptr<Node> analysisCondition();
	std::shared_ptr<Node> analysisAssignment();
	std::shared_ptr<Node> analysisExpression();
	std::shared_ptr<Node> analysisCallF();
	std::shared_ptr<Node> analysisLoop();
	std::shared_ptr<Node> analysisConst();
	std::shared_ptr<Node> analysisIdentif();
	std::shared_ptr<Node> analysisIdFunc();
	std::shared_ptr<Node> analysisLen();


	void error();
	std::shared_ptr<Node> Match(const std::string& expectedLex);
	std::shared_ptr<Node> Match(const std::vector<std::string>& expectedLexemes);
	bool IsMatch(const std::vector<std::string>& expectedLexemes);
	bool IsMatch(const std::string& expectedLex);
	void NextLexema();
	bool IsSame(const std::vector<std::string>& list);
	void PrintTable();
private:
	std::vector<std::string> typesAssignment;
	std::vector<std::string> typesExpression;
	std::vector<std::string> typesCondition;

	std::vector<Lexema> lexStream;
	std::vector<std::string> reprIdTable;
	std::vector<std::string> IdFuncTable;
	std::vector<std::string> IdTable;
	std::vector<std::string> constTable;
	std::vector<std::string> lexemClass;
	std::vector<int> inn;
	int inWhile;
	bool str;

	int countArg;

	size_t lexIndex;
	std::shared_ptr<Node> ParseTree;
	std::vector<std::string> idType;
	std::vector<std::string> constType;
};