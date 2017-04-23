#include <vector>
#include "XPathNamespace.h"

using namespace std;

//basic element of a sequence is one of these values 
enum XPathItemType 
{
	not_an_item,
	atomic, 
	node
};

//each non atomic item is of a node type
enum XPathNodeType 
{
	not_a_node,
	document, 
	element, 
	attribute, 
	text, 
	xml_namespace, 
	processing_instruction, 
	comment
};

class CXPathTerm 
{
public:
   CXPathTerm();
   CXPathTerm(int documentOrder,
			  CText xmlBase,
			  XPathItemType itemType,
			  XPathNodeType nodeType,
			  CText typeMatches,
			  CText qName,
			  CText stringValue,
			  CXPathTerm* parent);
			  
   virtual ~CXPathTerm();

   // generates a CXPathTerm object graph based on the 
   // CXMLNode object graph...to be done by Kimmie.
   //void GenerateTree(CXMLNode *pXML);

private:
   int           	m_DocumentOrder; 	
   CText	   		m_XMLBase; 		
   XPathItemType 	m_ItemType;			
   XPathNodeType 	m_NodeType;      	
   CText			m_TypeMatches; 	

   CText         	m_QName;         	

   CXPathTerm    	*m_Parent;       	

   CXPathNamespace	*m_Namespace;

   vector<CXPathTerm*>  m_Attributes;  	
   vector<CXPathTerm*> 	m_Children;    	

   CText 			m_StringValue;   		
   CXPathTerm 		*m_TypedValue;		

public:

	//accessor	
	bool IsSequence() const;
	bool IsAtomic() const;
	bool IsNode() const;

	XPathItemType GetItemType() const;
	XPathNodeType GetNodeType() const;
	
	int GetDocumentOrder() const;

	CText GetQName(); 		// returns Qname
	CText GetLocalName();		// returns Local Name
	CText GetExpandedQName();	// returns Expanded-QName	(ns URI:LocalName)
   
	CText GetStringValue(); 

	CXPathTerm* GetChildAt(int i); // return child CXPathTerm instance at index, i
	CXPathTerm* GetAttributeAt(int i); // return attribute (CXPathTerm instance) at index, i
	CXPathTerm* GetParent(); 

	void AppendChild(CXPathTerm*);
	void AppendAttribute(CXPathTerm*);

	//Mutators
	void SetChildAt(int i, CXPathTerm *x); // set child CXPathTerm instance at index, i
	void SetAttributeAt(int i, CXPathTerm *x); // set attribute (CXPathTerm instance) at i

	void setNSNode();
	void setStringValue(CText v);
	void setTypedValue(CXPathTerm* typedValueSequence);

	void ResetChildren();
	void ResetAttributes();

};
