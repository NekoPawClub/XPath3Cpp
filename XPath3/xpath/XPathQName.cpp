#include "StdAfx.h"
#include "xpathqname.h"

CXPathQName::CXPathQName(void){
	this->Namespace = "";
	this->LocalName = "";
}

CXPathQName::CXPathQName(string ns, string name):
	Namespace(ns), LocalName(name) {}

bool CXPathQName::operator==(CXPathQName other){
	return ( (this->LocalName==other.LocalName) && 
			 (this->Namespace==other.Namespace) );
}

bool CXPathQName::operator<(CXPathQName other){
	return ((this->Namespace==other.Namespace)?
			(this->LocalName<other.LocalName):
			(this->Namespace<other.Namespace));
}

bool CXPathQName::operator>(CXPathQName other){
	return ((this->Namespace==other.Namespace)?
			(this->LocalName>other.LocalName):
			(this->Namespace>other.Namespace));
}