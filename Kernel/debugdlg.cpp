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
// debugdlg.cpp - The debug dialogue

//////////////////////////////////////////////////////////////////////////////////////////////
// This dialogue appears on the debug menu in debug builds only								//
// It offers a dropdown list of 'command groups'. Selecting one of these then shows a list	//
// of available debugging commands which the user can invoke. Output from these commands	//
// will generally appear in the TRACE output. This offers a simple way of adding debug		//
// test code to the camelot menu tree so it can be easily triggered when you're ready for	//
// it. Add your code to the last 4 functions in this file									//
//		SetGroupList	- Sets the list of available groups									//
//		SetCommandList	- Sets the list of commands for the given group						//
//		SetCommandValue	- Sets the value field and help string for a given command			//
//		InvokeCommand	- Applies the given command from the given group, using whatever	//
//						  value the user typed into the 'value' field.						//
//////////////////////////////////////////////////////////////////////////////////////////////


#include "camtypes.h"

#if _DEBUG

// Includes for the dialogue itself
#include "debugdlg.h"	// kernel header
//#include "dbgdlg.h"		// winoil resource header
//#include "jason.h"		// For _R(IDS_DEBUGDLG)
#include "progress.h"	// For the progress hourglass
//#include "resource.h"	// For _R(IDS_CANCEL)

// Includes for specific debug commands
#include "colcontx.h"	// Colour separation commands
#include "colormgr.h"
#include "colplate.h"
#include "docview.h"

#include "app.h"		// Bitmap colour matching commands
#include "bmplist.h"
#include "bitmap.h"
#include "bitmpinf.h"

#include "prnmks.h"		// Printer marks

#include "princomp.h"	// Print component (for always overprint black)
#include "printctl.h"

#include "bmplist.h"	// For bitmap list dumps

#include "nodecont.h"	// ShadowType

#include "attrmgr.h"	// For Stroke Provider commands
#include "groupops.h"
#include "lineattr.h"
#include "mkshapes.h"
#include "pathstrk.h"
#include "ppairbsh.h"
#include "ppstroke.h"
#include "ppvecstr.h"
#include "strkattr.h"
#include "valfunc.h"

#include "objcache.h"	// Object cache
#include "cachrand.h"
#include "cachweak.h"
#include "cachfifo.h"
#include "fillattr.h"
#include "opimgset.h"

#include "opliveeffects.h"
//#include "xpehost.h"
#include "spread.h"

#include <time.h>
#include <stdlib.h>

#if FALSE
#include <activscp.h>
#include "ccc.h"	// <---- not here
#endif

//#include "mainfrm.h"

CC_IMPLEMENT_DYNCREATE(DebugDlg, DialogOp)

#define new CAM_DEBUG_NEW


const INT32 DebugDlg::IDD = _R(IDD_DEBUGDLG);
const CDlgMode DebugDlg::Mode = MODELESS;


// Remember which command was selected last time we were open
static INT32 LastSelectedIndex = 0;
static INT32 LastSelectedGroup = 8;		// StrokeProviders. Geez I'm lazy!
static BOOL CloseAndExecute = FALSE;

#if FALSE
static TCHAR g_szScriptToExecute[] = TEXT("ScriptToExecute");
static String_256 g_ScriptToExecute;

// Default to VBScript
static TCHAR g_szScriptEngineCLSID[] = TEXT("B54F3741-5B07-11CF-A4B0-00AA004A55E8");
static String_256 g_ScriptEngineCLSID;

static IActiveScript* m_pScriptEngine = NULL;
static IActiveScriptParse* m_pScriptParser = NULL;

// {BB1A2AE1-A4F9-11cf-8F20-00805F2CD064}
static const IID IID_IActiveScript =
{ 0xbb1a2ae1, 0xa4f9, 0x11cf, {0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64}};

// {BB1A2AE2-A4F9-11cf-8F20-00805F2CD064}
static const IID IID_IActiveScriptParse =
{ 0xbb1a2ae2, 0xa4f9, 0x11cf, {0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64}};

HRESULT InitializeScriptEngine(const GUID& ScriptCLSID)
{
	HRESULT Result = S_OK;

	Result = CoCreateInstance(ScriptCLSID, NULL, CLSCTX_INPROC_SERVER, IID_IActiveScript, (void**)&m_pScriptEngine);

	if (SUCCEEDED(Result))
	{
		Result = m_pScriptEngine->QueryInterface(IID_IActiveScriptParse, (void**)&m_pScriptParser);
	}

	if (SUCCEEDED(Result))
	{
		Result = m_pScriptEngine->SetScriptState(SCRIPTSTATE_INITIALIZED);
	}

	if (SUCCEEDED(Result))
	{
		Result = m_pScriptParser->InitNew();
	}

	if (SUCCEEDED(Result))
	{
		Result = m_pScriptEngine->AddNamedItem(OLESTR("Application"), SCRIPTITEM_ISVISIBLE);
	}


	return Result;
}

HRESULT ParseScript(const StringBase& ScriptName)
{
	HRESULT Result = S_OK;

	ScriptName; // dosomething withthis
	BSTR bstrScript;
	EXCEPINFO ExceptionInfo;
	Result = m_pScriptParser->ParseScriptText(bstrScript, NULL,NULL,NULL,0,0,SCRIPTTEXT_ISVISIBLE,NULL,&ExceptionInfo);

	if (SUCCEEDED(Result))
	{
		Result = m_pScriptEngine->SetScriptState(SCRIPTSTATE_STARTED);
	}

	return Result;
}


void ExecuteScript(const StringBase& ScriptName)
{
	HRESULT Result = S_OK;

	CLSID ScriptCLSID;
	LPOLESTR OSScriptCLSID = (LPOLESTR)((LPTSTR)g_ScriptEngineCLSID);
	Result = ::CLSIDFromString(OSScriptCLSID, &ScriptCLSID);
//    LPOLESTR lpsz,	//Pointer to the string representation of the CLSID
	if (SUCCEEDED(Result))
	{
		Result = InitializeScriptEngine(ScriptCLSID);
	}

	if (SUCCEEDED(Result))
	{
		Result = ParseScript(ScriptName);
	}


	
}

CC_IMPLEMENT_MEMDUMP(ConcurrencyController, CC_CLASS_MEMDUMP)
ConcurrencyController::ConcurrencyController()
{
}
ConcurrencyController::~ConcurrencyController()
{
}

void ExecuteScript(const StringBase& ScriptName)
{
	AfxMessageBox(TEXT("Not this week"));
}
#endif	// Automation

