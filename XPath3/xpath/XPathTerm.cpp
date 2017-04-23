#include "stdafx.h"
#include "XPathTerm.h"
#include <iostream>

CXPathTerm::CXPathTerm()
{
	m_ItemType = not_an_item;
	m_NodeType = not_a_node;
	m_Namespace = NULL;
	m_TypedValue = NULL;
}

CXPathTerm::CXPathTerm(
		int documentOrder,
		CText xmlBase,
		XPathItemType itemType,
		XPathNodeType nodeType,
		CText typeMatches,
		CText qName,
		CText stringValue,
		CXPathTerm* parent)
{
	m_DocumentOrder = documentOrder;
	m_XMLBase = xmlBase;
    m_ItemType = itemType;
    m_NodeType = nodeType;
    m_TypeMatches = typeMatches;
    m_QName = qName;
	m_StringValue = stringValue;
    m_Parent = parent;
}       	

CXPathTerm::~CXPathTerm(){
	//if(this->m_Namespace) delete &(this->m_Namespace);
	//if(this->m_TypedValue) delete &(this->m_TypedValue);
	//ResetAttributes();
	//ResetChildren();
	m_Children.clear();
	m_Attributes.clear();
}

//*************************************************************
// ACCESSOR METHODS
//*************************************************************

//-- GetItemType ----------------------------------------------
XPathItemType CXPathTerm::GetItemType() const
{
	return m_ItemType;
}

//-- GetNodeType ----------------------------------------------
XPathNodeType CXPathTerm::GetNodeType() const
{
	return m_NodeType;
}

//--IsSequence-------------------------------------------------
//a sequence is the basic input and output type of an XPath 
//expression. In CXPathTerm a sequence is an instance of the 
//class where m_Item is not_an_item. A sequence can be considered a 
//container of atomic values and/or nodes.
bool CXPathTerm::IsSequence() const 
{
	return (m_ItemType==not_an_item)?true:false;
}

//--IsAtomic---------------------------------------------------
//an atomic value is a primitive type defined by XML Schema
//an instance of CXpathTerm is considered an attomic value when
//is m_ItemType is of value 0.
//!!!TO DO!!! this should return the type of the atomic value
bool CXPathTerm::IsAtomic() const
{
	return (m_ItemType==atomic);
}

//--IsNode -----------------------------------------------------
//an CXPathTerm instance is a node if if m_NodeType is not not_a_node
//!!!TO DO!!! should probably return the node type or null for false
bool CXPathTerm::IsNode() const
{
	return (m_NodeType==not_a_node)?false:true;
}

//-- GetDocOrder ---------------------------------------------
//m_DocumentOrder relates to the position of the CXPathTerm as it 
//relates to a depth first, left to right traversal of the 
//CXPathTerm graph...for example, the attribute nodes of an element 
//will always have an m_DocumentOrder less than the child nodes of 
//that element. Document Order starts at the root (document) node of
//a documen. In the case where the CXPathTerm graph represents a sequence, 
//the container CXPathTerm will have a Document Order of NULL.
int CXPathTerm::GetDocumentOrder() const
{
	return m_DocumentOrder;
}

//-- GetQName ------------------------------------------------
//QName is defined as (prefix":")?LocalName relating to any node
CText CXPathTerm::GetQName() 
{
	return m_QName;
}

//-- GetLocalName --------------------------------------------
//Local Name is the tag or attribute name without any namespace prefix.
CText CXPathTerm::GetLocalName()
{
	int separateAt = m_QName.Find(':');
	if(separateAt>-1) 
		return m_QName.Mid(separateAt+1);
	else
        return m_QName;
}

//-- GetTextValue --------------------------------------------
//Text Values are stored in the m_TextValue member of text nodes
//and attribute nodes.
//!!!TO DO!!! calc the Text Value of an element node
CText CXPathTerm::GetStringValue() 
{
	return m_StringValue;
}

//!!!TO DO!!! -- GetNamespace -------------------------------

//-- GetExpandedQName ---------------------------------------
//the Expanded-QName is (Namespace URI":")?LocalName of a node
//!!!TO DO!!! must implement namespaces
CText CXPathTerm::GetExpandedQName() 
{
	return m_QName;
}

//-- GetChildAt ----------------------------------------------
//will return a pointer to a child CXPathTerm instance at a position i
//counting from 0. 
//!!!TO DO!!! Confirm starting index with Kimmie.
CXPathTerm * CXPathTerm::GetChildAt(int i) 
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

//-- GetAttributeAt -------------------------------------------
//will return a pointer to an CXPathTerm instance of m_NodeType = attribute
//from the m_Attributes collection at position i counting from 0
CXPathTerm * CXPathTerm::GetAttributeAt(int i)
{
	try{
		if(i<0 || i>=(int)m_Attributes.size()) 
			throw "out of range access on CXPathTerm m_Attributes collection";
		else return m_Attributes[i];
	}
	catch(char* s){
		cout << "Error: " << s << "..." << (i>=(int)m_Attributes.size()) << endl;
		return NULL;
	}
}

//-- GetParent ------------------------------------------------
//will return a pointer to the parent node of an attribute or
//child node. Will return null if the document node. 
CXPathTerm* CXPathTerm::GetParent()
{
	return m_Parent;
}

//*************************************************************
// MUTATOR METHODS
//*************************************************************

//-- SetChildAt -----------------------------------------------
//Will insert a pointer to an instance of CXPathTerm in the 
//m_Children member at position i of the collection (on a 0 start index)
void CXPathTerm::SetChildAt(int i, CXPathTerm *x) 
{
	try {
		if(i<0 || i>(int)m_Children.size()) 
			throw "out of range access on CXPathTerm Children collection";
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
//will insert a pointer to an instance of CXPathTerm (x) in the
//m_Attributes collection at position i (on a 0 start index).
//PRE: x->m_NodeType == attribute
void CXPathTerm::SetAttributeAt(int i, CXPathTerm *x) throw(string)
{
	try {
        if(i<0 || i>m_Attributes.size()) 
			throw "out of range access on CXPathTerm m_Attributes collection";
		else if (x->GetNodeType()!=attribute)
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
//will add a pointer to CXPathTerm after the last member of the
//m_Children collection;
void CXPathTerm::AppendChild(CXPathTerm *x)
{
	m_Children.push_back(x);
	//should add some safety tests.
}

//-- AppendAttribute --------------------------------------------
//will add a pointer to CXPathTerm after the last member of the
//m_Attributes collection;
void CXPathTerm::AppendAttribute(CXPathTerm *x)
{
	try{
		if(x->GetNodeType()==attribute)
			m_Attributes.push_back(x);
		else
			throw "can only add attribute nodes to m_Attributes collection";
	}
	catch(char *s){
		cout << "Error:" << s << endl;
	}
}

//-- ResetChildren ----------------------------------------------
//for CXPathterm destructor, will destroy all CXPathTerm instances in 
//m_Children collection
void CXPathTerm::ResetChildren()
{
	vector<CXPathTerm*>::iterator it = m_Children.begin();
	for (it; it != m_Children.end(); it++) {
		if (*it) delete *it;
	}
	m_Children.clear();
}

//-- ResetAttributes --------------------------------------------
//for CXPathterm destructor, will destroy all CXPathTerm instances in 
//m_Attributes collection
void CXPathTerm::ResetAttributes()
{
	vector<CXPathTerm*>::iterator it = m_Attributes.begin();
	for (; it != m_Attributes.end(); it++) {
		if (*it) delete *it; 
	}
	m_Attributes.clear();
}
