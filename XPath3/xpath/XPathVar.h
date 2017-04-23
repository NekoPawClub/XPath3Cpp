#pragma once

#include <string>
#include "XSDType.h"
#include "XPathSequence.h"

using namespace std;

class CXPathVar {
public:
	CXPathVar(void);
	virtual ~CXPathVar(void);
	string QName;
//	string Namespace;
//	string LocalName;
	union {
		CXPathSequence  *seqVal;
		CXPathItem		*itemVal;
	} Value;
};
