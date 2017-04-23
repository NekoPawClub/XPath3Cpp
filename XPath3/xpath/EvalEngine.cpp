#include "stdafx.h"
#include "evalengine.h"
#include <string>
#include <locale>
#include <iostream>

CXPathSequence* DoXPath(CXPathSequence *pInput, CText sXPath)
{
	CXPathSequence *pResult = new CXPathSequence();
	CXPathStaticEnv *SE = new CXPathStaticEnv();
	CXPathDynamicEnv *DE = new CXPathDynamicEnv(SE, pInput, pResult);

	CEvalEngine *EE = new CEvalEngine(SE, DE, pInput, pResult);
	string sExpr = string(sXPath);
	EE->ParseExpression(sExpr);

	pResult = EE->Evaluate(EE->ExprTree, pInput);

	//cout << "Result: " << pResult->ToString() << endl;
	//cout << "SIZE: " << pResult->GetSize() << endl << endl ;



	//	DE->Test[CXPathQName("mynode","ns")]=new CXPathVar();

	//	map<string,XPathFuncSigs*>::iterator seit = SE->Functions.find("root");
	//cout << "FUNCTION TEST: " << seit->second->Signatures.back()->Arity.back()->LocalName << endl;

	//test output
	//cout << endl << "Expression: " << sExpr << endl;
	//vector<XPathTokenEntry*>::iterator it = EE->Pass3Expr.begin();
	//for (it; it != EE->Pass3Expr.end(); it++) {
	//	cout << (*it)->Token->first << " ";
	//}
	//cout << endl;


	//cout << endl << "Expression Tree" << endl << "========================" << endl;
	//EE->ExprTree->TestTraverse();

	return pResult;
}

bool DoXPath(CXPathSequence *pInput, CText sXPath, CXPathSequence *pResult){
	CXPathStaticEnv *SE = new CXPathStaticEnv();
	CXPathDynamicEnv *DE = new CXPathDynamicEnv(SE, pInput, pResult);

	CEvalEngine *EE = new CEvalEngine(SE, DE, pInput, pResult);
	string sExpr = string(sXPath);
	EE->ParseExpression(sExpr);

	pResult = EE->Evaluate(EE->ExprTree,pInput);

//cout << "Result: " << pResult->ToString() << endl;
//cout << "SIZE: " << pResult->GetSize() << endl << endl ;



//	DE->Test[CXPathQName("mynode","ns")]=new CXPathVar();

//	map<string,XPathFuncSigs*>::iterator seit = SE->Functions.find("root");
	//cout << "FUNCTION TEST: " << seit->second->Signatures.back()->Arity.back()->LocalName << endl;

	//test output
	cout << endl << "Expression: " << sExpr << endl;
	vector<XPathTokenEntry*>::iterator it = EE->Pass3Expr.begin();
	for(it;it!=EE->Pass3Expr.end();it++){
		cout << (*it)->Token->first << " ";
	}
	cout << endl;


cout << endl << "Expression Tree" << endl << "========================" << endl;
EE->ExprTree->TestTraverse();	

	return true;
}

CEvalEngine::CEvalEngine(){
	this->Init();
}

CEvalEngine::CEvalEngine(CXPathStaticEnv *statEnv, 
						 CXPathDynamicEnv *dynEnv, 
						 CXPathSequence *input, 
						 CXPathSequence *result){
	this->StatEnv = statEnv; 
	this->DynamicEnv = dynEnv;
	this->Input = input;
	this->Result = result;
	this->Init();
}

CEvalEngine::~CEvalEngine(void)
{
}

void CEvalEngine::ParseExpression(string sExpr){
	//first pass, convert expressionn string to an
	//array of basic lexemes. Minimal error checking.
	//SCAN
	this->BuildBasicLexeme(sExpr);
	this->NormaliseWhiteSpace(); //second pass ...at the moment this actually deletes ws
	this->Tokenize(); //third pass
	//PARSE
	this->BuildExprTree();
}

void CEvalEngine::Init(void){

	//local info, mostly for interpretation of literals
	locale currentLocal;
	if(currentLocal.name()!="C") ;  //throw error.
	InitDictionaries();
//	if(this->DynamicEnv->Variables["fs:dot"]->Value.itemVal->IsNode()){
//		CXPathNode *n = dynamic_cast<CXPathNode*> (this->DynamicEnv->Variables["fs:dot"]->Value.itemVal);
		//cout << "ATTRIBUTE TEST: " << LPSTR(n->GetChildAt(0)->GetAttributeAt(0)->GetStringValue()) << endl << endl;
//	}
}

void CEvalEngine::InitTokens(void){
	//reserve 0 for NCNames
	this->Tokens[" "] = 1;
	this->Tokens["\t"] = 2;
	this->Tokens["\n"] = 3;
	this->Tokens["."] = 4;
	this->Tokens[","] = 5;
	this->Tokens[":"] = 6;
	this->Tokens["("] = 7;
	this->Tokens[")"] = 8;
	this->Tokens["{"] = 9;
	this->Tokens["}"] = 10;
	this->Tokens["["] = 11;
	this->Tokens["]"] = 12;
	this->Tokens["/"] = 13;
	this->Tokens["@"] = 14;
	this->Tokens["-"] = 15;
	this->Tokens["+"] = 16;
	this->Tokens["*"] = 17;
	this->Tokens["="] = 18;
	this->Tokens["<"] = 19;
	this->Tokens[">"] = 20;
	this->Tokens["?"] = 21;
	this->Tokens["$"] = 22;

	this->Tokens["child"] = 23;
	this->Tokens["descendant"] = 24;
	this->Tokens["parent"] = 25;
	this->Tokens["attribute"] = 26;
	this->Tokens["self"] = 27;
	this->Tokens["ancestor"] = 28;
	this->Tokens["ancestor-or-self"] = 29;
	this->Tokens["preceding"] = 30;
	this->Tokens["preceding-sibling"] = 31;
	this->Tokens["descendant-or-self"] = 32;
	this->Tokens["following"] = 33;
	this->Tokens["following-sibling"] = 34;
	this->Tokens["namespace"] = 35;

	
	this->Tokens["text"] = 36;
	this->Tokens["comment"] = 37;
	this->Tokens["node"] = 38;
	this->Tokens["processing-instruction"] = 39;
	
	this->Tokens[";"] = 40;

	this->Tokens["if"] = 41;
	this->Tokens["typeswitch"] = 42;
	this->Tokens["item"] = 43;
	this->Tokens["element"] = 44;
	this->Tokens["id"] = 45;
	this->Tokens["key"] = 46;


}

