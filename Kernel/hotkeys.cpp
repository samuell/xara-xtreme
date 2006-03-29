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
// The implementation of the hot-key system

/*
*/

#include "camtypes.h"
#include "hotkeys.h"
#include "list.h"
#include "keypress.h"
#include "opdesc.h"
#include "oilfiles.h"
//#include "mario.h"
#include "vkextra.h"
#include "ops.h"
#include "app.h"
#include "tool.h"
#include "toollist.h"
#include "dragmgr.h"
#include "basebar.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_MEMDUMP(HotKey,ListItem)

static List HotKeyList;

enum HK_TokenIndex
{
	HK_TOKEN_NONE = -1,
	HK_TOKEN_HOTKEY,
	HK_TOKEN_HOTKEYEND,
	HK_TOKEN_ADJUST,
	HK_TOKEN_CONSTRAIN,
	HK_TOKEN_ALTERNATIVE,
	HK_TOKEN_EXTENDED,
	HK_TOKEN_TOOLSWITCH,
	HK_TOKEN_WORKSINDRAG,
	HK_TOKEN_CHECKUNICODE,
	// Add new token indexs BEFORE HK_NUM_TOKENS
	HK_NUM_TOKENS
};

static struct
{
	TCHAR*  Token;
} TokenTable[] = 
{
	{ _T("HotKey") },
	{ _T("HotKeyEnd") },
	{ _T("Adjust") },
	{ _T("Constrain") },
	{ _T("Alternative") },
	{ _T("Extended") },
	{ _T("ToolSwitch") },
	{ _T("WorksInDrag") },
	{ _T("CheckUnicode") }
};

static HK_TokenIndex FindToken(const TCHAR* Token)
{
	for (INT32 i=0;i<HK_NUM_TOKENS;i++)
		if (lstrcmp(TokenTable[i].Token,Token) == 0) return ((HK_TokenIndex)i);

	return (HK_TOKEN_NONE);
}

// The size of the text description string object in a HotKey object
#define TEXT_DESC_SIZE 32

/********************************************************************************************

> static BOOL HotKey::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Initialises the HotKey class.
				At the moment it just involves the reading of the hot key definitions.
	SeeAlso:	-

********************************************************************************************/

BOOL HotKey::Init()
{
	return (HotKey::ReadHotKeys());
}

/********************************************************************************************

> static void HotKey::Deinit()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Cleans up the HotKey class on program exit
	SeeAlso:	-

********************************************************************************************/

void HotKey::Deinit()
{
	HotKeyList.DeleteAll();
}

/********************************************************************************************

> HotKey::HotKey()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The default constructor. 
				Does nothing at the moment
	SeeAlso:	-

********************************************************************************************/

HotKey::HotKey()
{
	pKeyPress	= NULL;
	pOpDesc		= NULL;
	pTextDesc	= NULL;
	CheckUnicode= FALSE;
}

/********************************************************************************************

> HotKey::~HotKey()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The default destructor. 
				Main job is to delete any attached objects (such as the key press object)
	SeeAlso:	-

********************************************************************************************/

HotKey::~HotKey()
{
	if (pKeyPress != NULL)
	{
		delete pKeyPress;
		pKeyPress = NULL;
	}

	if (pTextDesc != NULL)
	{
		delete pTextDesc;
		pTextDesc = NULL;
	}
}

/********************************************************************************************

> static BOOL HotKey::AddHotKey(KeyPress* pKeyPress, TCHAR* pOpToken,String_32* pTextDesc,BOOL CheckUnicode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		pKeyPress = ptr to the key press that will invoke the operation
				pOpToken  = ptr to the char array of the operation's op token
				pTextDesc = ptr to a string that describes the hot key (e.g. "Ctrl+X")
				CheckUnicode= Flag to determine how the hot key is compared with key press objects
	Outputs:	-
	Returns:	TRUE  if it could find the operation and the ky press was valid
				FALSE if something hideous happend
	Purpose:	The place where hot keys are added to Camelot.
	SeeAlso:	-

********************************************************************************************/

