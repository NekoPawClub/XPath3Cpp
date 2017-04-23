#pragma once

#include <string>
#include <vector>
#include <map>

#include "XSDType.h"
#include "XPathVar.h"

using namespace std;

//NB: have made functions in this structure, so that functions
//from different namespaces can share the same record in the
//Function member...this might make the search for functions
//more effective.
//CXPathStaticEnv
//	.Functions (1..*) 
//		-> Signatures (1..*)
//				-> Function
//					.Namespace
//					.LocalName
//					.ReturnType (1)
//						-> CXSDType
//							.Namespace
//							.LocalName
//							.Definition
//					.Arity (0..*)
//						-> CXSDType
//	...

struct XPathFuncArity : public std::vector<CXSDType*> {
	~XPathFuncArity(){
		for(iterator it=begin();it!=end();it++)
			delete *it;
	}
public:
	bool operator== (XPathFuncArity other);
};

struct XPathFunction {
	XPathFunction(string ns, string ln, CXSDType *rt):
		Namespace(ns), LocalName(ln), ReturnType(rt){}
	string Namespace; //this should point to an inscope ns
	string LocalName; 
	CXSDType *ReturnType;
	XPathFuncArity Arity;
	CXPathSequence* (CEvalEngine::*PFunc)(CXPathSequence* input, XPathFunction* func);
};

//A function with a name can be over-ridden.
struct XPathFuncSigs {
	vector<XPathFunction*> Signatures;
};

typedef pair <string, XPathFuncSigs*> String_FuncSignature_Pair;
typedef pair <string,CXSDType*> String_Type_Pair;

class CXPathStaticEnv{
public:
	CXPathStaticEnv(void);
	virtual ~CXPathStaticEnv(void);

	//all types available to the expression;
	map<string, CXSDType*> Types;

	//See all variable info in dynamic environment

	//allows multiple function of the same name
	//differing only in the number and type
	//of the arguments.
	map<string, XPathFuncSigs*> Functions;

	void Init();
	void InitTypes();
	void InitFunctions();

	XPathFunction* GetFunction(string ns, string name, XPathFuncArity* arity);

};
