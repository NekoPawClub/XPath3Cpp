#include "StdAfx.h"
#include "evalengine.h"
#include <string>
#include <iostream>

CXPathSequence* CEvalEngine::Evaluate(CExprTreeNode * n, CXPathSequence *input){
	vector<CExprTreeNode*>::iterator itExprNode;
	vector<CXPathItem*>::iterator itItem;
	CXPathSequence *result = new CXPathSequence(); //return 
	CXPathSequence *seqFocus = new CXPathSequence(); //focus
	CXPathSequence *tempSeq = new CXPathSequence();
	CXPathNode *tempNode, *contextNode;
	
	XPathTokenName axis;
	switch(n->Kind){
		case ETNK_STEP : // "/"
			itItem = input->m_Items->begin();
			for(itItem;itItem!=input->m_Items->end();itItem++){
				if((*itItem)->IsNode()){
					if(n->IsExprStart){
						//must append context item to a node so that child
						//nodes can be accessed. 
						//***TO DO*** check operation of step axis on sequence items.
						tempNode = new CXPathNode();
						contextNode = dynamic_cast<CXPathNode*> (*itItem);
						if(n->IsRoot) {
							contextNode->m_OwnerDocument = tempNode;
							tempNode->m_Children.push_back(contextNode);
						}
						else {
							tempNode->m_Children.push_back(contextNode->m_OwnerDocument);
						}
						seqFocus->m_Items->push_back(tempNode);
					}
					else seqFocus->m_Items->push_back(*itItem);
					if(n->Children.size()==(unsigned int)1) //will be forward or reverse axis
						result->ConcatSequence(this->Evaluate(n->Children.at(0),seqFocus));
					seqFocus->m_Items->clear();
				}
				else ; //error;
			}
			break; 
		case ETNK_DOUBLE_STEP :
			itItem = input->m_Items->begin();
			for(itItem;itItem!=input->m_Items->end();itItem++){
				if((*itItem)->IsNode()){
					// "//" at start of expression means.../descendant-or-self::node()
					if(n->IsExprStart){
						tempNode = new CXPathNode();
						contextNode = dynamic_cast<CXPathNode*> (*itItem);
						if(n->IsRoot) {
							contextNode->m_OwnerDocument = tempNode;
							tempNode->m_Children.push_back(contextNode);
						}
						else {
							tempNode->m_Children.push_back(contextNode->m_OwnerDocument);
						}
						seqFocus->m_Items->push_back(tempNode);
					}
					else seqFocus->m_Items->push_back(*itItem);

					vector<CXPathItem*>::iterator jtItem;
					//gather descendants of context sequence
					jtItem = seqFocus->m_Items->begin();
					for(jtItem;jtItem!=seqFocus->m_Items->end();jtItem++){
						tempNode = dynamic_cast<CXPathNode*> (*jtItem);
						tempSeq->m_Items->push_back(*jtItem);
						tempSeq->ConcatSequence(tempNode->GetDescendants());
					}
					seqFocus->m_Items->clear();

					//evaluate for each node in focus.
					jtItem = tempSeq->m_Items->begin();
					for(jtItem;jtItem!=tempSeq->m_Items->end();jtItem++){
						seqFocus->m_Items->push_back(*jtItem);
						if(n->Children.size()==(unsigned int)1) //will be forward or reverse axis
							result->ConcatSequence(Evaluate(n->Children.at(0),seqFocus));
						seqFocus->m_Items->clear();
					}
				}
				else ; //error;
			}
			result->SortItemsByDocOrder();
			break; 
		case ETNK_FORWARD_AXIS:
			axis = n->GetAxisName();
			if(n->IsRoot){
				//see ETNK_STEP for explanation
				CXPathNode *tempNode = new CXPathNode();
				itItem = input->m_Items->begin();
				for(itItem;itItem!=input->m_Items->end();itItem++){
					if((*itItem)->IsNode())
						tempNode->m_Children.push_back(dynamic_cast<CXPathNode*> (*itItem));
					else ; //error
				}
				seqFocus->m_Items->push_back(tempNode);
			}
			else seqFocus = input;
			switch(n->Children.at(0)->Kind){
				case ETNK_NAME_TEST :
					result = DoNameTest(seqFocus,axis,n->Children.at(0));
					break;
				case ETNK_NODE_TEST : 
					result = DoNodeTest(seqFocus,axis,n->Children.at(0)->Token->Token->second->Name);
					break;
			}
			for(itExprNode = n->Siblings.begin();itExprNode!=n->Siblings.end();itExprNode++){
				result = this->Evaluate(*itExprNode,result);
			}
			break;			
		case ETNK_REVERSE_AXIS :
			;
		case ETNK_NAME_TEST :
			;
		case ETNK_NODE_TEST :
			;
		case ETNK_LITERAL :
			;
		case ETNK_EXPR_SEQUENCE :
			;
		case ETNK_FUNCTION_CALL :
			itItem = input->m_Items->begin();
			for(itItem;itItem!=input->m_Items->end();itItem++){
				seqFocus->m_Items->push_back(*itItem);
				XPathFunction* pFunc = nullptr;
				XPathFuncArity *pFuncArity; //**TO DO** evaluation arguements here
				if((int)n->Token->Info.size()==2) //qname call
					pFunc = this->StatEnv->GetFunction(
						*(n->Token->Info.at(0).Value.sVal),
						*(n->Token->Info.at(1).Value.sVal),
						NULL);
				else if((int)n->Token->Info.size()==1) //localname call
					pFunc = this->StatEnv->GetFunction("",
						*(n->Token->Info.at(1).Value.sVal),
						NULL);
				result->ConcatSequence(((*this).*(pFunc->PFunc))(seqFocus,pFunc)); //call function
				seqFocus->m_Items->pop_back();
			}
			break;
		case ETNK_PATH_EXPR :
			;
		case ETNK_VARIABLE :
			;
		case ETNK_PREDICATE :
			itItem = input->m_Items->begin();
			for(itItem;itItem!=input->m_Items->end();itItem++){
				seqFocus->m_Items->push_back(*itItem);
				if(n->Children.size()==(unsigned int)1)
					if(Evaluate(n->Children.at(0),seqFocus)->GetSize()!=0)
						result->AppendItem(*itItem);
				seqFocus->m_Items->pop_back();
			}
			break;
	}
	return result;
}

