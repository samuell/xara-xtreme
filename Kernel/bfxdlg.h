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
// Header for bitmap effects dialog

#ifndef INC_BFXDLG
#define INC_BFXDLG

#include "dialogop.h"
#include "undoop.h"

#define OPTOKEN_BFXOP _T("BfxOp")

class KernelBitmap;
class AccusoftBitmapEffect;
class OpDescriptor;
class BitmapInfo;
class Spread;
class DocCoord;


/*********************************************************************************************
>	class BfxMessage : public Msg

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Purpose:	This is a foolish message to bring up a bitmap effects dialog
	SeeAlso:	BfxDlg

*********************************************************************************************/

class BfxMsg : public Msg
{
	CC_DECLARE_DYNAMIC(BfxMsg)
public:
	KernelBitmap * pBitmap;
	BfxMsg(KernelBitmap * pTheBitmap):pBitmap(pTheBitmap) { /* empty */ };
	static void OpenOrUse(KernelBitmap * pBitmap);
};


/********************************************************************************************

>	class BfxDlg : public UndoableOperation

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/94
	Purpose:	Handles insertion of bitmap effect nodes into the tree

********************************************************************************************/

class CCAPI BfxOp: public UndoableOperation
{                             
	CC_DECLARE_DYNCREATE( BfxOp )
public:
	BfxOp();								// Constructor
	static BOOL		Init();					// Register an OpDescriptor
	                                        // Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		
	void	DoWithParam(OpDescriptor* pOp, OpParam* pParam);	     
protected:
	BOOL FindCentreInsertionPosition(Spread** Spread, DocCoord* Position);
};

class BfxOpParam: public OpParam
{
	CC_DECLARE_MEMDUMP( BfxOpParam);
public:
	KernelBitmap * * ppBitmap;
};
     

/********************************************************************************************

>	class BfxDlg : public DialogTabOp

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/94
	Purpose:	Handles the dialog for bitmap effects

********************************************************************************************/

#define OPTOKEN_BFXDLG _T("BfxDlg")    
   
class BfxDlg: public DialogTabOp
{         
	CC_DECLARE_DYNCREATE( BfxDlg )  
public:

	BfxDlg(); 
	~BfxDlg();

	MsgResult Message( Msg* Message ); 
	 
	void Do(OpDescriptor*);		// "Do" function        
	void DoWithParam(OpDescriptor* pOp, OpParam* pParam);
	static BOOL Init();                        
	static OpState GetState(String_256*, OpDescriptor*);	
	virtual BOOL RegisterYourPagesInOrderPlease(); 
	static const CDlgMode Mode; 

	// We need to give our Tab dialog's a unique resource ID as this is used to 
	static const UINT32 IDD;

	private:
	
	INT32 Param1;
	INT32 Param2;
	INT32 Param3;
	double Matrix[9];

	// Message handlers for each page in the tabbed dialog
	void HandleFlipAndRotateMsg(DialogMsg* Msg, BOOL * pCommitValues, BOOL * pEndDialog); 
	void HandleResizeMsg(DialogMsg* Msg, BOOL * pCommitValues, BOOL * pEndDialog); 
	void HandleBrightnessAndContrastMsg(DialogMsg* Msg, BOOL * pCommitValues, BOOL * pEndDialog);
	void HandleDitherMsg(DialogMsg* Msg, BOOL * pCommitValues, BOOL * pEndDialog);
	void HandleSpecialEffectsMsg(DialogMsg* Msg, BOOL * pCommitValues, BOOL * pEndDialog);
	void HandleAlchemistMsg(DialogMsg* Msg, BOOL * pCommitValues, BOOL * pEndDialog);

	BOOL RenderBitmap(ReDrawInfoType* ExtraInfo, KernelBitmap * BitmapToUse);
	BOOL FixOriginalComboBox();
	BOOL ReadOriginalComboBox();
	BOOL SetParameters(INT32 p1=0, INT32 p2=0, INT32 p3=0)
					{ Param1=p1; Param2=p2; Param3=p3; return TRUE;};


	BOOL CheckOriginalBitmap();
	BOOL DestinationChanged();
	BOOL FindBitmap();
	BOOL OriginalChanged();
	BOOL ClearDestination(BOOL Redraw=TRUE);
	BOOL SetEffect(CCRuntimeClass * Effect);
	BOOL SetDirty(BOOL Redraw=TRUE);

	KernelBitmap * pOriginal;
	KernelBitmap * pDestination;
	KernelBitmap * pPrevOriginal;
	BitmapInfo * pOrigInfo;
	BOOL Clean;

	AccusoftBitmapEffect * pFX;

	CDlgResID OpenPage;

	static BOOL InteractiveBCC;	
}; 
                   

#endif  // INC_BFXDLG


