#include "StdAfx.h"
#include "evalengine.h"
#include <string>
#include <iostream>

using namespace std;

void CEvalEngine::BuildExprTree(){
	this->ItToken = this->Pass3Expr.begin();
	this->ExprTree = XPath();
	this->ExprTree->IsRoot = true;
}

XPathTokenName CEvalEngine::GetTokenName() {
	if(ItToken!=Pass3Expr.end())
		return (*ItToken)->Token->second->Name;
	else
		return ENDEXPR;
}

XPathTokenName CEvalEngine::GetNextToken() {
	XPathTokenName result;
	ItToken++;
	if(ItToken!=Pass3Expr.end())
		result = (*ItToken)->Token->second->Name;
	else 
		result = ENDEXPR;
	return result;
}

bool CEvalEngine::MatchToken(XPathTokenName t) {
	if(GetTokenName()==t){
		ItToken++;
		return true;
	}
	else return false;
}

bool CEvalEngine::RecogniseNonTerminal(XPathNonTerminal nt) {
	XPathTokenName tokenName = GetTokenName();
	bool result = false;
	switch(nt){
		case PARENTHESIZED_EXPR :
			if( tokenName == LEFTPAREN )
				result = true;
			return result;
		case PATH_EXPR :
			if( tokenName == FORWARDSLASH ||
				tokenName == DOUBLE_FORWARDSLASH ||
				RecogniseNonTerminal(RELATIVE_PATH_EXPR) ) 
				result = true;
			return result;
		case RELATIVE_PATH_EXPR : 
			if( IsTokenForwardAxis(tokenName)||
				IsTokenReverseAxis(tokenName)||
				IsTokenLiteral(tokenName)||
				tokenName == VARIABLE_MARKER ||
				tokenName == LOCALNAME_CALL ||
				tokenName == QNAME_CALL || 
				tokenName == LEFTPAREN ) 
				result = true;
			return result;
		case STEP_EXPR :
			if( IsTokenForwardAxis(tokenName) ||
				IsTokenReverseAxis(tokenName) ||
				RecogniseNonTerminal(PRIMARY_EXPR) )
				result = true;
			return result;
		case PRIMARY_EXPR :
			if( RecogniseNonTerminal(LITERAL) ||
				RecogniseNonTerminal(FUNCTION_CALL) ||
				RecogniseNonTerminal(VARNAME) || 
				RecogniseNonTerminal(PARENTHESIZED_EXPR) )
				result = true;
			return result;
		case NAME_TEST :
			if( tokenName == LOCALNAME ||
				tokenName == QNAME ||
				tokenName == WILDCARD ||
				tokenName == LOCALNAME_WILDCARD ||
				tokenName == NAMESPACE_WILDCARD )
				result = true;
			return result;
		case NODE_TEST :
			if( tokenName == TEXT_NODE_TEST ||
				tokenName == COMMENT_NODE_TEST ||
				tokenName == ANY_NODE_TEST ||
				tokenName == PI_NODE_TEST )
				result = true;
			return result;
		case PREDICATES :
			if( tokenName == OPEN_BRACKET )
				result = true;
			return result;
		case LITERAL :
			if( IsTokenLiteral(tokenName) )
				result = true;
			return result;
		case VARNAME :
			if( tokenName == VARIABLE_MARKER )
				result = true;
			return result;
		case FUNCTION_CALL :
			if( tokenName == LOCALNAME_CALL ||
				tokenName == QNAME_CALL )
				result = true;
			return result;
		default: return result;
	}
}

bool CEvalEngine::IsTokenForwardAxis(XPathTokenName n){
	switch(n){
		case CHILD_AXIS: return true;
		case DESCENDANT_AXIS: return true;
		case ATTRIBUTE_AXIS: return true;
		case SELF_AXIS: return true;
		case DESCENDANT_OR_SELF_AXIS: return true;
		case FOLLOWING_SIBLING_AXIS: return true;
		case FOLLOWING_AXIS: return true;
		case NAMESPACE_AXIS:return true;
		default: return false;
	}
}

