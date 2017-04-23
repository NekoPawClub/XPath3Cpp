// Gerallt © 2017.

#include <iostream>
#include <fstream>
#include <sstream>
#include "XmlParser.h"
#include "xpath/xh_scanner.h"

using namespace std;
using namespace markup;

//#define USE_RAPIDXML // IF defined, switches to the rapidxml library for parsing
#define PADDING_MULTIPLIER 4

XmlParser::XmlParser() {}

void XmlParser::Parse(string fileName) 
{
	string xmlText;
	ifstream* ifs = new ifstream(fileName);

	// READ
	if (ifs->is_open())
	{
		string line;
		stringstream ss;

		ss << ifs->rdbuf();

		xmlText = std::string(ss.str());
	}

	ifs->close();

	delete ifs;

	if (xmlText.size() == 0) 
	{
		cout << "level file returned no data" << endl;

		return;
	}

	// PARSE
#ifdef USE_RAPIDXML
	parseXMLRapid(&xmlText); // the old rapidxml parser that is having bad days
#else
	parseXmlBuiltIn(&xmlText); // my own implementation using a pull-parser technique adapted from Andrew Fedoniouk's xh_scanner
#endif
}

// Create a sequence of padding characters given the specified length and character
string XmlParser::padding(string padding, int paddingLength)
{
	string padded = string();
	int i;
	char* first = &padding.at(0);

	for (i = 0; i < paddingLength; i++)
	{
		padded.append(first);
	}

	return padded;
}

// Parse XML from the specified xml string. (using inbuilt library)
void XmlParser::parseXmlBuiltIn(string* xml)
{
	XmlStream si = XmlStream(&xml->at(0));
	XmlScanner sc = XmlScanner(si);;
	XmlToken t;
	int level;
	int depthPost;
	int depthPre;
	long order;
	long elemOrder;
	string spacing, spacing2, spacing3, spacing4;
	Stack work_items; // stack to maintain context of XmlScanner in tree
	dom_node* node;
	CXPathNode* xpNode;
	dom_node* docDomNode;
	string textNode;

	// Push-pull XML/html parser
	// Optimised to a single pass through with full XPath capability, boo yeah.

	// the owner document documentOwner will be later parsed as an automatic result from the aggregation of all TT_TAG_END tokens.
	documentOwner = new CXPathNode(); 
	documentOwner->SetNodeType(XPathNodeType::DOCUMENT_NODE);
	
	while (!sc.Eof())
	{
		depthPre = sc.GetDepth();

		t = sc.GetToken();

		elemOrder = sc.GetElemOrder();
		depthPost = sc.GetDepth();
		order = sc.GetOrder();

		if (t == XmlToken::TT_TAG_START)
		{
			level = depthPre;
		}
		else if (XmlToken::TT_TAG_END)
		{
			level = depthPost;
		}

		spacing = padding("*", level * PADDING_MULTIPLIER);
		spacing2 = padding("\t", level);
		spacing3 = spacing.append(padding("\t", level));
		spacing4 = padding(" ", level);

		switch (t)
		{
		case XmlToken::TT_ERROR:
			trace(spacing3, "ERROR", " error with dom fragment");

			break;
		case XmlToken::TT_TAG_START:

			trace(spacing3, "{ ", sc.GetTagName());

			// CREATE NODE
			createNode(sc.GetTagName(), level, elemOrder, order, &work_items);

			break;
		case XmlToken::TT_TAG_END:

			trace(spacing3, "} ", sc.GetTagName());

#ifdef DEBUG_INTERMEDIATES
			work_items.Print([](void* o) -> string
			{
				stringstream ss;
				dom_node* n = (dom_node*)o;
				ss << n->order;
				ss << "<" << n->name << ">";
				return ss.str();
			});
#endif

			break;
		case XmlToken::TT_ATTR:
			
			createAttribute(order, sc.GetAttrName(), sc.GetValue(), &work_items);

			trace(spacing3, "  -->", sc.GetAttrName(), "=", sc.GetValue());

			break;
		case XmlToken::TT_WORD:
			textNode = sc.GetValue();

			createTextNode(order, &textNode, &work_items);

			trace(spacing3, "  -->TEXTNODE:<", textNode, ">");
			
			break;
		case XmlToken::TT_SPACE:
			//trace(spacing3, "SPACING:", sc.GetValue());

			break;
		}
	}

	// PARSE COMPLETE
	_input = new CXPathSequence();
	_input->AppendItem(documentOwner);
}

