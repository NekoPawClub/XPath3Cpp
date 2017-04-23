#include "StdAfx.h"
#include "xpathstaticenv.h"
#include "EvalEngine.h"

bool XPathFuncArity::operator == (XPathFuncArity other){
	if(this->size()==0 && other.size()!=0) return false;
	if(this->size()!=other.size()) return false;
	else {
		int count = 0;
		while(count < (int) (this->size()-1) ){
			if(*(this->at(count))!=*(other.at(count)))
				return false;
			count++;
		}
	}
	return true;
}

CXPathStaticEnv::CXPathStaticEnv(void)
{
	this->Init();
}

CXPathStaticEnv::~CXPathStaticEnv(void)
{
}

void CXPathStaticEnv::Init(){
	this->InitTypes();
	this->InitFunctions();	
}

void CXPathStaticEnv::InitTypes(){
	this->Types["xs:anySimpleType"] = new CXSDType("xs","anySimpleType");
	this->Types["xs:anyType"] = new CXSDType("xs","anyType");
	this->Types["xs:string"] = new CXSDType("xs","string");
	this->Types["xs:integer"] = new CXSDType("xs","integer");
	this->Types["xs:decimal"] = new CXSDType("xs","decimal");
	this->Types["xs:double"] = new CXSDType("xs","double");

	this->Types["dm:sequence"] =  new CXSDType("dm","sequence");
	this->Types["dm:item"] = new CXSDType("dm","item");
	this->Types["dm:node"] = new CXSDType("dm","node");
}

void CXPathStaticEnv::InitFunctions(){
	//pointers to types
	CXSDType *ptItem = this->Types.find("dm:item")->second;
	CXSDType *ptNode = this->Types.find("dm:node")->second;
	CXSDType *ptInt = this->Types.find("xs:integer")->second;

	//**function -- fn:root
	this->Functions["root"] = new XPathFuncSigs();
	map<string,XPathFuncSigs*>::iterator it = this->Functions.find("root");
	//fn:root() as node
	it->second->Signatures.push_back(new XPathFunction("fn","root",ptNode));
	//fn:root($srcval as node) as node
	it->second->Signatures.push_back(new XPathFunction("fn","root",ptNode));
	it->second->Signatures.back()->Arity.push_back(ptNode);

	//**function -- fn:position
	XPathFuncSigs *pFunc = this->Functions["position"] = new XPathFuncSigs();
//	map<string,XPathFuncSigs*>::iterator it = this->Functions.find("position");
	//fn:position() as unsignedInt?
	pFunc->Signatures.push_back(new XPathFunction("fn","position",ptInt));
	pFunc->Signatures.back()->PFunc = &CEvalEngine::FuncPosition;
}

XPathFunction* CXPathStaticEnv::GetFunction(string ns, string name, XPathFuncArity* arity){
	//find the set of functions matching the local name.
	map<string,XPathFuncSigs*>::iterator it = this->Functions.find(name);
	//if found iterate thru the function signatures until a match is found and return
	if(it!=this->Functions.end()){	
		vector<XPathFunction*>::iterator jt = it->second->Signatures.begin();
		for(jt;jt!=it->second->Signatures.end();jt++){
			if((*jt)->Namespace==ns){
				if( ((int) (*jt)->Arity.size()==0 && !arity) || 
					((*jt)->Arity == (*arity)) )
					return (*jt);
			}
		}
	}
	return NULL;
}