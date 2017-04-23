// XPathTermDriver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "XPathSequence.h"
#include "XPathNode.h"
#include "EvalEngine.h"
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
//	cout << "enum:" << XPathNT.ExprSequence << endl;

	CXPathSequence *input  = new CXPathSequence();
	CXPathSequence *result = new CXPathSequence();
	/*
		<mydoc>
			<mynode myattr="123"/>
			<mynode myattr="456"/>
		</mydoc>
	

	CXPathNode *p, *n, *a;
	p = new CXPathNode("","xs:anyType",1,"","",DOCUMENT_NODE,"mydoc",NULL);
	n = new CXPathNode("","xs:anyType",2,"","",ELEMENT_NODE,"mynode",p);
	p->AppendChild(n);
	a = new CXPathNode("123","xs:anySimpleType",3,"","",ATTRIBUTE_NODE,"myattr",n);
	n->AppendAttribute(a);
	n = new CXPathNode("","xs:anyType",4,"","",ELEMENT_NODE,"mynode",p);
	p->AppendChild(n);
	a = new CXPathNode("456","xs:anySimpleType",5,"","",ATTRIBUTE_NODE,"myattr",n);
	n->AppendAttribute(a);

	input->AppendItem(p);
*/

/*	<mydoc>
		<mynode foo="bar">
			<mychild>101</mychild>
		</mynode>
		<mynode foo="bar2">
			<mychild>202</mychild>
		</mynode>
		<data>
			<datachild attr1="1" attr2="2" />
		</data>
		<data>
			<datachild attr1="1" />
		</data>
		<mynode/>
	</mydoc>
	,
	<mydoc>
		<mynode/>
	<mydoc>
*/

	CXPathNode *d, *n, *a, *c, *t;

	d = new CXPathNode("","xs:anyType",1,NULL,"",DOCUMENT_NODE,"mydoc",NULL);
	n = new CXPathNode("","xs:anyType",2,d,"",ELEMENT_NODE,"mynode",d);
	d->AppendChild(n);
	a = new CXPathNode("bar","XS:anySimpleType",3,d,"",ATTRIBUTE_NODE,"foo",n);
	n->AppendAttribute(a);
	c = new CXPathNode("101","xs:anyType",4,d,"",ELEMENT_NODE,"mychild",n);
	n->AppendChild(c);
	t = new CXPathNode("101","xs:anyType",5,d,"",TEXT_NODE,"",c);
	c->AppendChild(t);

	n = new CXPathNode("","xs:anyType",6,d,"",ELEMENT_NODE,"mynode",d);
	d->AppendChild(n);
	a = new CXPathNode("bar2","XS:anySimpleType",7,d,"",ATTRIBUTE_NODE,"foo",n);
	n->AppendAttribute(a);
	c = new CXPathNode("202","xs:anyType",8,d,"",ELEMENT_NODE,"mychild",n);
	n->AppendChild(c);
	t = new CXPathNode("202","xs:anyType",9,d,"",TEXT_NODE,"",c);
	c->AppendChild(t);

	n = new CXPathNode("","xs:anyType",10,d,"",ELEMENT_NODE,"data",d);
	d->AppendChild(n);
	c = new CXPathNode("202","xs:anyType",11,d,"",ELEMENT_NODE,"datachild",n);
	a = new CXPathNode("1","XS:anySimpleType",12,d,"",ATTRIBUTE_NODE,"attr1",c);
	c->AppendAttribute(a);
	a = new CXPathNode("2","XS:anySimpleType",13,d,"",ATTRIBUTE_NODE,"attr2",c);
	c->AppendAttribute(a);
	n->AppendChild(c);

	n = new CXPathNode("","xs:anyType",14,d,"",ELEMENT_NODE,"data",d);
	d->AppendChild(n);
	c = new CXPathNode("202","xs:anyType",15,d,"",ELEMENT_NODE,"datachild",n);
	a = new CXPathNode("1","XS:anySimpleType",16,d,"",ATTRIBUTE_NODE,"attr1",c);
	c->AppendAttribute(a);
	n->AppendChild(c);

	n = new CXPathNode("","xs:anyType",17,d,"",ELEMENT_NODE,"mynode",d);
	d->AppendChild(n);
	t = new CXPathNode("303","xs:anyType",18,d,"",TEXT_NODE,"",d);
	d->AppendChild(t);
	input->AppendItem(d);

//	d = new CXPathNode("","xs:anyType",1,NULL,"",DOCUMENT_NODE,"mydoc",NULL);
//	n = new CXPathNode("","xs:anyType",2,d,"",ELEMENT_NODE,"mynode",d);
//	d->AppendChild(n);
//	input->AppendItem(d);

// FOR TESTING EVALUATION

//	DoXPath(input, CText("mydoc/data/datachild") ,result);
//DoXPath(input, CText("mydoc/data/datachild[@attr2]") ,result);
//DoXPath(input, CText("mydoc/mynode") ,result);
//DoXPath(input, CText("/mydoc/data[datachild/@attr2]") ,result);
//DoXPath(input, CText("/bob") ,result);
	DoXPath(input, CText("//*[fn:position()]") ,result);



//	DoXPath(input, CText("/mynode/@myattr") , result);

	//THE FOLLOWING EXPRESSIONS HAVE BEEN PARSED
