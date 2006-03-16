// $Id: hlinkdlg.h 662 2006-03-14 21:31:49Z alex $
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

//The Web Address dialog
#ifndef INC_WEBADDRESSDLG
#define INC_WEBADDRESSDLG

#include "dialogop.h"
#include "selop.h"
#include "webattr.h"

#define OPTOKEN_WEBADDRESSDLG _T("WebAddressDlg")
#define OPTOKEN_HOTLINKOP _T("HotLinkOp")


/********************************************************************************************

>	typedef enum WebCommonAttributeResult

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Purpose:	This class shows what common WebAddressAttributes are applied
				to the selection, as follows:

  				WCA_NONE		Nothing is selected

				WCA_SINGLE		Identical attributes are applied to the entire selection

				WCA_MANY			Different attributes are applied to the selection

				WCA_DEFAULT		The default attribute is applied to the selection

				It's very similar to Range::CommonAttribResult, but with the extra
				"WCA_DEFAULT" member added in, because the Web Address dialog box
				treats the default attribute as a special case.
																				 
********************************************************************************************/

typedef enum 
{
	WCA_NOSELECTION,
	WCA_SINGLE,
	WCA_MANY,
	WCA_DEFAULT
} WebCommonAttributeResult;


/********************************************************************************************

>	class WebAddressDlg : public DialogOp

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96 - rewritten by Graham 21/3/97
	Purpose:	The new Web Address dialog

				Note that the WebAddressDlg class does not use the WebAddress class
				in any way! (I did that to confuse you).

********************************************************************************************/

class WebAddressDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE(WebAddressDlg);

	//Constructors, destructors and initialiser
public:
	WebAddressDlg();
	~WebAddressDlg();

	static	BOOL Init();

	// Creating the dialog
public:
	void Do(OpDescriptor*);
	void DoWithParam(OpDescriptor* pOp, OpParam* pParam);

protected:
	BOOL Create();

	// Operations (functions that other objects call)
public:
	virtual	MsgResult Message(Msg* Message);
	static	OpState GetState(String_256*, OpDescriptor*);

	//Message handling functions, called from ::Message
protected:
	void OnCreate();
	void OnAddButtonClicked();
	void OnRemoveButtonClicked();
	void OnAutoCorrectClicked();
	void OnDialogChange();
	void OnSelectionChange();

	//Actions (functions which do things to other objects)
protected:
	void SetDialogInitialState();
	void SetDialogChangedState();
	void SetDialogInitialControls();
	void ApplyWebAddress();
	void RemoveWebAddress();

	//Control functions (functions to display information in the dialog
	//box or to get information from the dialog box)
	void ShowURL(TCHAR* pcURL=NULL);
	void ShowURL(UINT32 uiURL);
	String_256 GetURL();

	void ShowCorrect(BOOL fValue=TRUE);
	BOOL GetCorrect();


	void ShowFrame(TCHAR* pcFrame=NULL);
	void ShowFrame(UINT32 uiFrame);
	String_256 GetFrame();

	void ShowClickableArea(UINT32 uiArea);
	UINT32 GetClickableArea();

	void EnableAdd(BOOL fValue=TRUE);
	void EnableRemove(BOOL fValue=TRUE);  

	void ShowWebAddressOnSelection(BOOL fValue=TRUE);

	//Toolkit functions (called by action functions)
protected:
	WebCommonAttributeResult FindCommonWebAttribute(WebAddressAttribute* pwaaReturn=NULL);

	BOOL SelectionConsistsOfText();


		
	//Member variables
protected:
	static WebAddressDlg   *TheDialog;	// Points to the only Web Address dialog object
	static BOOL				DontHandleNextTextMessage;
	BOOL fDialogIsInInitialState;
	
public:
	static const UINT32 IDD;
	static const CDlgMode Mode;

	//Variable used to store things in the registry
public:
	static BOOL ms_fCorrect;
		
};

#endif //INC_WEBADDRESSDLG

