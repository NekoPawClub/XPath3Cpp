#pragma once

#include "exprtreenode.h"




class CExprTree
{
public:
	CExprTree(void);
	virtual ~CExprTree(void);

	CExprTreeNode *Root;
	CExprTreeNode *CurrentNode;

	void SetLeftChild(CExprTreeNode*);
	void SetRightChild(CExprTreeNode*);

};