bool CEvalEngine::IsTokenReverseAxis(XPathTokenName n){
	switch(n){
		case PARENT_AXIS: return true;
		case ANCESTOR_AXIS: return true;
		case PRECEDING_AXIS: return true;
		case PRECEDING_SIBLING_AXIS: return true;
		case ANCESTOR_OR_SELF_AXIS: return true;
		default: return false;
	}
}

bool CEvalEngine::IsTokenLiteral(XPathTokenName n){
	switch(n){
		case STRING_LITERAL: return true;
		case INTEGER_LITERAL: return true;
		case DECIMAL_LITERAL: return true;
		case DOUBLE_LITERAL: return true;
		default: return false;
	}
}

CExprTreeNode* CEvalEngine::XPath(){
	if (GetTokenName()!= ENDEXPR)
		return PathExpr();
	else
		return NULL;
}

CExprTreeNode* CEvalEngine::Expr(){
	CExprTreeNode *temp = new CExprTreeNode();
	if(RecogniseNonTerminal(PATH_EXPR)) {
		temp->Kind = ETNK_PATH_EXPR;
		temp->Children.push_back(PathExpr());
		return temp;
	}
	else return NULL; //error
}

CExprTreeNode* CEvalEngine::PathExpr(){
	CExprTreeNode *temp = new CExprTreeNode();
	CExprTreeNode *relexpr;
	switch(GetTokenName()){
		case FORWARDSLASH:
            MatchToken(FORWARDSLASH);
			if (RecogniseNonTerminal(RELATIVE_PATH_EXPR)) 
				relexpr = RelativePathExpr();
			else relexpr = NULL;
			temp->Kind = ETNK_STEP;
			temp->Children.push_back(relexpr);
			temp->IsExprStart = true;
			return temp;
		case DOUBLE_FORWARDSLASH:
			MatchToken(DOUBLE_FORWARDSLASH);
			if (RecogniseNonTerminal(RELATIVE_PATH_EXPR)) 
				relexpr = RelativePathExpr();
			else ; //error
			temp->Kind = ETNK_DOUBLE_STEP;
			temp->Children.push_back(relexpr);
			temp->IsExprStart = true;
			return temp;
		default:
			if(RecogniseNonTerminal(RELATIVE_PATH_EXPR)){
				temp = RelativePathExpr();
				temp->IsExprStart = true;
				return temp;
			}
			else ;//error 
	}
	return NULL ;//error
}

CExprTreeNode* CEvalEngine::RelativePathExpr(){
	CExprTreeNode *temp = new CExprTreeNode();
	XPathTokenName tokenName;
	if(RecogniseNonTerminal(STEP_EXPR)){
		temp = StepExpr();
	}
	else return NULL; //error
	tokenName = GetTokenName();
	CExprTreeNode *sibling;
	while( tokenName == FORWARDSLASH || 
			tokenName == DOUBLE_FORWARDSLASH){
		sibling = new CExprTreeNode();
		switch(tokenName){
			case FORWARDSLASH:
				MatchToken(FORWARDSLASH); 
				if (RecogniseNonTerminal(STEP_EXPR)){
					sibling->Kind = ETNK_STEP; 
					sibling->Children.push_back(StepExpr());
				}
				else sibling = NULL; //error
				break;
			case DOUBLE_FORWARDSLASH:
				MatchToken(DOUBLE_FORWARDSLASH);
				if (RecogniseNonTerminal(STEP_EXPR)){
					sibling->Kind = ETNK_DOUBLE_STEP; 
					sibling->Children.push_back(StepExpr());
				}
				else sibling = NULL; //error
				break;
		}
		if(sibling!=NULL)
			temp->Siblings.push_back(sibling);
		tokenName = GetTokenName(); //udate token name
	}
	return temp; 
}

