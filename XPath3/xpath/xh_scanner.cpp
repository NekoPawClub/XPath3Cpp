#include "xh_scanner.h"
#include "string.h"
#include <ctype.h>

namespace markup 
{

  
    // case sensitive string equality test
    // s_lowcase shall be lowercase string
    inline bool equal(const string s, const string s1, size_t length)
    {
      switch(length)
      {
        case 8: if(s1[7] != s[7]) return false;
        case 7: if(s1[6] != s[6]) return false;
        case 6: if(s1[5] != s[5]) return false;
        case 5: if(s1[4] != s[4]) return false;
        case 4: if(s1[3] != s[3]) return false;
        case 3: if(s1[2] != s[2]) return false;
        case 2: if(s1[1] != s[1]) return false;
        case 1: if(s1[0] != s[0]) return false;
        case 0: return true;
        //default: return strncmp(s,s1,length) == 0;
		default: return s == s1;
      }
    }

	// Advance the XmlScanner to the next token and return it.
	XmlToken XmlScanner::GetToken()
	{
		XmlToken token;
		int t;

		// Call the scanner function and capture a token.
		t = (this->*c_scannerFunct)(); 

		// Captured a token, now update state of scanner.
		token = (XmlToken)t;
		eof = token == XmlToken::TT_EOF;

		if (token != XmlToken::TT_ERROR || eof)
		{
			++documentOrder;

			switch (token)
			{
			case XmlToken::TT_TAG_START:
				elemOrder++;
				++depth;
				break;
			case XmlToken::TT_TAG_END:
				--depth;
				//depth = depth > 1 ? --depth : depth;
				break;
			}
		}

		return token;
	}

	// Return if the scanner is at the end-of-file
	bool XmlScanner::Eof()
	{
		return eof;
	}

	// get the depth within the dom
	long XmlScanner::GetDepth()
	{
		return depth;
	}

	// get the order in the document
	long XmlScanner::GetOrder()
	{
		return documentOrder;
	}

	long XmlScanner::GetElemOrder()
	{
		return elemOrder;
	}

    const string XmlScanner::GetValue()
    {
      return value;
    }

    const string XmlScanner::GetAttrName()
    {
      attr_name[attr_name_length] = 0;
	  return string(attr_name);
    }

    const string XmlScanner::GetTagName() 
    {
      tag_name[tag_name_length] = 0;
      return tag_name;
    }
        
    XmlToken XmlScanner::scan_body() 
    {
      char c = get_char();

      value_length = 0;
         
      bool ws = false;

      if(c == 0) return TT_EOF;
      else if(c == '<') return scan_tag();
      else if(c == '&')
         c = scan_entity();
      else
         ws = is_whitespace(c);
        
      while(true) 
      {
        append_value(c);
        c = input.get_char();
        if(c == 0)  { push_back(c); break; }
        if(c == '<') { push_back(c); break; }
        if(c == '&') { push_back(c); break; }
          
        if(is_whitespace(c) != ws) 
        {
          push_back(c);
          break;
        }

      }
      return ws? TT_SPACE:TT_WORD;
    }

    XmlToken XmlScanner::scan_head()
    {
      char c = skip_whitespace();

      if(c == '>') 
	  { 
		  c_scannerFunct = &XmlScanner::scan_body; 
		  
		  return scan_body(); 
	  }
      if(c == '/')
      {
         char t = get_char();
         if(t == '>')   
		 { 
			 c_scannerFunct = &XmlScanner::scan_body; 

			 return TT_TAG_END; 
		 }
         else { push_back(t); return TT_ERROR; } // erroneous situtation - standalone '/'
      }

      attr_name_length = 0;
      value_length = 0;

      // attribute name...
      while(c != '=') 
      {
        if( c == 0) return TT_EOF;
        if( c == '>' ) { push_back(c); return TT_ATTR; } // attribute without value (HTML style)
        if( is_whitespace(c) )
        {
          c = skip_whitespace();
          if(c != '=') { push_back(c); return TT_ATTR; } // attribute without value (HTML style)
          else break;
        }
        if( c == '<') return TT_ERROR;
        append_attr_name(c);
        c = get_char();
      }

      c = skip_whitespace();
      // attribute value...
      
      if(c == '\"')
        while(c = get_char())
        {
            if(c == '\"') return TT_ATTR;
            if(c == '&') c = scan_entity();
            append_value(c);
        }
      else if(c == '\'') // allowed in html
        while(c = get_char())
        {
            if(c == '\'') return TT_ATTR;
            if(c == '&') c = scan_entity();
            append_value(c);
        }
      else  // scan token, allowed in html: e.g. align=center
        do
        {
            if( is_whitespace(c) ) return TT_ATTR;
            /* these two removed in favour of better html support:
            if( c == '/' || c == '>' ) { push_back(c); return TT_ATTR; }
            if( c == '&' ) c = scan_entity();*/
            if( c == '>' ) { push_back(c); return TT_ATTR; }
            append_value(c);
        } while(c = get_char());

      return TT_ERROR;
    }