/********************************************************************************************

>	DebugDlg::DebugDlg(): DialogOp(DebugDlg::IDD, DebugDlg::Mode) 


	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/96

	Purpose:	DebugDlg constructor.

********************************************************************************************/

DebugDlg::DebugDlg() : DialogOp(DebugDlg::IDD, DebugDlg::Mode) 
{
}



/********************************************************************************************

>	DebugDlg::~DebugDlg()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/96

	Purpose:	You'll never guess...

********************************************************************************************/
 
DebugDlg::~DebugDlg()
{
}



/********************************************************************************************

>	MsgResult DebugDlg::Message( CDlgMessage DlgMsg, CGadgetID Gadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/96
	Inputs:		Message - the message
	Outputs:	-
	Returns:	OK if it is happy
	Purpose:	Handles all the debug dialog's messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult DebugDlg::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
		BOOL EndDlg = FALSE;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				{
					// Fill in the combobox of available command groups
					SetGroupList();

					// Fill in the list of available commands
					SetCommandList(LastSelectedGroup);

					// And then fill in the value/description fields as appropriate to whatever
					// option is now selected in the list.
					INT32 Index = GetSelectedValueIndex(_R(IDC_DEBUGDLG_LIST));
					SetCommandValue(LastSelectedGroup, Index);
					SetKeyboardFocus (_R(IDC_DEBUGDLG_VALUE1)) ;
					HighlightText     (_R(IDC_DEBUGDLG_VALUE1)) ;
					CloseAndExecute = FALSE;
				}
				break;

				
			case DIM_COMMIT:
			case DIM_SOFT_COMMIT:
				{
					LastSelectedGroup = GetSelectedValueIndex(_R(IDC_DEBUGDLG_GROUP));
					LastSelectedIndex = GetSelectedValueIndex(_R(IDC_DEBUGDLG_LIST));
					String_256 Value = GetStringGadgetValue(_R(IDC_DEBUGDLG_VALUE1));
					InvokeCommand(LastSelectedGroup, LastSelectedIndex, &Value);

					// And update the window state to reflect any change
					SetCommandValue(LastSelectedGroup, LastSelectedIndex);
				}
				break;

			case DIM_CANCEL:
				EndDlg = TRUE;
				break;

			case DIM_SELECTION_CHANGED:
				if (Msg->GadgetID == _R(IDC_DEBUGDLG_LIST))
				{
					LastSelectedIndex = GetSelectedValueIndex(_R(IDC_DEBUGDLG_LIST));
					SetCommandValue(LastSelectedGroup, LastSelectedIndex);
				}
				else if (Msg->GadgetID == _R(IDC_DEBUGDLG_GROUP))
				{
					LastSelectedGroup = GetSelectedValueIndex(_R(IDC_DEBUGDLG_GROUP));
					SetCommandList(LastSelectedGroup);
					LastSelectedIndex = 0;
					SetCommandValue(LastSelectedGroup, LastSelectedIndex);
				}
				break;

			case DIM_SELECTION_CHANGED_COMMIT:
				if (Msg->GadgetID == _R(IDC_DEBUGDLG_LIST))
				{
					LastSelectedGroup = GetSelectedValueIndex(_R(IDC_DEBUGDLG_GROUP));
					LastSelectedIndex = GetSelectedValueIndex(_R(IDC_DEBUGDLG_LIST));
					SetCommandValue(LastSelectedGroup, LastSelectedIndex);

					String_256 Value = GetStringGadgetValue(_R(IDC_DEBUGDLG_VALUE1));
					InvokeCommand(LastSelectedGroup, LastSelectedIndex, &Value);

					// And update the window state to reflect any change
					SetCommandValue(LastSelectedGroup, LastSelectedIndex);
				}
				break;
			default:
				// Do nothing
				break;
		}

		if (EndDlg)
		{
			Close();
			End();
			return(OK);
		}

		if (CloseAndExecute)
		{
			CloseAndExecute=FALSE;
			Close();
			End();
			InvokeLateCommand(LastSelectedGroup, LastSelectedIndex);
			return(OK);
		}

		return (DLG_EAT_IF_HUNGRY(Msg)); 
	}

	return OK; 
}



/********************************************************************************************

>	OpState	DebugDlg::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/96
	Purpose:	DebugDlg GetState method

********************************************************************************************/

OpState	DebugDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}


		 
/********************************************************************************************

>	BOOL DebugDlg::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/96

	Purpose:	DebugDlg Init method. Called by sginit.cpp

********************************************************************************************/

BOOL DebugDlg::Init(void)
{
#if FALSE
	Camelot.DeclarePref( "DebugFlags", g_szScriptToExecute, &g_ScriptToExecute);
	Camelot.DeclarePref( "DebugFlags", g_szScriptEngineCLSID, &g_ScriptEngineCLSID);
#endif
	return (RegisterOpDescriptor(
								0,
								_R(IDS_DEBUGDLG),
								CC_RUNTIME_CLASS(DebugDlg),
								OPTOKEN_DEBUGDLG,
								DebugDlg::GetState,
								0,	// help ID
								0   // bubble ID
								)
			);
}
 


/********************************************************************************************

>	BOOL DebugDlg::Create()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/96
	Returns:	TRUE if successful, else FALSE
	Purpose:	DebugDlg create method 

********************************************************************************************/

BOOL DebugDlg::Create()
{
	return(DialogOp::Create());
}



/********************************************************************************************

>	void DebugDlg::Do(OpDescriptor *Bob)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/96
	Inputs:		OpDescriptor of this op

	Purpose:	Creates then opens the dialog 

********************************************************************************************/

void DebugDlg::Do(OpDescriptor *Bob)
{
	if (!Create())
	{
		InformError();
  		End();
		return;
	}

	Open();
}



/********************************************************************************************

>	void DebugDlg::SetGroupList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/96

	Purpose:	Sets the list of debug dialogue command groups

				ADD YOUR OWN GROUP NAMES TO THE END OF THE LIST

				Note the index of the group in the list - this is the GroupIndex used
				in the related functions (below).

	Notes:		This only appears in DEBUG builds, so it's fine to inline strings

********************************************************************************************/