BOOL HotKey::AddHotKey(KeyPress* pKeyPress, TCHAR* pOpToken,String_32* pTextDesc,BOOL CheckUnicode)
{
	PORTNOTE( "other", "AddHotKey - Carry on even if OpDesc not found" );
	BOOL ok = TRUE;

	// Is this key press ok?
	if (pKeyPress->IsValid())
	{
		// We have a valid key press object, but do we have a valid op token?

		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(pOpToken);

		if (pOpDesc != NULL)
		{
			// Yes, we now have a valid op token too. Now make a HotKey object

			HotKey* pHotKey = new HotKey;

			if (pHotKey != NULL)
			{
				// We have everything we need, so set up the HotKey and add it to the list

				pHotKey->pKeyPress 	 = pKeyPress;
				pHotKey->pOpDesc 	 = pOpDesc;
				pHotKey->pTextDesc	 = pTextDesc;
				pHotKey->CheckUnicode= CheckUnicode;

				HotKeyList.AddTail(pHotKey);

				ok = TRUE;
			}
		}
		else
			TRACE( _T("HotKey: Unable to find op desc \"%s\"\n"),pOpToken);
	}

	return ok;
}


/********************************************************************************************

> static BOOL HotKey::OnKeyPress(KeyPress* pKeyPress)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		pKeyPress = ptr to the key press that's just been generated
	Outputs:	-
	Returns:	TRUE  if processed (i.e. a hot key was pressed), so do not pass on
				FALSE if not processed, so pass on to others
	Purpose:	This will invoke an operation if the given key press matches a registered hot key.
	SeeAlso:	-

********************************************************************************************/

BOOL HotKey::OnKeyPress(KeyPress* pKeyPress)
{
	BOOL Processed = FALSE;
	BOOL DuringADrag = (Operation::GetCurrentDragOp() != NULL ||
PORTNOTE( "other", "Removed DragManagerOp usage" )
#if 0
						DragManagerOp::IsDragActive() ||
#endif
						BaseBar::IsDragging());

	HotKey* pHotKey = (HotKey*)HotKeyList.GetHead();
	while (pHotKey != NULL && !Processed)
	{
		if (DoesHotKeyMatchKeyPress(pHotKey,pKeyPress))
		{
			// The key press is a hot key combination
			if ((pHotKey->pKeyPress->IsOkInDrags() || !DuringADrag) && !pKeyPress->IsRelease())
			{
				// There is no drag happening (or the keypress works in drags), and it is a "key down" key press
				// If the hot key's op is enabled, invoke it
				if (pHotKey->IsEnabled())
					pHotKey->pOpDesc->Invoke();
			}

			// Do not pass on if it is a hot key combination, whether the op was invoked or not
			Processed = TRUE;
		}

		// Get the next hot key
		pHotKey = (HotKey*)HotKeyList.GetNext(pHotKey);
	}

	return Processed;
}

/********************************************************************************************

>	static BOOL HotKey::DoesHotKeyMatchKeyPress(HotKey* pHotKey,KeyPress* pKeyPress)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/95
	Inputs:		pHotKey   = ptr to HotKey
				pKeyPress = ptr to KeyPress
	Outputs:	-
	Returns:	TRUE if HotKey matches the key press, FALSE otherwise
	Purpose:	Central routine for checking HotKeys against key press objects
	SeeAlso:	-

********************************************************************************************/