    // caller already consumed '<'
    // scan header start or tag tail
    XmlToken XmlScanner::scan_tag() 
    {
      tag_name_length = 0;

      char c = get_char();

      bool is_tail = c == '/';
      if(is_tail) c = get_char();
      
      while(c) 
      {
        if(is_whitespace(c)) { c = skip_whitespace(); break; }
        if(c == '/' || c == '>') break;
        append_tag_name(c);

        switch(tag_name_length)
        {
        case 3: 
          if(equal(tag_name,"!--",3))  { c_scannerFunct = &XmlScanner::scan_comment; return TT_COMMENT_START; }
          break;
        case 8:
          if( equal(tag_name,"![CDATA[",8) ) { c_scannerFunct = &XmlScanner::scan_cdata; return TT_CDATA_START; }
          break;
        case 7:
          if( equal(tag_name,"!ENTITY",8) ) { c_scannerFunct = &XmlScanner::scan_entity_decl; return TT_ENTITY_START; }
          break;
        }

        c = get_char();
      }
 
      if(c == 0) return TT_ERROR;    
              
      if(is_tail)
      {
          if(c == '>') return TT_TAG_END;
          return TT_ERROR;
      }
      else 
           push_back(c);
      
      c_scannerFunct = &XmlScanner::scan_head;
      return TT_TAG_START;
    }

    // skip whitespaces.
    // returns first non-whitespace char
    char XmlScanner::skip_whitespace() 
    {
        while(char c = get_char()) 
        {
            if(!is_whitespace(c)) return c;
        }
        return 0;
    }

    void    XmlScanner::push_back(char c) { input_char = c; }

    char XmlScanner::get_char() 
    { 
      if(input_char) { char t(input_char); input_char = 0; return t; }
      return input.get_char();
    }


    // caller consumed '&'
    char XmlScanner::scan_entity() 
    {
      char buf[32];
      int i = 0;
      char t;
      for(; i < 31 ; ++i )
      {
        t = get_char();
        if(t == 0) return TT_EOF;
        if( !isalnum(t) )
        {
          push_back(t);
          break; // appears a erroneous entity token.
                 // but we try to use it.
        }
        buf[i] = char(t); 
        if(t == ';')
          break;
      }
      buf[i] = 0;
      if(i == 2)  
      {
        if(equal(buf,"gt",2)) return '>';
        if(equal(buf,"lt",2)) return '<';
      }
      else if(i == 3 && equal(buf,"amp",3)) 
        return '&';
      else if(i == 4) 
      {
        if(equal(buf,"apos",4)) return '\'';
        if(equal(buf,"quot",4)) return '\"';
      }
      t = resolve_entity(buf,i);
      if(t) return t;
      // no luck ...
      append_value('&');
      for(int n = 0; n < i; ++n)
        append_value(buf[n]);
      return ';';
    }

    bool XmlScanner::is_whitespace(char c)
    {
        return c <= ' ' 
            && (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f');
    }

    void XmlScanner::append_value(char c) 
    { 
		if (value_length == 0)
		{
			value = string();
		}
		value.append(&c);
		value_length++;
    }

    void XmlScanner::append_attr_name(char c)
    {
      if(attr_name_length < (MAX_NAME_SIZE - 1)) 
        attr_name[attr_name_length++] = (c);
    }

    void XmlScanner::append_tag_name(char c)
    {
      if(tag_name_length < (MAX_NAME_SIZE - 1)) 
        tag_name[tag_name_length++] = (c);
    }

    XmlToken XmlScanner::scan_comment()
    {
      if(got_tail)
      {
        c_scannerFunct = &XmlScanner::scan_body;
        got_tail = false;
        return TT_COMMENT_END;
      }
      for(value_length = 0; value_length < (MAX_TOKEN_SIZE - 1); ++value_length)
      {
        char c = get_char();
        if( c == 0) return TT_EOF;

		
        value[value_length] = c;
		
        if(value_length >= 2 
          && value.at(value_length) == '>'
          && value.at(value_length - 1) == '-' 
          && value.at(value_length - 2) == '-')
        {
          got_tail = true;
          value_length -= 2;
          break;
        }
      }
      return TT_DATA;
    }

    XmlToken XmlScanner::scan_cdata()
    {
      if(got_tail)
      {
        c_scannerFunct = &XmlScanner::scan_body;
        got_tail = false;
        return TT_CDATA_END;
      }
      for(value_length = 0; value_length < (MAX_TOKEN_SIZE - 1); ++value_length)
      {
        char c = get_char();
        if( c == 0) return TT_EOF;
        value[value_length] = c;

        if(value_length >= 2 
          && value.at(value_length) == '>' 
          && value.at(value_length - 1) == ']' 
          && value.at(value_length - 2) == ']')
        {
          got_tail = true;
          value_length -= 2;
          break;
        }
      }
      return TT_DATA;
    }

    XmlToken XmlScanner::scan_pi()
    {
      if(got_tail)
      {
        c_scannerFunct = &XmlScanner::scan_body;
        got_tail = false;
        return TT_PI_END;
      }
      for(value_length = 0; value_length < (MAX_TOKEN_SIZE - 1); ++value_length)
      {
        char c = get_char();
        if( c == 0) return TT_EOF;
        value[value_length] = c;

        if(value_length >= 1 
          && value.at(value_length) == '>' 
          && value.at(value_length - 1) == '?')
        {
          got_tail = true;
          value_length -= 1;
          break;
        }
      }
      return TT_DATA;
    }

    XmlToken XmlScanner::scan_entity_decl()
    {
      if(got_tail)
      {
        c_scannerFunct = &XmlScanner::scan_body;
        got_tail = false;
        return TT_ENTITY_END;
      }
      char t;
      unsigned int tc = 0;
      for(value_length = 0; value_length < (MAX_TOKEN_SIZE - 1); ++value_length)
      {
        t = get_char();
        if( t == 0 ) return TT_EOF;
        value[value_length] = t;
        if(t == '\"') tc++;
        else if( t == '>' && (tc & 1) == 0 )
        {
          got_tail = true;
          break;
        }
      }
      return TT_DATA;
    }




}
 
