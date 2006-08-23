// $Id: blobby.cpp 1282 2006-06-09 09:46:49Z alex $
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

// Implemetation of the following classes
/*
	OperationHistory : The operation history used to store undoable operations
	OpUndo           : The undo operation
	OpRedo           : The redo operation

	BlobbyOp         : Demonstration temporary operation which changes the colour of a
					   documents paper.

	UndoColAction    : BlobbyOp undo action
	RedoColAction    : BlobbyOp redo action
*/

/*
*/
#include "camtypes.h"
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]

DECLARE_SOURCE("$Revision: 1282 $");

#include "selmedia.h"
#include "ophist.h"
#include "lineattr.h"
//#include "resource.h"
#include "page.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"
//#include "simon.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "blobtab.h"
#include <stdlib.h>

#include "bfxatom.h"
#include "tracergn.h"
#include "bubbleid.h"

CC_IMPLEMENT_DYNCREATE(SelMediaDlg, DialogOp)
CC_IMPLEMENT_DYNAMIC(SelMediaDlgParam, OpParam)

#define new CAM_DEBUG_NEW

//------------------------------------------------------------------------------------------
// Demo Dialog

const CDlgMode SelMediaDlg::Mode = MODAL;// Mode of the dialog

const UINT32 SelMediaDlg::IDD = _R(IDD_SEL_MEDIA_APP_DIALOG);

SelMediaDlg::SelMediaDlg(): DialogOp(SelMediaDlg::IDD, SelMediaDlg::Mode), m_pOptions( NULL )
{
}

// -----------------------------------------------------------------------------------------

void SelMediaDlg::InitValues()
{
	bool				fItemSel = false;

	// Add possible replay apps
	SelMediaDlgParam::CMediaAppListIter	end( m_pOptions->m_pAppList->end() );
	SelMediaDlgParam::CMediaAppListIter	iter( m_pOptions->m_pAppList->begin() );
	for( INT32 ord = 0; iter != end; ++iter, ++ord )
	{
		SetStringGadgetValue( _R(IDC_LIST1), String_256(iter->first), FALSE, ord );

		if( m_pOptions->m_strSel != _T("") )
		{
			if( m_pOptions->m_strSel == iter->first )
			{
				fItemSel = true;
				SetSelectedValueIndex( _R(IDC_LIST1), ord );
			}
		}
		else
		if( iter->second )
		{
			fItemSel = true;
			SetSelectedValueIndex( _R(IDC_LIST1), ord );
		}
	}

	if( !fItemSel )
		SetSelectedValueIndex( _R(IDC_LIST1), 0 );
}

// -----------------------------------------------------------------------------------------

MsgResult SelMediaDlg::Message( Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
		// Handle ok button
		if ((Msg->DlgMsg == DIM_COMMIT) || (Msg->DlgMsg == DIM_CANCEL))
		{
			m_pOptions->m_strSel = GetStringGadgetValue( _R(IDC_LIST1) );
			TRACEUSER( "jlh92", _T("Return sel is %s\n"), PCTSTR(m_pOptions->m_strSel) );

			m_pOptions->m_fValid = ( DIM_COMMIT == Msg->DlgMsg );

			Close(); // Hide the dialog
			End();
		}
		else
		if( DIM_SOFT_COMMIT == Msg->DlgMsg ||
			DIM_CREATE == Msg->DlgMsg )
			InitValues();

		return (DLG_EAT_IF_HUNGRY(Msg));
	}
	return OK;
}

// -----------------------------------------------------------------------------------------

OpState	SelMediaDlg::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	return(OpSt);
}

// -----------------------------------------------------------------------------------------

BOOL SelMediaDlg::Init()
{
	return (RegisterOpDescriptor(
 								0,
 								_R(IDS_SELMEDIADLG),
								CC_RUNTIME_CLASS(SelMediaDlg),
 								OPTOKEN_SELMEDIADLG,
 								SelMediaDlg::GetState,
 								0,	/* help ID */
 								_R(IDBBL_SELMEDIADLG),
 								0	/* bitmap ID */
								));
}

// -----------------------------------------------------------------------------------------

BOOL SelMediaDlg::Create()
{
	if (DialogOp::Create())
	{
		return TRUE;
    }
    else
		return FALSE;
}

// -----------------------------------------------------------------------------------------

void SelMediaDlg::Do(OpDescriptor*)
{
	Create();
	Open();
}

/******************************************************************************************

>	void BmpPrefsDlg::DoWithParam(OpDescriptor*)

	Author:		Luke_hart (Xara Group Ltd) <LukeH@xara.com>
	Created:	23/08/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user and
				allows values to be passed in and returned to the caller via the
				SelMediaDlgParam class.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void SelMediaDlg::DoWithParam(OpDescriptor*, OpParam* pParam)
{
	if (pParam == NULL)
	{
		ERROR3("SelMediaDlg::DoWithParam - NULL Args");
		return;
	}

	// Use the OpParam that has been passed in to us
	ERROR3IF( !pParam->IS_KIND_OF(SelMediaDlgParam), "pParam isn't" );
	m_pOptions = (SelMediaDlgParam*) pParam;

	// Force the dialog box to be created, as it is modal it will be opened via a message
	BOOL				fOk = Create();
	if( !fOk )
	{
		// Could not create the dialog box so call inform error 
		InformError();
  		End(); 		   // End the operation 
	}
}

