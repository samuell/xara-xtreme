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
// Trace dialog

#ifndef INC_TRACEDLG
#define INC_TRACEDLG


// Find out about the DialogOp Class
#include "dialogop.h"
#include "doccolor.h"
//#include "undoop.h"
#include "selop.h"
#include "tracectl.h"
//#include <afxtempl.h>

class KernelBitmap;
class TraceControl;
class Spread;
class DocCoord;

#define OPTOKEN_TRACEDLG _T("TraceDlg")
#define OPTOKEN_TRACEOP _T("TraceOp")

enum TraceSetting { TRACESETTING_PASSES = 0,
					TRACESETTING_REMOVENOISE,
					TRACESETTING_MINIMUMAREA,
					TRACESETTING_INITIALCOLOURTOLERANCE,
					TRACESETTING_FINALCOLOURTOLERANCE,
					TRACESETTING_ACCURACY,
					TRACESETTING_SMOOTING,

					// the end
					TRACESETTING_ILLEGAL,
					NUM_TRACESETTING=TRACESETTING_ILLEGAL};

/*********************************************************************************************
>	class TraceMessage : public Msg

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Purpose:	This is a foolish message to bring up a tracer dialog
	SeeAlso:	BfxDlg

*********************************************************************************************/

class TraceMsg : public Msg
{
	CC_DECLARE_DYNAMIC(TraceMsg)
public:
	KernelBitmap * pBitmap;
	TraceMsg(KernelBitmap * pTheBitmap):pBitmap(pTheBitmap) { /* empty */ };
	static void OpenOrUse(KernelBitmap * pBitmap);
};

/********************************************************************************************

>	class TraceOp : public UndoableOperation

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/94
	Purpose:	Handles insertion of bitmap effect nodes into the tree

********************************************************************************************/

class CCAPI TraceOp: public SelOperation
{                             
	CC_DECLARE_DYNCREATE( TraceOp )
public:
	TraceOp();								// Constructor
	static BOOL		Init();					// Register an OpDescriptor
	                                        // Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		
	void DoWithParam(OpDescriptor* pOp, OpParam* pParam);
	BOOL FindCentreInsertionPosition(Spread** Spread, DocCoord* Position);
};

class TraceOpParam: public OpParam
{
	CC_DECLARE_MEMDUMP( TraceOpParam);
public:
	TraceControl * pTraceControl;
};
     

/********************************************************************************************

>	class TraceDlg : public DialogOp

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	Dialog for tracer

********************************************************************************************/

class TraceDlg : public DialogOp
{
	// Give my name in memory dumps
	CC_DECLARE_DYNCREATE(TraceDlg);

public:
	// Construction etc
	TraceDlg();
	~TraceDlg();
	static	BOOL Init();

	// Called when the dialog is to be displayed
	void	Do(OpDescriptor*);
	void 	DoWithParam(OpDescriptor* pOp, OpParam* pParam);
	virtual	MsgResult Message(Msg* Message);

	// Find out about the state of the dlg
	static	OpState GetState(String_256*, OpDescriptor*);
	
protected:
	BOOL RenderBitmap(ReDrawInfoType* ExtraInfo, KernelBitmap * BitmapToUse);
	BOOL FindBitmap();

	BOOL LoadSettings(INT32 * pSettings);
	BOOL SaveSettings(INT32 * pSettings);
	BOOL ReadParams();
	BOOL WriteParams();
	BOOL SetStatus(BOOL Tracing=FALSE);

	BOOL DoTrace(BOOL SingleStep);
	BOOL FixOriginalComboBox();
	BOOL ReadOriginalComboBox();
	BOOL CheckOriginalBitmap();
	BOOL DestinationChanged();
	BOOL OriginalChanged();

	BOOL SetTraceMethod(TraceMethod Method);
	TraceMethod GetTraceMethod();

	std::list< String_256 > m_p24BitCopyNames;
	void Remove24BitTempCopy(); // Remove any 24bit copies of 32bit BMPs

public:
	// Vars
	static const UINT32 IDD;
	static const CDlgMode Mode;

protected:

	KernelBitmap * pOriginal;
	KernelBitmap * pPrevOriginal;
	TraceControl * pTraceControl;

	static INT32 DefaultMode;
	static INT32 DefaultSetting[NUM_TRACEMETHOD][NUM_TRACESETTING];
	static INT32 FactorySetting[NUM_TRACEMETHOD][NUM_TRACESETTING];

};

#endif  // INC_TRACEDLG


