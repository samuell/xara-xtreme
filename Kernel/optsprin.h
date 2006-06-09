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
//
// Header for the handling code for the Print tabs of the options dialog box 
//	 WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER

#ifndef INC_OPTSPRIN
#define INC_OPTSPRIN

//#include "dialogop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "appprefs.h"
#include "printctl.h"

//--------------------------------------------------
class Layer;
//#include "stockcol.h" - in camtypes.h [AUTOMATICALLY REMOVED]

/********************************************************************************************

>	class PrintBaseTab: public OptionsTabs

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Purpose:	Allows the user to set any options which are associated with general document printing.

	SeeAlso:	OptionsTabs; AppPrefsDlg; DocPrefsDlg;

********************************************************************************************/

class PrintBaseTab: public OptionsTabs
{         
	CC_DECLARE_DYNCREATE( PrintBaseTab )  
public:

	PrintBaseTab();
	~PrintBaseTab();

	// Function to set up the values on this tab when being opened
	virtual BOOL InitSection();

	// Function to handle dialogue messages. Called by derived classes for unhandled messages
	virtual BOOL HandleMsg(DialogMsg* Msg);

	// Functions to handle document changing messages
	virtual BOOL GreySection();								// Called when the user has closed all documents
	virtual BOOL UngreySection();							// Called when the user has opened a document
	virtual BOOL UpdateSection(String_256 *DocumentName);	// Called when the user has switched to a new document
	virtual BOOL PageSizeHasChanged();						// Called when the spread's page changes size

	virtual BOOL IsDocumentOption();	// Return True if document option  
	virtual BOOL IsProgramOption();		// Return True if program option
	virtual BOOL IsPrintingOption();	// Return True if printing option

	BOOL IsAllGreyed() { return AllGreyed; }

	// Called when AppPrefsDlg::Message() gets a print settings changed message
	void PrintSettingsChanged();

	static void DocumentSettingsUpdated(void);
		// Called by the AppPrefs dlg when committing to make sure that we don't have an invalid PrintControl cache


protected:
	void CopyLocalPrintControlFromDoc(void);				// Copy PrintCtrl data between the local copy and the
	void CopyLocalPrintControlToDoc(void);					// selected document copy

	virtual BOOL CommitSection();

	PrintControl* 	GetPrintControl();	// Finds the print control object contained within the associated doc
	void 			EnableControl(CGadgetID Gadget, BOOL Enabled);	// Calls EnableGadget() taking AllGreyed into acount
	BOOL 			TalkToPage();		// Calls pPrefsDlg->TalkToPage() with GetPageID() ID

	virtual BOOL EnableControls() 	= 0;	// Enables/disables the controls
	virtual BOOL ShowDetails() 		= 0;	// Displays the print control data in the tab

	// This local PrintControl contains a copy of the associated document's print control object.
	// The local copy is copied from the document when InitSection() and UpdateSection() is called.
	// The local copy is copied into the document when CommitSection() is called.
	// NOTE that we have a static print control because it must be SHARED by all the different tabs!
	// The document is a point to the last document we copied the info from - used only to allow us to
	// cache the data to stop multiple attempts to read it from continually overwriting other tab's "cache".
	static PrintControl LocalPrintControl;
	static Document *pLastPrintControlDocument;

private:
	void UpdateDocName();
	BOOL AllGreyed;			// Remember the state that we are currently in

	friend class SepsDlg;
}; 

//-------------------------------------------------

/********************************************************************************************

>	class PrintGeneralTab: public PrintBaseTab

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Purpose:	Allows the user to set any options which are associated with general document printing.

	SeeAlso:	OptionsTabs; AppPrefsDlg; DocPrefsDlg;

********************************************************************************************/

class PrintGeneralTab: public PrintBaseTab
{         
	CC_DECLARE_DYNCREATE( PrintGeneralTab )  
public:
	PrintGeneralTab();
	~PrintGeneralTab();

	// Function to handle the messages for this tab/pane window
	virtual BOOL HandleMsg(DialogMsg* Msg);

	// Function to commit the values on this tab on ok
	virtual BOOL CommitSection();

	// Return section/page id system
	virtual CDlgResID GetPageID();

private:
	BOOL EnableControls();								// Enable/disable all tab controls correctly
	virtual BOOL ShowDetails();
}; 

//-------------------------------------------------

/********************************************************************************************

>	class PrintLayoutTab: public PrintBaseTab

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Purpose:	Allows the user to set any options which are associated with general document printing.

	SeeAlso:	OptionsTabs; AppPrefsDlg; DocPrefsDlg;

********************************************************************************************/

class PrintLayoutTab: public PrintBaseTab
{         
	CC_DECLARE_DYNCREATE( PrintLayoutTab )  
public:
	PrintLayoutTab();
	~PrintLayoutTab();

	// Function to handle the messages for this tab/pane window
	virtual BOOL HandleMsg(DialogMsg* Msg);

	// Function to commit the values on this tab on ok
	virtual BOOL CommitSection();

	// Function to set up the values on this tab when being opened
	virtual BOOL InitSection();

	// Return section/page id system
	virtual CDlgResID GetPageID();

private:
	BOOL EnableControls();								// Enable/disable all tab controls correctly
	virtual BOOL ShowDetails();
	
	BOOL ShowDetails(CGadgetID* pGadgetIDList,CGadgetID Exclude);

public:
	#ifdef _DEBUG
	static void TestPrinting(PrintControl* pPrCtrl);
	static void MakeRectangle(Layer* pLayer,DocRect Rect,StockColour Col);
	#endif
}; 


