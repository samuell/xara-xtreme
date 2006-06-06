// $Id$
/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE
================================XARAHEADERSTART===========================
 
               Xara LX, a vector drawing and manipulation program.
                    Copyright (C) 1993-2006 Xara Group Ltd.
       Copyright on certain contributions may be held in joint with their
              respective authors. See AUTHORS file for details.

LICENSE TO USE AND MODIFY SOFTWARE
----------------------------------

This file is part of Xara LX.

Xara LX is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as published
by the Free Software Foundation.

Xara LX and its component source files are distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with Xara LX (see the file GPL in the root directory of the
distribution); if not, write to the Free Software Foundation, Inc., 51
Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


ADDITIONAL RIGHTS
-----------------

Conditional upon your continuing compliance with the GNU General Public
License described above, Xara Group Ltd grants to you certain additional
rights. 

The additional rights are to use, modify, and distribute the software
together with the wxWidgets library, the wxXtra library, and the "CDraw"
library and any other such library that any version of Xara LX relased
by Xara Group Ltd requires in order to compile and execute, including
the static linking of that library to XaraLX. In the case of the
"CDraw" library, you may satisfy obligation under the GNU General Public
License to provide source code by providing a binary copy of the library
concerned and a copy of the license accompanying it.

Nothing in this section restricts any of the rights you have under
the GNU General Public License.


SCOPE OF LICENSE
----------------

This license applies to this program (XaraLX) and its constituent source
files only, and does not necessarily apply to other Xara products which may
in part share the same code base, and are subject to their own licensing
terms.

This license does not apply to files in the wxXtra directory, which
are built into a separate library, and are subject to the wxWindows
license contained within that directory in the file "WXXTRA-LICENSE".

This license does not apply to the binary libraries (if any) within
the "libs" directory, which are subject to a separate license contained
within that directory in the file "LIBS-LICENSE".


ARRANGEMENTS FOR CONTRIBUTION OF MODIFICATIONS
----------------------------------------------

Subject to the terms of the GNU Public License (see above), you are
free to do whatever you like with your modifications. However, you may
(at your option) wish contribute them to Xara's source tree. You can
find details of how to do this at:
  http://www.xaraxtreme.org/developers/

Prior to contributing your modifications, you will need to complete our
contributor agreement. This can be found at:
  http://www.xaraxtreme.org/developers/contribute/

Please note that Xara will not accept modifications which modify any of
the text between the start and end of this header (marked
XARAHEADERSTART and XARAHEADEREND).


MARKS
-----

Xara, Xara LX, Xara X, Xara X/Xtreme, Xara Xtreme, the Xtreme and Xara
designs are registered or unregistered trademarks, design-marks, and/or
service marks of Xara Group Ltd. All rights in these marks are reserved.


      Xara Group Ltd, Gaddesden Place, Hemel Hempstead, HP2 6EX, UK.
                        http://www.xara.com/

=================================XARAHEADEREND============================
 */

// MenuPref.cpp - This module is responsible for parsing a menu script and generating
// a set of Camelot Menus.


#include "camtypes.h"
#include "menuitem.h"
#include "menucmds.h"
#include "menupref.h"
#include "menuops.h"
#include "opdesc.h"
#include "errors.h"
#include "ensure.h"
//#include "mario.h"
#include "oilfiles.h"

DECLARE_SOURCE("$Revision$");  

// We want better memory tracking
#define new CAM_DEBUG_NEW

MenuItem* CreateMenuItem(TCHAR *OpToken, UINT32 ParentId, BOOL Separator);

UINT32 WindowMenuID;       


/********************************************************************************************

>	inline void ClearArray(TCHAR buffer[], INT32 size);

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/93
	Inputs:		Char Array
				Integer - Array Size
	Outputs:	-
	Returns:	-
	Purpose:	Initialises a Character Array with Nulls
	Errors:		-

********************************************************************************************/

inline void ClearArray(TCHAR buffer[], INT32 size)
{
	::memset(buffer, 0, size * sizeof(buffer[0]));
}



/********************************************************************************************

>	MenuItem *GetMenuPreferences(UINT32 ResourceID)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/93
	Inputs:		ResourceID representing the menu resource to be loaded.
	Outputs:	-
	Returns:	A MenuItem representing the default Menu that was parsed in or NULL if failed
				(SetError will be called)
	Purpose:	Parses a Menu Script representing the Camelot Default Menus. 
	Errors:		-
	SeeAlse:	GetToken();
	SeeAlso:	GetKeyword();
	SeeAlso:	GetSeparator();

********************************************************************************************/

