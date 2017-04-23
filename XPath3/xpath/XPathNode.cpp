#include "stdafx.h"
#include "XPathNode.h"
#include <iostream>

//*************************************************************
// CONSTRUCTION/DECONSTRUCTION
//*************************************************************

CXPathNode::CXPathNode():CXPathItem(){}

CXPathNode::CXPathNode(	CText sVal, CText sXSDType,
	long docOrder, CXPathNode* ownerDoc, CText xmlBase, 
	XPathNodeType nodeType, CText qName, CXPathNode *parent):
		CXPathItem(sVal, sXSDType), 
		m_DocumentOrder(docOrder),
		m_OwnerDocument(ownerDoc),
		m_XMLBase(xmlBase),
		m_QName(qName),
		m_NodeType(nodeType),
		m_Parent(parent){}

CXPathNode::~CXPathNode(){}

//*************************************************************
// ACCESSOR METHODS
//*************************************************************

//-- IsNode ----------------------------------------------------
bool CXPathNode::IsNode() const {
	return true;
}

//-- IsNode ----------------------------------------------------
//used for identity comparison of nodes.
bool CXPathNode::Is(CXPathItem *item) {
	bool result;
	if(item->IsNode()){
		CXPathNode *node = dynamic_cast<CXPathNode*> (item);
		string s1 = string(LPSTR(this->m_OwnerDocument));
		string s2 = string(LPSTR(node->GetOwnerDocument()));
		if( (s1.compare(s2) == 0) && 
			(this->m_DocumentOrder==node->GetDocumentOrder()) )
			result = true;
	}
	return result;
}

void CXPathNode::SetNamespace(CText ns)
{
	m_XMLBase = ns;
}

CText CXPathNode::GetNamespace()
{
	return m_XMLBase;
}

void CXPathNode::SetNodeType(XPathNodeType type)
{
	m_NodeType = type;
}

//-- GetNodeType ----------------------------------------------
//Each node has a specified type as it relates to the XPath Data
//Model. See enum XPathNodeType in header file.
XPathNodeType CXPathNode::GetNodeType() const
{
	return m_NodeType;
}

//-- GetDocumentOrder ---------------------------------------------
//m_DocumentOrder relates to the position of the CXPathNode as it 
//relates to a depth first, left to right traversal of the 
//CXPathTerm graph.  Document Order starts at the root (document) node of
//a document. 
long CXPathNode::GetDocumentOrder() const
{
	return m_DocumentOrder;	
}
void CXPathNode::SetDocumentOrder(long order)
{
	m_DocumentOrder = order;
}

CXPathNode* CXPathNode::GetOwnerDocument() {
	return this->m_OwnerDocument;
}

void CXPathNode::SetQName(CText qName)
{
	m_QName = qName;
}
//-- GetQName ------------------------------------------------
//QName is defined as (prefix":")?LocalName relating to any node
CText CXPathNode::GetQName() 
{
	return m_QName;
}

//-- GetLocalName --------------------------------------------
//Local Name is the tag or attribute name without any namespace prefix.
CText CXPathNode::GetLocalName()
{
	int separateAt = m_QName.Find(':');
	if(separateAt>-1) 
		return m_QName.Mid(separateAt+1);
	else
        return m_QName;
}

//-- GetExpandedQName ---------------------------------------
//the Expanded-QName is (Namespace URI":")?LocalName of a node
//!!!TO DO!!! must implement namespaces
CText CXPathNode::GetExpandedQName() 
{
	return m_QName;
}

//-- GetChildAt ----------------------------------------------
//will return a pointer to a child CXPathNode instance at a position i
//counting from 0. 
//!!!TO DO!!! Confirm starting index with Kimmie.
CXPathNode* CXPathNode::GetChildAt(int i) 
{
	try{
		if(i<0 || i>=(int)m_Children.size()) 
			throw "out of range access on CXPathTerm Children collection";
		else return m_Children[i];
	}
	catch(char* s){
		cout << "Error: " << s << "..." << (i>=(int)m_Children.size()) << endl;
		return NULL;
	}
}

//--GetChildren() -----------------------------------------------
//return all childnodes as items in a sequence
CXPathSequence* CXPathNode::GetChildren(){
	CXPathSequence *result = new CXPathSequence();
	result->m_Items->insert(result->m_Items->begin(),this->m_Children.begin(),this->m_Children.end());
	return result;
}

//--GetDescendants() -----------------------------------------------
//return the childnodes of all deseas items in a sequence
CXPathSequence* CXPathNode::GetDescendants(){
	CXPathSequence *result = new CXPathSequence();
	vector<CXPathNode*>::iterator it = this->m_Children.begin();
	for(it;it!=this->m_Children.end();it++){
		result->m_Items->push_back(*it);
		result->ConcatSequence((*it)->GetDescendants());
	}
	return result;
}


//-- GetAttributeAt -------------------------------------------
//will return a pointer to an CXPathNode instance of m_NodeType = ATTRIBUTE_NODE
//from the m_Attributes collection at position i counting from 0
CXPathNode * CXPathNode::GetAttributeAt(int i)
{
	try {
		if (i<0 || i >= (int)m_Attributes.size())
			throw "out of range access on CXPathTerm m_Attributes collection";
		else return m_Attributes[i];
	}
	catch (char* s) {
		cout << "Error: " << s << "..." << (i >= (int)m_Attributes.size()) << endl;
		return NULL;
	}

}

