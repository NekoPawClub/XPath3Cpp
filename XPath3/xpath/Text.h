/*
Filename:		Text.h
Purpose:		Interface for the CText class.
Created:		Kimanzi Mati	June 7, 2002 12:55
Last Edited:	Kimanzi Mati	June 7, 2002 12:55	Original code

Copyright (C) 2002 Kimanzi Musili Mati

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the
	Free Software Foundation, Inc.,
	59 Temple Place, Suite 330,
	Boston, MA 02111-1307
	USA
For a copy of the GNU Lesser General Public License on the web, visit:
	http://www.opensource.org/licenses/lgpl-license.html
*/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#if !defined(AFX_TEXT_H__90BA0DDA_CFF9_41D3_8D70_06FB7D4C5D93__INCLUDED_)
#define AFX_TEXT_H__90BA0DDA_CFF9_41D3_8D70_06FB7D4C5D93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <comdef.h>

class CText  
{
public:
	CText();
	virtual ~CText();

	CText(CText &x);
	CText(LPCTSTR sName);
	CText(char ch);
	CText(unsigned short * sName);

	int operator=(CText &x);
	int operator=(unsigned short *x);
	int operator=(char * str);

	CText operator+(CText &x);
	CText operator+(LPCTSTR str);
	CText operator+(unsigned short *x);
	CText operator+=(CText &x);
	CText operator+=(LPCTSTR str);
	CText operator+=(unsigned short *x);
	CText operator+=(char str);
	operator LPSTR();
	BOOL	Compare(char * s);
	BOOL	CompareNoCase(char * s);
	void	MakeLower();
	void	MakeUpper();
	void	MakeReverse();
	_bstr_t	AllocSysString();
	int		GetLength();
	char	GetAt(int i);
	void	SetAt(int i, char ch);
	CText	Mid(int i);
	CText	Mid(int i, int count);
	CText	Left(int count);
	CText	Right(int count);
	int		Replace(char * sOld, char * sNew);
	void	TrimLeft();
	void	TrimRight();
	int		Find(char * sText, int start = 0);
	int		Find(char sChar, int start = 0);
	int		ReverseFind(char sChar);
	int		ReverseFind(char *sText);

protected:
	char *	m_Text;
};

#endif // !defined(AFX_TEXT_H__90BA0DDA_CFF9_41D3_8D70_06FB7D4C5D93__INCLUDED_)
