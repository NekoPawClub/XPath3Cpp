#include "StdAfx.h"
#include "evalengine.h"
#include <string>
#include <iostream>

//fn:position() as unsignedInt?
//s15.2
//Returns an unsignedInt indicating the position of the context item within 
//the sequence of items currently being being processed. Returns the 
//empty sequence is the context is the empty sequence.
CXPathSequence* CEvalEngine::FuncPosition(CXPathSequence *input, XPathFunction *func){
cout << "CALL SUCCESS " << endl;
	return new CXPathSequence();
}