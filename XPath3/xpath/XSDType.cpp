#include "StdAfx.h"
#include "xsdtype.h"

CXSDType::CXSDType(void){
	Namespace = "";
	LocalName = "";
	Definition = "";
}

CXSDType::CXSDType(string ns, string name){
	Namespace = ns;
	LocalName = name;
	Definition = "";
}

CXSDType::~CXSDType(void)
{
}

bool CXSDType::operator == (CXSDType other) {
	return ( (Namespace == other.Namespace) && 
			 (LocalName == other.LocalName) )?true:false;
}

bool CXSDType::operator != (CXSDType other) {
	return !((*this)==other);
}