MenuItem *GetMenuPreferences(UINT32 ResourceID)
{                              
	// Keyword Buffer
	const INT32 keywordSize 	= 10;
	TCHAR keyword[keywordSize];

	// Token Buffer
	const INT32 TokenSize 	= 30;
	TCHAR OpToken[TokenSize];

	// Lexical Tokens
	const String_256 menu_sym 		= "MENU";
	const String_256 submenu_sym 	= "SUBMENU";
	const String_256 menuitem_sym 	= "MENUITEM";
	const String_256 end_sym 		= "END";
	const String_256 separator_sym	= "SEPARATOR";
	
	CCResTextFile MenuScript;	// Resource File
                                                
	// Open Resource File                                                
    MenuScript.open(ResourceID, _R(IDT_CAM_MENU_RES));

    // Stack of all the Menus and SubMenus currently in Scope!             
    List MenuStack;
    
    MenuItem* 		pCurMenu = NULL; 	// Menu Currently in Scope

    MenuItem* 		pMenu = NULL; 
    MenuItem* 		pSubMenu; 
    MenuItem* 		pMenuItem; 
    
   	while ((MenuScript.isOpen()) && (!MenuScript.eof()))
    {
		ClearArray(keyword, keywordSize);

		//Get The Keyword
		if (GetKeyword(&MenuScript, keyword, keywordSize))
		{
        // MENU reserved word
			if (menu_sym == keyword)				
			{
				ERRORIF((!GetToken(&MenuScript, OpToken, TokenSize)), 
						_R(IDE_SYNTAX_ERROR), 
						NULL);
				
				pMenu = new MenuItem(OpToken);

				if (pMenu==NULL)
				{
					TRACEALL( _T("GetMenuPrefs: failed to allocate MenuItem"));
					break;											// willeventually return NULL
				}
			    
				pCurMenu = pMenu;
			}
			// SUBMENU reserved word
			else if (submenu_sym == keyword)
			{                                       
				ERRORIF((!GetToken(&MenuScript, OpToken, TokenSize)), 
						_R(IDE_SYNTAX_ERROR), 
						NULL);
                
				pSubMenu = CreateMenuItem(	OpToken, 
											pCurMenu->GetMenuId(),
											GetSeparator(	&MenuScript, 
															separator_sym, 
															keyword,
															keywordSize)
										 );

				// stop now if run out of memory
				if (pSubMenu==NULL)
					break;

				// Get the ID for the Window Menu
				if (String(OpToken) == String(OPTOKEN_WINDOW_MENU))
					//MenuItem ID must be first sub element of menu because in windows 
					//menus do not have IDs
					WindowMenuID = ((pSubMenu->GetMenuId()) + 1);
				
				pCurMenu->AddMenuItem(pSubMenu);
				MenuStack.AddHead(pCurMenu);	//Push Current Menu onto stack
				pCurMenu = pSubMenu;
			}
			// MENUITEM reserved word
			else if (menuitem_sym == keyword)
			{
				ERRORIF((!GetToken(&MenuScript, OpToken, TokenSize)), 
						_R(IDE_SYNTAX_ERROR), 
						NULL);
			
				pMenuItem = CreateMenuItem(	OpToken,
											pCurMenu->GetMenuId(),
											GetSeparator(	&MenuScript, 
															separator_sym, 
															keyword,
															keywordSize)
										  );
				
           		if (pMenuItem != NULL)
           			pCurMenu->AddMenuItem(pMenuItem);
				else
					break;

			}
			// END reserved word
			else if (end_sym == keyword)
			{                                       // Pop Menu from stack
				pCurMenu = (MenuItem*) MenuStack.RemoveHead();
				// WEBSTER - markn 22/1/97
				// Stops circular menu lists being generated when the last main menu item
				// contains a sub menu (thank you mario)
				if (pCurMenu != NULL)
					pCurMenu->ClearPointers();
			}
		}

	}                                        
		
    MenuScript.close();
    
	return pMenu;	
}

/********************************************************************************************

>	MenuItem *CreateMenuItem(TCHAR *OpToken, UINT32 ParentId, BOOL Separator)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		OpToken used to get the appropriate operation
				Parent Identifier
				Separator Boolean 
	Returns:	MenuItem if created ok else NULL
	Purpose:	Creates a MenuItem given an Optoken.

********************************************************************************************/

MenuItem *CreateMenuItem(TCHAR *OpToken, UINT32 ParentId, BOOL Separator)
{
	OpDescriptor *Operation = OpDescriptor::FindOpDescriptor(OpToken);

PORTNOTE( "other", "Use DO_NOTHING op for menuitem, if optoken not found" );
#if defined(EXCLUDE_FROM_XARALX)
	if( NULL == Operation )
		Operation = OpDescriptor::FindOpDescriptor( OPTOKEN_DO_NOTHING );
#endif
	
	if (!Operation)
	{
		String_256 ErrorMsg(_R(IDE_OPNOTFOUND));
		ErrorMsg += OpToken;	

 		Error::SetError(_R(IDE_OPNOTFOUND), ErrorMsg, 0);
		InformError();

    	return NULL;
    }
    else	
	{
	    MenuItem *pMenuItem;
	
		// Create an instance of a MenuItem
		pMenuItem = new MenuItem(	ParentId,
									Operation,
									Separator
								);
    
		ERROR3IF( pMenuItem==NULL, "CreateMenuItem failed" );
		
		return pMenuItem;
    }
}