void DebugDlg::SetGroupList(void)
{
	String_256 Temp;

	DeleteAllValues(_R(IDC_DEBUGDLG_GROUP));

#define ADDGROUP(name) { Temp = TEXT(name); SetStringGadgetValue(_R(IDC_DEBUGDLG_GROUP), Temp); }

	ADDGROUP("Imagesetting (Jason)");		// Group 0
	ADDGROUP("Jason personal debug");		// Group 1
	ADDGROUP("Mikes personal debug");		// Group 2
	ADDGROUP("Wills bitmap debug");			// Group 3
	ADDGROUP("Low memory debug");			// Group 4
	ADDGROUP("Operation history dump");		// Group 5
// Neville 2/10/97 removal of vector stroking bits
#ifdef VECTOR_STROKING
	ADDGROUP("Shadow rendering tests");		// Group 6
	ADDGROUP("Stroke providers")			// Group 7
	ADDGROUP("Vector strokes")				// Group 8
	ADDGROUP("ObjectCache tests");			// Group 9
#endif // VECTOR_STROKING
#if FALSE
	ADDGROUP("Script Execution");			// Group 10
#endif
	ADDGROUP("Phil Debug");					// Group 10
	ADDGROUP("Gerry Debug");				// Group 11
	// Add more groups here...


#undef ADDGROUP

	// Select the last item the user selected, in the hope that this is useful
	SetSelectedValueIndex(_R(IDC_DEBUGDLG_GROUP), LastSelectedGroup);
}



/********************************************************************************************

>	void DebugDlg::SetCommandList(INT32 GroupIndex)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/96

	Inputs:		GroupIndex - indicates which command group is to be used

	Purpose:	Sets the list of debug dialogue commands

				ADD YOUR OWN COMMAND NAMES TO THE END OF THE LIST

				Note the index of the command in the list - this is the CommandIndex used
				in the related functions (below).

	Notes:		This only appears in DEBUG builds, so it's fine to inline strings

********************************************************************************************/

void DebugDlg::SetCommandList(INT32 GroupIndex)
{
	String_256 Temp;

	DeleteAllValues(_R(IDC_DEBUGDLG_LIST));

#define ADDCOMMAND(name) { Temp = TEXT(name); SetStringGadgetValue(_R(IDC_DEBUGDLG_LIST), Temp); }

	switch (GroupIndex)
	{
		case 0:		// Jason's colour separation controls ------------------------------------
			ADDCOMMAND("Normal document view");						// Command 0

			ADDCOMMAND("  Composite preview");						// Command 1
			ADDCOMMAND("  Cyan preview");							// Command 2
			ADDCOMMAND("  Magenta preview");						// Command 3
			ADDCOMMAND("  Yellow preview");							// Command 4
			ADDCOMMAND("  Key preview");							// Command 5

			ADDCOMMAND("  Spot plate");								// Command 6

			ADDCOMMAND("Monochrome toggle");						// Command 7
			ADDCOMMAND("Photographic negative toggle");				// Command 8

			ADDCOMMAND("Overprint toggle");							// Command 9
			ADDCOMMAND("Always overprint black");					// Command 10
			break;


		case 1:		// Jason's misc controls -------------------------------------------------
			ADDCOMMAND("Write line to debug stream");				// Command 0
			break;

		case 2:		// Mike's misc controls --------------------------------------------------
			ADDCOMMAND("Initialise print marks");					// Command 0
			ADDCOMMAND("Add doc print mark");						// Command 1
			ADDCOMMAND("Add a colour ramp entry");					// Command 2
			ADDCOMMAND("Add a transparency ramp entry");			// Command 3
			break;

		case 3:		// Will's debug controls -------------------------------------------------
			ADDCOMMAND("Dump global bitmap list");					// Command 0
			ADDCOMMAND("Dump greyscale bitmap list");				// Command 1
			break;

		case 4:		// Low memory testing (by Peter) -----------------------------------------
			ADDCOMMAND("Enable Low Memory Testing");				// Command 0
			ADDCOMMAND("  Fail all claims");						// Command 1
			ADDCOMMAND("  Fail all claims after n bytes");			// Command 2
			ADDCOMMAND("Memory dump");								// Command 3
			ADDCOMMAND("Camelot memory check");						// Command 4
			ADDCOMMAND("MFC memory check (not for SmartHeap users)");	// Command 5
			break;

		case 5:		// Operation history dump (by Peter) -------------------------------------
			ADDCOMMAND("Dump last operation");						// Command 0
			ADDCOMMAND("Dump all operations");						// Command 1
			break;

// Neville 2/10/97 removal of vector stroking bits
#ifdef VECTOR_STROKING
		case 6:
			ADDCOMMAND("Random positioning");						// Command 0
			ADDCOMMAND("Linewidth");								// Command 1
			ADDCOMMAND("StrokePathToPath");							// Command 2
			ADDCOMMAND("FlattenExpand");							// Command 3
			ADDCOMMAND("Bitmap Shadow");							// Command 4
			break;

		case 7:		// Stroke provider tests (Jason) -----------------------------------------
			ADDCOMMAND("Old-style line");							// Command 0
			ADDCOMMAND("Constant width stroke");					// Command 1
			ADDCOMMAND("Linear Ramp");								// Command 2
			ADDCOMMAND("S Ramp");									// Command 3
			ADDCOMMAND("Blip");										// Command 4
			ADDCOMMAND("Ellipse");									// Command 5
			ADDCOMMAND("Teardrop");									// Command 6
			ADDCOMMAND("Thumbtack");								// Command 7
			ADDCOMMAND("Airbrush - S");								// Command 8
			ADDCOMMAND("Airbrush - linear");						// Command 9
			ADDCOMMAND("Airbrush - random");						// Command 10
			ADDCOMMAND("Airbrush - L");								// Command 11
			break;

		case 8:		// Stroke provider tests (Jason) -----------------------------------------
			ADDCOMMAND("Create new vector stroke");					// Command 0
			ADDCOMMAND("Create new repeating vector stroke");		// Command 1
			ADDCOMMAND("Apply last created vector stroke");			// Command 2
			break;

		case 9:		// Object cache tests (Olivier) ------------------------------------------
			ADDCOMMAND("Fill up the cache");						// Command 1
			ADDCOMMAND("Add fixed size objects");					// Command 2
			ADDCOMMAND("Add randomed size objects")					// Command 3
			ADDCOMMAND("Add a big object");							// Command 4
			break;
#endif // VECTOR_STROKING

#if FALSE
		case 10:	// Script Execution (Colin) ------------------------------------------
			ADDCOMMAND("Run the Script");							// Command 0
			break;
		// Add further command groups here...
#endif
		case 10:
			ADDCOMMAND("Apply default Live Effect (blur)");				// Command 0
			ADDCOMMAND("Invoke IXaraPhotoEditor3");						// Command 1
			ADDCOMMAND("End IXaraPhotoEditor3 Session");				// Command 2
			ADDCOMMAND("Dump Bitmap Cache");							// Command 3
			ADDCOMMAND("Create new spread");							// Command 4
			break;

		case 11:	// Gerry Debug ------------------------------------------
			ADDCOMMAND("Command 0");								// Command 0
			break;
	}

#undef ADDCOMMMAND

	// Select the last item the user selected, in the hope that this is useful
	SetSelectedValueIndex(_R(IDC_DEBUGDLG_LIST), LastSelectedIndex);
}



