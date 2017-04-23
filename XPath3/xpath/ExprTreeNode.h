#pragma once

#include "exprtoken.h"
#include "XPathDynamicEnv.h"

#include <vector>
#include <string>

using namespace std;

typedef pair <string, CExprToken*> Pattern_Token_Pair;

enum XPathSyntaxNodeKind {
	ETNK_STEP,			
	ETNK_DOUBLE_STEP,	
	ETNK_FORWARD_AXIS,
	ETNK_REVERSE_AXIS,
	ETNK_NAME_TEST,
	ETNK_NODE_TEST,
	ETNK_LITERAL,
	ETNK_EXPR_SEQUENCE,
	ETNK_FUNCTION_CALL,
	ETNK_PATH_EXPR,
	ETNK_VARIABLE,
	ETNK_PREDICATE
};

// non terminal means that it can be substituted by other EBNF symbols,
enum XPathNonTerminal{
		XPATH, 
		EXPR_SEQUENCE, 
		EXPR,
		OR_EXPR,
		AND_EXPR,
		FOR_EXPR,
		QUANTIFIED_EXPR,
		IF_EXPR,
		INSTANCE_OF_EXPR,
		CASTABLE_EXPR,
		COMAPARISON_EXPR,
		RANGE_EXPR,
		ADDITIVE_EXPR,
		MULTIPLICATIVE_EXPR,
		UNARY_EXPR,
		UNION_EXPR,
		INTERSECT_EXCEPT_EXPR,
		VALUE_EXPR,
		PATH_EXPR,
		RELATIVE_PATH_EXPR, //OK
		STEP_EXPR,
		SIMPLE_FOR_CLAUSE,
		VALIDATE_EXPR,
		CAST_EXPR,
		TREAT_EXPR,
		GENERAL_COMP,
		VALUE_COMP,
		NODE_COMP,
		ORDER_COMP,
		PRIMARY_EXPR,
		FORWARD_AXIS,
		REVERSE_AXIS,
		NODE_TEST,
		NAME_TEST, //OK
		WILDCARD_TEST,
		KIND_TEST,
		PROCESSING_INSTRUCTION_TEST,
		COMMENT_TEST,
		TEXT_TEST,
		ANY_KIND_TEST,
		FORWARD_STEP,
		REVERSE_STEP,
		ABBREVIATED_FORWARD_STEP,
		ABBREVIATED_REVERSE_STEP,
		PREDICATES,
		NUMERIC_LITERAL,
		LITERAL,
		PARENTHESIZED_EXPR, 
		FUNCTION_CALL,
		SCHEMA_CONTEXT,
		SCHEMA_GLOBAL_CONTEXT,
		SCHEMA_CONTEXT_STEP,
		SINGLE_TYPE,
		SEQUENCE_TYPE,
		ITEM_TYPE,
		ELEM_OR_ATTRIBUTE_TYPE,
		SCHEMA_TYPE,
		ATOMIC_TYPE,
		OCCURRENCE_INDICATOR,
		VARNAME
};

struct XPathTokenInfo {
	union  {
		int iVal;
		double dVal;
		string *sVal;
		XPathFunction *funcVal;
	} Value;
	string Type;
};

struct XPathTokenEntry{
	XPathTokenEntry(Pattern_Token_Pair *t):
			Token(t){};
	Pattern_Token_Pair *Token;
	vector<XPathTokenInfo> Info;
};

class CExprTreeNode
{
public:
	CExprTreeNode(void);
	CExprTreeNode(XPathSyntaxNodeKind kind);
	virtual ~CExprTreeNode(void);

	XPathSyntaxNodeKind Kind;
	XPathTokenEntry *Token;
	vector<CExprTreeNode*> Children;
    vector<CExprTreeNode*> Siblings;

	bool IsExprStart;
	bool IsRoot;

	XPathTokenName GetAxisName(void);

	void Traverse();
	void Visit();

	void TestTraverse();
	void TestVisit();
};
