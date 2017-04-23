//enable run-time type info, compile with: /GR
#include "stdafx.h"
#include "XPathSequence.h"
#include "XPathNode.h"
#include <algorithm>
#include <iostream>

CXPathSequence::CXPathSequence(){}
CXPathSequence::~CXPathSequence(){}

//-- GetItemAt ----------------------------------------------
//will return a pointer to an item in the sequence
//index counting from 0. 
//Note that pointer may possess dynamic type of CXPathNode
CXPathItem* CXPathSequence::GetItemAt(int i) 
{
	try{
		if(i<0 || i>=(int)m_Items->size()) 
			throw "out of range access on CXPathSequence Items collection";
		else return (*m_Items)[i];
	}
	catch(char* s){
		cout << "Error: " << s << "..." << (i>=(int)(*m_Items).size()) << endl;
		return NULL;
	}
}

//-- AppendItem ------------------------------------------------
//will add a pointer to CXPathItem after the last member of the
//m_Items collection;
//also intend to accept pointers to CXPathNode
void CXPathSequence::AppendItem(CXPathItem *item)
{
	m_Items->push_back(item);
}

//-- ConcatSequence ------------------------------------------------
//will concatenate the other's m_Items collection to the back of the 
//owner sequence's m_Items collection;
void CXPathSequence::ConcatSequence(CXPathSequence *other){
	this->m_Items->insert(this->m_Items->end(),other->m_Items->begin(),other->m_Items->end());
}

//-- GetAtomicValues -------------------------------------------
//extract all atomic values from a sequence
//EG: xpath(atomic value in (10,11,<elementA>,12,<elementB>))
//		will return sequence instance of (10,11,12)
CXPathSequence* CXPathSequence::GetAtomicValues()
{
	CXPathSequence *sq = new CXPathSequence();
	vector<CXPathItem*>::iterator it = m_Items->begin();
	for (; it != m_Items->end(); it++) {
		if(!(*it)->IsNode())
			sq->AppendItem(*it);
	}
	return sq;
}



//extract all atomic values from a sequence of type 
//EG: xpath(atomic value of type xs:string in (10,"bob",<elementA>,12,<elementB>))
//		will return sequence instance of ("bob")
CXPathSequence* CXPathSequence::GetAtomicValues(CText xsdType)
{
	CXPathSequence *sq = new CXPathSequence();
	vector<CXPathItem*>::iterator it = m_Items->begin();
	for (; it != m_Items->end(); it++) {
		if (!(*it)->IsNode()) { //is not a node
			CText s1 = (*it)->GetXSDType();
			CText s2 = xsdType;
			if(s1.Compare(s2)==0)
				sq->AppendItem(*it);
		}
	}
	return sq;
}

//-- GetNodesOfType ---------------------------------------------
//extract all nodes from a sequence that match a given XPathNodeType.
//see enum in XPathSequence.h header file.
//EG: xpath(element in (10,11,<elementA>,12,<elementB>))
//		will return sequence instance of (<elementA>,<elementB>)
CXPathSequence* CXPathSequence::GetNodesOfType(XPathNodeType nodeType)
{
	CXPathSequence *sq = new CXPathSequence();
	vector<CXPathItem*>::iterator it = m_Items->begin();
	for (; it != m_Items->end(); it++) {
		CXPathNode *node = dynamic_cast<CXPathNode*>(*it);
		if (node) {
			if(node->GetNodeType()==nodeType)
				sq->AppendItem(*it);
		}
	}
	return sq;	
}

//-- IsSingleton -------------------------------------------------
// Sequence has one item return true
bool CXPathSequence::IsSingleton(void){
	return (this->m_Items->size()==1);
}

//-- IsEmpty -------------------------------------------------
// Input Sequence contains no items return true
bool CXPathSequence::IsEmpty(void){
	return (this->m_Items->size()==0);
}

//-- GetSequenceSize ------------------------------------------
// returns number of items in sequence.
int CXPathSequence::GetSize(){
	return ((int) this->m_Items->size());
}

//-- SortItemsByDocOrder --------------------------------------
void CXPathSequence::SortItemsByDocOrder(void){
	std::sort(this->m_Items->begin(),this->m_Items->end(),XPathDocumentOrderSort());
}

//--ToString ---------------------------------------------------
//for testing
string CXPathSequence::ToString(){
	string result;
	string *ct;
	vector<CXPathItem*>::iterator it = this->m_Items->begin();
	int sz = m_Items->size() - 1;
	int i = 0;
	for(it;it!=m_Items->end();it++){
		
		ct = new string((*it)->ToString());
		if((*it)->IsNode())
			result += "<"+(*ct)+">" + ((i == sz) ? "" : " , ");
		else
			result += (*ct) + ((i == sz) ? "" : " , ");
		i++;
	}
	return result;
}