BOOL HotKey::DoesHotKeyMatchKeyPress(HotKey* pHotKey,KeyPress* pKeyPress)
{
	ERROR3IF(pHotKey   == NULL,"pHotKey == NULL");
	ERROR3IF(pKeyPress == NULL,"pHotKey == NULL");

	if (pHotKey == NULL || pKeyPress == NULL)
		return FALSE;

	if (pHotKey->CheckUnicode)
	{
		ERROR3IF(pHotKey->pTextDesc == NULL,"pHotKey->pTextDesc == NULL");

		if (pHotKey->pTextDesc == NULL)
			return FALSE;

		TCHAR str[3];

		str[0] = (pKeyPress->GetUnicode() & 0xff);
		str[1] = ((pKeyPress->GetUnicode() & 0xff00)>>8);
		str[2] = 0;

		String_32 KeyPressStr = String_32(str);

		return (KeyPressStr == *pHotKey->pTextDesc);
	}
	else
		return (*pHotKey->pKeyPress == *pKeyPress);
}

/********************************************************************************************

>	static HotKey* HotKey::FindHotKey(OpDescriptor* pThisOpDesc)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Inputs:		pThisOpDesc = ptr to an op desc
	Returns:	ptr to a HotKey, or NULL
	Purpose:	Finds the first hot key that is associated with the given op desc
				NULL is returned if there isn't one or the hot key has been disabled
	SeeAlso:	OpDescriptor::IsHotKeyEnabled()

********************************************************************************************/

HotKey*	HotKey::FindHotKey(OpDescriptor* pThisOpDesc)
{
	if (!pThisOpDesc->IsHotKeyEnabled())
	{
		return NULL;
	}

	HotKey* pHotKey = (HotKey*)HotKeyList.GetHead();

	while (pHotKey != NULL && (pHotKey->pOpDesc != pThisOpDesc))
		pHotKey = (HotKey*)HotKeyList.GetNext(pHotKey);

	return pHotKey;
}

/********************************************************************************************

>	BOOL HotKey::IsEnabled()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the hot key operation is enabled
				FALSE otherwise
	Purpose:	Tests to see if it is possible to invoke a hot key's operation.
				This will also update the status bar with a relevent message.
				If the op is enabled, a general description of the op is displayed
				If the op is disabled, a description of why is displayed
	Errors:		None

*********************************************************************************************/

BOOL HotKey::IsEnabled()
{
	ENSURE(pOpDesc != NULL, "Hot key's op desc ptr is NULL");

	// Check if the hot key is available to the user, i.e., it is shown
	if (!pOpDesc->IsHotKeyEnabled())
	{
		return FALSE;
	}

	// String that will contain the string to display in the status bar
	String_256 StatusMsg;
	
	// get hotkey's op state
	OpState HotKeyState = pOpDesc->GetOpsState(&StatusMsg);

	// If the op state is greyed, then the description of why it's grey should have been
	// set up by the GetOpsState() call

	// If it's not greyed, get a general description of the op to place in the status bar
	if (!HotKeyState.Greyed)
		pOpDesc->GetText(&StatusMsg,OP_DESC_TEXT);
    
	// Display msg in the status bar
	GetApplication()->UpdateStatusBarText(&StatusMsg);

	// Return TRUE if not greyed
	return (!HotKeyState.Greyed);
}

//----------------------------------------------------------------------

/********************************************************************************************

> static BOOL HotKey::ReadHotKeys()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the hot key definitions from somewhere (either file or bound-in resource file).
	SeeAlso:	-

********************************************************************************************/

BOOL HotKey::ReadHotKeys()
{
	BOOL ok = HotKey::ReadHotKeysFromDisk();

	if (!ok) ok = HotKey::ReadHotKeysFromRes();

	return (ok);
}

/********************************************************************************************

> static BOOL HotKey::ReadHotKeysFromDisk()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the hot key definitions from disk (i.e. writable media)
	SeeAlso:	-

********************************************************************************************/

BOOL HotKey::ReadHotKeysFromDisk()
{	
	// Not yet implemented

	return FALSE;
}

/********************************************************************************************

> static BOOL HotKey::ReadHotKeysFromRes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the hot key definitions from the bound in resource.
	SeeAlso:	-

********************************************************************************************/

