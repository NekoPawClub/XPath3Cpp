// Gerallt © 2017.

#include "XmlParserMemoryPool.h"
#include <iostream>

using namespace std;

Stack::Stack()
{
	ptop = 0;
	memset(pool, 0, sizeof(pool));
	//tos = &pool[ptop++];
	stackCount = 0;
	eos = nullptr;
	tos = nullptr;
}
Stack::~Stack()
{

}
void Stack::Insert(void* o)
{
	node *n = eos;
	while (n->next != nullptr)
	{
		n = n->next;
	}

	n->next = pool[ptop++];
	n->next->data = o;
}

void* Stack::Behind()
{
	if (eos == nullptr)
	{
		return nullptr;
	}

	return eos->data;
}

void* Stack::ElementAt(int index)
{
	node* n = pool[index];

	if (n != nullptr && n->data != nullptr)
	{
		return pool[index]->data;
	}
	return nullptr;
}

void* Stack::PeekPrev()
{
	if (tos == nullptr || tos->prev == nullptr)
	{
		return nullptr;
	}
	return tos->prev->data;
}

Stack::node* Stack::PeekNode()
{
	if (tos == nullptr)
	{
		return nullptr;
	}
	return tos;
}

void* Stack::Peek() 
{
	if (tos == nullptr)
	{
		return nullptr;
	}
	return tos->data;
}

void Stack::Push(void* o)
{
	node *n = tos;

	if (n == nullptr)
	{
		n = new node();
		n->data = o;
		n->prev = nullptr;
		n->next = nullptr;
		
		pool[ptop] = n;

		eos = n;
		tos = n;

		stackCount++;

		return;
	}

	// Assign data to next node
	n->next = new node();


	//n->next = **((&pool) + (ptop)); //  &pool[ptop++];
	n->next->data = o;
	n->next->prev = tos;
	//tos->prev->next = tos;

	pool[++ptop] = n->next; // Quick lookup of end node

	// Assign new top of stack
	tos = n->next;
	
	// Increment stack size
	stackCount++;
}

void* Stack::Pop()
{
	node *n = tos;
	void* o = tos->data;

	node* prev = tos->prev;
	if (prev != nullptr)
	{
		prev->next = tos->next;
	}
	
	tos = tos->prev;
	pool[ptop] = nullptr;
	ptop--;

	if (tos == nullptr)
	{
		eos = nullptr;
	}

	// Decrement stack size
	stackCount--;

	return o;
}

bool Stack::IsEmpty()
{
	return stackCount == 0;
}

void Stack::Print(StackItemPrintFunction dataFormatFunction)
{
	node *tmp;
	bool printedTos = false;

	cout << "[STACK BEGIN]" << endl;

	for (int i = 0; i < stackCount; i++)
	{
		tmp = pool[i];

		if (tmp->data == nullptr) break;

		if (dataFormatFunction == nullptr)
		{
			cout << "0x" << (int)tmp->data << ((tmp == tos) ? " (tos)" : "" ) << endl;
		}
		else
		{
			cout << "0x" << (int)tmp->data << "->" << dataFormatFunction(tmp->data);
			if (tmp == tos)
			{
				printedTos = true;

				cout << " (tos)";
			}
			cout << endl;
		}
	}

	if (!printedTos && tos != nullptr)
	{
		cout << " (tos:0x" << (int)tos << " WEIRD!)" << endl;
	}
	
	if (tos == nullptr)
	{
		cout << "(is empty)";
	}

	cout << "[STACK END]" << endl;
}