// Build a node and append to a DOM tree
void XmlParser::createNode(string qName, long level, long elemOrder, long order, Stack* context)
{
	dom_node* node = new dom_node();
	node->order = elemOrder;
	node->name = qName;
	node->level = level;

	if (level > 0)
	{
		node->xpath = new CXPathNode(); // have to create now and use later to maintain single pass 
	}
	else
	{
		node->xpath = documentOwner;
	}

	CText xsType = CText("xs:anyType");
	CText ns = XmlBaseNamespace == nullptr ? CText("") : CText(&XmlBaseNamespace->at(0));
	CText qname = CText(&qName.at(0));
	node->xpath->SetQName(qname);
	node->xpath->SetNodeType(XPathNodeType::ELEMENT_NODE);
	node->xpath->SetNamespace(ns);
	node->xpath->SetXSDType(xsType);

	// setup relationships now while we have context so we can maintain a single parser pass

	dom_node *dnNode, *dnParent;
	CXPathNode *xpNode, *parent;
	if (context->IsEmpty())
	{
		// presumably the node is a root node and has no parent.
		parent = nullptr;
		dnParent = nullptr;
	}
	else
	{
		// Pull context of tokeniser from stack to find who the parent node should be

		Stack::node* previous = context->PeekNode();
		bool atParent = false;

		dnParent = (dom_node*)context->Peek();

		while (previous != nullptr && !atParent)
		{
			dnParent = (dom_node*)previous->data;
			
			atParent = dnParent->level < level;
			
			previous = previous->prev;
		}

		parent = dnParent->xpath;
		node->parent = dnParent;
		node->xpath->SetParent(parent);
		
		parent->AppendChild(node->xpath);
	}

	context->Push(node);


#ifdef DEBUG_INTERMEDIATES
	context->Print([](void* o) -> string
	{
		stringstream ss;
		dom_node* n = (dom_node*)o;
		ss << n->order;
		ss << "<" << n->name << ">";
		return ss.str();
	});
#endif
}

// Create an attribute and append to the DOM tree.
void XmlParser::createAttribute(long order, string name, string value, Stack* context)
{
	dom_node *dnNode;
	CXPathNode *xpNode, *parent;

	// Get the current node
	dnNode = ((dom_node*)context->Peek());
	xpNode = dnNode->xpath;
	parent = xpNode;

	// Update attributes in node
	CXPathNode *n;
	CText ct_qName = CText(&name.at(0));
	CText ct_value = CText(&value.at(0));
	CText ns = XmlBaseNamespace == nullptr ? CText("") : CText(&XmlBaseNamespace->at(0));

	n = new CXPathNode(ct_value, "XS:anySimpleType", order, documentOwner, ns, XPathNodeType::ATTRIBUTE_NODE, ct_qName, parent);
	
	// Append to DOM
	xpNode->AppendAttribute(n);
}

void XmlParser::createTextNode(long order, string* value, Stack* context)
{
	dom_node *dnNode;
	CXPathNode *xpNode, *parent;

	// Get the current node
	dnNode = ((dom_node*)context->Peek());
	xpNode = dnNode->xpath;
	parent = xpNode;

	// Create a text node
	CXPathNode *n;
	CText ct_value = CText(&value->at(0));
	CText ns = XmlBaseNamespace == nullptr ? CText("") : CText(&XmlBaseNamespace->at(0));

	n = new CXPathNode(ct_value, "XS:anySimpleType", order, documentOwner, ns, XPathNodeType::TEXT_NODE, "", parent);
	
	xpNode->SetStringValue(ct_value);

	// Append to DOM
	xpNode->AppendAttribute(n);
}

// Creates an XPath node with the specified settings.
CXPathNode* XmlParser::createXpNode(string qName, const string* value, XPathNodeType type, int level, int elemOrder, int order, CXPathNode* owner, CXPathNode* parent)
{
	CXPathNode* n;
	CText xsType = CText("xs:anyType");
	CText ns = XmlBaseNamespace == nullptr ? CText("") : CText(&XmlBaseNamespace->at(0));
	CText qname = CText(&qName.at(0));
	CText val = value == nullptr || (*value) == "" ? CText() : CText(&(*value).at(0));

	if (level > 0)
	{
		n = new CXPathNode(val, xsType, order, owner, ns, type, qname, parent);
	}
	else
	{
		n = owner;
		n->SetStringValue(val);
		n->SetXSDType(xsType);
		n->SetNamespace(ns);
		n->SetNodeType(type);
		n->SetDocumentOrder(0);
		n->SetQName(qname);
		n->SetParent(nullptr);
		n->UpdateOwnerDocument(nullptr);
	}

	

	return n;
}