BOOL HotKey::ReadHotKeysFromRes()
{
	CCResTextFile file;				// Resource File

	BOOL ok = file.open(_R(IDH_STANDARD_HOTKEYS), _R(IDT_CAM_HOTKEY_RES));		// Open resource

	if (ok)
	{
		ok = HotKey::ReadHotKeysFromFile(file);
		file.close();
	}

	return (ok);
}

/********************************************************************************************

> static BOOL HotKey::ReadHotKeysFromFile(CCLexFile& file)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		file = file that contains the hot key definitions
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the hot key definitions from the given file
	SeeAlso:	-

********************************************************************************************/

BOOL HotKey::ReadHotKeysFromFile(CCLexFile& file)
{
	BOOL finished = FALSE;
	BOOL ok;;	
	
	// Initialise lexing routines, and aspects of the lexer
	ok = file.InitLexer();
	file.SetDelimiters("\r\n");			// Set token delimiting characters
	file.SetCommentMarker(';');			// Set comment marker char
	file.SetWhitespace(" \t");			// Set whitespace chars
	file.SetStringDelimiters("\"\"");	// Set string delimiters

	HK_TokenIndex Token;
	const TCHAR* TokenBuf = file.GetTokenBuf();	// Token buffer remains constant until lexer deinitialisation
	
	while (ok && !finished)
	{
		// Grab a token
		ok = file.GetSimpleToken();

		// Look the token up in our table
		Token = FindToken(TokenBuf);

		switch (Token)
		{
			case HK_TOKEN_HOTKEY:
				ok = HotKey::ReadHotKey(file);
				break;

			case HK_TOKEN_TOOLSWITCH:
				ok = HotKey::ReadToolSwitch(file);
				break;

			case HK_TOKEN_HOTKEYEND:
				finished = TRUE;
				break;

			default:
				TRACE( _T("HotKey: Unexpected token - %s\n"),TokenBuf);
				break;
		}
	}

	if (!ok)
	{
		TRACE( _T("\nHotKey: Offending line - %s\n"),file.GetLineBuf());
		ENSURE(FALSE,"Error reading hot keys.  See TRACE output for details");
	}

	// We are now finished with the lexer
	file.DeinitLexer();

	return (ok);
}

/********************************************************************************************

> static BOOL HotKey::ReadHotKey(CCLexFile& file)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		file = file that contains the hot key definitions
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Read one hot key definition from the given file
	SeeAlso:	-

********************************************************************************************/

BOOL HotKey::ReadHotKey(CCLexFile& file)
{
	KeyPress* pKeyPress;
	String_256 OpToken;
	String_32* pTextDesc;
	BOOL CheckUnicode;;

	BOOL ok = HotKey::ReadKeyDef(file,&pKeyPress,&OpToken,&pTextDesc,&CheckUnicode);

	if (ok)
	{
		ok = HotKey::AddHotKey(pKeyPress,OpToken,pTextDesc,CheckUnicode);
		if (!ok) TRACE( _T("HotKey: Unable to add hot key to the system\n"));
	}

	return (ok);
}

/********************************************************************************************

> static BOOL HotKey::ReadToolSwitch(CCLexFile& file)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Inputs:		file = file that contains the tool switch definitions
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Read one tool switch definition from the given file
	SeeAlso:	-

********************************************************************************************/

