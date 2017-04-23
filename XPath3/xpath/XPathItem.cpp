#include "stdafx.h"
#include "XPathItem.h"

using namespace std;

CXPathItem::CXPathItem()
{
	m_StringValue = "";
	m_XSDType = "xs:anySimpleType";
}

CXPathItem::CXPathItem(CText sVal, CText sXSDType)
{
	m_StringValue = sVal;
	m_XSDType = sXSDType;
}

CXPathItem::~CXPathItem(){}


std::string CXPathItem::GetValue()
{
	return std::string(m_StringValue);
}

//-- GetStringValue --------------------------------------------
//return the XML Schema type. Will always be of XSD type xs:string
CText CXPathItem::GetStringValue()
{
	return m_StringValue;
}

void CXPathItem::SetStringValue(CText sVal)
{
	m_StringValue = sVal;
}

//-- GetXSDType ------------------------------------------------
//return the XML Schema type
//!!!TO DO!!! make this return type XSDType
CText CXPathItem::GetXSDType()
{
	return m_XSDType;
}

void CXPathItem::SetXSDType(CText type)
{
	m_XSDType = type;
}

//-- GetDocumentOrder ------------------------------------------
//Atomic items do not have document order so return -1.
long CXPathItem::GetDocumentOrder(void) const {
	return -1;
}

//-- IsNode ----------------------------------------------------
bool CXPathItem::IsNode() const
{
	return false;
}

//-- Is --------------------------------------------------------
bool CXPathItem::Is(CXPathItem *item) const {
	return false;
}

//-- ToString --------------------------------------------------
//this function returns text representation of the item
CText CXPathItem::ToString() {
	return this->m_StringValue;
}