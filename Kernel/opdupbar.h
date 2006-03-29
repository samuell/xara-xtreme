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
#ifndef	OPTOKEN_BARDUPLICATIONOP

#include "cutop.h"

#define OPTOKEN_BARDUPLICATIONOP	_T("BarDuplicationOp")
#define OPTOKEN_SHORTENBAROP		_T("ShortenBarOp")
#define OPTOKEN_DELBAROP			_T("DeleteBarOp")
#define OPTOKEN_SHOWSTATE			_T("ShowStateOp")

#include "slicehelper.h"

class TemplateAttribute;
/********************************************************************************************

>	class OpBarDuplication : public CarbonCopyOp

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Purpose:	Make a complete copy of the bar.
				Only the buttons names are made unique
				and there is a new bar name.
				It is positioned next to the original bar.
********************************************************************************************/
class CCAPI OpDuplicateBar: public CarbonCopyOp
{         
	CC_DECLARE_DYNCREATE( OpDuplicateBar )    

public:
	OpDuplicateBar();								
	static BOOL	Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	//virtual	void	Do(OpDescriptor* token);
    void			DoWithParam(OpDescriptor* token, OpParam* pOpParam);

	String_256 GetNewButtonName(String_256 &Name);
	void DuplicateBar(String_256 &OriginalBarName, String_256 &NewBarName, INT32 ButtonsToAdd = 0, INT32 Spacing = 0, BOOL IsHoriz = FALSE);
	Node * CopyBarMember(Node * pNode, const String_256 &OriginalBarName, const String_256 &NewBarName, Trans2DMatrix * pTransformer, const INT32 * pKnownButonIndex = NULL);

private:
	INT32 m_ButtonNumberList[MAX_BUTTONS_IN_A_BAR];
	TemplateAttribute ** m_ppFoundButton[MAX_BUTTONS_IN_A_BAR];
	INT32 m_ExistingButtonsInBar;
};    


class OpParamBarDuplication : public OpParam
{
public:
	OpParamBarDuplication()
			{	
				m_OldBarName = _T("Bar1");
				m_NewBarName = _T("Bar2");
				m_ButtonsToAdd = 0;
				m_Spacing = 0;
				m_IsHoriz = FALSE;
			}

	OpParamBarDuplication(String_256 OldBarName, String_256 NewBarName, INT32 ButtonsToAdd = 0, INT32 Spacing = 0, BOOL IsHoriz = FALSE)
			{	
				m_OldBarName = OldBarName;
				m_NewBarName = NewBarName;
				m_ButtonsToAdd = ButtonsToAdd;
				m_Spacing = Spacing;
				m_IsHoriz = IsHoriz;
			}

	String_256 m_OldBarName;
	String_256 m_NewBarName;
	INT32 m_ButtonsToAdd;
	INT32 m_Spacing;
	BOOL m_IsHoriz;
};



/********************************************************************************************

>	class CCAPI OpShortenBar: public CarbonCopyOp

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Purpose:	Will cut down the size of a bar
				to the number of buttons requested by
				deleting the last buttons in the bar
********************************************************************************************/
class CCAPI OpShortenBar: public CarbonCopyOp
{
	CC_DECLARE_DYNCREATE( OpShortenBar )    

public:
	OpShortenBar();								
	static BOOL	Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void			DoWithParam(OpDescriptor* token, OpParam* pOpParam);
	BOOL TrimBarDownToThisManyButtons(const StringBase &BarName, INT32 NewNoOfButtons);
};    


class OpParamShortenBar : public OpParam
{
public:
	OpParamShortenBar()
			{	
				m_BarName = _T("Bar1");
				m_NoOfButtons = 4;
			}

	OpParamShortenBar(String_256 BarName, INT32 NoOfButtons)
			{	
				m_BarName = BarName;
				m_NoOfButtons = NoOfButtons;
			}

	String_256 m_BarName;
	INT32 m_NoOfButtons;
};



/********************************************************************************************

>	class CCAPI OpShortenBar: public CarbonCopyOp

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Purpose:	Will cut down the size of a bar
				to the number of buttons requested by
				deleting the last buttons in the bar
********************************************************************************************/
class CCAPI OpDelBar: public CarbonCopyOp
{
	CC_DECLARE_DYNCREATE( OpDelBar )    

public:
	OpDelBar();								
	static BOOL	Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void			DoWithParam(OpDescriptor* token, OpParam* pOpParam);
	static BOOL DelBar(const StringBase &BarName, INT32 State, UndoableOperation * pOp);
};    


class OpParamDelBar : public OpParam
{
public:
	OpParamDelBar()
			{	
				m_BarName = _T("Bar1");
				m_State = 5; // 5 == all states
			}

	OpParamDelBar(String_256 BarName, INT32 State)
			{	
				m_BarName = BarName;
				m_State = State;
			}

	String_256 m_BarName;
	INT32 m_State;
};

/********************************************************************************************

>	class CCAPI OpShortenBar: public CarbonCopyOp

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Purpose:	Will cut down the size of a bar
				to the number of buttons requested by
				deleting the last buttons in the bar
********************************************************************************************/
class CCAPI OpShowState: public UndoableOperation
{
	CC_DECLARE_DYNCREATE( OpShowState )    

public:
	OpShowState();								
	static	BOOL	Init();				
	static	OpState	GetState(String_256*, OpDescriptor*);		
    void	DoWithParam(OpDescriptor* token, OpParam* pOpParam);
	static void	ShowState(INT32 ShowLayer, UndoableOperation * pOp);
};    




#endif // OPTOKEN_BARDUPLICATIONOP
