#ifndef _XPATHITEM_H
#define _XPATHITEM_H

#include "Text.h"

#include <string> //for testing purposes only

class CXPathItem 
{
public:
	CXPathItem();
	//CXPathItem(string sVal, string sXSDType);
	CXPathItem(CText sVal, CText sXSDType);
	~CXPathItem();
protected:
	//string m_StringValue;
	//string m_XSDType; //with eventually point to instance of CXSDType
	CText m_StringValue;
	CText m_XSDType; //with eventually point to instance of CXSDType

public:
	virtual std::string GetValue();
	virtual CText GetStringValue(void);
	virtual void SetStringValue(CText sVal);
	virtual CText GetXSDType(void);
	virtual void SetXSDType(CText type);
	virtual long GetDocumentOrder(void) const;
	virtual bool IsNode(void) const;
	virtual bool Is(CXPathItem *item) const;

	virtual CText ToString(void);
};

#endif