// "/" at the start of an expression is an abreviation for 
// fn:root(self::node()) ... this function returns the result
// of this function.
CXPathNode* CEvalEngine::GetRootContextNode(){
	CXPathItem* item = this->DynamicEnv->ContextItem.back();
	if(item->IsNode()){
		CXPathNode *node  = dynamic_cast<CXPathNode*> (item);
		if(!node->GetOwnerDocument()) return node;
		else return node->GetOwnerDocument();	
	}
	else return NULL; //error
}

CXPathSequence* CEvalEngine::DoNameTest(CXPathSequence *input, XPathTokenName axis, CExprTreeNode *exprTreeNode){
	CXPathSequence *result = new CXPathSequence();
	CXPathNode *node;
	CText name;
	string sVal;
	vector<CXPathItem*>::iterator itItem = input->m_Items->begin();
	while(itItem!=input->m_Items->end()){	
		if((*itItem)->IsNode()) {
			node = dynamic_cast<CXPathNode*> (*itItem);
			vector<CXPathNode*>::iterator itNode;
			switch(axis) {
				case CHILD_AXIS:
					itNode = node->m_Children.begin();
					while(itNode!=node->m_Children.end()){
						switch(exprTreeNode->Token->Token->second->Name){ //must adjust identifiers
							case LOCALNAME: 
								sVal = *(exprTreeNode->Token->Info.back().Value.sVal);
								name = CText(sVal.c_str());
								if(name.Compare((*itNode)->GetLocalName())==0)
									result->m_Items->push_back(*itNode);
								break;
							case QNAME: 
								break;
							case WILDCARD:
								if((*itNode)->GetNodeType()!=TEXT_NODE)
									result->m_Items->push_back(*itNode);
								break;
							case LOCALNAME_WILDCARD: 
								break;
							case NAMESPACE_WILDCARD: 
								break;
						}
						itNode++;
					}
					break;
				case ATTRIBUTE_AXIS:
					itNode = node->m_Attributes.begin();
					while(itNode!=node->m_Attributes.end()){
						switch(exprTreeNode->Token->Token->second->Name){ //must adjust identifiers
							case LOCALNAME: 
								sVal = *(exprTreeNode->Token->Info.back().Value.sVal);
								name = CText(sVal.c_str());
								if(name.Compare((*itNode)->GetLocalName())==0)
									result->m_Items->push_back(*itNode);
								break;
							case QNAME: 
								break;
							case WILDCARD: 
								result->m_Items->push_back(*itNode);
								break;
							case LOCALNAME_WILDCARD: 
								break;
							case NAMESPACE_WILDCARD: 
								break;
						}
						itNode++;
					}
					break;
			}
		}
		else ; //error
		itItem++;
	}
	return result;
}

CXPathSequence* CEvalEngine::DoNodeTest(CXPathSequence* input, XPathTokenName axis, XPathTokenName kind){
	CXPathSequence *result = new CXPathSequence();
	CXPathNode *node;
	vector<CXPathItem*>::iterator itItem = input->m_Items->begin();
	while(itItem!=input->m_Items->end()){
		if((*itItem)->IsNode()) {
			node = dynamic_cast<CXPathNode*> (*itItem);
			vector<CXPathNode*>::iterator itNode;
			switch(axis){
				case CHILD_AXIS:
					switch(kind){
						case ANY_NODE_TEST :
							itNode = node->m_Children.begin();
							while(itNode!=node->m_Children.end()){
								result->m_Items->push_back(*itNode);
								itNode++;
							}
							break;
						case TEXT_NODE_TEST :
							itNode = node->m_Children.begin();
							while(itNode!=node->m_Children.end()){
								if((*itNode)->GetNodeType()==TEXT_NODE)
									result->m_Items->push_back(*itNode);
								itNode++;
							}
							break;
						case COMMENT_NODE_TEST :
							break;
						case PI_NODE_TEST :
							break;
					}
				break;
			}
		}
		itItem++;
	}
	return result;	
}

//set variable $dot to node item in input sequence
void CEvalEngine::SetDot(void){
	this->DynamicEnv->SetVariable("fs:dot",this->DynamicEnv->ContextItem.back());
//	if( this->Input->IsSingleton() && 
//		this->Input->GetItemAt(0)->IsNode())
//		this->DynamicEnv->Variables["fs:dot"]->Value.itemVal = this->Input->GetItemAt(0);
//	else
//		this->DynamicEnv->Variables["fs:dot"]->Value.itemVal = NULL;
//	this->DynamicEnv->Variables["fs:dot"]
}
