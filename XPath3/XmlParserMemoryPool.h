// Gerallt © 2017.

#ifndef XML_MEM_POOL_H
#define XML_MEM_POOL_H

#include <stdio.h>
#include <string.h>
#include "xpath/XPathSequence.h"
#include "xpath/XPathNode.h"
#include "xpath/EvalEngine.h"

/* memory pool for contiguious memory access */

#define MAX_POOL_SIZE 10000

class Stack 
{
public:
	typedef struct node
	{
		void* data = nullptr;
		struct node *next = nullptr;
		struct node *prev = nullptr;
	} node;

private:
	int ptop;	// pool stack top
	node* pool[MAX_POOL_SIZE];
	node *tos;
	node *eos;
	int stackCount = 0;

	typedef string(*StackItemPrintFunction)(void* o);
public:
	Stack();
	~Stack();

	// Stack Operations
	void* ElementAt(int index);
	void* Peek();
	node* PeekNode();
	void* PeekPrev();
	void* Behind();
	void Push(void* o);
	void* Pop();
	bool IsEmpty();
	void Insert(void* o); // use stack as a linked list
	void Print(StackItemPrintFunction dataFormatFunction);
};



class dom_node
{
public:
	void* data;
	CXPathNode* xpath;
	dom_node* next = nullptr;
	dom_node* parent = nullptr;
	long order;
	long level;
	string name;
};

#endif