void CEvalEngine::InitLexemePatterns(void){
	map<string,CExprToken*>::iterator it;

//debug...use to test that the patterns have been added
//if(it!= this->DictQName.end()) cout << "...OK..."<< it->first;
//else cout << "...BAD..."<< it->first;

	// "StringLiteral"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(-1);
	it = this->DictDefault.find("StringLiteral");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictOperator.find("StringLiteral");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "IntegerLiteral"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(-2);
	it = this->DictDefault.find("IntegerLiteral");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictOperator.find("IntegerLiteral");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "DecimalLiteral"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(-3);
	it = this->DictDefault.find("DecimalLiteral");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictOperator.find("DecimalLiteral");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "DoubleLiteral"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(-4);
	it = this->DictDefault.find("DoubleLiteral");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictOperator.find("DoubleLiteral");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "/"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(13);
	it = this->DictDefault.find("/");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("/");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictOperator.find("/");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "//"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(13);
	this->LexemePatterns.back()->Pattern.push_back(13);
	it = this->DictDefault.find("//");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("//");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictOperator.find("//");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "@"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(14);
	it = this->DictDefault.find("attribute::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("attribute::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "["
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(11);
	it = this->DictDefault.find("[");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictOperator.find("[");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "]"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(12);
	it = this->DictDefault.find("]");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictOperator.find("]");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "NCName" = QName LocalPart
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(0);
	it = this->DictQName.find("LocalPart");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictDefault.find("LocalPart");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictVarName.find("LocalPart");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "NCName:NCName" = QName
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(0);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(0);
	it = this->DictDefault.find("NCName:NCName");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("NCName:NCName");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictVarName.find("NCName:NCName");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "*:NCName" = Wildcard Namespace Prefix QName
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(17);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(0);
	it = this->DictDefault.find("*:NCName");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("*:NCName");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "NCName:*" = Wildcard Local Name for given namespace prefix
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(0);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(17);
	it = this->DictQName.find("NCName:*");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "*" = QName wildcard ...everything
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(17);
	it = this->DictDefault.find("*");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("*");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	// as multiplication operator
	it = this->DictOperator.find("*");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "child::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(23);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("child::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("child::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "self::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(27);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("self::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("self::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "descendant::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(24);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("descendant::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("descendant::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "parent::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(25);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("parent::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("parent::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "attribute::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(26);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("attribute::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("attribute::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "ancestor::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(28);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("ancestor::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("ancestor::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "ancestor-or-self::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(29);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("ancestor-or-self::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("ancestor-or-self::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "preceding::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(30);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("preceding::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("preceding::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "preceding-sibling::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(31);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("preceding-sibling::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("preceding-sibling::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "descendant-or-self::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(32);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("descendant-or-self::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("descendant-or-self::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "following::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(33);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("following::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("following::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "following-sibling::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(34);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("following-sibling::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("following-sibling::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "namespace::"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(35);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(6);
	it = this->DictDefault.find("namespace::");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("namespace::");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// text()
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(36);
	this->LexemePatterns.back()->Pattern.push_back(7);
	this->LexemePatterns.back()->Pattern.push_back(8);
	it = this->DictDefault.find("text()");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("text()");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// comment()
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(37);
	this->LexemePatterns.back()->Pattern.push_back(7);
	this->LexemePatterns.back()->Pattern.push_back(8);
	it = this->DictDefault.find("comment()");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("comment()");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// node()
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(38);
	this->LexemePatterns.back()->Pattern.push_back(7);
	this->LexemePatterns.back()->Pattern.push_back(8);
	it = this->DictDefault.find("node()");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("node()");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// processing-instruction()
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(39);
	this->LexemePatterns.back()->Pattern.push_back(7);
	this->LexemePatterns.back()->Pattern.push_back(8);
	it = this->DictDefault.find("processing-instruction()");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("processing-instruction()");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// variable prefix
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(22);
	it = this->DictDefault.find("$");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("$");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictOperator.find("$");
	this->LexemePatterns.back()->TokenMap.push_back(it);
//following not implemented
//	it = this->DictItemType.find("$");
//	this->LexemePatterns.back()->TokenMap.push_back(it);

	//functions
	// "NCName(" = QName LocalPart (
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(0);
	this->LexemePatterns.back()->Pattern.push_back(7);
	it = this->DictDefault.find("LocalPart(");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "NCName:NCName(" = QName
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(0);
	this->LexemePatterns.back()->Pattern.push_back(6);
	this->LexemePatterns.back()->Pattern.push_back(0);
	this->LexemePatterns.back()->Pattern.push_back(7);
	it = this->DictDefault.find("NCName:NCName(");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// ","
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(5);
	it = this->DictDefault.find(",");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find(",");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictOperator.find(",");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// "("
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(7);
	it = this->DictDefault.find("(");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find("(");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictOperator.find("(");
	this->LexemePatterns.back()->TokenMap.push_back(it);

	// ")"
	this->LexemePatterns.push_back(new XPathLexemePattern());
	this->LexemePatterns.back()->Pattern.push_back(8);
	it = this->DictDefault.find(")");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictQName.find(")");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictOperator.find(")");
	this->LexemePatterns.back()->TokenMap.push_back(it);
	it = this->DictItemType.find(")");
	this->LexemePatterns.back()->TokenMap.push_back(it);

}

void CEvalEngine::InitDictionaries(void){
	InitDefaultDictionary();
	InitOperatorDictionary();
	InitQNameDictionary();
	InitItemTypeDictionary();
	InitVarNameDictionary();

	InitTokens();
	InitLexemePatterns();
}

void CEvalEngine::InitDefaultDictionary(void) {

	this->DictDefault["("] = new CExprToken(XXXTEMPXXX,LEFTPAREN,DEFAULT_STATE,DEFAULT_STATE,0);
	this->DictDefault[")"] = new CExprToken(XXXTEMPXXX,RIGHTPAREN,DEFAULT_STATE,OPERATOR_STATE,0);

	// "StringLiteral" 
	this->DictDefault["StringLiteral"] = new CExprToken(XXXTEMPXXX,STRING_LITERAL,DEFAULT_STATE,OPERATOR_STATE,0);
	this->DictDefault["IntegerLiteral"] = new CExprToken(XXXTEMPXXX,INTEGER_LITERAL,DEFAULT_STATE,OPERATOR_STATE,0);
	this->DictDefault["DecimalLiteral"] = new CExprToken(XXXTEMPXXX,DECIMAL_LITERAL,DEFAULT_STATE,OPERATOR_STATE,0);
	this->DictDefault["DoubleLiteral"] = new CExprToken(XXXTEMPXXX,DOUBLE_LITERAL,DEFAULT_STATE,OPERATOR_STATE,0);

	//steps
	this->DictDefault["/"] = new CExprToken(XXXTEMPXXX,FORWARDSLASH,DEFAULT_STATE,QNAME_STATE,0);
	this->DictDefault["//"] = new CExprToken(XXXTEMPXXX,DOUBLE_FORWARDSLASH,DEFAULT_STATE,QNAME_STATE,0);

	// axis
	this->DictDefault["child::"] = new CExprToken(XXXTEMPXXX,CHILD_AXIS,DEFAULT_STATE, QNAME_STATE,6);
	this->DictDefault["descendant::"] = new CExprToken(XXXTEMPXXX,DESCENDANT_AXIS,DEFAULT_STATE, QNAME_STATE,6);
	this->DictDefault["parent::"] = new CExprToken(XXXTEMPXXX,PARENT_AXIS,DEFAULT_STATE, QNAME_STATE,6);
	this->DictDefault["attribute::"] = new CExprToken(XXXTEMPXXX,ATTRIBUTE_AXIS,DEFAULT_STATE, QNAME_STATE,6);
	this->DictDefault["self::"] = new CExprToken(XXXTEMPXXX,SELF_AXIS,DEFAULT_STATE, QNAME_STATE,6);
	this->DictDefault["descendant-or-self::"] = new CExprToken(XXXTEMPXXX,DESCENDANT_OR_SELF_AXIS,DEFAULT_STATE, QNAME_STATE,6);
	this->DictDefault["ancestor::"] = new CExprToken(XXXTEMPXXX,ANCESTOR_AXIS,DEFAULT_STATE, QNAME_STATE,6);
	this->DictDefault["ancestor-or-self::"] = new CExprToken(XXXTEMPXXX,ANCESTOR_OR_SELF_AXIS,DEFAULT_STATE, QNAME_STATE,6);
	this->DictDefault["following-sibling::"] = new CExprToken(XXXTEMPXXX,FOLLOWING_SIBLING_AXIS,DEFAULT_STATE, QNAME_STATE,6);
	this->DictDefault["following::"] = new CExprToken(XXXTEMPXXX,FOLLOWING_AXIS,DEFAULT_STATE, QNAME_STATE,6);
	this->DictDefault["preceding::"] = new CExprToken(XXXTEMPXXX,PRECEDING_AXIS,DEFAULT_STATE, QNAME_STATE,6);
	this->DictDefault["preceding-sibling::"] = new CExprToken(XXXTEMPXXX,PRECEDING_SIBLING_AXIS,DEFAULT_STATE,QNAME_STATE,6);
	this->DictDefault["namespace::"] = new CExprToken(XXXTEMPXXX,NAMESPACE_AXIS,DEFAULT_STATE, QNAME_STATE,6);

	//names
	this->DictDefault["*"] = new CExprToken(XXXTEMPXXX,WILDCARD,DEFAULT_STATE,OPERATOR_STATE,0);
	this->DictDefault["NCName:*"] =	new CExprToken(XXXTEMPXXX,LOCALNAME_WILDCARD,DEFAULT_STATE,OPERATOR_STATE,0);
	this->DictDefault["*:NCName"] =	new CExprToken(XXXTEMPXXX,NAMESPACE_WILDCARD,DEFAULT_STATE,OPERATOR_STATE,0);
	this->DictDefault["LocalPart"] =	new CExprToken(XXXTEMPXXX,LOCALNAME,DEFAULT_STATE,OPERATOR_STATE,0);
	this->DictDefault["NCName:NCName"] = new CExprToken(XXXTEMPXXX,QNAME,DEFAULT_STATE,OPERATOR_STATE,0);

	this->DictDefault["LocalPart("] =	new CExprToken(XXXTEMPXXX,LOCALNAME_CALL,DEFAULT_STATE,DEFAULT_STATE,6);
	this->DictDefault["NCName:NCName("] = new CExprToken(XXXTEMPXXX,QNAME_CALL,DEFAULT_STATE,DEFAULT_STATE,6);

	//node tests
	this->DictDefault["text()"] = new CExprToken(XXXTEMPXXX,TEXT_NODE_TEST,DEFAULT_STATE,DEFAULT_STATE,0);
	this->DictDefault["comment()"] =	new CExprToken(XXXTEMPXXX,COMMENT_NODE_TEST,DEFAULT_STATE,DEFAULT_STATE,0);
	this->DictDefault["node()"] =	new CExprToken(XXXTEMPXXX,ANY_NODE_TEST,DEFAULT_STATE,DEFAULT_STATE,0);
	this->DictDefault["processing-instruction()"] =	new CExprToken(XXXTEMPXXX,PI_NODE_TEST,DEFAULT_STATE,DEFAULT_STATE,0);

	//variable prefix
	this->DictDefault["$"] = new CExprToken(XXXTEMPXXX,VARIABLE_MARKER,DEFAULT_STATE,VARNAME_STATE,0);

	//"," comma delimiter
	this->DictDefault[","] = new CExprToken(XXXTEMPXXX,COMMA,DEFAULT_STATE,DEFAULT_STATE,0);

	//"[" "]" predicates
	this->DictDefault["["] = new CExprToken(XXXTEMPXXX,OPEN_BRACKET,DEFAULT_STATE,DEFAULT_STATE,0);
	this->DictDefault["]"] = new CExprToken(XXXTEMPXXX,CLOSE_BRACKET,DEFAULT_STATE,OPERATOR_STATE,0);

}

void CEvalEngine::InitOperatorDictionary(void){

	this->DictOperator["("] = new CExprToken(XXXTEMPXXX,LEFTPAREN,OPERATOR_STATE,DEFAULT_STATE,0);
	this->DictOperator[")"] = new CExprToken(XXXTEMPXXX,RIGHTPAREN,OPERATOR_STATE,OPERATOR_STATE,0);

	this->DictOperator["/"] = new CExprToken(XXXTEMPXXX,FORWARDSLASH,OPERATOR_STATE,DEFAULT_STATE,6);
	this->DictOperator["//"] = new CExprToken(XXXTEMPXXX,DOUBLE_FORWARDSLASH,OPERATOR_STATE,DEFAULT_STATE,6);

	// "StringLiteral" 
	this->DictOperator["StringLiteral"] = new CExprToken(XXXTEMPXXX,STRING_LITERAL,OPERATOR_STATE,OPERATOR_STATE,0);
	this->DictOperator["IntegerLiteral"] = new CExprToken(XXXTEMPXXX,INTEGER_LITERAL,OPERATOR_STATE,OPERATOR_STATE,0);
	this->DictOperator["DecimalLiteral"] = new CExprToken(XXXTEMPXXX,DECIMAL_LITERAL,OPERATOR_STATE,OPERATOR_STATE,0);
	this->DictOperator["DoubleLiteral"] = new CExprToken(XXXTEMPXXX,DOUBLE_LITERAL,OPERATOR_STATE,OPERATOR_STATE,0);

	this->DictOperator["$"] = new CExprToken(XXXTEMPXXX,VARIABLE_MARKER,OPERATOR_STATE,VARNAME_STATE,0);

	this->DictOperator["*"] = new CExprToken(XXXTEMPXXX,MULTIPLY,OPERATOR_STATE, DEFAULT_STATE,5);

	//"," comma delimiter
	this->DictOperator[","] = new CExprToken(XXXTEMPXXX,COMMA,OPERATOR_STATE,DEFAULT_STATE,0);

	//"[" "]" predicates
	this->DictOperator["["] = new CExprToken(XXXTEMPXXX,OPEN_BRACKET,OPERATOR_STATE,DEFAULT_STATE,0);
	this->DictOperator["]"] = new CExprToken(XXXTEMPXXX,CLOSE_BRACKET,OPERATOR_STATE,OPERATOR_STATE,0);

	//"=" equals
	this->DictOperator["="] = new CExprToken(XXXTEMPXXX,EQUALS,OPERATOR_STATE,DEFAULT_STATE,0);

}

void CEvalEngine::InitQNameDictionary(void){

	this->DictQName["("] = new CExprToken(XXXTEMPXXX,LEFTPAREN,QNAME_STATE,DEFAULT_STATE,0);
	this->DictQName[")"] = new CExprToken(XXXTEMPXXX,RIGHTPAREN,QNAME_STATE,OPERATOR_STATE,0);

	//name tests
	this->DictQName["*"] =	new CExprToken(XXXTEMPXXX,WILDCARD,QNAME_STATE,OPERATOR_STATE,0);
	this->DictQName["NCName:*"] = new CExprToken(XXXTEMPXXX,LOCALNAME_WILDCARD,QNAME_STATE,OPERATOR_STATE,0);
	this->DictQName["*:NCName"] = new CExprToken(XXXTEMPXXX,NAMESPACE_WILDCARD,QNAME_STATE,OPERATOR_STATE,0);
	this->DictQName["LocalPart"] = new CExprToken(XXXTEMPXXX,LOCALNAME,QNAME_STATE,OPERATOR_STATE,0);
	this->DictQName["NCName:NCName"] = new CExprToken(XXXTEMPXXX,QNAME,QNAME_STATE,OPERATOR_STATE,0);

	//steps
	this->DictQName["/"] = new CExprToken(XXXTEMPXXX,FORWARDSLASH,QNAME_STATE,QNAME_STATE,6);
	this->DictQName["//"] = new CExprToken(XXXTEMPXXX,DOUBLE_FORWARDSLASH,QNAME_STATE,QNAME_STATE,6);

	//axis
	this->DictQName["child::"] = new CExprToken(XXXTEMPXXX,CHILD_AXIS,QNAME_STATE, QNAME_STATE,6);
	this->DictQName["descendant::"] = new CExprToken(XXXTEMPXXX,DESCENDANT_AXIS,QNAME_STATE, QNAME_STATE,6);
	this->DictQName["parent::"] = new CExprToken(XXXTEMPXXX,PARENT_AXIS,QNAME_STATE, QNAME_STATE,6);
	this->DictQName["attribute::"] = new CExprToken(XXXTEMPXXX,ATTRIBUTE_AXIS,QNAME_STATE, QNAME_STATE,6);
	this->DictQName["self::"] = new CExprToken(XXXTEMPXXX,SELF_AXIS,QNAME_STATE, QNAME_STATE,6);
	this->DictQName["descendant-or-self::"] = new CExprToken(XXXTEMPXXX,DESCENDANT_OR_SELF_AXIS,QNAME_STATE, QNAME_STATE,6);
	this->DictQName["ancestor::"]= new CExprToken(XXXTEMPXXX,ANCESTOR_AXIS,QNAME_STATE, QNAME_STATE,6);
	this->DictQName["ancestor-or-self::"]= new CExprToken(XXXTEMPXXX,ANCESTOR_OR_SELF_AXIS,QNAME_STATE, QNAME_STATE,6);
	this->DictQName["following-sibling::"]= new CExprToken(XXXTEMPXXX,FOLLOWING_SIBLING_AXIS,QNAME_STATE, QNAME_STATE,6);
	this->DictQName["following::"]= new CExprToken(XXXTEMPXXX,FOLLOWING_AXIS,QNAME_STATE, QNAME_STATE,6);
	this->DictQName["preceding::"]= new CExprToken(XXXTEMPXXX,PRECEDING_AXIS,QNAME_STATE, QNAME_STATE,6);
	this->DictQName["preceding-sibling::"]= new CExprToken(XXXTEMPXXX,PRECEDING_SIBLING_AXIS,QNAME_STATE, QNAME_STATE,6);
	this->DictQName["namespace::"]= new CExprToken(XXXTEMPXXX,NAMESPACE_AXIS,QNAME_STATE, QNAME_STATE,6);


	//node tests
	this->DictQName["text()"] = new CExprToken(XXXTEMPXXX,TEXT_NODE_TEST,QNAME_STATE,OPERATOR_STATE,0);
	this->DictQName["comment()"] = new CExprToken(XXXTEMPXXX,COMMENT_NODE_TEST,QNAME_STATE,OPERATOR_STATE,0);
	this->DictQName["node()"] = new CExprToken(XXXTEMPXXX,ANY_NODE_TEST,QNAME_STATE,OPERATOR_STATE,0);
	this->DictQName["processing-instruction()"] = new CExprToken(XXXTEMPXXX,PI_NODE_TEST,QNAME_STATE,OPERATOR_STATE,0);

	//var prefix
	this->DictQName["$"] = new CExprToken(XXXTEMPXXX,VARIABLE_MARKER,QNAME_STATE,VARNAME_STATE,0);

	//"," comma delimiter
	this->DictQName[","] = new CExprToken(XXXTEMPXXX,COMMA,QNAME_STATE,DEFAULT_STATE,0);


}

void CEvalEngine::InitItemTypeDictionary(void){

	this->DictItemType[")"] = new CExprToken(XXXTEMPXXX,RIGHTPAREN,ITEMTYPE_STATE,OPERATOR_STATE,0);

}

void CEvalEngine::InitVarNameDictionary(void){
	//rather than create a varname token have included the two acceptable forms of QName
	this->DictVarName["LocalPart"] = new CExprToken(XXXTEMPXXX,LOCALNAME,VARNAME_STATE,OPERATOR_STATE,0);
	this->DictVarName["NCName:NCName"] = new CExprToken(XXXTEMPXXX,QNAME,VARNAME_STATE,OPERATOR_STATE,0);
}

void CEvalEngine::BuildBasicLexeme(string sExpr){
	string ct = "";
	for(int count=0; count<(int)sExpr.size();count++){
		ct = sExpr[count];
		map<string,int>::iterator it = this->Tokens.find(ct);
		if(it!=this->Tokens.end() && it->second<=XPATH_NUM_KNOWN_BASIC_TOKENS){ //this is a known symbol
			this->Pass1Expr.push_back(it->second);
		}
		else {
			string wordToken = "";
			if(isdigit(ct[0]))
				wordToken = GetNumber(sExpr,count);
			else if(ct=="\"")
				wordToken = GetStringLiteral(sExpr,count,'"');
			else if(ct=="'")
				wordToken = GetStringLiteral(sExpr,count,'\'');
			else
				wordToken = GetWord(sExpr,count);
			if(wordToken!=""){
				map<string,int>::iterator it = this->Tokens.find(wordToken);
				if(it!=this->Tokens.end()){
					this->Pass1Expr.push_back(it->second); //add to expr list
				}
				else { //create new token
					int newId = (int)this->Tokens.size()+1;
					this->Tokens[wordToken] = newId; //
					this->Pass1Expr.push_back(newId);
				}
			}
		}
	}
}

void CEvalEngine::NormaliseWhiteSpace(){
	vector<int>::iterator it = this->Pass1Expr.begin();
	//discard leading ws
	if(*it==1 || *it==2 || *it==3){			
		while(*it==1 || *it==2 || *it==3){
			it++;
		}
	}
	//collapse contained ws 
//	int countws = 0;
	for(it;it!=this->Pass1Expr.end();it++){
		if(*it==1 || *it==2 || *it==3) {
//			countws++;
// NB: Undecided as to whether to incorporate whitespace at all
//			if(countws==1)	this->Pass2Expr.push_back(1);
		}
		else {
//			countws = 0;
			this->Pass2Expr.push_back(*it);
		}
	}
}

void CEvalEngine::Tokenize(){
	this->State = DEFAULT_STATE;
	map<string,CExprToken*>::iterator it;
	vector<XPathTokenEntry*> pte;
	for(int i=0;i<(int)this->Pass2Expr.size();i++){
		pte.clear();
		pte = GetPatternMatch(i);
		if(!pte.size()) cout << "Error!!!"; //throw error here 
		this->Pass3Expr.insert(this->Pass3Expr.end(),pte.begin(),pte.end());
	}

	vector<XPathTokenEntry*>::iterator itest = this->Pass3Expr.begin();
//cout << "Tokenized expression:" << this->Pass3Expr.size() << endl;
//for(itest;itest!=Pass3Expr.end();itest++){
//	cout << (*itest)->Token->first << endl;
//}
}

XPathTokenEntry* CEvalEngine::GetChildAxisTokenEntry(void){
	map<string,CExprToken*>::iterator it = this->DictQName.find("child::");
	XPathTokenEntry *t = NULL;
	if(it!=this->DictQName.end()){
		t = new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second));
	}
	return t;
}

XPathTokenEntry* CEvalEngine::GetFunctionCallTokenEntry(XPathTokenName name){
	map<string,CExprToken*>::iterator it;
	switch(name){
		case QNAME_CALL:
			it = this->DictDefault.find("NCName:NCName(");
			break;
		case LOCALNAME_CALL:
			it = this->DictDefault.find("LocalPart(");
			break;
	}
	XPathTokenEntry *t = NULL;
	if(it!=this->DictDefault.end()) {
		t = new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second));
	}
	return t;
}
/*
vector<XPathTokenEntry*> CEvalEngine::GetInitialStepTokenEntries(int kind){
	// "/" at start of expression or in default state
	// is abreviation for "fn:root(self::node())"
	// "//" at start of expression or in default state
	// is abreviation for "fn:root(self::node())/decendant-or-self::node()"
	vector<XPathTokenEntry*> vte;
	map<string,CExprToken*>::iterator it;
	it = this->DictDefault.find("NCName:NCName(");
	if(it!=this->DictDefault.end()){
		vte.push_back(new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second)));
		vte.back()->Info.push_back(XPathTokenInfo());
		string *name = new string("fn");
		vte.back()->Info.back().Value.sVal = name;
		vte.back()->Info.back().Type = "string";
		vte.back()->Info.push_back(XPathTokenInfo());
		name = new string("root");
		vte.back()->Info.back().Value.sVal = name;
		vte.back()->Info.back().Type = "string";
	}
	it = this->DictDefault.find("self::");
	if(it!=this->DictDefault.end()){
		vte.push_back(new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second)));
	}			
	it = this->DictQName.find("node()");
	if(it!=this->DictQName.end()){
		vte.push_back(new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second)));
	}
	it = this->DictOperator.find(")");
	if(it!=this->DictOperator.end()){
		vte.push_back(new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second)));
	}	
	it = this->DictOperator.find("/");
	if(it!=this->DictOperator.end()){
		vte.push_back(new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second)));
	}	
	if(kind==2) { //for "//" token
		it = this->DictDefault.find("descendant-or-self::");
		if(it!=this->DictDefault.end()){
			vte.push_back(new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second)));
		}			
		it = this->DictQName.find("node()");
		if(it!=this->DictQName.end()){
			vte.push_back(new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second)));
		}
		it = this->DictOperator.find("/");
		if(it!=this->DictOperator.end()){
			vte.push_back(new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second)));
		}	
	}
	return vte;
}
//DOS as in Descendant or self
vector<XPathTokenEntry*> CEvalEngine::GetDOSTokenEntries(void){
	// "//" in operator state
	// is abreviation for "fn:root(self::node())/decendant-or-self::node()"
	vector<XPathTokenEntry*> vte;
	map<string,CExprToken*>::iterator it;
	it = this->DictOperator.find("/");
	if(it!=this->DictOperator.end()){
		vte.push_back(new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second)));
	}	
	it = this->DictDefault.find("descendant-or-self::");
	if(it!=this->DictDefault.end()){
		vte.push_back(new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second)));
	}			
	it = this->DictQName.find("node()");
	if(it!=this->DictQName.end()){
		vte.push_back(new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second)));
	}
	it = this->DictOperator.find("/");
	if(it!=this->DictOperator.end()){
		vte.push_back(new XPathTokenEntry(new Pattern_Token_Pair((it)->first,(it)->second)));
	}	
	return vte;
}
*/
vector<XPathTokenEntry*> CEvalEngine::GetPatternMatch(int &count){
	vector<XPathTokenEntry*> result;
	int currentPosition = (int)this->Pass3Expr.size();
	int startCount = count;
	int lookahead = ((count+XPATH_MAX_PATTERN_LOOKAHEAD)>=(int)this->Pass2Expr.size())?
					(int)this->Pass2Expr.size()-(count+1):XPATH_MAX_PATTERN_LOOKAHEAD;
	int jumpahead = 0;
	vector<int> originalPattern;
	vector<int> patternBuffer;
	int ct;
	bool match = false;
	for(int i = 0; i<=lookahead; i++){
		ct = this->Pass2Expr[count+i];
		originalPattern.push_back(ct);
		if(ct>XPATH_NUM_KNOWN_BASIC_TOKENS) 
			ct = this->ClassifyUnknownLexeme(ct);		
		patternBuffer.push_back(ct);
	}

	vector<XPathLexemePattern*>::iterator it; 
	do {
		it = this->LexemePatterns.begin();
		for(it;it!=this->LexemePatterns.end();it++){
			if(patternBuffer==(*it)->Pattern){
				match = true;
				jumpahead = lookahead;
			}		
		}
		if(!match) {
			patternBuffer.pop_back();
			lookahead--;
		}
	} while (!match && lookahead>=0);

	if(match) {
		it = this->LexemePatterns.begin();
		bool foundToken;
		for(it;it!=this->LexemePatterns.end();it++){
			if( (patternBuffer==(*it)->Pattern) ){
				foundToken = false; //must not add two tokens for the same pattern
									//this is important in the case where a token sets
									//the state to a value equal to another token
									//in the token list for the pattern.
				for(int i=0;i<(int)(*it)->TokenMap.size();i++){
					if(this->State==(*it)->TokenMap[i]->second->State && !foundToken){
						foundToken = true;
						this->State = (*it)->TokenMap[i]->second->ToState;
						result.push_back(new XPathTokenEntry(new Pattern_Token_Pair((*it)->TokenMap[i]->first,(*it)->TokenMap[i]->second)));
						string tokenName = (*it)->TokenMap[i]->first;
						string *name;
						string precedingToken = (((int) this->Pass3Expr.size() > 0)?this->Pass3Expr.back()->Token->first:"");

						if(tokenName=="LocalPart"){
							if( (precedingToken=="/") || 
								(precedingToken=="//") ||
								(precedingToken=="[") ||
								(precedingToken=="") ||
								((*it)->TokenMap[i]->second->State==DEFAULT_STATE) ){
								//It's ok to insert token here because it will
								//not effect state.
								result.insert(result.begin(),GetChildAxisTokenEntry());
							}
							result.back()->Info.push_back(XPathTokenInfo());
							name = new string(GetLexemeTokenString(originalPattern[0]));
							result.back()->Info.back().Value.sVal = name;
							result.back()->Info.back().Type = "string";
						}
						else if(tokenName=="StringLiteral"){ 
							result.back()->Info.push_back(XPathTokenInfo());
							name = new string(GetLexemeTokenString(originalPattern[0]));
							name->erase(name->begin());
							name->erase(name->end()-1);
							result.back()->Info.back().Value.sVal = name;
							result.back()->Info.back().Type = "string";
						}
						else if(tokenName=="IntegerLiteral"){
							if(precedingToken=="["){
								//abreviation somenode[5] => somenode[position()=5]
								map<string,CExprToken*>::iterator jt;
								jt = this->DictOperator.find("=");
								if(jt!=this->DictOperator.end()){
									result.insert(result.begin(),new XPathTokenEntry(new Pattern_Token_Pair((jt)->first,(jt)->second)));
								}
								jt = this->DictDefault.find(")");
								if(jt!=this->DictDefault.end()){
									result.insert(result.begin(),new XPathTokenEntry(new Pattern_Token_Pair((jt)->first,(jt)->second)));
								}
								XPathTokenEntry * te = GetFunctionCallTokenEntry(QNAME_CALL);
								te->Info.push_back(XPathTokenInfo());
								string *name = new string("fn");
								te->Info.back().Value.sVal = name;
								te->Info.back().Type = "string";
								te->Info.push_back(XPathTokenInfo());
								name = new string("position");
								te->Info.back().Value.sVal = name;
								te->Info.back().Type = "string";
								result.insert(result.begin(),te);
							}
							result.back()->Info.push_back(XPathTokenInfo());
							name = new string(GetLexemeTokenString(originalPattern[0]));
							result.back()->Info.back().Value.iVal = atoi((*name).c_str());
							result.back()->Info.back().Type = "integer";
						}
						else if(tokenName=="DoubleLiteral"){ 
							result.back()->Info.push_back(XPathTokenInfo());
							name = new string(GetLexemeTokenString(originalPattern[0]));
							result.back()->Info.back().Value.dVal = atof((*name).c_str());
							result.back()->Info.back().Type = "double";
						}
						else if(tokenName=="NCName:NCName"){
							if( (precedingToken=="/") || 
								(precedingToken=="//") ||
								(precedingToken=="[") ||
								(precedingToken=="") ){
								result.insert(result.begin(),GetChildAxisTokenEntry());
							}
							result.back()->Info.push_back(XPathTokenInfo());
							name = new string(GetLexemeTokenString(originalPattern[0]));
							result.back()->Info.back().Value.sVal = name;
							result.back()->Info.back().Type = "string";
							result.back()->Info.push_back(XPathTokenInfo());
							name = new string(GetLexemeTokenString(originalPattern[2]));
							result.back()->Info.back().Value.sVal = name;
							result.back()->Info.back().Type = "string";
						}
						else if(tokenName=="*"){
							if( (precedingToken=="/") || 
								(precedingToken=="//") ||
								(precedingToken=="[") ||
								(precedingToken=="") ){
								result.insert(result.begin(),GetChildAxisTokenEntry());
							}
							result.back()->Info.push_back(XPathTokenInfo());
						}
						else if(tokenName=="NCName:*"){
							if( (precedingToken=="/") || 
								(precedingToken=="//") ||
								(precedingToken=="[")||
								(precedingToken=="")){
								result.insert(result.begin(),GetChildAxisTokenEntry());
							}
							result.back()->Info.push_back(XPathTokenInfo());
							name = new string(GetLexemeTokenString(originalPattern[0]));
							result.back()->Info.back().Value.sVal = name;
							result.back()->Info.back().Type = "string";
						}
						else if(tokenName=="*:NCName"){
							if( (precedingToken=="/") || 
								(precedingToken=="//") ||
								(precedingToken=="[")  ||
								(precedingToken=="") ){
								result.insert(result.begin(),GetChildAxisTokenEntry());
							}
							result.back()->Info.push_back(XPathTokenInfo());
							string *name = new string(GetLexemeTokenString(originalPattern[2]));
							result.back()->Info.back().Value.sVal = name;
							result.back()->Info.back().Type = "string";
						}
						else if(tokenName=="NCName:NCName("){
							result.back()->Info.push_back(XPathTokenInfo());
							name = new string(GetLexemeTokenString(originalPattern[0]));
							result.back()->Info.back().Value.sVal = name;
							result.back()->Info.back().Type = "string";
							result.back()->Info.push_back(XPathTokenInfo());
							name = new string(GetLexemeTokenString(originalPattern[2]));
							result.back()->Info.back().Value.sVal = name;
							result.back()->Info.back().Type = "string";
						}
						else if(tokenName=="LocalPart("){
							result.back()->Info.push_back(XPathTokenInfo());
							name = new string(GetLexemeTokenString(originalPattern[0]));
							result.back()->Info.back().Value.sVal = name;
							result.back()->Info.back().Type = "string";
						}
						else if(tokenName=="node()" || 
								tokenName=="text()" || 
								tokenName=="comment()" || 
								tokenName=="processing-instruction()"){
							if( (precedingToken=="/") || 
								(precedingToken=="//") ||
								(precedingToken=="[") ||
								(precedingToken=="") ||
								((*it)->TokenMap[i]->second->State==DEFAULT_STATE) ){
								//It's ok to insert token here because it will
								//not effect state.
								result.insert(result.begin(),GetChildAxisTokenEntry());
							}
						}
					}
				}
			}		
		}
	}
	count = startCount + jumpahead;
	return result;
}

int CEvalEngine::ClassifyUnknownLexeme(int ct){
	int result = 1;
	string ts = this->GetLexemeTokenString(ct);
	if(isalpha(ts[0])) result = 0;
	else if(ts[0]=='"' || ts[0]=='\'') result = -1;
	else if(isdigit(ts[0])) result = -2;
	return result;
}

string CEvalEngine::GetWord(string s, int &c){
	bool endofword = false;
	string result = "";
	result += s[c];
	while(c<s.size() && !endofword){
		if(c!=s.size()-1){
			if(IsKnownSymbol(s,c+1)) {
				if(s[c+1]=='-' || s[c+1]=='.')	result += s[++c];
				else endofword=true;
			}
			else result += s[++c];
		}
		else endofword=true;
	}
	return result;
}

string CEvalEngine::GetNumber(string s, int &c){
	bool endofword = false;
	string result = "";

	int countPeriod = 0;

	result += s[c];
	while(c<s.size() && !endofword){
		if(c!=s.size()-1){
			if(s[c+1]=='.'){
				if(countPeriod==0) {
					result += s[++c];
					countPeriod++;
				}
				else endofword=true; 
			}
			else if(isdigit(s[c+1])) result += s[++c];
			else endofword=true;
		}
		else endofword=true;
	}
	return result;
}

string CEvalEngine::GetStringLiteral(string s, int &c, char delimit){
	bool endofword = false;
	string result = "";

	int countQuote = 1;

	while(c<(int)s.size() && !endofword){
		if(c!=s.size()-1){
			if(s[c+1]!=delimit){
				result += s[++c];
			}
			else if(s[c+2] == delimit){
				result += delimit;
				c += 2;
				countQuote += 2;
			}
			else {
				c++;
				countQuote++;
				endofword=true; 
			}
		}
		else endofword=true;
	}
	if(countQuote%2!=0) ; //throw error unclosed quote.
	return delimit + result + delimit;
}


bool CEvalEngine::IsWhiteSpace(char c){
	return ( (c==' ') || (c=='\t') || (c=='\n') );
}

bool CEvalEngine::IsKnownSymbol(string s, int c){
	string temp = "";
	temp = s[c];
	map<string,int>::iterator it = this->Tokens.find(temp); 
	return (( it!=this->Tokens.end() && it->second<=XPATH_NUM_KNOWN_BASIC_TOKENS)?true:false);
}

string CEvalEngine::GetLexemeTokenString(int id){
	map<string,int>::iterator it = this->Tokens.begin();
	bool found = false;
	string result;
	int count = 0;
	while(!found && count<(int)this->Tokens.size()){
		if(it->second==id) {
			found = true;
			result = it->first;
		}
		it++;
		count++;
	}
	return result;
}

map<string,CExprToken*>* CEvalEngine::GetDictionaryForState(void){
	map<string,CExprToken*> *result;
	switch(this->State){
		case DEFAULT_STATE: result = &this->DictDefault;break;
		default:
			result = nullptr;
			break;
	}
	return result;
}