BOOL HotKey::ReadToolSwitch(CCLexFile& file)
{
	KeyPress* pKeyPress;
	String_256 OpToken;
	String_32* pTextDesc;	// This is not used with tool switches
	BOOL CheckUnicode;		// This is not used with tool switches

	BOOL ok = HotKey::ReadKeyDef(file,&pKeyPress,&OpToken,&pTextDesc,&CheckUnicode);

	// This text desc is ignored at the mo by the tool switch mechanism	
	// so if ReadKeyDef() happens to return one, destory it
	if (pTextDesc != NULL)
	{
		delete pTextDesc;
		pTextDesc = NULL;
	}

	if (ok)
	{
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OpToken);
		ok = (pOpDesc != NULL);

		if (ok)
		{
			UINT32 ToolID = pOpDesc->GetToolID();

			ToolListItem* pToolListItem = Tool::Find(ToolID);
			ok = ((pToolListItem != NULL) && (pToolListItem->m_pTool != NULL));

			if (ok)
			{
				ok = pToolListItem->m_pTool->RegisterToolSwitch(pKeyPress,TRUE);
				if (!ok) TRACE( _T("HotKey: Unable to register the tool switch\n"));
			}
			else
				TRACE( _T("HotKey: Dodgy ToolListItem for the tool ID %ld\n"),ToolID);
		}
		else
			TRACE( _T("HotKey: Unable to find op '%s'\n"),(TCHAR*)OpToken);
	}

	return (ok);
}

/********************************************************************************************

> static BOOL HotKey::ReadKeyDef(	CCLexFile& file,
									KeyPress** ppKeyPress,
									String_256* pOpToken,
									String_32** ppTextDesc,
									BOOL*		pCheckUnicode)


	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Inputs:		file 			= file that contains the key definitions
				ppKeyPress 		= Place to put the ptr to a key press object
				pOpToken   		= ptr to string to place the op token string
				ppTextDesc		= ptr to a ptr to a String_32 object that describes the key def (e.g. "Ctrl+X")
				pCheckUnicode	= Flag to determine how the hot key is compared with key press objects

	Outputs:	*ppKeyPress 	= a ptr to a key press object OR NULL
				*pOpToken   	= op token of op associated with the key press object OR empty string
				*ppTextDesc		= ptr to a String_32 describing the key combination.
							  	  If this is NULL, then the key combo has no text description
				*pCheckUnicode	= TRUE if Unicode checking should be done, FALSE otherwise

	Returns:	TRUE if OK
				FALSE otherwise (syntax error, not enough memory, etc)
	Purpose:	Generates a key press object and op token from the given file
	SeeAlso:	-

********************************************************************************************/

