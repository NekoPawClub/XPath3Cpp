#pragma once

#include <string>

using namespace std;

class CXSDType{
public:
	CXSDType(void);
	CXSDType(string ns, string name);
	virtual ~CXSDType(void);
	string Namespace;
	string LocalName;
	string Definition;

	bool operator == (CXSDType other);
	bool operator != (CXSDType other);
};
