// Gerallt © 2017.

#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <vector>

#include "xml/rapidxml.hpp"
#include "xpath/XPathSequence.h"
#include "xpath/XPathNode.h"
#include "xpath/EvalEngine.h"
#include "XmlParserMemoryPool.h"

using namespace rapidxml;

#define DEBUG_INTERMEDIATES // IF defined, prints out the state of any intermediate data structures used in parsing

class XmlParser
{
public: /* Public Data */

	string* XmlBaseNamespace = nullptr;
	bool debugging = true;

	typedef pair<long, string>* AttributeValue;

private: /* Private Data */

	CXPathSequence* _input = nullptr;
	xml_document<> doc;
	CXPathNode* documentOwner;
	
public: /* Public Functions and Members */

	XmlParser();
	~XmlParser();

	// Parse XML from the specified filename.
	void Parse(string fileName);

	// Perform an XPath Query on the current DOM.
	string Query(string xpathExpression);

	// Perform an XPath Query on the current DOM. 
	CXPathSequence* Expr(string xpathExpression);

private: /* Private Helper Members */

	// Parse XML from the specified xml string. (using the rapidxml library)
	void parseXMLRapid(string* xml);

	// Parse XML from the specified xml string. (using inbuilt library)
	void parseXmlBuiltIn(string* xml);

	// Create a node and append to a DOM tree.
	void createNode(string qName, long level, long elemOrder, long order, Stack* context);

	// Create an attribute and append to the DOM tree.
	void createAttribute(long order, string name, string value, Stack* context);

	// Create a text node and append it to the DOM tree.
	void createTextNode(long order, string* value, Stack* context);

	// Creates an XPath node with the specified settings.
	CXPathNode* createXpNode(string qName, const string* value, XPathNodeType type, int level, int elemOrder, int order, CXPathNode* owner, CXPathNode* parent);

	// Collects attributes from xml_node and copies them into the specified CXPathNode
	void collectAttribs(CXPathNode* parent, xml_node<char> *xml, CText xmlBase, int* order);

	// Create a sequence of padding characters given the specified length and character
	string padding(string padding, int paddingLength);

	void trace(string m);
	void trace(string m, string m2);
	void trace(string m, string m2, string m3);
	void trace(string m, string m2, string m3, string m4);
	void trace(string m, string m2, string m3, string m4, string m5);
};

#endif