BOOL HotKey::ReadKeyDef(CCLexFile& file,
						KeyPress** ppKeyPress,
						String_256* pOpToken,
						String_32** ppTextDesc,
						BOOL*		pCheckUnicode)
{
	// Check the params
	ENSURE(ppKeyPress 	!= NULL,"ppKeyPress is NULL");
	ENSURE(pOpToken   	!= NULL,"pOpToken is NULL");
	ENSURE(ppTextDesc 	!= NULL,"ppTextDesc is NULL");
	ENSURE(pCheckUnicode!= NULL,"pCheckUnicode is NULL");
	if (ppKeyPress == NULL || pOpToken == NULL || ppTextDesc == NULL || pCheckUnicode == NULL) return FALSE;

	*ppKeyPress 	= NULL;		// We haven't made a key press object yet
	*ppTextDesc 	= NULL;		// No text description yet
	*pCheckUnicode 	= FALSE;	// Assume we don't want Unicode checking

	HK_TokenIndex Token;
	const TCHAR* TokenBuf = file.GetTokenBuf();

	// This set of parameters will combine to form a key press object that represents
	// the hot key combination
	UINT32 		VirtKey 	= CAMKEY(CC_NONE);
	BOOL 		Adjust      = FALSE;
	BOOL 		Constrain   = FALSE;
	BOOL 		Alternative = FALSE;
	BOOL 		Extended    = FALSE;
	BOOL 		WorksInDrag = FALSE;
	String_32*	pTextDesc 	= NULL;

	// This is FALSE until we have read the text that describes the key combination
	BOOL TextDescRead = FALSE;

	// We haven't finsihed, but we're OK at the mo
	BOOL finished = FALSE;
	BOOL ok 	  = TRUE;

	while (ok && !finished)
	{
		// Get the next token
		ok = file.GetSimpleToken();

		if (ok)
		{
			// Find out the type of the token
			LexTokenType TokenType = file.GetTokenType();

			switch (TokenType)
			{
				case TOKEN_STRING:

					if (!TextDescRead)
					{
						// We haven't yet read the textual desc of the key definition, so assume
						// that this is it

						// Make sure the desc is not too long
						ok = (_tcslen(TokenBuf) <= TEXT_DESC_SIZE);

						if (ok)
						{
							// Is the string empty (i.e. "") ?
							if (cc_strlenCharacters(TokenBuf) > 0)
							{
								// if there is a valid text desc, get a String_32 ready for it
								// and put the desc in it
								pTextDesc = new String_32;
								if (pTextDesc != NULL)
									*pTextDesc = TokenBuf;
								else
									TRACE( _T("HotKey: Not enough memory or a String_32 - pathetic eh?"));
							}
						}
						else
							TRACE( _T("HotKey: Menu text ('%s') is too long - should be <= %d"),TokenBuf,TEXT_DESC_SIZE);

						// We have read the text desc string
						TextDescRead = TRUE;
					}
					else
					{
						// When we get a string after the menu text string, assume this is 
						// the op token of the operation the hot key is intended to invoke
						// Also, regard the op token string as the last aspect of a key def
						*pOpToken  = TokenBuf;
						finished = TRUE;
					}

					break;

				case TOKEN_NORMAL:
					{
						Token = FindToken(TokenBuf);
						switch (Token)
						{
							case HK_TOKEN_ADJUST		: Adjust 		 = TRUE; break;
							case HK_TOKEN_CONSTRAIN 	: Constrain 	 = TRUE; break;
							case HK_TOKEN_ALTERNATIVE 	: Alternative 	 = TRUE; break;
							case HK_TOKEN_EXTENDED 	: Extended		 = TRUE; break;
							case HK_TOKEN_WORKSINDRAG	: WorksInDrag	 = TRUE; break;
							case HK_TOKEN_CHECKUNICODE	: *pCheckUnicode = TRUE; break;

							case HK_TOKEN_NONE:
								ok = (_stscanf(TokenBuf,_T("%li"),&VirtKey) == 1);
								if (!ok) TRACE( _T("HotKey: Expected a virtual key code but got : '%s'\n"),TokenBuf);
								break;

							default:
								ok = FALSE;
								TRACE( _T("HotKey: Didn't expect to get this token ('%s') in the middle of a hot key def\n"),TokenBuf);
								break;
						}
					}
					break;

				default:
					TRACE( _T("HotKey: Unexpected token - %s\n"),TokenBuf);
					ok = FALSE;
					break;
			}
		}
	}


	if (ok)
	{
		ok = (VirtKey != CAMKEY(CC_NONE));
		if (ok)
		{
			*ppKeyPress = new KeyPress(VirtKey,Adjust,Constrain,Alternative,Extended,WorksInDrag);
			ok = (*ppKeyPress != NULL);
			if (ok)
				*ppTextDesc = pTextDesc;
			else
				TRACE( _T("HotKey: Not enough memory for the key press object"));
		}
		else
			TRACE( _T("HotKey: Virtual key code was not found\n"));
	}

	if (!ok)
	{
		if (*ppKeyPress != NULL)
		{
			delete *ppKeyPress;
			*ppKeyPress = NULL;
		}
	}

	return (ok);
}

/********************************************************************************************

> void HotKey::GetTextDesc(String_256* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Inputs:		pStr = ptr to a string to put the text desc of the hot key
	Outputs:	*pStr contains the description
	Returns:	-
	Purpose:	Stuffs the hot key text desc (if it has one) into *pStr
	SeeAlso:	-

********************************************************************************************/

void HotKey::GetTextDesc(String_256* pStr)
{
	ENSURE(pStr != NULL,"pStr is NULL");
	if (pStr == NULL) return;

	if (pTextDesc != NULL)
		*pStr = *pTextDesc;
}