/********************************************************************************************

>	BOOL GetKeyword(CCResTextFile *pResScript, TCHAR keyword[], INT32 size)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/93
	Inputs:		Input Stream - Menu Script
				Integer - Maximum size of keyword
	Outputs:	Char Array - The Keyword
	Returns:	TRUE if Keyword is found
				FALSE otherwize
	Purpose:	Parses a Menu Script Keyword.
	Errors:		-

********************************************************************************************/

BOOL GetKeyword(CCResTextFile *pResScript, TCHAR keyword[], INT32 size)
{
	// Get Keyword
	TCHAR ch; 
	INT32 i = 0;			
    
	ClearArray(keyword, size);

    pResScript->read(ch);
    
	// if not a keyword return false!
    if (!String::IsAlpha(ch) && (!pResScript->eof()))
		return FALSE;	
	   
	while (String::IsAlpha(ch) && !pResScript->eof() && i < size)
	{
		keyword[i++] = ch;
	    pResScript->read(ch);
	}           

	if (!pResScript->eof())	
		return TRUE;
	else
	{
//		TRACEUSER( "JustinF", _T("Menu GetKeyword() - read %s\n"), keyword);
		return FALSE;	
	}
}           

/********************************************************************************************

>	BOOL GetToken(CCResTextFile *pMenuScript, TCHAR OpToken[], INT32 TokenSize) 

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/93
	Inputs:		Input Stream - Menu Script
				Integer - Maximum size of Token
	Outputs:	Char Array - The Token
	Returns:	TRUE if Token is found
				FALSE otherwize
	Purpose:	Parses a Menu Script Token.
	Errors:		-

********************************************************************************************/

BOOL GetToken(CCResTextFile *pMenuScript, TCHAR OpToken[], INT32 TokenSize) 
{        
	// Get Keyword
	TCHAR ch;
	INT32 i = 0;			

	ClearArray(OpToken, TokenSize);

	//Go back one place and reread last character.
	pMenuScript->seek((pMenuScript->tell() - 1));
    pMenuScript->read(ch);
	 
	while (StringBase::IsSpace(ch) && !pMenuScript->eof())
		pMenuScript->read(ch);
		
	if (ch == TEXT('"'))
	    pMenuScript->read(ch);
	else
	{
//		TRACEUSER( "JustinF", _T("Menu GetToken() - no opening \"\n"));
		pMenuScript->close();
		return FALSE;	
	}

	while ((String::IsAlphaNumeric(ch) || (ch == _T('_'))) &&
		   ch != TEXT('"') &&
		   !pMenuScript->eof() &&
		   i < TokenSize)
	{
		OpToken[i++] = ch;
	    pMenuScript->read(ch);
	}           
	
	if (ch == TEXT('"') && !pMenuScript->eof())
		return TRUE;
	else
	{
//		TRACEUSER( "JustinF", _T("Menu GetToken() - read %s\n"), OpToken);
		pMenuScript->close();
		return FALSE;	
	}
}

/********************************************************************************************

>	BOOL GetSeparator(	CCResTextFile *pMenuScript, 
						const String& separator_sym, 
						TCHAR SepKeyword[], 
						INT32 size) 

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/93
	Inputs:		Input Stream - Menu Script
				constant String - Separator Symbol
				Char Array - Keyword Array  
				Integer - size of Symbol
	Outputs:	-
	Returns:	TRUE if Separator is found
				FALSE otherwize
	Purpose:	Parses a Menu Script Menu Separator Token.
	Errors:		-

********************************************************************************************/

BOOL GetSeparator(	CCResTextFile *pMenuScript, 
					const String& separator_sym, 
					TCHAR SepKeyword[], 
					INT32 size) 
{        
	// Get Keyword
	TCHAR ch;
	INT32 i = 0;			

	ClearArray(SepKeyword, size);
	
	//Go back one place and reread last character.
	pMenuScript->seek((pMenuScript->tell() - 1));
    pMenuScript->read(ch);
    
	//Find First Alpha Numeric Character
	while(!String::IsAlpha(ch) && (!pMenuScript->eof()) && (ch != '\n'))
	    pMenuScript->read(ch);

	//Read in the keyword until not Alpha Numeric
	while (String::IsAlpha(ch) && (!pMenuScript->eof()) && (i < size))
	{
		SepKeyword[i++] = ch;
	    pMenuScript->read(ch);
	}           

	if (separator_sym == SepKeyword)
		return TRUE;
	else
	{
//		TRACEUSER( "JustinF", _T("Menu GetSeparator() - read %s\n"), SepKeyword);
		return FALSE;
	}
}