/********************************************************************************************

>	void DebugDlg::SetCommandValue(INT32 GroupIndex, INT32 CommandIndex)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/96

	Inputs:		GroupIndex - indicates which command group is to be used
				CommandIndex - the index of the command in the list - see DebugDlg::SetCommandList

	Purpose:	Whenever a new command is chosen from the list, this function is called
				to fill in the "value" field with whatever number you like.

				ADD AN ENTRY TO THE SWITCH STATEMENT TO HANDLE YOUR COMMAND
					Set "Description" to a helpful description string
					Set "Value" to the current value (if any) associated with your command
					Set "IsEditable" if you want the value to be editable

	Notes:		This only appears in DEBUG builds, so it's fine to inline strings

********************************************************************************************/

void DebugDlg::SetCommandValue(INT32 GroupIndex, INT32 CommandIndex)
{
	String_256 Description(TEXT("Does an unknown debug option"));

	String_256 Value(TEXT("0"));
	BOOL IsEditable = FALSE;

	switch(GroupIndex)
	{
		case 0:		// Jason's colour separation controls ------------------------------------
			switch (CommandIndex)
			{
				case 0: Description = TEXT("Disables global colour separations");			break;
				case 1: Description = TEXT("Changes to a composite WYSIWYG print preview");	break;
				case 2: Description = TEXT("Changes to CYAN colour separation");			break;
				case 3: Description = TEXT("Changes to MAGENTA colour separation");			break;
				case 4: Description = TEXT("Changes to YELLOW colour separation");			break;
				case 5: Description = TEXT("Changes to KEY colour separation");				break;
				case 6:
					{
						Description = TEXT("Changes to specified SPOT separation");
						Value = TEXT("1");

						ColourContext *cc = ColourManager::GetColourContext(COLOURMODEL_RGBT, DocView::GetSelected());
						ColourPlate *SepDescriptor = cc->GetColourPlate();
						if (SepDescriptor && SepDescriptor->GetType() == COLOURPLATE_SPOT)
						{
							IndexedColour *pSpotCol = SepDescriptor->GetSpotColour();
							if (pSpotCol)
							{
								// Count the spot colours until we find the required one
								ColourList* pColList = Document::GetSelected()->GetIndexedColours();
								IndexedColour* pCol = (IndexedColour*) pColList->GetHead();
								INT32 Count = 0;

								while (pCol != NULL)
								{
									if (pCol->GetType() == COLOURTYPE_SPOT && !pCol->IsDeleted())
									{
										Count++;
									}

									if (pCol == pSpotCol)
										break;

									pCol = (IndexedColour*) pColList->GetNext(pCol);
								}

								if (pCol && Count > 0)
									Value._MakeMsg(_T("#1%d"), Count);
							}
						}						

						// And allow the user to edit the string as they see fit
						IsEditable = TRUE;
					}
					break;

				case 7:
					{
						Description = TEXT("Toggles colour output to/from monochrome");

						ColourContext *cc = ColourManager::GetColourContext(COLOURMODEL_RGBT, DocView::GetSelected());
						ColourPlate *SepDescriptor = cc->GetColourPlate();
						BOOL Mono   = (SepDescriptor != NULL) ? SepDescriptor->IsMonochrome() : FALSE;

						Value = (Mono) ? TEXT("Enabled") : TEXT("Disabled");
					}
					break;

				case 8:
					{
						Description = TEXT("Toggles colour output to/from photographic negative");

						ColourContext *cc = ColourManager::GetColourContext(COLOURMODEL_RGBT, DocView::GetSelected());
						ColourPlate *SepDescriptor = cc->GetColourPlate();
						BOOL Negate = (SepDescriptor != NULL) ? SepDescriptor->IsNegative() : FALSE;

						Value = (Negate) ? TEXT("Enabled") : TEXT("Disabled");
					}
					break;

				case 9:  Description = TEXT("Toggles plate overprint for the current plate");	break;
				case 10:
					Description = TEXT("Toggles 'Always overprint black' state");
					{
						Document *Bob = Document::GetSelected();

						if (Bob != NULL)
						{
							PrintComponent *Sally = (PrintComponent *) 
													Bob->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
							if (Sally != NULL)
							{
								PrintControl *Ernest = Sally->GetPrintControl();
								if (Ernest != NULL)
								{
									TypesetInfo *Mavis = Ernest->GetTypesetInfo();
									if (Mavis != NULL)
									{
										Value = (Mavis->AlwaysOverprintBlack()) ? TEXT("Enabled") : TEXT("Disabled");
									}
								}
							}
						}
					}
					break;
			}
			break;


		case 1:		// Jason's misc controls -------------------------------------------------
			switch (CommandIndex)
			{
				case 0:
					Description = TEXT("Writes the 'value' string to the debug stream");

					// Set up a default string in the value field
					Value = TEXT("--- trace marker inserted by user ---");

					// And allow the user to edit the string as they see fit
					IsEditable = TRUE;
					break;
			}
			break;

#ifndef WEBSTER // Neville 6/8/97
		case 2:		// Mikes's misc controls -------------------------------------------------
			switch (CommandIndex)
			{
				case 0:	Description = TEXT("Initialises the print mark managers cache"); break;
				case 1: Description = TEXT("Adds the first print mark to the selected doc"); break;
				
				case 2: Description = TEXT("Adds a new colour to a grad fills colour ramp"); 
						// set up a default string in the value field
						Value = TEXT("128");
						IsEditable = TRUE;
						break;
				case 3: Description = TEXT("Adds a new transp to a grad transp ramp");
						// set up a default string in the value field
						Value = TEXT("128");
						IsEditable = TRUE;
						break;
			}
			break;
#endif // WEBSTER

		case 3:		// Will's bitmap controls -------------------------------------------------
			switch (CommandIndex)
			{
				case 0:	Description = TEXT("Dumps the current global bitmap list"); break;
				case 1: Description = TEXT("Dumps the current greyscale bitmap list"); break;
			}
			break;

		case 4:		// Low memory Testing (Peter) ---------------------------------------------
			switch (CommandIndex)
			{
				case 0:	Description = TEXT("Enable or disbale low memory testing");
					Value = (SimpleCCObject::GetLowMemoryState()) ? TEXT("Enabled") : TEXT("Disabled");
					break;
				case 1:	Description = TEXT("Fails all memory claims.");
					if (SimpleCCObject::GetLowMemoryState() && SimpleCCObject::GetLowMemoryLimit()==0)
						Value = TEXT("Enabled");
					else
						Value = TEXT("Disabled");
					break;
				case 2:	Description = TEXT("Fails memory claims after the specified number of bytes have been claimed.");
				{
					String_32 Number;
					Number._MakeMsg(_T("#1%lu"), SimpleCCObject::GetLowMemoryLimit());
					Value = Number;
					IsEditable = TRUE;
					break;
				}
				case 3:	Description = TEXT("Lists all allocated objects to the TRACE display.");
					break;
				case 4:	Description = TEXT("Checks guardwords on Camelot memory allocations.");
					break;
				case 5:	Description = TEXT("MFC memory allocation check.");
					break;
			}
			break;

		case 5:		// Operation history listing -------------------------------------------------
			switch (CommandIndex)
			{
				case 0:	Description = TEXT("Dump actions in the last operation (in the selected document) to TRACE display"); break;
				case 1: Description = TEXT("Dump actions in all operations (in the selected document) to TRACE display"); break;
			}
			break;

#ifdef VECTOR_STROKING // Neville 6/8/97
		case 6:
			switch (CommandIndex)
			{
				case 0:	Description = TEXT("Render n copies around a centrepoint (Fails on large penumbras)"); break;
				case 1: Description = TEXT("Render decreasing linewidths with decreasing transparency (Fails on transparent shadows)"); break;
				case 2: Description = TEXT("Render StrokePathToPath linewidths (Far too slow (on complex shapes))"); break;
				case 3: Description = TEXT("Flatten and expand paths (Fails on inverted paths)"); break;
				case 4: Description = TEXT("Bitmap shadower Using new Shadower COM Object"); break;
			}
			break;

		case 7:		// Stroke provider tests (Jason) -----------------------------------------
			switch (CommandIndex)
			{
				case 0:	Description = TEXT("Reset the stroke to 'old style line' (the default)");		break;
				case 1:	Description = TEXT("Apply a constant width variable-width stroke attribute");	break;
				case 2:	Description = TEXT("Apply a linear ramp variable-width stroke attribute");		break;
				case 3: Description = TEXT("Apply an 'S' ramp variable-width stroke attribute");		break;
				case 4: Description = TEXT("Apply a 'blip' variable-width stroke attribute");			break;
				case 5: Description = TEXT("Apply an 'ellipse' variable-width stroke attribute");		break;
				case 6: Description = TEXT("Apply a 'teardrop' variable-width stroke attribute");		break;
				case 7: Description = TEXT("Apply a 'thumbtack' variable-width stroke attribute");		break;
				case 8: Description = TEXT("Apply an airbrush stroke attribute");						break;
				case 9: Description = TEXT("Apply an airbrush stroke attribute");						break;
				case 10:Description = TEXT("Apply an airbrush stroke attribute");						break;
				case 11:Description = TEXT("Apply an airbrush stroke attribute");						break;
			}
			break;

		case 8:
			switch (CommandIndex)
			{
				case 0: Description = TEXT("Set the vector stroke brush from the selection");			break;
				case 1: Description = TEXT("Set the vector stroke brush, and make it repeating");		break;
				case 2: Description = TEXT("Apply the last vector stroke you created");					break;
			}
			break;

		case 9:
			switch (CommandIndex)
			{
				case 0: Description = TEXT("Add as many objects as needed to fill up the cache");break;
				case 1: Description = TEXT("Add several objects which size increase");break;
				case 2: Description = TEXT("Add several objects with a random size");break;
				case 3: Description = TEXT("Add an object bigger than the cachesize");break;
			}
			break;
#endif // VECTOR_STROKING

#if FALSE
		case 10:		// Colin's script executioner -------------------------------------------------
			switch (CommandIndex)
			{
				case 0:
					Description = TEXT("Run a Script");
					break;
			}
			break;
#endif
		case 10:		// Phil's LiveEffects test -------------------------------------------------
			switch (CommandIndex)
			{
				case 0:
					Description = TEXT("Apply the default debug effect to the selection");
					break;
				case 1:
					Description = TEXT("Call the new XPE interface for modeless editing");
					break;
				case 2:
					Description = TEXT("End the XPE editing session successfully");
					break;
				case 3:
					Description = TEXT("Dump a description of the bitmap cache to the debug output");
					break;
				case 4:
					Description = TEXT("Create a second spread! OOER!");
					break;
			}
			break;

		case 11:		// Gerry Debug-------------------------------------------------
			switch (CommandIndex)
			{
				case 0:
					Description = TEXT("Command 0");
					break;
			}
			break;
		// case n:
		// Add your own command group(s) here		
	}

	SetStringGadgetValue(_R(IDC_DEBUGDLG_COMMENT), Description);

	SetStringGadgetValue(_R(IDC_DEBUGDLG_VALUE1), Value);
	EnableGadget(_R(IDC_DEBUGDLG_VALUE1), IsEditable);
}


