#include "StdAfx.h"
#include "exprtoken.h"

CExprToken::CExprToken(void)
{
}

//CExprToken::CExprToken(XPathTokenType tt, string name, XPathLexicalState state, XPathLexicalState tostate, int priority)
CExprToken::CExprToken(XPathTokenType tt, XPathTokenName name, XPathLexicalState state, XPathLexicalState tostate, int priority)
{
	this->Type = tt;
	this->Name = name;
	this->State = state;
	this->ToState = tostate;
	this->Priority = priority;
}

CExprToken::~CExprToken(void)
{
}
