#include "Text.h"

using namespace std;

//special names for NS node (which are virtual by account
//of the fact that they need not appear with the physical 
//document.
static enum XPathNSNodeType
{
	ExpEType,
	ExpAName
};

//in accordance with namespace 1.0 spec. 
class CXPathNamespace 
{
public:
	CXPathNamespace();
	CXPathNamespace(CText sName, CText sURI);
	virtual ~CXPathNamespace();
private:
	XPathNSNodeType m_NSNodeType;	//local name of ns node 
	CText* m_Prefix;			//pointer to top of CXPathInScopeNamespaces
	CText* m_URI;				//as above
//	CXPathTerm* parent;

	void SetNamespaceNodeType();

public:
	CText GetNamespaceURI();
	CText GetNamespacePrefix();
	CText GetNamespaceNodeType();
	CText GetNamespaceElementType();  //parent element name
	CText GetNamespaceAttributeName();
	

};