//-- GetAttributeByName -------------------------------------------
//will return a pointer to an CXPathNode instance of m_NodeType = ATTRIBUTE_NODE
//from the m_Attributes collection given the specified name
CXPathNode* CXPathNode::GetAttributeByName(string name)
{
	try {
		if ((int)m_Attributes.size() == 0) 
		{
			throw "out of range access on CXPathTerm m_Attributes collection";
		}
		else
		{
			return m_Attributes_map.at(name);
		}
	}
	catch (char* s) {
		cout << "Error: " << s << "..." << (name) << endl;
		return NULL;
	}
}

void CXPathNode::SetParent(CXPathNode* parent)
{
	m_Parent = parent;
}

//-- GetParent ------------------------------------------------
//will return a pointer to the parent node of an attribute or
//child node. Will return null if the document node. 
CXPathNode* CXPathNode::GetParent()
{
	return m_Parent;
}

//*************************************************************
// MUTATOR METHODS
//*************************************************************

//-- UpdateOwnerDocument -----------------------------------------------
//Will update the pointer to the owning document to the specified CXPathNode instance.
void CXPathNode::UpdateOwnerDocument(CXPathNode* owner)
{
	this->m_OwnerDocument = owner;
}

//-- SetChildAt -----------------------------------------------
//Will insert a pointer to an instance of CXPathNode in the 
//m_Children member at position i of the collection (on a 0 start index)
void CXPathNode::SetChildAt(int i, CXPathNode *x) 
{
	try {
		if(i<0 || i>(int)m_Children.size()) 
			throw "out of range access on CXPathNode Children collection";
		else if(x==this)
			throw "can not pass self to child collection";
		else
			m_Children.insert(m_Children.begin()+i,x);
	}
	catch(char* s){
		cout << "Error: " << s << (i>=0) << "..." << ((i-1)<(int)m_Children.size()) << endl;
	}
}

//-- SetAttributeAt --------------------------------------------
//will insert a pointer to an instance of CXPathNode (x) in the
//m_Attributes collection at position i (on a 0 start index).
//PRE: x->m_NodeType == ATTRIBUTE_NODE
void CXPathNode::SetAttributeAt(int i, CXPathNode *x) 
{
	try {
        if(i<0 || i>(int)m_Attributes.size()) 
			throw "out of range access on CXPathNode m_Attributes collection";
		else if (x->GetNodeType()!=ATTRIBUTE_NODE)
			throw "attempt to insert a non-attribute node to m_attributes collection";
		else if (x==this)
			throw "can not pass self to child collection.";
		else
			m_Attributes.insert(m_Attributes.begin()+i,x);
	}
	catch (char* s) {
		cout << "Error: " << s << endl;
	}
}

//-- AppendChild ------------------------------------------------
//will add a pointer to CXPathNode after the last member of the
//m_Children collection;
void CXPathNode::AppendChild(CXPathNode *x)
{
	m_Children.push_back(x);
	//!!!TO DO!!! should add some safety tests.
}

//-- AppendAttribute --------------------------------------------
//will add a pointer to CXPathNode after the last member of the
//m_Attributes collection;
void CXPathNode::AppendAttribute(CXPathNode *x)
{
	try{
		if (x->GetNodeType() == ATTRIBUTE_NODE) 
		{
			m_Attributes.push_back(x);

			string qname = x->m_QName;

			m_Attributes_map[qname] = x;

		}
		else
			throw "can only add attribute nodes to m_Attributes collection";
	}
	catch(char *s){
		cout << "Error:" << s << endl;
	}
}

//-- ToString ---------------------------------------------------
//for testing purposes.
CText CXPathNode::ToString(){
	CText str;
	switch(this->m_NodeType){
		case ELEMENT_NODE:
			str = this->GetQName();
			break;
		case ATTRIBUTE_NODE:
			str = CText("attr:")+this->GetQName();
			break;
		case DOCUMENT_NODE:
			str = this->GetQName();
			break;
		case TEXT_NODE:
			str = CText("text:")+this->m_StringValue;
			break;
	}
	return str;
}

/*
//-- ResetChildren ----------------------------------------------
//for CXPathNode destructor, will destroy all CXPathNode instances in 
//m_Children collection
void CXPathNode::ResetChildren()
{
	vector<CXPathNode*>::iterator it = m_Children.begin();
	for (; it != m_Children.end(); it++) {
		if (*it) delete *it;
	}
	m_Children.clear();
}

//-- ResetAttributes --------------------------------------------
//for CXPathNode destructor, will destroy all CXPathNode instances in 
//m_Attributes collection
void CXPathNode::ResetAttributes()
{
	vector<CXPathNode*>::iterator it = m_Attributes.begin();
	for (; it != m_Attributes.end(); it++) {
		if (*it) delete *it; 
	}
	m_Attributes.clear();
}
*/