CExprTreeNode* CEvalEngine::StepExpr(){
	CExprTreeNode *temp = new CExprTreeNode();
	XPathTokenName tokenName = GetTokenName();
	if(IsTokenForwardAxis(tokenName)){
		temp->Token = (*ItToken);
		CExprTreeNode *test;
		this->ItToken++; //not matching a specific token so just move forward
		if (RecogniseNonTerminal(NAME_TEST)) 
			test = NameTest();
		else if(RecogniseNonTerminal(NODE_TEST))
			test = NodeTest();
		else return NULL; // error
		temp->Kind = ETNK_FORWARD_AXIS;
		temp->Children.push_back(test);
	}
	else if(IsTokenReverseAxis(tokenName)){
		temp->Token = (*ItToken);
		CExprTreeNode *test;
		this->ItToken++; //not matching a specific token so just move forward
		if (RecogniseNonTerminal(NAME_TEST)) 
			test = NameTest();
		else if(RecogniseNonTerminal(NODE_TEST))
			test = NodeTest();
		else return NULL; // error
		temp->Kind = ETNK_REVERSE_AXIS;
		temp->Children.push_back(test);
	}
	else if(RecogniseNonTerminal(PRIMARY_EXPR)){
		temp = PrimaryExpr();
	}
	else return NULL; //error
	CExprTreeNode *sibling = NULL;
	while(RecogniseNonTerminal(PREDICATES)){
		MatchToken(OPEN_BRACKET);
		sibling = Expr();
		
		if(sibling!=NULL) {
			sibling->Kind = ETNK_PREDICATE;
			temp->Siblings.push_back(sibling);	
		}
		if(!MatchToken(CLOSE_BRACKET)) return NULL;
	}
	
	return temp; 
}

CExprTreeNode* CEvalEngine::NameTest(){
	CExprTreeNode *temp = new CExprTreeNode();
	temp->Kind = ETNK_NAME_TEST;
	temp->Token = (*ItToken);
	ItToken++;
	return temp;
}

CExprTreeNode* CEvalEngine::NodeTest(){
	CExprTreeNode *temp = new CExprTreeNode();
	temp->Kind = ETNK_NODE_TEST;
	temp->Token = (*ItToken);
	ItToken++;
	return temp;
}

CExprTreeNode* CEvalEngine::PrimaryExpr(){
	CExprTreeNode *temp = new CExprTreeNode();
	XPathTokenName tokenName;
	if(RecogniseNonTerminal(LITERAL)){
		temp->Kind = ETNK_LITERAL;
		temp->Token = (*ItToken);
		ItToken++;
		return temp;
	}
	else if(RecogniseNonTerminal(VARNAME)){
		temp->Kind = ETNK_VARIABLE;
		MatchToken(VARIABLE_MARKER);
		tokenName = GetTokenName();
		if( tokenName==LOCALNAME || 
			tokenName == QNAME) {
			temp->Token = (*ItToken);
			ItToken++;
		}
		else return NULL; //error;
		return temp;
	}
	else if(RecogniseNonTerminal(FUNCTION_CALL)){
		temp->Kind = ETNK_FUNCTION_CALL;
		temp->Token = (*ItToken);
		tokenName = GetNextToken();
		while(tokenName!=RIGHTPAREN){
			temp->Children.push_back(Expr());
			while(GetTokenName()==COMMA) {
				MatchToken(COMMA);
				temp->Children.push_back(Expr());
			}
			tokenName = GetTokenName();
		}
		if(tokenName==RIGHTPAREN){
			ItToken++;
			return temp;
		}
		else return NULL; //error
	}
	else if(RecogniseNonTerminal(PARENTHESIZED_EXPR)){
		temp->Kind = ETNK_EXPR_SEQUENCE;
		MatchToken(LEFTPAREN);
		tokenName = GetTokenName();
		while(tokenName!=RIGHTPAREN){
			temp->Children.push_back(Expr());
			while(GetTokenName()==COMMA) {
				MatchToken(COMMA);
				temp->Children.push_back(Expr());
			}
			tokenName = GetTokenName();
		}
		if(tokenName==RIGHTPAREN){
			ItToken++;
			return temp;
		}
		else return NULL; //error
	}
	else return NULL; //error
}