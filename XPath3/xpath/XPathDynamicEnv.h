#pragma once

#include <map>
#include <string>
#include "XPathVar.h"
#include "XPathQName.h"
#include "XPathStaticEnv.h"

using namespace std;

class CXPathDynamicEnv{
public:
	//CXPathDynamicEnv(void);
	CXPathDynamicEnv(CXPathStaticEnv *staticEnv,
					CXPathSequence *input,
					CXPathSequence *result);
	virtual ~CXPathDynamicEnv(void);

	CXPathStaticEnv *StatEnv;

	map<string, CXPathVar*> Variables;

	CXPathSequence* Focus; //this is the sequence that contains the context item
	vector<CXPathItem*> ContextItem;
	vector<int> ContextPosition;
	vector<int> ContextSize;

	bool FilterMode;

	void Init(void);
	void InitVariables(void);
	void SetVariable(string qname, CXPathSequence *val);
	void SetVariable(string qname, CXPathItem *val);
};