class NodeShadowController;

/********************************************************************************************

>	void DebugDlg::InvokeCommand(INT32 GroupIndex, INT32 CommandIndex, StringBase *NewValue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/96

	Inputs:		GroupIndex - indicates which command group is to be used
				CommandIndex - the index of the command in the list - see DebugDlg::SetCommandList
				NewValue - A string entered by the usert in the "Value" field of the dialogue.
							(e.g. this is used by Jason's CommandIndex 0 to output a string into
							the debug stream)

	Purpose:	Invokes one of the debug commands.

				ADD AN ENTRY TO THE SWITCH STATEMENT TO HANDLE YOUR COMMAND

	Notes:		This only appears in DEBUG builds

********************************************************************************************/

void DebugDlg::InvokeCommand(INT32 GroupIndex, INT32 CommandIndex, StringBase *NewValue)
{
	String_256 Desc(TEXT("Doing debug command..."));
	Progress Hourglass((StringBase *) &Desc, -1, FALSE);	// Hourglass while we're busy

	switch (GroupIndex)
	{
		case 0:		// Jason's colour separation controls ------------------------------------
			switch (CommandIndex)
			{
				case 0:		// Normal document view
					if (DocView::GetSelected() != NULL)
					{
						// Remove any attached ColourPlate, and redraw all affected thingies
						DocView::GetSelected()->SetColourPlate(NULL);
						ColourManager::SelViewContextHasChanged();
					}
					break;

PORTNOTE("Separations", "Removed use of SetNewColourPlate")
#ifndef EXCLUDE_FROM_XARALX
				case 1:  SetNewColourPlate(COLOURPLATE_COMPOSITE,0,0); break;
				case 2:  SetNewColourPlate(COLOURPLATE_CYAN,	 0,0); break;
				case 3:  SetNewColourPlate(COLOURPLATE_MAGENTA,	 0,0); break;
				case 4:  SetNewColourPlate(COLOURPLATE_YELLOW,	 0,0); break;
				case 5:  SetNewColourPlate(COLOURPLATE_KEY,		 0,0); break;
				case 6:
					{
						INT32 Pos = 0;
						INT32 SpotPlate = NewValue->ConvertToInteger(Pos);
						SetNewColourPlate(COLOURPLATE_SPOT,	 0,SpotPlate);
					}
					break;
				case 7:  SetNewColourPlate(COLOURPLATE_NONE,	 1,0); break;
				case 8:  SetNewColourPlate(COLOURPLATE_NONE,	 2,0); break;
				case 9:  SetNewColourPlate(COLOURPLATE_NONE,	 3,0); break;

				case 10:
					{
						Document *Bob = Document::GetSelected();

						if (Bob != NULL)
						{
							PrintComponent *Sally = (PrintComponent *)
													Bob->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
							if (Sally != NULL)
							{
								PrintControl *Ernest = Sally->GetPrintControl();
								if (Ernest != NULL)
								{
									TypesetInfo *Mavis = Ernest->GetTypesetInfo();
									if (Mavis != NULL)
									{
										Mavis->SetOverprintBlack(!Mavis->AlwaysOverprintBlack());
									}
								}
							}
						}
					}
					break;
#endif

				default:
					ERROR3("Unknown debug command");
					break;
			}
			break;


		case 1:		// Jason's misc controls -------------------------------------------------
			switch (CommandIndex)
			{
				case 0:
					// Output the "value" string (entered by the user) into the trace output.
					// I find this invaluable when dumping screeds of debug stuff, in order to
					// mark positions in the output so I can find the important info amongst
					// all the chaff.
					TRACE( _T("%s\n"), (TCHAR *) (*NewValue));
					break;

				default:
					ERROR3("Unknown debug command");
					break;
			}
			break;

#ifndef WEBSTER // Neville 6/8/97
		case 2:		// Mike's misc controls -------------------------------------------------
			switch (CommandIndex)
			{
PORTNOTE("PrintMarks", "Removed use of PrintMarksMan")
#ifndef EXCLUDE_FROM_XARALX
				case 0:
				{
					// We need to execute this one after the dialogue has closed.
					// CloseAndExecute=TRUE;
					PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
					if (pMarksMan!=NULL)
						pMarksMan->ConstructCache();
				}
				break;

				case 1:
				{
					PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
					if (pMarksMan!=NULL)
					{
						// get the first mark from the cache
						PrintMarkItem *pMarkItem = pMarksMan->PMMCache.GetFirstMark();
						if (pMarkItem!=NULL)
						{
							UINT32 Handle=pMarkItem->GetHandle();
							Document* pDoc = Document::GetSelected();
							if (pDoc)
								pMarksMan->AddMarkToDoc(Handle,pDoc);
						}
					}				
					
				}
				break;
#endif

				case 2:
				{
					// ok we need to find the selected object and alter its attribute to get a
					// colour fill applied. How do we do this?
					// hmm I'll work it out in a moment.

					// Well like this really...
					AttrColFillRampChange* pAttrib = new AttrColFillRampChange;
					if (pAttrib!=NULL)
					{
						INT32 Pos = 0;
						INT32 NewVal = NewValue->ConvertToInteger(Pos);
						float pos = ((float)NewVal) / 255.0f;
						if (pos<0.0f) pos=0.0f;
						if (pos>1.0f) pos=1.0f;
						// ok full yellow in the middle please
						
						srand((unsigned)time(NULL));
						INT32 r,g,b;
						r = rand();
						g = rand();
						b = rand();
						r &= 0xFF;
						g &= 0xFF;
						g &= 0xFF;
						DocColour TestColour(r,g,b);
						pAttrib->InitForColourAdd(&TestColour, pos);
						
						// AttributeSelected knows what to do with a selected attribute
						AttributeManager::AttributeSelected(NULL, pAttrib); 
					}
				}
				break;

				case 3:
				{
					// Well like this really...
					AttrTranspFillRampChange* pAttrib = new AttrTranspFillRampChange;
					if (pAttrib!=NULL)
					{
						INT32 Pos = 0;
						INT32 NewVal = NewValue->ConvertToInteger(Pos);
						
						float pos = ((float)NewVal) / 255.0f;
						if (pos<0.0f) pos=0.0f;
						if (pos>1.0f) pos=1.0f;

						// ok full yellow in the middle please
						srand((unsigned)time(NULL));
						UINT32 t = (UINT32)rand();
						t = t & 0xFF;
						pAttrib->InitForTranspAdd(t, pos);
						
						// AttributeSelected knows what to do with a selected attribute
						AttributeManager::AttributeSelected(NULL, pAttrib); 
					}
				}
				break;

				default:
					ERROR3("Unknown debug command");
					break;
			}
			break;
#endif // WEBSTER

		case 3:		// Will's bitmap controls -------------------------------------------------
			switch (CommandIndex)
			{
				case 0:
				{
					GlobalBitmapList* pBmpList = GetApplication()->GetGlobalBitmapList();
					if (pBmpList)
						pBmpList->DumpGlobalBitmapListToTrace();
				}
				break;

//				case 1:
//				{
//					GreyscaleBitmapList* pGreyList = 
//							GetApplication()->GetGlobalBitmapList()->GetGreyscaleBitmapList();
//					if (pGreyList)
//						pGreyList->DumpGreyscaleBitmapListToTrace();
//				}
//				break;

				default:
					ERROR3("Unknown debug command");
					break;
			}
			break;

		case 4 :
			switch (CommandIndex)
			{
				case 0:
				{
					SimpleCCObject::EnableLowMemoryTesting();
				}
				break;

				case 1:
				{
					SimpleCCObject::EnableLowMemoryTesting();
					SimpleCCObject::SetClaimLimit(0);
				}
				break;

				case 2:
				{
					INT32 Pos = 0;
					INT32 NewVal = NewValue->ConvertToInteger(Pos);
					SimpleCCObject::SetClaimLimit(NewVal);
				}
				break;

				case 3:
				{
					SimpleCCObject::MemoryDump();
				}
				break;

				case 4:
				{
					SimpleCCObject::CheckMemory(TRUE);
				}
				break;
				
				case 5:
				{
//					BOOL MemOK = AfxCheckMemory();
//					ERROR3IF(!MemOK, "MFC Memory check failure (look at TRACE output)");
				}
				break;

				default:
					ERROR3("Unknown/Unimplemented debug command");
					break;
			}
			break;

		case 5:
			switch (CommandIndex)
			{
				case 0:		GetApplication()->DumpLastOp();	break;
				case 1:		GetApplication()->DumpAllOps();	break;
				default:	ERROR3("Unknown/Unimplemented debug command");	break;
			}
			break;

#ifdef VECTOR_STROKING // Neville 6/8/97
		case 6:
			// NodeShadowController::RenderMethod = (ShadowType)CommandIndex;
			break;

		case 7:		// Stroke provider tests (Jason) -----------------------------------------
			{
				AttrStrokeType *pStrokeAttr = new AttrStrokeType;
				PathProcessorStroke *pProcessor = NULL;

				AttrVariableWidth *pWidthAttr = new AttrVariableWidth;
				VariableWidthID Function = VarWidth_NotPredefined;
				ValueFunction *pFunction = NULL;

				// Check to see if there is a path processor already applied - if it is, the
				// options which only need to change the width function will not overwrite
				// the stroker (allowing you to change the shape on an Airbrushed stroke, etc)
				BOOL NeedToApplyType = FALSE;
				
				// BLOCK
				{
					SelRange *Selection = Camelot.FindSelection();
					ERROR3IF(Selection == NULL, "No Selection SelRange!?!");

					NodeAttribute *pNode = NULL;
					switch(Selection->FindCommonAttribute(CC_RUNTIME_CLASS(AttrStrokeType), &pNode))
					{
						case SelRange::ATTR_COMMON:
							if (pNode != NULL)		// Let's just be really safe
							{
								// There is a common attribute - but we must check if it supplies
								// a proper path processor (or if it is just an old-style line)
								StrokeTypeAttrValue *pSAV = (StrokeTypeAttrValue *) ((AttrStrokeType *)pNode)->GetAttributeValue();
								if (pSAV == NULL || pSAV->GetPathProcessor() == NULL)
									NeedToApplyType = TRUE;
							}
							else
								NeedToApplyType = TRUE;
							break;

						default:
							NeedToApplyType = TRUE;
							break;
					}
				}

				switch (CommandIndex)
				{
					case 0:
						// leave processor and function undefined
						break;

								// Different width functions
					case 1:	Function	= VarWidth_Constant;			break;
					case 2:	Function	= VarWidth_LinRamp;				break;
					case 3:	Function	= VarWidth_SRamp;				break;
					case 4:	pFunction	= new ValueFunctionBlip;		break;
					case 5:	pFunction	= new ValueFunctionEllipse;		break;
					case 6:	pFunction	= new ValueFunctionTeardrop;	break;
					case 7:	pFunction	= new ValueFunctionThumbtack;	break;

					case 8:	// Airbrush stroke type
					case 9:
					case 10:
					case 11:
						pProcessor = new PathProcessorStrokeAirbrush;

						switch(CommandIndex)
						{
							case 8:	// Airbrush stroke type
								break;

							case 9:
								{
									ValueFunction *pFunc = new ValueFunctionRampLinear(1.0, 0.0);
									((PathProcessorStrokeAirbrush *)pProcessor)->SetIntensityFunction(pFunc);
								}
								break;

							case 10:
								{
									ValueFunction *pFunc = new ValueFunctionRandom(0, 0.0, 0.8);
									((PathProcessorStrokeAirbrush *)pProcessor)->SetIntensityFunction(pFunc);
								}
								break;

							case 11:
								{
									ValueFunction *pFunc = new ValueFunctionRampL;
									((PathProcessorStrokeAirbrush *)pProcessor)->SetIntensityFunction(pFunc);
								}
								break;
						}

						// BLOCK
						{
							// Airbrush defaults to using round caps & joins
							AttrJoinType *pJoinAttr = new AttrJoinType();
							if (pJoinAttr != NULL)
							{
								pJoinAttr->Value.JoinType = RoundJoin;
								AttributeManager::AttributeSelected(pJoinAttr);
							}

							AttrStartCap *pCapAttr = new AttrStartCap;
							if (pCapAttr != NULL)
							{
								pCapAttr->Value.StartCap = LineCapRound;
								AttributeManager::AttributeSelected(pCapAttr);
							}
						}
						break;
				}

				if (CommandIndex != 0 && pProcessor == NULL)
				{
					// We only apply a variable-width PathProcessor if there was not already one
					// applied to to the selection. This allows us to change the width function
					// on airbrushed strokes, etc, without losing the stroke type
					if (NeedToApplyType)
						pProcessor	= new PathProcessorStroke;
					else
					{
						delete pStrokeAttr;
						pStrokeAttr = NULL;
					}
				}

				if (pStrokeAttr != NULL && pProcessor != NULL)
					((StrokeTypeAttrValue *)pStrokeAttr->GetAttributeValue())->SetPathProcessor(pProcessor);

				if (pWidthAttr != NULL)
				{
					if (Function != VarWidth_NotPredefined)
						((VariableWidthAttrValue *)pWidthAttr->GetAttributeValue())->SetWidthFunction(Function);
					else if (pFunction != NULL)
						((VariableWidthAttrValue *)pWidthAttr->GetAttributeValue())->SetWidthFunction(pFunction);
					else
					{
						// We're not trying to override the width, so vape our width attr
						delete pWidthAttr;
						pWidthAttr = NULL;
					}
				}

				// Now apply the attributes
				if (pStrokeAttr != NULL)
					AttributeManager::AttributeSelected(pStrokeAttr);

				if (pWidthAttr != NULL)
					AttributeManager::AttributeSelected(pWidthAttr);
			}
			break;

		case 8:
			{
				switch (CommandIndex)
				{
					case 0:
					case 1:
						// Rip up the selection and use that as the brush in future, please
						{
							// First, Make Shapes on everything so they're all simple paths
							String_256 Desc("Building new stroke brush...");
							Progress::Start(FALSE, &Desc);
							OpDescriptor *pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_MAKE_SHAPES);
							if (pOp != NULL)
								pOp->Invoke();

							// Second, Group everything
							pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_GROUP);
							if (pOp != NULL)
								pOp->Invoke();

							// Finally, create a new brush
							PathStrokerVector::BodgeRipSelection((CommandIndex == 0) ? FALSE : TRUE);
							Progress::Stop();
						}
						break;


					case 2:
						{
							AttrStrokeType *pStrokeAttr = new AttrStrokeType;
							PathProcessorStrokeVector *pProcessor = new PathProcessorStrokeVector;

							if (pStrokeAttr != NULL && pProcessor != NULL)
							{
								pProcessor->SetStrokeDefinition(PathStrokerVector::GetCurrentBrush());
								((StrokeTypeAttrValue *)pStrokeAttr->GetAttributeValue())->SetPathProcessor(pProcessor);
								AttributeManager::AttributeSelected(pStrokeAttr);
							}
						}
						break;
				}
			}
			break;


		case 9:
			{
				// Get the ObjectCache
				ObjectCache* pObjCache = GetApplication()->GetObjectCache();

				if (pObjCache == NULL)
					return;

				WORD hObj = 0;
										
				switch(CommandIndex)
				{
					case 0:	// Add 50 objects which size is sizeof(DummyCachedObject)
						{
							for (INT32 i=0;i<=50;i++)
							{
								DummyCachedObject* a = new DummyCachedObject;
								pObjCache->Add(a);
							}
						}
						break;
					case 1:	// Add 50 objects which size is 'i'
						{
							for (INT32 i=0;i<=50;i++)
							{
								DummyCachedObject* b = new DummyCachedObject(i);
								pObjCache->Add(b);
							}
						}
						break;
					case 2:	// Add 50 objects which size is rand()%50+1
						{
							srand((unsigned)time(NULL));
							for (INT32 i=1;i<=50;i++)
							{
								DummyCachedObject* c = new DummyCachedObject(rand()%50+1);
								pObjCache->Add(c);
							}
						}
						break;

					case 3:	// Add an object which size is bigger than the cache size
						{
							DummyCachedObject* u = new DummyCachedObject(55);
							hObj = pObjCache->Add(u);
						}
						break;
					default:
						ERROR3("Unknown/Unimplemented debug command");
						break;
				}
			}
			break;
