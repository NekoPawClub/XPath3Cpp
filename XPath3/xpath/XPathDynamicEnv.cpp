#include "StdAfx.h"
#include "xpathdynamicenv.h"

CXPathDynamicEnv::CXPathDynamicEnv(
					CXPathStaticEnv *staticEnv,
					CXPathSequence *input,
					CXPathSequence *result) {
	if(!input->IsEmpty()){

		this->Focus = input;
		this->ContextItem.push_back(input->GetItemAt(0));
		this->ContextPosition.push_back(0);
		this->ContextSize.push_back(input->GetSize());
		this->FilterMode = false;

	}
	this->StatEnv = staticEnv;
	this->Init();
}

CXPathDynamicEnv::~CXPathDynamicEnv(void)
{
}

void CXPathDynamicEnv::Init(void) {
//	this->InitVariables();
}

void CXPathDynamicEnv::InitVariables(void) {
/*
	//pointers to types
	CXSDType *ptItem = StatEnv->Types.find("dm:item")->second;
	CXSDType *ptNode = StatEnv->Types.find("dm:node")->second;

	CXPathVar *var = new CXPathVar();
	var->LocalName = "dot";
	var->Namespace = "fs";
//	var->Type = ptItem;
	var->Value.itemVal = NULL;
	this->Variables["fs:dot"] = var;
*/	
}

void CXPathDynamicEnv::SetVariable(string qname, CXPathSequence *val){
	CXPathVar *var = new CXPathVar();
	var->QName = qname;
	var->Value.seqVal = val;
	this->Variables[qname] = var;
}

void CXPathDynamicEnv::SetVariable(string qname, CXPathItem *val){
	CXPathVar *var = new CXPathVar();
	var->QName = qname;
	var->Value.itemVal = val;
	this->Variables[qname] = var;
}