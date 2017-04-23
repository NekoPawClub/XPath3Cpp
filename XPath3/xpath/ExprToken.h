#pragma once

#include <string>

using namespace std;

enum XPathTokenType { XXXTEMPXXX }; //DO NOT USE THIS WILL DELETE

enum XPathLexicalState {
	DEFAULT_STATE,
	OPERATOR_STATE,
	QNAME_STATE,
	ITEMTYPE_STATE,
	VARNAME_STATE
};

enum XPathTokenName{
	ENDEXPR,
	LEFTPAREN,
	RIGHTPAREN,
	STRING_LITERAL,
	INTEGER_LITERAL,
	DECIMAL_LITERAL,
	DOUBLE_LITERAL,
	FORWARDSLASH,
	DOUBLE_FORWARDSLASH,
	CHILD_AXIS,
	DESCENDANT_AXIS,
	PARENT_AXIS,
	ATTRIBUTE_AXIS,
	SELF_AXIS,
	DESCENDANT_OR_SELF_AXIS,
	ANCESTOR_AXIS,
	FOLLOWING_SIBLING_AXIS,
	FOLLOWING_AXIS,
	PRECEDING_AXIS,
	PRECEDING_SIBLING_AXIS,
	NAMESPACE_AXIS,
	ANCESTOR_OR_SELF_AXIS,
	WILDCARD,
	LOCALNAME_WILDCARD,
	NAMESPACE_WILDCARD,
	LOCALNAME,
	QNAME,
	LOCALNAME_CALL,
	QNAME_CALL,
	TEXT_NODE_TEST,
	COMMENT_NODE_TEST,
	ANY_NODE_TEST,
	PI_NODE_TEST,
	VARIABLE_MARKER,
	COMMA,
	MULTIPLY,
	EQUALS,
	OPEN_BRACKET,
	CLOSE_BRACKET
} ;

class CExprToken {
public:
	CExprToken(void);
	CExprToken(XPathTokenType tt, XPathTokenName name, XPathLexicalState state, XPathLexicalState tostate, int priority);
	virtual ~CExprToken(void);

	XPathTokenType Type;
	XPathTokenName Name;
//	string Name;
	XPathLexicalState State;
	XPathLexicalState ToState;
	int Priority;
};