#endif // VECTOR_STROKING

#if FALSE
		case 10:
			{
				ExecuteScript(g_ScriptToExecute);
			}
			break;
#endif
		case 10:
			{
				switch(CommandIndex)
				{
PORTNOTE("LiveEffects", "Removed use of LiveEffects")
#ifndef EXCLUDE_FROM_XARALX
				case 0:
					{
						OpDescriptor* pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_APPLY_LIVEEFFECT);
						if (pOp != NULL)
						{
							OpLiveEffectParam Param;
							Param.bIsDestructive = FALSE;
							Param.strOpUnique = String(_T("Enhance"));
							pOp->Invoke(&Param);
						}
					}
					break;
				case 1:
					{
						OpDescriptor* pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_EDIT_LIVEEFFECT);
						if (pOp != NULL)
						{
							OpLiveEffectParam Param;
							Param.bIsDestructive = FALSE;
							Param.strOpUnique = String(_T("Enhance"));
							pOp->Invoke(&Param);
						}
					}
					break;
#endif
				case 2:
					{
//						XPEHost::EndEditSession(TRUE);
					}
					break;
				case 3:
					{
						CBitmapCache* pCache = GetApplication()->GetBitmapCache();
						pCache->DebugDump();
					}
					break;
				case 4:
					{
//#if NEW_PASTEBOARD
						Spread* pSpread = Document::GetSelectedSpread();
						Spread* pNewSpread = NULL;
						pSpread->NodeCopy((Node**)&pNewSpread);
						if (pNewSpread)
						{
							pNewSpread->AttachNode(pSpread, NEXT);
							pSpread->AdjustPasteboards();
						}
//#else
//						InformWarning(_R(IDS_SPREADTESTWARNING));
//#endif
					}
					break;
				}
			}
			break;

		case 11:		// Gerry Debug
			{
				switch(CommandIndex)
				{
				case 0:
					{
						// Set the size of a slot to something weird
//						OILFixedDockingBar* pDockBar = (OILFixedDockingBar*)(GetMainFrame()->GetDockBar(DOCKBAR_RIGHT));
//						pDockBar->SetSlotSize(0, 150);
//						GetMainFrame()->RecalcLayout();		
					}
					break;
				}
			}
			break;

	//	case n:
	//		Add other command group(s) here...
	}
}




