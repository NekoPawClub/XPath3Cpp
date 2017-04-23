#pragma once

#include "exprtoken.h"
#include "exprTreeNode.h"

#include "XPathSequence.h"
#include "XPathNode.h"
#include "XPathStaticEnv.h"
#include "XPathDynamicEnv.h"
#include "XPathVar.h"

#include <vector>
#include <map>
#include <string>

using namespace std;

const int XPATH_NUM_KNOWN_BASIC_TOKENS = 46;
const int XPATH_MAX_PATTERN_LOOKAHEAD = 3;


typedef pair <string, int> String_Int_Pair;
/*
typedef pair <string, CExprToken*> Pattern_Token_Pair;
struct XPathTokenInfo {
	union  {
		int iVal;
		double dVal;
		string *sVal;
	} Value;
	string Type;
};

struct XPathTokenEntry{
	XPathTokenEntry(Pattern_Token_Pair *t):
			Token(t){};
	Pattern_Token_Pair *Token;
	//CExprToken *Token;
	vector<XPathTokenInfo> Info;
};
*/
struct XPathLexemePattern{
	vector<int> Pattern;
	vector<map<string,CExprToken*>::iterator> TokenMap;
};

//ENTRY POINT INTO DLL...
bool DoXPath(CXPathSequence *pInput, CText sXPath, CXPathSequence *pResults);
CXPathSequence* DoXPath(CXPathSequence *pInput, CText sXPath);

class CEvalEngine
{
public:
	CEvalEngine();
	CEvalEngine(CXPathStaticEnv *statEnv, 
				CXPathDynamicEnv *dynEnv, 
				CXPathSequence *input, 
				CXPathSequence *result);
	virtual ~CEvalEngine(void);

	CXPathStaticEnv *StatEnv;
	CXPathDynamicEnv *DynamicEnv;
	CXPathSequence *Input; 
	CXPathSequence *Result;

	XPathLexicalState State;

	map<string,int> Tokens;
	vector<XPathLexemePattern*> LexemePatterns;

	vector<int> Pass1Expr;		//tokenize into basic lexemes
	vector<int> Pass2Expr;		//normalise white space
	vector<XPathTokenEntry*> Pass3Expr; //tokens with state
	vector<XPathTokenEntry*>::iterator ItToken;
	//vector<XPathTokenEntry*> ExprStack;

	CExprTreeNode *ExprTree;

	//vector<CExprToken*> Dictionary;
	//Dictionary maps to stroe token relating to various lexical states.
	map<string,CExprToken*> DictDefault;
	map<string,CExprToken*> DictOperator;
	map<string,CExprToken*> DictQName;
	map<string,CExprToken*> DictItemType;
	map<string,CExprToken*> DictVarName;

	void Init(void);

	void InitTokens(void);
	void InitLexemePatterns(void);
	void InitDictionaries(void);

	void InitDefaultDictionary(void);
	void InitOperatorDictionary(void);
	void InitQNameDictionary(void);
	void InitItemTypeDictionary(void);
	void InitVarNameDictionary(void);

	void ParseExpression(string sExpr);
	void BuildBasicLexeme(string sExpr);
	void NormaliseWhiteSpace(void);
	void Tokenize(void);
	void BuildExprTree(void);


	XPathTokenEntry* GetChildAxisTokenEntry(void);
	XPathTokenEntry* GetFunctionCallTokenEntry(XPathTokenName name);
//	vector<XPathTokenEntry*> GetInitialStepTokenEntries(int kind);
//	vector<XPathTokenEntry*> GetDOSTokenEntries(void);

	vector<XPathTokenEntry*> GetPatternMatch(int &count);
	string GetWord(string s, int &c);
	string GetNumber(string s, int &c);
	string GetStringLiteral(string s, int &c, char delimit);
	bool IsWhiteSpace(char c);
	bool IsKnownSymbol(string s, int c);

	int ClassifyUnknownLexeme(int ct);
	string GetLexemeTokenString(int id);

	map<string,CExprToken*>* GetDictionaryForState(void);

	XPathTokenName GetTokenName(void);
	XPathTokenName GetNextToken(void);
	bool MatchToken(XPathTokenName nt);
	bool RecogniseNonTerminal(XPathNonTerminal nt);
	bool IsTokenForwardAxis(XPathTokenName n);
	bool IsTokenReverseAxis(XPathTokenName n);
	bool IsTokenLiteral(XPathTokenName n);

	//Parse
	CExprTreeNode* XPath(void);
	CExprTreeNode* Expr(void);
	CExprTreeNode* PathExpr(void);
	CExprTreeNode* RelativePathExpr(void);
	CExprTreeNode* StepExpr(void);
	CExprTreeNode* ForwardStep(void);
	CExprTreeNode* ForwardAxis(void);
	CExprTreeNode* NodeTest(void);
	CExprTreeNode* NameTest(void);
	CExprTreeNode* KindTest(void);
	CExprTreeNode* PrimaryExpr(void);

	//Interpret
	CXPathSequence* Evaluate(CExprTreeNode *n, CXPathSequence *input);
	CXPathNode* GetRootContextNode(void);
	CXPathSequence* DoNameTest(CXPathSequence* input, XPathTokenName axis, CExprTreeNode *exprTreeNode);
	CXPathSequence* DoNodeTest(CXPathSequence* input, XPathTokenName axis, XPathTokenName kind);
	void SetDot(void);

	//Function Implementations
	CXPathSequence* FuncPosition(CXPathSequence *input, XPathFunction *func);

};
