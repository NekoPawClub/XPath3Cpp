#include "StdAfx.h"
#include "exprtreenode.h"
#include <iostream>

CExprTreeNode::CExprTreeNode(void)
{
	this->Token = NULL;
	this->IsExprStart = false;
	this->IsRoot = false;
}

CExprTreeNode::~CExprTreeNode(void)
{
}

void CExprTreeNode::Traverse(){
	;
}

void CExprTreeNode::Visit(){

	switch(this->Kind){
		case ETNK_STEP: 
			if(this->IsExprStart){
				;
			}

	}

}

void CExprTreeNode::TestTraverse(){
	TestVisit();
	vector<CExprTreeNode*>::iterator it = this->Siblings.begin();
	for(it;it!=Siblings.end();it++){
		cout << " sibling:  ";
		(*it)->TestTraverse();
	}
	it = this->Children.begin();
	for(it;it!=Children.end();it++){
		cout << " child:  ";
		(*it)->TestTraverse();
	}
	cout << endl;
}

void CExprTreeNode::TestVisit(){
	cout << " Kind: " << this->Kind;
}

XPathTokenName CExprTreeNode::GetAxisName(){
	if( this->Kind == ETNK_FORWARD_AXIS || 
		this->Kind == ETNK_REVERSE_AXIS) ;
	else ; //error
	return this->Token->Token->second->Name;
}