/********************************************************************************************

>	void DebugDlg::InvokeLateCommand(INT32 GroupIndex, INT32 CommandIndex)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96

	Inputs:		GroupIndex - indicates which command group is to be used
				CommandIndex - the index of the command in the list - see DebugDlg::SetCommandList

	Purpose:	Invokes one of the debug commands after the dialogue has been closed and
				the dialogue op has been ended. If you need to trigger an operation from this
				debug dialogue, just write your standard command handler to set the global
				CloseAndExecute variable, and add you command to this 'Late' command execution
				table. You will notice when you Apply/Execute your command the dialogue will
				disappear which is not that much of a disaster.

	Notes:		This only appears in DEBUG builds

********************************************************************************************/

void DebugDlg::InvokeLateCommand(INT32 GroupIndex, INT32 CommandIndex)
{
	String_256 Desc(TEXT("Doing debug command..."));
	Progress Hourglass((StringBase *) &Desc, -1, FALSE);	// Hourglass while we're busy

	switch (GroupIndex)
	{
		case 0:		// Jason's colour separation controls ------------------------------------
		break;

		case 1:		// Jason's misc controls -------------------------------------------------
		break;

		case 2:		// Mike's misc controls -------------------------------------------------
			switch (CommandIndex)
			{
				case 0:
				{
				//	PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
				//	if (pMarksMan!=NULL)
				//		pMarksMan->ConstructCache();
				}
				break;
			}
			break;

	//	case 3:
	//		Add other late command group(s) here...
	}
}


#endif		// if _DEBUG
