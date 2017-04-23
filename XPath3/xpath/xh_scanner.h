#ifndef __MARKUP
#define __MARKUP

//| 
//| simple and fast XML/HTML scanner/tokenizer
//|
//| (C) Andrew Fedoniouk @ terrainformatica.com
//|
// Modified by Gerallt 2017.

#include <stdlib.h> // char_t
#include <string.h> // strcmp, etc.
#include <string>
#include <map>
#include <sstream>

using namespace std;

namespace markup 
{
  //typedef char_t char;

  struct instream 
  {
    virtual char get_char() = 0;
  };

  struct XmlStream : public markup::instream
  {
	  const char* p;
	  const char* end;

	  XmlStream(const char* src) : p(src), end(src + strlen(src)) {}
	  virtual char get_char() { return p < end ? *p++ : 0; }
  };

  enum XmlToken
  {
	TT_ERROR = -1,
	TT_EOF = 0,
	TT_TAG_START,   // <tag ...
					  //     ^-- happens here
	TT_TAG_END,     // </tag>
					//       ^-- happens here 
					// <tag ... />
					//            ^-- or here 
	TT_ATTR,        // <tag attr="value" >      
					//                  ^-- happens here   
	TT_WORD,
	TT_SPACE,

	TT_DATA,        // content of followings:

	TT_COMMENT_START, TT_COMMENT_END, // after "<!--" and "-->"
	TT_CDATA_START, TT_CDATA_END,     // after "<![CDATA[" and "]]>"
	TT_PI_START, TT_PI_END,           // after "<?" and "?>"
	TT_ENTITY_START, TT_ENTITY_END,   // after "<!ENTITY" and ">"
  };

  class XmlScanner
  {
  public:


    enum $ { MAX_TOKEN_SIZE = 1024, MAX_NAME_SIZE = 128 };

  public:
  
	  XmlScanner(instream& is):
        input(is), 
        input_char(0), 
        value_length(0), 
        tag_name_length(0), 
        attr_name_length(0),
		got_tail(false) { c_scannerFunct = &XmlScanner::scan_body; }

    // get next token
	XmlToken GetToken();

	// Return if the scanner is at the end-of-file
	bool Eof();

	// get the depth within the dom
	long GetDepth();

	// get the order in the document
	long GetOrder();

	// get the order of the element in the document
	long GetElemOrder();
    
    // get value of TT_WORD, TT_SPACE, TT_ATTR and TT_DATA
    const string GetValue();
      
    // get attribute name
    const string GetAttrName();
    
    // get tag name
    const string GetTagName();
    
    // should be overrided to resolve entities, e.g. &nbsp;
    virtual char resolve_entity(const char* buf, int buf_size) { return 0; }
        
  private: /* methods */

    typedef XmlToken (XmlScanner::*scan)();
    scan        c_scannerFunct; // current 'reader'

    // content 'readers'
    XmlToken  scan_body();
    XmlToken  scan_head();
    XmlToken  scan_comment();
    XmlToken  scan_cdata();
    XmlToken  scan_pi();
    XmlToken  scan_tag();
    XmlToken  scan_entity_decl();

    char       skip_whitespace();
    void        push_back(char c);
  
    char       get_char();
    char       scan_entity();

    bool        is_whitespace(char c);
      
    void        append_value(char c);
    void        append_attr_name(char c);
    void        append_tag_name(char c);

  private: /* data */

	bool eof = false;

    //enum state { TEXT = 0, MARKUP = 1, COMMENT = 2, CDATA = 3, PI = 4 };
    //state       where;
    XmlToken  token;

	long depth = 0; // the current depth in the dom
	long documentOrder = -1; // the unique identifier of the node in the document
	long elemOrder = -1; // the unique identifier of the element node in the document

	string value;
	//char        value[MAX_TOKEN_SIZE];     //char       value[MAX_TOKEN_SIZE];
	int         value_length;

    char        tag_name[MAX_NAME_SIZE];
    int         tag_name_length;

    char        attr_name[MAX_NAME_SIZE];
    int         attr_name_length;
  
    instream&   input;
    char       input_char; 

    bool        got_tail; // aux flag used in scan_comment, etc. 

  };
}

#endif