// Collects attributes from xml_node and copies them into the specified CXPathNode
void XmlParser::collectAttribs(CXPathNode* parent, xml_node<char> *xml, CText xmlBase, int* order)
{
	CXPathNode *n;
	int docOrder = *order;
	xml_attribute<char>* attrib;
	CText qName; 
	CText value;

	attrib = xml->first_attribute();

	while (attrib != nullptr)
	{
		qName = CText(LPSTR(attrib->name()));
		value = CText(LPSTR(attrib->value()));
		n = new CXPathNode(value, "XS:anySimpleType", ++docOrder, NULL, "", XPathNodeType::ATTRIBUTE_NODE, qName, parent);
		parent->AppendAttribute(n);

		// NEXT
		attrib = attrib->next_attribute();
	}

	*order = docOrder;
}

// Parse XML from the specified xml string. (using the rapidxml library)
void XmlParser::parseXMLRapid(string* xmlText) 
{
	xml_node<char> *root, *n, *child, *sibling;
	Stack work_items;
	CXPathNode *d, *xp;
	int docOrder = 0;
	dom_node *dn, *dnn, *dnc, *dns;
	CXPathNode* parent;
	CText qName;
	CText value;
	CText xmlBase;
	node_type type;

	doc.parse<0>((char*)xmlText->c_str());
	root = doc.first_node();
	qName = CText(LPSTR(root->name()));
	value = CText(LPSTR(root->value()));
	xmlBase = "";

	d = new CXPathNode(value, "xs:anyType", ++docOrder, NULL, xmlBase, XPathNodeType::DOCUMENT_NODE, qName, NULL);
	collectAttribs(d, root, xmlBase, &docOrder);

	dn = new dom_node();
	dn->data = (void*)root;
	dn->xpath = d;

	work_items.Push((void*)dn);

	_input = new CXPathSequence();
	_input->AppendItem(d);
	
	// DFS TREE TRAVERSAL
	while (!work_items.IsEmpty())
	{
		dnn = (dom_node*)work_items.Pop();
		n = (xml_node<char>*)dnn->data;
		parent = dnn->xpath;
		type = n->type();
		qName = CText(LPSTR(n->name()));
		value = CText(LPSTR(n->value()));
		
		// ignores comments, data, doctype, declaration, pi
		
		if (type == node_type::node_element || type == node_type::node_document)
		{
			xp = new CXPathNode(value, "xs:anyType", ++docOrder, d, xmlBase, XPathNodeType::ELEMENT_NODE, qName, parent);

			collectAttribs(xp, n, xmlBase, &docOrder);
			parent->AppendChild(xp);

			// Setup next tree traversal iteration.
			sibling = n->next_sibling();
			child = n->first_node();

			// root -> child -> sibling -> sibling -> child
			if (sibling != nullptr)
			{
				dns = new dom_node();
				dns->xpath = parent;
				dns->data = (void*)sibling;

				work_items.Push((void*)dns);
			}

			if (child != nullptr)
			{
				dnc = new dom_node();
				dnc->xpath = parent;
				dnc->data = (void*)child;

				work_items.Push((void*)dnc);
			}
		}
		else if (type == node_type::node_cdata)
		{
			xp = new CXPathNode(value, "xs:anyType", ++docOrder, d, xmlBase, XPathNodeType::TEXT_NODE, qName, parent);

			parent->AppendChild(xp);
		}
		else 
		{
			// ignore adding other node types for now
		}

		// ^ keep processing all nodes until they are all parsed into CXPathNode types for XPath.
	}

	
}

string XmlParser::Query(string xpathExpression)
{
	CXPathSequence* result;

	result = Expr(xpathExpression);

	return result->ToString();
}

CXPathSequence* XmlParser::Expr(string xpathExpression) 
{
	if (_input == nullptr) return nullptr;

	CXPathSequence* result = new CXPathSequence();
	CText query = CText(LPSTR(xpathExpression.c_str()));

	result = DoXPath(_input, query);


	return result;
}

void XmlParser::trace(string m)
{
	if (debugging)
	{
		cout << m << endl;
	}
}

void XmlParser::trace(string m, string m2)
{
	if (debugging)
	{
		cout << m << m2 << endl;
	}
}

void XmlParser::trace(string m, string m2, string m3)
{
	if (debugging)
	{
		cout << m << m2 << m3 << endl;
	}
}

void XmlParser::trace(string m, string m2, string m3, string m4)
{
	if (debugging)
	{
		cout << m << m2 << m3 << m4 << endl;
	}
}

void XmlParser::trace(string m, string m2, string m3, string m4, string m5)
{
	if (debugging)
	{
		cout << m << m2 << m3 << m4 << m5 << endl;
	}
}

XmlParser::~XmlParser()
{
	delete _input;
}