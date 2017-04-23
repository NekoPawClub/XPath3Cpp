#ifndef _XPATHNODE_H
#define _XPATHNODE_H

#include "XPathSequence.h"
#include <vector>
#include <map>

using namespace std;

class CXPathNode : public CXPathItem {
friend class CEvalEngine;
public:
    CXPathNode();
	CXPathNode(CText sVal,
			   CText sXSDType,
			   long docOrder,
			   CXPathNode* ownerDoc,
			   CText xmlBase,
			   XPathNodeType nodeType,
			   CText qName,
			   CXPathNode* parent);
			  
   virtual ~CXPathNode();

private:

	long			m_DocumentOrder;
	CXPathNode*		m_OwnerDocument;
	CText			m_XMLBase;
	CText			m_QName;
	XPathNodeType	m_NodeType;
	CXPathNode*		m_Parent;
	CText			m_Namespace;

	std::map<string, CXPathNode*> m_Attributes_map;
	vector<CXPathNode*> m_Attributes;
	vector<CXPathNode*> m_Children;

	CXPathSequence *m_TypedValue;

public:

	bool IsNode()const;
	bool Is(CXPathItem *item);

	XPathNodeType GetNodeType() const;
	void SetNodeType(XPathNodeType type);

	void SetNamespace(CText ns);
	CText GetNamespace();

	long GetDocumentOrder() const;
	void SetDocumentOrder(long order);
	CXPathNode* GetOwnerDocument();
	void UpdateOwnerDocument(CXPathNode* owner);

	CText GetQName(); 		// returns Qname
	void SetQName(CText qName);
	CText GetLocalName();		// returns Local Name
	CText GetExpandedQName();	// returns Expanded-QName	(ns URI:LocalName)
   
	CXPathNode* GetChildAt(int i); // return child CXPathTerm instance at index, i
	CXPathSequence* GetChildren(void);
	CXPathSequence* GetDescendants(void);
	CXPathNode* GetAttributeAt(int i); // return attribute (CXPathTerm instance) at index, i
	CXPathNode* GetAttributeByName(string name);
	CXPathNode* GetParent(void); 
	void SetParent(CXPathNode* parent);

	void AppendChild(CXPathNode*);
	void AppendAttribute(CXPathNode*);

	//Mutators
	void SetChildAt(int i, CXPathNode *x); // set child CXPathTerm instance at index, i
	void SetAttributeAt(int i, CXPathNode *x); // set attribute (CXPathTerm instance) at i

	void setNSNode(void);
	//void setStringValue(CText sVal);

	void ResetChildren();
	void ResetAttributes();

	bool HasChildNodes(void);
	bool HasAttributes(void);

	CText ToString(void);

};

#endif