//	DoXPath(input, CText("/mynode/@myattr") , result); //OK
//	DoXPath(input, CText("//mynode/@myattr") , result); //OK
//	DoXPath(input, CText("/mynode/mynode2//@myattr") , result); //OK
//	DoXPath(input, CText("/mynode//mynode2/@myattr") , result); //OK
//	DoXPath(input, CText("/mynode/mynode2[mypred]") , result); //OK
//	DoXPath(input, CText("/mynode[mypred][mypred]/parent::myparent") , result);
//	DoXPath(input, CText("/mynode[mynode/@myattr]") , result);

	//DoXPath(input, CText("/*/mynode[anelem]/@myns:myattr") , result);
	//cout << "IS SINGLETON: " << input->IsSingleton() << endl << endl;
/*
	DoXPath(input, CText("/child::node()//$myvar"), result);

	cout << endl << "===============================" << endl;

	DoXPath(input, CText("$myvar * 20"), result);

	cout << endl << "===============================" << endl;

	DoXPath(input, CText("(1,2,3,4)"), result);

	cout << endl << "===============================" << endl;

	DoXPath(input, CText("/ancestor-or-self::myns:mynode"), result);

	cout << endl << "===============================" << endl;

	DoXPath(input, CText("fn:root(self::node())/mynode/@myattr"), result);

	cout << endl << "===============================" << endl;

	DoXPath(input, CText("/mynode/@myattr"), result);

	cout << endl << "===============================" << endl;

	DoXPath(input, CText("//20"), result);

	cout << endl << "===============================" << endl;

	DoXPath(input, CText("//ns:mynode()"), result);

	cout << endl << "===============================" << endl;

	DoXPath(input, CText("('str1','str2',100)"), result);

	cout << endl << "===============================" << endl;
*/



		// initialise a new instance of the XPath Expression Evaluator
//	CEvalEngine *EE = new CEvalEngine();

	//testing access of the various lexical state dictionaries
//	map<string,CExprToken*>::iterator it = EE->DictDefault.find("(");
//	cout << it->second->Info.sVal << endl;
//	cout << EE->DictDefault["("]->Info.sVal << endl;
//	it = EE->DictDefault.find(")");
//	cout << it->second->Type << endl;
	//search fail == end() iterator
//	cout << ((EE->DictDefault.find(")")==EE->DictDefault.end())?false:true) << endl;

	/*
		TEST SEQUENCE
		(10, 1, "bob" , <elementA>, 12, <elementB>)
	*/
/*	
	CXPathSequence *sq = new CXPathSequence();

	CXPathItem* ints[6] = {
		new CXPathItem("10","xs:integer"),
		new CXPathItem("1","xs:integer"),
		new CXPathItem("bob","xs:string"),
		new CXPathNode("elementA","test",1,NULL,"",ELEMENT_NODE,"test",NULL),		
		new CXPathItem("12","xs:integer"),
		new CXPathNode("elementB","test",1,NULL,"",ELEMENT_NODE,"test",NULL)	
	};

	
	for(int i=0;i<6;i++) sq->AppendItem(ints[i]);

	//TEST 0
	cout << endl << "*** TEST 0 ***" << endl;
	cout << "for $i in (10,1,'bob', elementA, 12, elementB) is-node($i) ==>" << endl;
	for(int i=0;i<6;i++) 
		cout << "is node " << i << ":  " << sq->GetChildAt(i)->IsNode() << endl;

	//TEST 1
	cout << endl << "*** TEST 1 ***" << endl;
	cout << "atomic value in (10,1,'bob', elementA, 12, elementB) ==>" << endl;

	CXPathSequence *atomicSq = sq->GetAtomicValues();

	for(i=0;i<4;i++)
		cout << "item value:" << LPSTR(atomicSq->GetChildAt(i)->GetStringValue()) << endl;

	//TEST 2
	cout << endl << "*** TEST 2 ***" << endl;
	cout << "atomic value of type xs:string in (10,1,'bob', elementA, 12, elementB) ==>" << endl;

	CXPathSequence *atomicStringSq = sq->GetAtomicValues(CText("xs:string"));
	cout << "item value:" << LPSTR(atomicStringSq->GetChildAt(0)->GetStringValue()) << endl;

	//TEST 3
	cout << endl << "*** TEST 3 ***" << endl;
	cout << "element in (10,1,'bob', elementA, 12, elementB) ==>" << endl;

	CXPathSequence *elementSq = sq->GetNodesOfType(ELEMENT_NODE);

	for(i=0;i<2;i++)
		cout << "item value:" << LPSTR(elementSq->GetChildAt(i)->GetStringValue()) << endl;

*/
	
	//TESTING EXPRESSION EVALUATION
	//Test1

//	EE->ParseExpression("	//m:y     node/node()/m:y-othernode/bob.bob/100.[\"this is a \"\"test\"\"\"]['another quote test']");
//	EE->ParseExpression("//mynode/@*:myothernode"); //self::mynode
//	cout << "TEST 1." << endl;
//	EE->ParseExpression("'mynode'"); //self::mynode
//	cout << endl << "TEST 2." << endl;
//	EE->ParseExpression("//mynode/child::mychild/@myns:myattr"); //self::mynode

/*
	vector<int>::iterator iit = EE->Pass1Expr.begin();
	for(iit; iit!=EE->Pass1Expr.end();iit++){
		cout << *iit << " " << EE->GetLexemeTokenString(*iit) << endl;
	}
	
	cout << endl << "XPATH_NUM_KNOWN_BASIC_TOKENS: " << XPATH_NUM_KNOWN_BASIC_TOKENS << endl;

	for(iit = EE->Pass2Expr.begin(); iit!=EE->Pass2Expr.end();iit++){
		cout << *iit << " " << EE->GetLexemeTokenString(*iit) << endl;
	}
*/
	while(true);
	return 0;
}