class SepsDlgParam : public OpParam
{
public:
	ColourPlate *pColourPlate;
	PrintBaseTab* pParent;
	BOOL Result;
};


/********************************************************************************************

>	class SepsDlg : public DialogOp

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Purpose:	Dialog for changing the properties of individual separations

	SeeAlso:	PrintSepsTab

********************************************************************************************/

#define OPTOKEN_SEPSDIALOG _T("SepsDialog")

class SepsDlg : public DialogOp
{
	public:
		SepsDlg();
		~SepsDlg();

		MsgResult Message(Msg* Message);
		BOOL Create();

		static const INT32 IDD;
		static const CDlgMode Mode;

		static OpState GetState(String_256*, OpDescriptor*);

		void Do(OpDescriptor*);
		void DoWithParam(OpDescriptor*, OpParam *Param);

		static BOOL Init(void);
		static BOOL InvokeDialog(ColourPlate *pTheColourPlate, PrintBaseTab* pParentDialog);

	protected:
		void InitControls(void);
		MsgResult HandleMessage(DialogMsg* Msg);

	private:
		ColourPlate* pColourPlate;
		ColourPlate* pLocalColourPlate;
		PrintBaseTab* pParent;
		SepsDlgParam *pParam;

	CC_DECLARE_DYNCREATE(SepsDlg);
};




/********************************************************************************************

>	class PrintSepsTab: public PrintBaseTab

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Purpose:	Allows the user to set options which are associated with printing colour separations.

	SeeAlso:	OptionsTabs; AppPrefsDlg; DocPrefsDlg;

********************************************************************************************/

class PrintSepsTab: public PrintBaseTab
{         
	CC_DECLARE_DYNCREATE( PrintSepsTab )  
public:
	PrintSepsTab();
	~PrintSepsTab();

	// Function to handle dialog messages for this tab/pane window
	virtual BOOL HandleMsg(DialogMsg* Msg);

	// Function to handle generic messages for this tab/pane window
	virtual MsgResult Message(Msg *pMessage);

	// Function to commit the values on this tab on ok
	virtual BOOL CommitSection();

	// Function to set up the values on this tab when being opened/closed/shaded/unshaded
	virtual BOOL InitSection();
	virtual BOOL GreySection();				// Called when the user has closed all documents
	virtual BOOL UngreySection();			// Called when the user has opened a document
	virtual BOOL UpdateSection(String_256 *DocumentName);	// Called when the user has switched to a new document

	// Return section/page id system
	virtual CDlgResID GetPageID();


private:
		INT32 ConvertDoubleToMinString(double Value, String_256 *Result);
				// Converts a double into a string with a minimal number of decimal places, up to a max of 4.
				// e.g 1.23456 -> 1.2345    1.230000 -> 1.23    1.00001 -> 1.0

		void ReadCurrentPlateSettings(TypesetInfo *pTypesetInfo);
				// Reads the current plate settings for all plates into the given TypesetInfo structure

private:
	BOOL OldPrintSpotsAsProcess;			// Used to allow cancel of this option (which must be applied interactively)

private:
	BOOL EnableControls();								// Enable/disable all tab controls correctly
	virtual BOOL ShowDetails();
	ColourPlate* GetSelectedPlate();
	BOOL ShowDetails(CGadgetID* pGadgetIDList,CGadgetID Exclude);

/*
public:
	#ifdef _DEBUG
	static void TestPrinting(PrintControl* pPrCtrl);
	static void MakeRectangle(Layer* pLayer,DocRect Rect,StockColour Col);
	#endif

*/

}; 

//-------------------------------------------------

/********************************************************************************************

>	class PrintImagesetterTab: public PrintBaseTab

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Purpose:	Allows the user to set various imagesetting options.

	SeeAlso:	OptionsTabs; AppPrefsDlg; DocPrefsDlg;

********************************************************************************************/

class PrintImagesetterTab: public PrintBaseTab
{         
	CC_DECLARE_DYNCREATE( PrintImagesetterTab )  
public:
	PrintImagesetterTab();
	~PrintImagesetterTab();

	// Function to handle the messages for this tab/pane window
	virtual BOOL HandleMsg(DialogMsg* Msg);

	// Function to handle generic messages for this tab/pane window
	virtual MsgResult Message(Msg *pMessage);

	// Function to commit the values on this tab on ok
	virtual BOOL CommitSection();

	// Function to set up the values on this tab when being opened
	virtual BOOL InitSection();

	virtual BOOL GreySection();				// Called when the user has closed all documents

	virtual BOOL UngreySection();			// Called when the user has opened a document
	
	virtual BOOL UpdateSection(String_256 *DocumentName);	// Called when the user has switched to a new document

	// Return section/page id system
	virtual CDlgResID GetPageID();


private:
	BOOL EnableControls();								// Enable/disable all tab controls correctly
	virtual BOOL ShowDetails();


private:
	void EnsureSensiblePrintMarks(void);				// Make sure no 2 star/target marks are simultaneously enabled
};




#define	OPTOKEN_SHOWPRINTBORDERS	_T("ShowPrintBorders")

class OpDescriptor;

/*******************************************************************************************

>	class OpShowPrintBorders : public Operation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	Purpose:	Allows the user to toggle the visible state of the print borders
	Notes:		-

********************************************************************************************/

class OpShowPrintBorders : public Operation
{
	CC_DECLARE_DYNCREATE( OpShowPrintBorders )

public:
	OpShowPrintBorders();

	void Do(OpDescriptor*);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);
};



#endif // INC_OPTSPRIN

#endif //webster
