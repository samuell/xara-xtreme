// $Id: wxOil/xsepsops.cpp, 1, 01-Jan-2006, Anonymous $
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
//
// XSEPSExportOptions - implements the options relevant to the export of XS EPS

/*
*/

#include "camtypes.h"
#include "xsepsops.h"
//#include "printdlg.h"
#include "epsfiltr.h"
//#include "markn.h"
//#include "resource.h"
#include "helpuser.h"

DECLARE_SOURCE("$Revision: 2 $");

CC_IMPLEMENT_DYNCREATE(XSEPSExportOptions, DialogOp)

#define new CAM_DEBUG_NEW

const CDlgMode XSEPSExportOptions::Mode = MODAL ;// Mode of the dialog
const UINT32 XSEPSExportOptions::IDD = _R(IDD_XSEPSOPS);
BOOL XSEPSExportOptions::Aborted = FALSE;

/********************************************************************************************

>	XSEPSExportOptions::XSEPSExportOptions()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14/06/2006
	Inputs:		-
	Returns:	-
	Purpose:	Constructs the dlg.
	SeeAlso:	-

********************************************************************************************/


XSEPSExportOptions::XSEPSExportOptions(): DialogOp(XSEPSExportOptions::IDD, XSEPSExportOptions::Mode)
{
}


/********************************************************************************************

>	BOOL XSEPSExportOptions::Do(OpDescriptor*)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14/06/2006
	Inputs:		-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Inits the dialog's controls, and calls the base classes OnInitDialog() function
	SeeAlso:	-

********************************************************************************************/

void XSEPSExportOptions::Do(OpDescriptor*)
{
	Create(); // Modal dialog, so this will actually run the dialog
}

/********************************************************************************************

>	BOOL XSEPSExportOptions::GetState(String_256*, OpDescriptor*)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14/06/2006
	Inputs:		-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Inits the dialog's controls, and calls the base classes OnInitDialog() function
	SeeAlso:	-

********************************************************************************************/

OpState	XSEPSExportOptions::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	return(OpSt);
}

/********************************************************************************************

>	BOOL XSEPSExportOptions::XSEPSExportOptions::Init()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14/06/2006
	Inputs:		-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Inits the dialog's controls, and calls the base classes OnInitDialog() function
	SeeAlso:	-

********************************************************************************************/

BOOL XSEPSExportOptions::Init()
{
	return (RegisterOpDescriptor(
									0,
									_R(IDD_XSEPSOPS),
									CC_RUNTIME_CLASS(XSEPSExportOptions),
									OPTOKEN_XSEPSEXPORTOPTIONS,
									XSEPSExportOptions::GetState,
									0,	/* help ID */
									0, 	/* bubble help*/
									0	/* bitmap ID */
									));
}

/********************************************************************************************

>	MsgResult XSEPSExportOptions::Message( Msg* Message)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14/06/2006
	Inputs:		-
	Returns:	-
	Purpose:	Overrides the default OnOK() func so that we can check the values set by the user.
				If dodgy values have been put in, the user is warned.
	SeeAlso:	-

********************************************************************************************/

MsgResult XSEPSExportOptions::Message( Msg* Message)
{

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
		// Handle ok button
		if (Msg->DlgMsg == DIM_CREATE)
		{
			SetLongGadgetValue(_R(IDC_DPIEDIT), 150, FALSE, 0);
			SetLongGadgetValue(_R(IDC_DPIEDIT), 200, FALSE, 1);
			SetLongGadgetValue(_R(IDC_DPIEDIT), 300, FALSE, 2);
			SetLongGadgetValue(_R(IDC_DPIEDIT), (INT32) EPSFilter::XSEPSExportDPI, FALSE, -1);
	
			/*
			SetBoolGadgetSelected(_R(IDC_PSLEVELAUTO),	EPSFilter::XSEPSExportPSType == 0);
			SetBoolGadgetSelected(_R(IDC_PSLEVEL1),	EPSFilter::XSEPSExportPSType == 1);
			SetBoolGadgetSelected(_R(IDC_PSLEVEL2),	EPSFilter::XSEPSExportPSType == 2);
			*/
	
			SetBoolGadgetSelected(_R(IDC_EXPORTTEXTASCURVES), EPSFilter::XSEPSExportTextAsCurves);
		}
		else if (Msg->DlgMsg == DIM_COMMIT)
		{
			Aborted = FALSE;
			// DPI ed field
			UINT32 DPI = GetLongGadgetValue(_R(IDC_DPIEDIT),10,2400);
			if (DPI < 10)
				DPI = 10;
		
			if (DPI > 600)
				DPI = 600;

			if (DPI > 300)
			{
				INT32 b = InformWarning(_R(IDS_EXPORT_BIGDPI),_R(IDS_OK),_R(IDS_CANCEL));
				if (b != 1)
					Aborted = TRUE;
			}

			if (Aborted)
			{
				Msg->DlgMsg=DIM_NONE; // prevent dialog from going away
			}
			else
			{
				/*
				// Job 10463: remove PS Level bits - default to Level 2
				INT32 ps = 0;
				if (GetBoolGadgetSelected(_R(IDC_PSLEVEL1)))
					ps = 1;
				if (GetBoolGadgetSelected(_R(IDC_PSLEVEL2)))
					ps = 2;
				*/
			
				EPSFilter::XSEPSExportDPI 	 = DPI;
				// EPSFilter::XSEPSExportPSType = ps;
			
				EPSFilter::XSEPSExportTextAsCurves = GetBoolGadgetSelected(_R(IDC_EXPORTTEXTASCURVES));
			}	
		}
	}
	return DialogOp::Message(Message);
}

