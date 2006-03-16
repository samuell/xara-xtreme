// $Id: sglfills.cpp 662 2006-03-14 21:31:49Z alex $
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

// SGLFills.cpp - LibFills SuperGallery classes

#include "camtypes.h"
#include "sglfills.h"

#include "app.h"		// For GetApplication()
//#include "galstr.h"
#include "pathname.h"
#include "sginit.h"
#include "sglfills.h"	// This .cpp file's corresponding header
#include "sglib.h"
#include "sgmenu.h"
#include "thumb.h"
#include "thumbmsg.h"

#include "fillval.h"
#include "grnddib.h"
//#include "galres.h"

//#include "richard.h"
#include "sgliboil.h"
#include "errors.h"
#include "fixmem.h"
#include "selop.h"

#include "attrmgr.h"
#include "fillattr.h"
#include "progress.h"
#include "camelot.h"

#include "dragmgr.h"
//#include "viewrc.h"		// FOR _R(IDC_CANDROPONPAGE)
//#include "resource.h"		// FOR _R(IDC_CANDROPONPAGE)
#include "docview.h"
#include "spread.h"
#include "nodebmp.h"

// For LibraryGallery
#include "sglbase.h"

#include "sgscan.h"
#include "sgscanf.h"

//#include "richard2.h"
#include "keypress.h"
#include "backgrnd.h"	// OpBackground
#include <io.h>
#include "inetop.h"
#include "helpuser.h"
//#include "xshelpid.h"
//#include "helppath.h"
#include "resdll.h"


// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(LibFillsSGallery, LibraryGallery)
CC_IMPLEMENT_DYNCREATE(OpDisplayLibFillsGallery, Operation)
CC_IMPLEMENT_DYNCREATE(SGFillsItem, SGLibDisplayItem)

CC_IMPLEMENT_DYNCREATE(GalleryFillsDragInfo, BitmapDragInformation)
CC_IMPLEMENT_DYNAMIC(SGFillsDragTarget, SGListDragTarget)

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

using namespace InetUtils;

// Static variables
#ifdef _DEBUG
//String_256 LibFillsSGallery::DefaultLibraryPath = TEXT("\\\\deepthought\\camelotcd\\cd\\fills");
//String_256 LibFillsSGallery::DefaultLibraryPath = TEXT("\\\\jimpc\\corelxra\\fills");
String_256 LibFillsSGallery::DefaultLibraryPath = TEXT("D:\\fills");
#else
String_256 LibFillsSGallery::DefaultLibraryPath = TEXT("D:\\fills");
#endif

// display mode setting for ini file
INT32 LibFillsSGallery::DefaultDisplayMode = 0;

// For keeping the sort keys constant when next loaded (default == sort alphabetically)
// 1st sort key = DefaultSortKeys & 0x8f
// 2nd sort key = ((DefaultSortKeys>>8) & 0x8f)
// 1st sort key reversed = ((DefaultSortKeys>>7) & 0x01)==1
// 2nd sort key reversed = ((DefaultSortKeys>>15) & 0x01)==1
// So 0 means no sorting at all
// and 1 means sort the gallery alphabetically
UINT32 LibFillsSGallery::DefaultSortKeys = 1;

// Pointer to the gallery itself...
LibFillsSGallery *LibFillsSGallery::ThisGallery = NULL;

// Quiet status of gallery
BOOL LibFillsSGallery::QuietStatus = FALSE;

/********************************************************************************************

>	LibFillsSGallery::LibFillsSGallery()
												 
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Purpose:	LibFillsSGallery default constructor

********************************************************************************************/

LibFillsSGallery::LibFillsSGallery()
{
	LibFillsSGallery::ThisGallery = this;

	// Default gallery size
	CSize Size(((333 * 2) - 32) - 32, 256);
	SetGallerySize(Size);
} 



/********************************************************************************************

>	LibFillsSGallery::~LibFillsSGallery()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Purpose:	LibFillsSGallery destructor.

********************************************************************************************/

LibFillsSGallery::~LibFillsSGallery()
{
	OpenLibFiles.DeleteAll();		// Ensure all open libraries are closed
	LibFillsSGallery::ThisGallery = NULL;
}



/********************************************************************************************

>	static BOOL LibFillsSGallery::Init(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Returns:	TRUE if the sgallery initialised successfully
				FALSE if it failed to initialise
	Purpose:

********************************************************************************************/

BOOL LibFillsSGallery::Init(void)
{
	return(TRUE);
}



/********************************************************************************************

>	BOOL LibFillsSGallery::PreCreate(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Returns:	TRUE if the Gallery initialised successfully
				FALSE if it should not be opened due to a failure to initialise

	Purpose:	The LibFillsSGallery PreCreate handler. This overrides the base class
				PreCreate function. It is called at the very beginning of the
				SuperGallery::Create method, before the window has been created.

	Notes:		As this is called before the window is open, it must not attempt to touch
				any of the button gadgets in the window, or force redraws, etc. Also,
				events cannot be passed to the tree, as the tree formatting relies on
				knowing the window size - however, the tree will be reformatted and
				redrawn automatically when the window is opened - this will happen shortly.

********************************************************************************************/

BOOL LibFillsSGallery::PreCreate(void)
{
	// If there isn't already one, create a DisplayTree
	if (DisplayTree == NULL)
	{
		DisplayTree = new SGDisplayRootScroll(this);	// New root node, with a scrollbar
		if (DisplayTree == NULL)
			return(FALSE);
	}

  	// Add the library groups to the gallery if they're not there already
	if(OpenLibFiles.IsEmpty())
	{
		String_256 sLoc = DefaultLibraryPath;
		// Set DefaultLibraryPath to <ExeLocation>\Fills - the user might have installed
		// the fills to his hard disk:
		if(CResDll::GetExecutablePath((TCHAR*)DefaultLibraryPath))
		{
			// Look for the resources in the main Xara X¹ folder first
			String_256 LibDirName;
			GetLibraryDirectoryName(&LibDirName);
			DefaultLibraryPath += "\\";
			DefaultLibraryPath += LibDirName;
			PathName ThisPath(DefaultLibraryPath);
			if(!SGLibOil::FileExists(&ThisPath))	// will also work for paths (not just paths with a filename on the end)
			{
				// Fills not on hard disk. Try the CD location
				if(!ScanForLocation(SGLib_Texture, NULL))
				{
					// hard drive and CD location not found
					// put the original path back
					DefaultLibraryPath = sLoc;
				}
			}
		}

		LibraryGallery::AddLibraryGroups(SGLib_Texture, &DefaultLibraryPath);
	}

	// Use last time's display mode
	DisplayMode = LibFillsSGallery::DefaultDisplayMode;

	return TRUE;
}

/********************************************************************************************

>	void LibFillsSGallery::SortGallery(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95

	Inputs:		
	Returns:

	Purpose:	Sorts the contents of the gallery in an alphabetical fashion, whilst keeping
				the old sort key status...
	Notes:		
	SeeAlso:

********************************************************************************************/

void LibFillsSGallery::SortGallery(void)
{
	LibraryGallery::SortGallery();
}

/********************************************************************************************

>	BOOL LibFillsSGallery::BrowseClicked(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95

	Inputs:		
	Returns:	TRUE if the gallery has new stuff in it (FALSE if cancel clicked, etc)...

	Purpose:	Pops up the browse box and lets a new location be set for the data

********************************************************************************************/

BOOL LibFillsSGallery::BrowseClicked(void)
{
	return (LibraryGallery::BrowseClicked(&DefaultLibraryPath, SGLib_Texture, _R(IDS_REMOVE_OLD_GOUPS_FILLS)));
}

/********************************************************************************************

>	virtual BOOL LibFillsSGallery::CanCreateIndexes(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Returns:	TRUE to if index generation is possible

	Purpose:	To determine if this gallery can generate indexes or not

********************************************************************************************/

BOOL LibFillsSGallery::CanCreateIndexes(void)
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL LibFillsSGallery::GetDefaults(String_256 *DefaultIndex, String_256 *IndexDesc, SGLibType *Type)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Outputs:	DefaultIndex	- The filename for the default index file (Xaraclip.txt)
				IndexDesc		- Description of the index / gallery (Clipart)
				Type			- Default library type associated with this gallery

	Returns:	TRUE if this was overridden successfully

	Purpose:	To determine various library gallery default properties

********************************************************************************************/

BOOL LibFillsSGallery::GetDefaults(String_256 *DefaultIndex, String_256 *IndexDesc, SGLibType *Type)
{
	if(DefaultIndex != NULL)
		*DefaultIndex = _R(IDS_LIBRARIES_FILLS_FILENAME); 	// "XaraText.txt";

	if(IndexDesc != NULL)
		*IndexDesc = _R(IDS_LIBRARIES_FILLS_DESC);			// "Fills";

	if(Type != NULL)
		*Type = SGLib_Texture;

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL LibFillsSGallery::GetLibraryDirectoryName(String_256 *LibDirName)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95

	Outputs:	LibDirName	- Returns the Default directory (on the clipart CD) for the gallery
	Returns:	TRUE if overridden and directory obtained...

	Purpose:	Get the default CD directory name for the gallery

********************************************************************************************/

BOOL LibFillsSGallery::GetLibraryDirectoryName(String_256 *LibDirName)
{
	LibDirName->MakeMsg(_R(IDS_LIBRARIES_FILLS_DIRNAME));	
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL LibFillsSGallery::GetQuietStatus(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Returns:	TRUE if Quiet has been pressed (SetQuiet status called with TRUE)
	Purpose:	Get the Quiet status of the gallery

********************************************************************************************/

BOOL LibFillsSGallery::GetQuietStatus(void)
{
	return LibFillsSGallery::QuietStatus;
}

/********************************************************************************************

>	virtual void LibFillsSGallery::SetQuietStatus(BOOL Status)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Inputs:		Status - Set to TRUE and call if Quiet has just been pressed on an 'adding
						 indexes' dialog
	Purpose:	Set the Quiet status of the gallery

********************************************************************************************/
		
void LibFillsSGallery::SetQuietStatus(BOOL Status)
{
	LibFillsSGallery::QuietStatus = Status;
}

/***********************************************************************************************

>	virtual BOOL LibFillsSGallery::ScanForLocation(SGLibType Type, StringBase *Result = NULL);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95

	Inputs:		Type of library to scan for
	Outputs:	(We assume the library static strings exist and use them - bit yucky)
				If a Result pointer if given then we copy this into there as well..
	Returns:	FALSE if it fails; TRUE if we got a path

	Purpose:	Searches all the drives for a CDROM drive. If it finds the Camelot CD
				mount here at Xara HQ, we point to that instead.
	Notes:

***********************************************************************************************/

BOOL LibFillsSGallery::ScanForLocation(SGLibType Type, StringBase *Result)
{
#ifndef STANDALONE
	// Search for a CD ROM drive
	String_256 DriveName;
	BOOL AreWeXara = FALSE;
	String_256 XaraDrive;

	BOOL Adjust = KeyPress::IsAdjustPressed();
#ifndef _DEBUG
	Adjust = FALSE;
#endif

	if(SGLibOil::LocateCDROMDrive(this, Type, &DriveName, &AreWeXara, &XaraDrive, Adjust))
	{
		if(AreWeXara)
			DriveName = XaraDrive;

		switch(Type)
		{
			case SGLib_Texture:
			case SGLib_Fractal:
				LibFillsSGallery::DefaultLibraryPath = DriveName;
#ifdef _DEBUG
				if(Adjust)
				{
					LibFillsSGallery::DefaultLibraryPath += TEXT("Graphics\\Fills");
				}
				else
#endif
				{
					String_256 LibDirName;
					GetLibraryDirectoryName(&LibDirName);
					LibFillsSGallery::DefaultLibraryPath += LibDirName;
				}
			
				if(Result)
					*Result = LibFillsSGallery::DefaultLibraryPath;

				return TRUE;
		}
	}
#endif
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL LibFillsSGallery::CheckForIndexMatch(StringBase *Txt)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Inputs:		Kind	- Last column entry in the index.txt file (" T")
	Returns:	TRUE if this signifies the gallery in question...

	Purpose:	To see whether we should add this line of the index.txt file to this gallery

********************************************************************************************/

BOOL LibFillsSGallery::CheckForIndexMatch(StringBase *Txt)
{
	BOOL Match = FALSE;

	// Textures (fills)
	if(((Txt->Sub(String_8("T"))!=-1) || (Txt->Sub(String_8("t"))!=-1)) ) Match = TRUE;

	return Match;
}


/********************************************************************************************

>	virtual BOOL LibFillsSGallery::ApplyAction(SGActionType Action)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95

	Inputs:		Action - Indicates what action to apply

	Returns:	TRUE to indicate successful handling of the action, or
				FALSE to indicate failure

	Purpose:	Applies certain conventional gallery actions (usually associated with
				gallery buttons, for new, edit, delete, etc)

	SeeAlso:	SGActionType

********************************************************************************************/

BOOL LibFillsSGallery::ApplyAction(SGActionType Action)
{
	// No display tree? Better forget about it then!
	if (DisplayTree == NULL)
		return(FALSE);

	// Now, process the action	TO DO! - see Colour gallery for examples
	switch(Action)
	{
		case SGACTION_APPLYADJUST:
			// Drop through to SGACTION_APPLY handler

		case SGACTION_APPLY:
			// Import the current selection and apply it as a fill
			if(Document::GetSelected() == NULL)
				return FALSE;

			if(!FillUsingFillGallerySelection(ApplyToObject))
				return FALSE;
		
			break;

		case SGACTION_SETOPTIONS:	// Set values in the options dialogue as it is opened
			{
				if (CurrentOptionsDlg == NULL)
					return(FALSE);

				CurrentOptionsDlg->AddDisplayModeName(_R(IDS_GALLERYDM_LARGE));		// 0
				CurrentOptionsDlg->AddDisplayModeName(_R(IDS_GALLERYDM_FULLINFO));	// 1
				CurrentOptionsDlg->AddDisplayModeName(_R(IDS_GALLERYDM_SMALL));		// 2
			}
			break;

		case SGACTION_SETSORTMODE:
			{
				if (CurrentSortDlg == NULL)
					return(FALSE);

				CurrentSortDlg->AddSortKeyName(_R(IDS_SORTBY_NAME));
				CurrentSortDlg->AddSortKeyName(_R(IDS_SORTBY_MEMORY));
				CurrentSortDlg->AddSortKeyName(_R(IDS_SORTBY_NAMELENGTH));
				CurrentSortDlg->AddSortKeyName(_R(IDS_SORTBY_FILETYPE));
			}									
			break;


		case SGACTION_DISPLAYMODECHANGED:
			if(DisplayMode >= 3) DisplayMode = 0;
			DefaultDisplayMode = DisplayMode;
			InvalidateCachedFormat();
			ReformatAndRedrawIfNecessary();
  			break;
	
		default:
			return(SuperGallery::ApplyAction(Action));
			break;
	}

	return(TRUE);
}



/********************************************************************************************

>	virtual MsgResult LibFillsSGallery::Message(Msg* Message)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		Message - The message to handle

	Purpose:	A standard message handler, really.

	Notes:		Any messages that this does not handle must be passed down to the
				SuperGallery base class message handler.

				NOTE WELL that the SuperGallery base class handler does some funky things
				for us - see SuperGallery::Message - such as deleting our display subtree
				for any document which dies (which, uncannily, would explain why they go
				away like that when you close documents ;-), and shading the gallery when
				there are no documents present. [To override this behaviour in these cases,
				you should respond to the message, and return OK rather than calling the
				base class message handler]

	SeeAlso:	SuperGallery::Message

********************************************************************************************/

MsgResult LibFillsSGallery::Message(Msg* Message)
{
	// Added by Craig Hamilton 18/1/01.
	static HANDLE	handle = NULL;
	CString			mutexName = "autorunMutex";
	// End added.

	// If we have no displaytree, then we have not been shown, or something terrible has
	// happened, so we don't bother handling any of these messages.
	if (DisplayTree == NULL)
		return(LibraryGallery::Message(Message));

	KernelBitmap *SelectedBitmap = NULL;

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				// Added by Craig Hamilton 18/1/01.
				// This and the section of code of the same date in the DIM_CANCEL handler below
				// deal with the creation and destruction of a kernel object that is recognised by
				// the autorun. If this object exists then the autorun does not run. This is so
				// that the user can enter their resources cd while the gallery is open and not be
				// annoyed by the autorun appearing.
				handle = CreateMutex(NULL,TRUE,mutexName);
				// End added.

				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYFILLSGALLERY, TRUE);
				LibFillsSGallery::ThisGallery->GalleryAboutToReOpen();
				break;

			case DIM_CANCEL:
				// Added by Craig Hamilton 18/1/01.
				if(handle != NULL)
				{
					CloseHandle(handle);
				}
				// End added.

				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYFILLSGALLERY, FALSE);

				// Flush the thumbnail cache
				BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::ThumbState::KILLCACHE, SGLib_Texture), DialogOp);
				BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::ThumbState::KILLCACHE, SGLib_Fractal), DialogOp);
				GalleryAboutToClose();
				break;

			case DIM_LFT_BN_CLICKED:
				switch (Msg->GadgetID)
				{
					case _R(IDC_LIBGAL_OPEN):
						if(Document::GetSelected() != NULL)
							FillUsingFillGallerySelection(AddToPage);
						break;
					
					case _R(IDC_LIBGAL_IMPORT):
						FillUsingFillGallerySelection(AddToPage);
						break;

					case _R(IDC_LIBGAL_BROWSE):
					case _R(IDC_LIBGAL_ADD_FILLS):
						BrowseClicked();
						break;

					case _R(IDC_BMPGAL_FILL):
						if(!FillUsingFillGallerySelection(ApplyToObject))
						{
							return(SuperGallery::Message(Message));
						}
						break;

					case _R(IDC_BMPGAL_TEXTURE):
						if(Document::GetSelected() != NULL)
							if(!FillUsingFillGallerySelection(ApplyToObjectAsTransp))
							{
								return(SuperGallery::Message(Message));					
							}
						break;

					case _R(IDC_BMPGAL_BACKGROUND):
						if(Document::GetSelected() != NULL)
							if(!FillUsingFillGallerySelection(SetBackground))
							{
								return(SuperGallery::Message(Message));					
							}
						break;

					case _R(IDC_BMPGAL_SAVE):
#ifdef _DEBUG		
						{
							SGDisplayNode *Item = DisplayTree->FindNextSelectedItem(NULL);
							if(Item != NULL && Item->IsKindOf(CC_RUNTIME_CLASS(SGLibDisplayItem)))
							{
								SGLibDisplayItem *LibItem = (SGLibDisplayItem *)Item;

								Library *Parent = LibItem->GetParentLibrary();
								if(Parent != NULL)
								{
									PathName *Source = Parent->ReturnIndexLocation();
									PathName Dest(*Source);
									Dest.SetType((String_256)"BAK");
									SGLibOil::FileCopy(Source, &Dest);

									Parent->SaveIndexInDisplayedOrder(Source, TRUE);
								}
							}
						}
#endif
						break;

					case _R(IDC_GALLERY_HELP):		// Show help page
						HelpUserTopic(_R(IDS_HELPPATH_Gallery_Fill));
						break;
	

					
					default:
						break;
				}
		}
	}
	else if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *Msg = (DocChangingMsg *) Message;
		switch (Msg->State)
		{
			case DocChangingMsg::DocState::SELCHANGED:
				if (Msg->pNewDoc == NULL)
				{
					// There is no selected doc - this can only mean there are no docs
					// at all, so we had better shade the gallery
					DoShadeGallery(TRUE);
					SuperGallery::ShadeGallery(TRUE);
				}
				else
				{
					// Possibly a new document
					DoShadeGallery(FALSE);
					SuperGallery::ShadeGallery(FALSE);
					SelectionHasChanged();
				}
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, ThumbMessage) && DisplayTree != NULL)
	{
		ThumbMessage *Msg = (ThumbMessage *) Message;

		// If a library Thumb message comes around, flush the redraw stuff, etc
		if(Msg->State == ThumbMessage::ThumbState::CACHESIZECHANGED)
		{
			FlushBackgroundRedraws();
			ForceRedrawOfList();
		}
	}

	return(LibraryGallery::Message(Message));
}    


 /********************************************************************************************

>	void LibFillsSGallery::SelectionHasChanged(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> (based on Jason code)
	Created:	28/3/95

	Purpose:	To inform the gallery that the selection has changed in some way.
				We need to grey different buttons on different occasions
	Notes:

********************************************************************************************/

void LibFillsSGallery::SelectionHasChanged(void)
{
	if (DisplayTree == NULL)
		return;

	// As we now allow the gallery to be closed and all the groups to remain, we must now
	// also check to see if the gallery is visible i.e. open or not. If not then get out now.
	// Required as SetStringGadgetValue will ERROR and memory leak if called when dialog closed.
	if (!IsVisible())
		return;

	INT32 Count = DisplayTree->GetSelectedItemCount();
	BOOL Documents = (Document::GetSelected() != NULL);

	// Set up button states

	// Enable the import button if there is a selection and valid documents
	EnableGadget(_R(IDC_LIBGAL_IMPORT), (Count > 0) && Documents);

	String_256 strCommand, strDummy;

	BOOL bEnable = Count == 1 && Documents;

	// Import button
	strCommand = SGCmd_Import; 
	EnableGadget(_R(IDC_LIBGAL_IMPORT),		bEnable && !((GetCommandState(&strCommand, &strDummy)).Greyed));

	// Fill button
	strCommand = SGCmd_Fill;
	EnableGadget(_R(IDC_BMPGAL_FILL),		bEnable && !((GetCommandState(&strCommand, &strDummy)).Greyed));

	// Transp button
	strCommand = SGCmd_Transp;
	EnableGadget(_R(IDC_BMPGAL_TEXTURE),	bEnable && !((GetCommandState(&strCommand, &strDummy)).Greyed));

	// Background button
	strCommand = SGCmd_SetBackground; 
	EnableGadget(_R(IDC_BMPGAL_BACKGROUND),	bEnable && !((GetCommandState(&strCommand, &strDummy)).Greyed));

	EnableGadget(_R(IDC_LIBGAL_ADD_FILLS),	TRUE);

	LibraryGallery::SelectionHasChanged();
}

/********************************************************************************************

>	void LibFillsSGallery::DoShadeGallery(BOOL ShadeIt)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> (based on Jason code)
	Created:	28/3/95

	Purpose:	To un-grey the options... button when there are no docs
	Notes:

********************************************************************************************/

void LibFillsSGallery::DoShadeGallery(BOOL ShadeIt)
{
	if (DisplayTree == NULL)
		return;

	// Keep this here always
	EnableGadget(_R(IDC_GALLERY_MENU), TRUE);

	EnableGadget(_R(IDC_LIBGAL_IMPORT), !ShadeIt);
	EnableGadget(_R(IDC_BMPGAL_FILL), !ShadeIt);
	EnableGadget(_R(IDC_BMPGAL_TEXTURE), !ShadeIt);

	EnableGadget(_R(IDC_BMPGAL_BACKGROUND), !ShadeIt);

	EnableGadget(_R(IDC_LIBGAL_ADD_FILLS), TRUE);

	LibraryGallery::DoShadeGallery(ShadeIt);
}

/********************************************************************************************

>	BOOL LibFillsSGallery::FillUsingFillGallerySelection(FillTypeEnum FillType)

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/2000
	Inputs:		FillType - The type of fill required (see def. of FillTypeEnum)
	Returns:	TRUE if things went OK, false otherwise
	Purpose:	To import a fill into camelot, then apply it in the mannor selected
	SeeAlso:	OnPageDrop

********************************************************************************************/

bool LibFillsSGallery::FillUsingFillGallerySelection(FillTypeEnum FillType)
{
	PathName FileName;		
	
	UINT32 TagObjectToFill = 0;	// used for ApplyToObject and ApplyToObjectAsTransp
								// when downloading a fill

	if (GetSelectedItemCount() != 1 && FillType != AddToPage)
	{
		TRACEUSER( "Jonathan", _T("Fills: FillUsingGallerySelection failing due to multiple selection\n"));
		return false;
	}

	SGLibDisplayItem *FillItem = static_cast<SGLibDisplayItem *>(DisplayTree->FindNextSelectedItem());

	if (!FillItem->GetFileName(&FileName))
		return false;

	if (!FileName.IsValid(FileName.GetPath()))
	{
		ERROR3("Filename is invalid");
		return false;
	}

	bool NeedToDownload;
	if (_taccess(FileName.GetPath(), 0) == -1)
		NeedToDownload = true;
	else
		NeedToDownload = false;

	if (FillType == ApplyToObject || FillType == ApplyToObjectAsTransp)
	{
		SelRange* Sel = GetApplication()->FindSelection();
		if (Sel==0)
		{
			ERROR3("Can't find SelRange!");
			return false;
		}
		if (NeedToDownload)
		{
			Node *NodeToFill = Sel->FindFirst();
			if (Sel->FindNext(NodeToFill) != 0)
			{
				// If mulitple objects are selected, saving the tag of just one object is not
				// going to do much good so for now we just give up in this case
				InformError(_R(IDE_SGLFILLS_NOMULTI_DL_FILL));
				return false;
			}
			if (NodeToFill)
				TagObjectToFill = NodeToFill->GetTag();
			else
			{
				// No node selected.  This means the user has pressed fill from the gallery
				// with no selection, intending to set the default fill of the document.  To
				// do this we set TagObjectToFill to zero which results in TagObjectValid not
				// being set.  We do not have to use this logic in the case where the fill is
				// already downloaded as the ApplyFill(...) stuff just uses the current selection
				// (or the lack of current selection).
				TagObjectToFill = 0;
			}
		}
	}
	// Calculate where to put an imported object
	DocCoord DropPos;
	DocRect PageRect;
	Spread *s;
	if ((s = Document::GetSelectedSpread()) != 0)
	{
		if (s->GetPagesRect(&PageRect))
			DropPos = PageRect.Centre();
		else
			return false;
	}
	else
		return false;

	if (!NeedToDownload)
	// Don't need to download the fill
	{
		TRACEUSER( "Jonathan", _T("Fills: File in cache\n"));
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpBitmapImport));

		if (pOpDesc != 0)
		{
			BitmapImportParam Param;

			Param.File				= &FileName;
			Param.Result			= TRUE;
			Param.pTargetDoc		= Document::GetSelected();
			Param.FillType			= FillType;
			Param.pSpread			= Document::GetSelectedSpread();
			Param.DropPos			= DropPos;
			Param.TagObjectValid	= false;
			Param.pObjectValid		= false;

			// Do it...
			pOpDesc->Invoke((OpParam *) &Param);

			if (!Param.Result)
			{
				ERROR3("Problem importing file from cache");
				return false;
			}
		}
	}
	else
	// Need to download the fill
	{
		TRACEUSER( "Jonathan", _T("Fills: File not in cache : downloading\n"));
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpAsynchBitmapImport));

		if (pOpDesc != 0)
		{
			AsynchBitmapImportParam* Param = new AsynchBitmapImportParam;

			if (!Param)
				return false;
			Library* pLibrary		= FillItem->GetParentLibrary();
			String_256 strFileURL	= FileName.GetPath();
			pLibrary->LocalPath2URL(&strFileURL);
			Param->file				= FileName;
			Param->pTargetDoc		= Document::GetSelected();
			Param->strURL			= strFileURL;
			String_256 strDescription;
			FillItem->GetNameText(&strDescription);
			Param->strDescription	= _T("'");
			Param->strDescription	+= strDescription;
			Param->strDescription	+= _T("'");
			Param->type				= TYPE_FILL;
			Param->FillType			= FillType;
			Param->priority			= AsynchDownload::PRIORITY_HIGH;
			Param->pSpread			= Document::GetSelectedSpread();
			Param->DropPos	 		= DropPos;
			Param->TagObjectToFill	= TagObjectToFill;
			Param->pObjectValid		= false;
			// If TagObjectToFill == 0, don't set pObjectValid or TagObjectValid so
			// ApplyFill(...) just applies the fill to nothing (except if the user has
			// changed the selection during the download, in which case that object will
			// get filled - bit bad, but that is a very rare case).
			if (TagObjectToFill)
				Param->TagObjectValid	= true;
			else
				Param->TagObjectValid	= false;

			// Do it...
			pOpDesc->Invoke((OpParam *) Param);
		}
	}
	return true;
}


/********************************************************************************************

>	virtual SGDisplayItem *LibFillsSGallery::AddLibraryItem(SGDisplayGroup *LibraryGroup,
												Library *ParentLib,
												LibraryIndex ItemIndex)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95

	Inputs:		LibraryGroup - The group to add the item into
				ParentLib - (For cross checking inputs) the library you allege the above
				group is for.
				ItemIndex - The Library generated index for this item

	Returns:	NULL, or a pointer to the created item

	Purpose:	Called by the Library class to create a display item for every item in 
				a newly-scanned library file. It is essentially a callback to the gallery
				which requested that the library be scanned.
				
	Notes:		This method MUST BE OVERRIDDEN by the derived gallery that opens the library,
				in order to create appropriate SGDisplayItem-derived nodes for the things
				in the library (e.g. a Fills library gallery will have to create items
				that display Fills thumbnails)

	SeeAlso:	SuperGallery::AddLibraryGroup; SuperGallery::RemoveLibraryGroup

********************************************************************************************/

SGDisplayItem *LibFillsSGallery::AddLibraryItem(SGDisplayGroup *LibraryGroup,
													Library *ParentLib,
													LibraryIndex ItemIndex, BOOL bNew)
{
	ERROR3IF(LibraryGroup == NULL || ParentLib == NULL,
				"LibFillsSGallery::AddLibraryItem - NULL params are illegal");

	ERROR3IF(LibraryGroup->GetParentLibrary() != ParentLib,
				"LibFillsSGallery::AddLibraryitem - The DisplayGroup is not for the same library!");

	// Create a Fills library item
	SGFillsItem *NewItem = new SGFillsItem(ItemIndex, bNew);

	// Alphabetic add...
/*	SGSortKey SortKeys[MaxSGSortKeys];
	for (INT32 i = 0; i < MaxSGSortKeys; i++)
	{
		SortKeys[i].SortKey  = 0;
		SortKeys[i].Reversed = FALSE;
	}

	SortKeys[0].SortKey  = 1;*/

	if (NewItem != NULL)
		LibraryGroup->AddItem(NewItem/*, (SGSortKey *)SortKeys*/);

	return(NewItem);
}


/********************************************************************************************

>	virtual BOOL LibFillsSGallery::CanSearchKeywords(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95

	Returns:	TRUE

	Purpose:	Used to determine if this type of gallery supports keyword searching.
				This one does
				
********************************************************************************************/

BOOL LibFillsSGallery::CanSearchKeywords(void)
{
	return(TRUE);
}


/********************************************************************************************

>	virtual void LibFillsSGallery::HandleDragStart(DragMessage *DragMsg)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95

	Inputs:		DragMsg - The DRAGSTARTED message that we've just recieved, indicating
				the type of drag being started

	Purpose:	Checks a DragMessage to see if it is a colour drag.
				If it is, then it creates a drag target for this gallerys listbox.

	Notes:		Overrides the default base-class action. Calls down to the base class
				if it is not a colour drag, so that dragging of gallery groups is allowed

********************************************************************************************/

void LibFillsSGallery::HandleDragStart(DragMessage *DragMsg)
{
	// If it's a bitmap drag, add a target for our window. If not, let the base class
	// have a look at it (to see if it is a gallery item being dragged)
	if (DragMsg->pInfo->IsKindOf(CC_RUNTIME_CLASS(GalleryFillsDragInfo)))
		SGFillsDragTarget *NewTarget = new SGFillsDragTarget(this, GetListGadgetID());
	else
		SuperGallery::HandleDragStart(DragMsg);
}

/********************************************************************************************

>	virtual void LibFillsSGallery::WorkOutSectionName(String_256 *Section);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/95

	Outputs:	Section	- String used to return the section name

	Purpose:	Returns the section name to use in the grm file

********************************************************************************************/

void LibFillsSGallery::WorkOutSectionName(String_256 *Section)
{
	if(Section)
		*Section = 	_R(IDS_SGLFILLS_SECTION_NAME); // "FillsGallery";
}



/********************************************************************************************

>	virtual BOOL LibFillsSGallery::InitMenuCommands(void)
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95

	Returns:	TRUE for success

	Purpose:	Initialises any menu commands that this gallery needs.

	Notes:		Will only create the menu commands once - further calls in the future
				will return TRUE immediately wihtout doing anything.

********************************************************************************************/

BOOL LibFillsSGallery::InitMenuCommands(void)
{
	static BOOL MenusInitialised = FALSE;

	BOOL ok = TRUE;

	if (!MenusInitialised)
	{
		// Initialise menu command Ops

		// "Standard" entries for options menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Find, _R(IDS_SGMENU_FIND));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Sort, _R(IDS_SGMENU_SORT));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Properties, _R(IDS_SGMENU_PROPERTIES));

		// "Special" entries for options menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Add, _R(IDS_SGMENU_ADDFILL));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_EmptyFillsCache, _R(IDS_SGMENU_EMPTYFILLCACHE));


		// "Special" entries for over-list menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Import, _R(IDS_SGMENU_IMPORT));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Fill, _R(IDS_SGMENU_FILL));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Transp, _R(IDS_SGMENU_TRANSP));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_SetBackground, _R(IDS_SGMENU_SETBACKGROUND));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Download, _R(IDS_SGMENU_DOWNLOAD));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Remove, _R(IDS_SGMENU_REMOVE));

		// "Standard" commands for over-list menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_FoldGroup, _R(IDS_SGMENU_FOLD));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_UnfoldGroup, _R(IDS_SGMENU_UNFOLD));

		ok = ok && InitMenuCommand((StringBase *) &SGCmd_NextGroup, _R(IDS_SGMENU_NEXTGROUP));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_PrevGroup, _R(IDS_SGMENU_PREVGROUP));

		MenusInitialised = TRUE;
	}

	return(ok);
}


/********************************************************************************************

>	virtual BOOL LibFillsSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		TheMenu - The menu to add commands to
				MenuID  - The type of menu (over-list or from-options-button) to create

	Returns:	TRUE if it succeeded

	Purpose:	To build a menu of commands to be popped up over the gallery.
	
	Notes:		Override this method to stop the default menus being built

********************************************************************************************/

BOOL LibFillsSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)
{
 	BOOL ok = TRUE;

	if (MenuID == SGMENU_OPTIONS)
	{
		// Options menu
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Add);		// With separator
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_EmptyFillsCache, TRUE);		// With separator


		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Find);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Sort);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Properties);
	}
	else
	{
		// Over-list menu
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Import);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Fill);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Transp,TRUE);

		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Remove, TRUE);		// With separator

		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_SetBackground,TRUE);

		SGDisplayGroup *TheGroup = FindCommandGroup();		// Fold or unfold as appropriate
		if (TheGroup == NULL || !TheGroup->Flags.Folded)
			ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_FoldGroup);
		else
			ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_UnfoldGroup);

		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_PrevGroup);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_NextGroup);
	}

	return(ok);
}



/********************************************************************************************

>	virtual OpState LibFillsSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		CommandID - TheString ID of the command
	Outputs:	ShadeReason - If you return (OpState.Greyed == TRUE) then this should be filled
				ion with the reason that the item is shaded/greyed.

	Returns:	An OpState indicating the current menu item state.

	Purpose:	To determine the state of a given menu item. This method is an exact
				parallel to an Op's GetState method (in fact, it is called by an Op's GetState)
	
	Notes:		Override this method to provide state info for your special commands
				Call the base class for unknown commands to allow it to handle them for you

				The base class handles all of these (maybe more - see the base class help)
					Properties, Sort, Find;
					New, Edit, Delete, Redefine;
					NextGroup, PrevGroup, FoldGroup, UnfoldGroup;

********************************************************************************************/

OpState LibFillsSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)
{
	TRACEUSER( "Jonathan", _T("Fills::GetCommandState called for "));
	TRACEUSER("Jonathan", *CommandID);
	TRACEUSER( "Jonathan", _T("\n"));
	OpState State;

	if (*CommandID == SGCmd_Add)									// --- Add (always available)
	{	
		return(State);
	}
	else if (*CommandID == SGCmd_EmptyFillsCache)
	{
		if (m_bDiscardWebFolders)
			State.Greyed = TRUE;
		return (State);
	}
	else if (*CommandID == SGCmd_Fill || *CommandID == SGCmd_Transp || // --- Fill/Transp/Set Background/Import
			 *CommandID == SGCmd_SetBackground || *CommandID == SGCmd_Import)		
	{
		if (GetSelectedItemCount() != 1)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_SINGLE));
		}
	}
	else if (*CommandID == SGCmd_Remove)							// --- Remove
	{
		if (GetSelectedGroupCount() < 1)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSELGROUP));
		}
	}
	else
		return(SuperGallery::GetCommandState(CommandID, ShadeReason));	// Unknown command- pass to baseclass

	return(State);
}



/********************************************************************************************

>	virtual void LibFillsSGallery::DoCommand(StringBase *CommandID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		CommandID - The String ID of the command

	Purpose:	To apply a given command when it is chosen from the menu.
	
	Notes:		Override this method to provide handling for your special commands.
				Call the base class if you don't recognise the command, so that it can
				handle standard commands.

				The base class handles all of these (maybe more - see the base class help)
					Properties, Sort, Find;
					New, Edit, Delete, Redefine; (it calls ApplyAction as appropriate)
					NextGroup, PrevGroup, FoldGroup, UnfoldGroup;

********************************************************************************************/

void LibFillsSGallery::DoCommand(StringBase *CommandID)
{
	if (*CommandID == SGCmd_Import)											// --- Import
	{
		if (Document::GetSelected() != NULL)
			FillUsingFillGallerySelection(AddToPage);
	}
	else if (*CommandID == SGCmd_Fill)										// --- Fill
	{
		if (Document::GetSelected() != NULL)
			FillUsingFillGallerySelection(ApplyToObject);
	}
	else if (*CommandID == SGCmd_EmptyFillsCache)
	{
		RemoveWebFolders(SGLib_Fractal);
	}
	else if (*CommandID == SGCmd_Transp)									// --- Transp
	{
		if (Document::GetSelected() != NULL)
			FillUsingFillGallerySelection(ApplyToObjectAsTransp);
	}
	else if (*CommandID == SGCmd_Add)										// --- Add fills
		BrowseClicked();
	else if (*CommandID == SGCmd_Remove)									// --- Remove
	{
		RemoveSelectedLibraries(TRUE);
		UpdateGRMFile();
	}
	else if (*CommandID == SGCmd_SetBackground)								// --- Set background
	{
		if (Document::GetSelected() != NULL)
			FillUsingFillGallerySelection(SetBackground);
	}
	else
		SuperGallery::DoCommand(CommandID);		// Unknown command- pass to the base class
}






/***********************************************************************************************

>	virtual SGDisplayGroup *LibFillsSGallery::AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		LibraryToDisplay - Pointer to the library associated with the group
				NumItems - Number of items in the group
	Returns:	The newly created library group, or NULL if there were problems
	Purpose:	Create a library group, as opposed to a display group
	Notes:

***********************************************************************************************/

SGDisplayGroup *LibFillsSGallery::AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems)
{
	ERROR3IF(LibraryToDisplay == NULL, "LibFillsSGallery::AddLibraryGroup - NULL parameter is illegal");

	if (DisplayTree == NULL)
	{
		ERROR3("LibFillsSGallery::AddLibraryGroup called before the DisplayTree was initialised!");
		return(NULL);
	}

	SGLibGroup *TheGroup = (SGLibGroup *)DisplayTree->FindSubtree(this, NULL, LibraryToDisplay);

	if (TheGroup == NULL)
	{
		// No existing group for that library, so create a new one
		TheGroup = new SGLibGroup(this, NULL, LibraryToDisplay);

		if (TheGroup == NULL)				// Failed!
			return(NULL);

		// And add it to our display tree
		DisplayTree->AddItem(TheGroup);
	}
	else
		TheGroup->DestroySubtree(FALSE);	// Delete all items in the group

	return(TheGroup);
}

/********************************************************************************************

>	virtual SGDisplayItem *LibFillsSGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
													SGDisplayGroup *DestGroup,
													SGDisplayItem *TargetPosition = NULL)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95

	Inputs:		SourceItem - The item to copy elsewhere in the tree (see below)

				DestGroup - The group into which the item should be inserted

				TargetPosition - NULL (to add the copied item to the end of the sibling
				list), or points to an item BEFORE which the copied item will be inserted.

	Returns:	NULL (failed) or a pointer to the new (copied) display item

	Purpose:	"Copies" the existing node in the tree in an appropriate fashion.
				
				This method is normally called when a gallery-organising drag completes,
				and it is discovered that the dragged item(s) have been dragged to a 
				different display group.

	Notes:		This method should be overridden by derived galleries to provide
				appropriate behaviour (some galleries (e.g colour) will copy the real-item
				that the given display-item references to the new group (document), while
				other galleries (layer) may just move the item after all).

				Note the handy InsertCopiedItem and MoveBefore/After methods which
				are available to take all of the hard work out of copying/moving items!

				See the body of this method in the source code for example pseudocode.
				For real code, see the Colour Gallery (sgcolour.cpp)

	SeeAlso:	SuperGallery::InsertCopiedItem; SGDisplayItem::MoveBefore;
				SGDisplayItem::MoveAfter; ColourSGallery::CopyDisplayItem

********************************************************************************************/

SGDisplayItem *LibFillsSGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
								SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition)
{
	if(SourceItem == NULL || DestGroup == NULL)
	{
		ERROR3("LibFillsSGallery::CopyDisplayItem -> Illegal NULL param");
		return NULL;
	}

	// Check if we've dragged from one group to another and warn if that's the case
	if(DestGroup->IsKindOf(CC_RUNTIME_CLASS(SGLibGroup)))
	{
		SGDisplayNode *SourceGroup = SourceItem->GetParent();
		if((SGDisplayGroup *)SourceGroup != DestGroup)
		{
			// "Can't move library fills between groups."
			InformError(_R(IDS_FILLS_BETWEEN_GROUPS), _R(IDS_OK));
			Error::ClearError();
		}
	}

	return(NULL);
}


/********************************************************************************************

>	BOOL OpDisplayLibFillsGallery::Init()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDisplayLibFillsGallery initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDisplayLibFillsGallery::Init()
{

	// **** TO DO ****
	// Ensure the IDS_ OPTOKEN_ and IDBBL_ constants are available and correct
	// You should really also uppercase these constant names.

	return (RegisterOpDescriptor(
	 							0,
	 							_R(IDS_DISPLAY_FILLS_GALLERY),
								CC_RUNTIME_CLASS(OpDisplayLibFillsGallery),
	 							OPTOKEN_DISPLAYFILLSGALLERY,
	 							OpDisplayLibFillsGallery::GetState,
	 							0,	/* help ID */
	 							_R(IDBBL_DISPLAY_FILLS_GALLERY),
	 							0	/* bitmap ID */));
}               

/********************************************************************************************

>	OpState	OpDisplayLibFillsGallery::GetState(String_256*, OpDescriptor*)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDisplayLibFillsGallery operation
	Purpose:	For finding the OpDisplayLibFillsGallery's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDisplayLibFillsGallery::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;  

	// If the gallery is currenty open, then the menu item should be ticked
	DialogBarOp* pDialogBarOp = FindGallery();
	if (pDialogBarOp != NULL)
		OpSt.Ticked = pDialogBarOp->IsVisible();

//	OpSt.Greyed = (Document::GetSelected() == NULL);
 	return(OpSt);   
}



/********************************************************************************************

>	void OpDisplayLibFillsGallery::Do(OpDescriptor*)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Displays the LibFillss gallery
				Updates the button state for this Op (the button sticks down while the
				gallery is open)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDisplayLibFillsGallery::Do(OpDescriptor*)
{
	DialogBarOp *pDialogBarOp = FindGallery();

	if (pDialogBarOp != NULL)
	{
		// Toggle the visible state of the gallery window
		pDialogBarOp->SetVisibility( !pDialogBarOp->IsVisible() );

		// And update our button state
		SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYFILLSGALLERY, pDialogBarOp->IsVisible());

		// If we're closing the gallery, flush the thumbnail caches
		if(pDialogBarOp->IsVisible() == FALSE)
		{
			// Flush the thumbnail cache
			BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::ThumbState::KILLCACHE, SGLib_Texture), DialogOp);
			BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::ThumbState::KILLCACHE, SGLib_Fractal), DialogOp);

			// Ensure all open libraries are closed and memory reclaimed when the gallery closes
			if(LibFillsSGallery::ThisGallery != NULL)
			{
				// Free memory, etc...
				LibFillsSGallery::ThisGallery->GalleryAboutToClose();
			}
			
		}
	}

	End();
}


/********************************************************************************************

>	static DialogBarOp *OpDisplayLibFillsGallery::FindGallery(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95

	Returns:	NULL or a pointer to the LibFills gallery instance

	Purpose:	Finds the LibFills gallery class instance

	Notes:		The bars system always keeps one LibFills gallery alive for us.
				If one is not found, this usually indicates that it can't be found
				in bars.ini: Check that the 'Name' string *exactly* matches the
				title string given in bars.ini.
				Also check that bars.ini indicates the bar is of the LibFillsSGallery class

********************************************************************************************/

DialogBarOp *OpDisplayLibFillsGallery::FindGallery(void)
{
	String_32 Name = _R(IDS_SGLFILLS_GALLERY_NAME); //  "Fill gallery"
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

	if (pDialogBarOp != NULL)
	{
		if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(LibFillsSGallery))
			return(pDialogBarOp);

		ERROR3("Got the Fills gallery but it's not of the LibFillsSGallery class");
	}
	else
	{
		ERROR3("Can't find the Fills gallery in bars.ini!\n");
	}

	return(NULL);
}

/***********************************************************************************************

>	virtual LibDisplayType SGFillsItem::GetDisplayType(SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95

	Inputs:		MiscInfo - Contains a few useful bits of info that may be
				needed for all event types.
	Outputs:	
	Returns:	The display mode type to use (position of text, and size of thumb)

	Purpose:	Return the display type to use - Fills gallery override
	Notes:
	SeeAlso:

***********************************************************************************************/

LibDisplayType SGFillsItem::GetDisplayType(SGMiscInfo *MiscInfo)
{
	switch(MiscInfo->DisplayMode)
	{
		case 2:
			return LibDisplay_SmallThumbText;
			break;
		case 1:
			return LibDisplay_FullInfo;
			break;
		case 0:
		default:
			return LibDisplay_LargeThumbTextUnder;
			break;
	}

	return LibDisplay_LargeThumbTextUnder;
}

/***********************************************************************************************

>	SGFillsItem::SGFillsItem(LibraryIndex LibraryIndexToDisplay);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		LibraryIndex - Offset into index file which describes this item
	Outputs:	
	Returns:
	Purpose:	Creates and initialises a new item for the Fills gallery
	Notes:
	SeeAlso:

***********************************************************************************************/

SGFillsItem::SGFillsItem(LibraryIndex LibraryIndexToDisplay, BOOL bNew) :
	SGLibDisplayItem(LibraryIndexToDisplay, bNew)
{
}

/* default constructor and destructor */

SGFillsItem::SGFillsItem()
{
}

SGFillsItem::~SGFillsItem()
{
}


/***********************************************************************************************

	Drag stuff added by Will.

***********************************************************************************************/

/***********************************************************************************************

>	virtual BOOL SGFillsItem::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		EventType - An enumerated value describing what type of event is to be processed

				EventInfo - A structure describing the event (may be NULL). The exact thing
							pointed at by this pointer depends upon the event type:

							MonoOn
							Event				Thing EventInfo points at
							SGEVENT_FORMAT		(SGFormatInfo *)
							SGEVENT_REDRAW		(SGRedrawInfo *)
							SGEVENT_MOUSECLICK	(SGMouseInfo *)
							MonoOff
				Use the provided SGDisplayNode::Get[Format]Info() inlines to retrieve this
				information - they provide useful error/type checking, and hide the cast

				MiscInfo - always provided. Contains a few useful bits of info that may be
				needed for all event types.

	Outputs:	FormatInfo is updated as appropriate

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a SuperGallery DisplayTree event

	Notes:		This overrides the pure virtual SGDisplayNode::HandleEvent method

				A node need not handle a specific event - if it does not handle it, it
				should return FALSE.

				Redraw and Formatting handlers should never return TRUE, as this will
				prevent the event from continuing through the tree.

				Non-leaf-nodes must call SGDisplayNode::GiveEventToMyChildren in order
				to pass the event dow the tree. THIS node is a leaf-node, so it doesn't.

	SeeAlso:	SGDisplayNode::HandleEvent

***********************************************************************************************/

BOOL SGFillsItem::HandleEvent(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo)
{
	switch (EventType)
	{
		case SGEVENT_MOUSECLICK:
		{
			SGMouseInfo *Mouse = GetMouseInfo(EventType, EventInfo);

			if (Mouse != NULL && FormatRect.ContainsCoord(Mouse->Position))
			{					
				INT32 XSize=0, YSize=0;
				if(SGLibDisplayItem::StartDrag(EventType, EventInfo, MiscInfo, &XSize, &YSize))
				{
					GalleryFillsDragInfo *DragFill;

					DragFill = new GalleryFillsDragInfo(this, Mouse, MiscInfo,
														Mouse->MenuClick, XSize, YSize);
					if (DragFill != NULL)
						DragManagerOp::StartDrag(DragFill);
					else
					if(LibraryGallery::TmpDraggingBitmap != NULL)
					{
						delete LibraryGallery::TmpDraggingBitmap;
						LibraryGallery::TmpDraggingBitmap = NULL;
					}
				}
				return TRUE;		// Claim this event - nobody else can own this click
			}
		}
	}
	
	return(SGLibDisplayItem::HandleEvent(EventType, EventInfo, MiscInfo));
}


/********************************************************************************************

>	SGFillsDragTarget::SGFillsDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL)
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Inputs:		TheDialog - The kernel dialog in which the target exists
				TheGadget - The gadget within that dialogue which is the target

	Purpose:	Constructor

********************************************************************************************/

SGFillsDragTarget::SGFillsDragTarget(DialogOp *TheDialog, CGadgetID TheGadget)
					: SGListDragTarget(TheDialog, TheGadget)
{
	ERROR3IF(!TheDialog->IsKindOf(CC_RUNTIME_CLASS(LibFillsSGallery)),
			"You can only use SGFillsDragTargets with LibFillsSGallery dialogues!");
}



/********************************************************************************************

	BOOL SGFillsDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
											OilCoord *pMousePos, KeyPress* pKeyPress)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Inputs:		Event - Indicates what has happened
				pDragInfo - points to drag information describing this drag. This
				should be a FillsDragInformation or derivation thereof
				pMousePos - points to information on the current mouse position, in OIL coords
				pKeyPress - NULL, or if for a keypress event, keypress information

	Returns:	TRUE to claim the event, FALSE to let it through to other targets

	Purpose:	Event Handler for SuperGallery listitem drag events. Overrides the
				base class handler to enable it to sort out the node being dragged
				for Fills drags.

********************************************************************************************/

BOOL SGFillsDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
										OilCoord *pMousePos, KeyPress* pKeyPress)
{
	if (!pDragInfo->IsKindOf(CC_RUNTIME_CLASS(BitmapDragInformation)))
		return(FALSE);

	SGDisplayNode *DraggedNode = NULL;
	BOOL IsSimpleBitmapDrag = TRUE;

	// This target only knows about Fill gallery items.
	if (IS_A(pDragInfo, GalleryFillsDragInfo))
	{
		DraggedNode = ((GalleryFillsDragInfo *)pDragInfo)->GetDraggedFill();
		IsSimpleBitmapDrag = FALSE;
	}

	if (DraggedNode != NULL)
	{
		switch(Event)
		{
			case DRAGEVENT_COMPLETED:
				// If the DragInfo didn't know what to do, then call our baseclass
				HandleDragCompleted((SuperGallery *) TargetDialog,
									DraggedNode, pMousePos, IsSimpleBitmapDrag);
				return(TRUE);


			case DRAGEVENT_MOUSESTOPPED:
			case DRAGEVENT_MOUSEMOVED:
			case DRAGEVENT_MOUSEIDLE:
				{
					// Specialised cursor changing code - can't drag between groups...

					SuperGallery *ParentGallery = (SuperGallery *) TargetDialog;
					if (ParentGallery != NULL && pMousePos != NULL && DraggedNode != NULL)
					{
						// Convert the OilCoords into DocCoords
						DocCoord MousePos(pMousePos->x, pMousePos->y);
						SGDisplayNode *DestNode = ParentGallery->FindNodeUnderPointer(&MousePos);

						if(DestNode != NULL)
						{
							SGDisplayNode *DestGroup = DestNode->GetParent();
							SGDisplayNode *SourceGroup = DraggedNode->GetParent();

							if(DestGroup != NULL && SourceGroup != NULL)
							{	
								// We're dragging within a group - that's ok
								if(SourceGroup == DestGroup)
									return(DetermineCursorShape(ParentGallery, DraggedNode, pMousePos));
							}
						}		
					}
				}

				// If we're trying to drag to unimplemented areas (between groups), set the no-can-do cursor
				CurrentCursorID = _R(IDC_DRAGGING_COLOUR);		// No-can-drop cursor shape
				return TRUE;
		}
	}

	// Otherwise, we aren't interested in the event, so we don't claim it
	return(FALSE);
}

/********************************************************************************************

>	void GalleryFillsDragInfo::GalleryFillsDragInfo() 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  

	Purpose:	Default constructor - do not call this constructor

********************************************************************************************/

GalleryFillsDragInfo::GalleryFillsDragInfo()
{
	ERROR3("Default GalleryFillsDragInfo constructor called");	
}

/********************************************************************************************

>	void GalleryFillsDragInfo::~GalleryFillsDragInfo() 
	 
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/95		  

	Purpose:	Destructor - reclaim temporary bitmap memory

********************************************************************************************/

GalleryFillsDragInfo::~GalleryFillsDragInfo()
{
	if(LibraryGallery::TmpDraggingBitmap != NULL)
	{
		delete LibraryGallery::TmpDraggingBitmap;
		LibraryGallery::TmpDraggingBitmap = NULL;
	}
}


/********************************************************************************************

>	GalleryFillsDragInfo::GalleryFillsDragInfo(SGFillsItem *pSourceItem,
												SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 												BOOL IsAdjust = FALSE, INT32 XSize = 0, INT32 YSize = 0)
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  

	Inputs:		pSourceItem - The gallery item from which the drag originated
				pMouseInfo	- The mouse info which made the item start the drag
				pMiscInfo	- The MiscInfo which accompanied the mouse event
				IsAdjust	- TRUE if this is an adjust (line-Bitmap) drag
				XSize		- Width in pixels of bmp to drag (0 for actual size of bmp)
				YSize		- Height in pixels of bmp to drag (0 for actual size of bmp)

	Purpose:	Constructor

********************************************************************************************/

GalleryFillsDragInfo::GalleryFillsDragInfo(SGFillsItem *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust, INT32 XSize, INT32 YSize)
					  : BitmapDragInformation(LibraryGallery::TmpDraggingBitmap,
					  							XSize, YSize, 0, 0, IsAdjust)
{
	SourceItem	= pSourceItem;	// Copy the source item pointer

	MouseInfo	= *pMouseInfo;	// Duplicate the structures (they may cease to exist
	MiscInfo	= *pMiscInfo;	// soon after the drag is started)
}



/********************************************************************************************

>	void GalleryFillsDragInfo::OnClick(INT32 Flags,POINT Point) 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This is called if a drag was attempted but never started because it was a 
				click all along. It calls back the SourceItem SGDisplayBitmap, to get it
				to handle the click.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GalleryFillsDragInfo::OnClick(INT32 Flags ,POINT Point)
{
	if (SourceItem != NULL)
		SourceItem->DragWasReallyAClick(&MouseInfo, &MiscInfo);
}

/********************************************************************************************

>	void GalleryFillsDragInfo::GetCursorID(DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Purpose:	Set cursor over this target


********************************************************************************************/

UINT32 GalleryFillsDragInfo::GetCursorID(DragTarget* pDragTarget)
{
	if (pDragTarget && pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		PageDropInfo PageDropInfo;
		((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);

		// Find the object we are over (and which bit of it)
		NodeRenderableInk* pObjectHit 	= PageDropInfo.pObjectHit;
		ObjectDragTarget TargetHit 		= PageDropInfo.TargetHit;
/*
		// Applying fills doesn't work asynchronously, so if we don't have the file we won't let the user drop it on objects
		PathName fillPath;
		SourceItem->GetFileName(&fillPath);
		if (pObjectHit && _taccess(fillPath.GetPath(), 0) == -1)
			return _R(IDC_CANTDROP);
*/
		// We can't apply a fill to a NodeBitmap 
		if (pObjectHit && pObjectHit->IS_KIND_OF(NodeBitmap))
			TargetHit = NO_TARGET;				// Ignore NodeBitmap objects

		ClickModifiers ClickMods = ClickModifiers::GetClickModifiers();
		BOOL IsInside = ClickMods.Constrain;

		if (!IsInside && pObjectHit && pObjectHit->IsCompound())
		{
			TargetHit = MANY_TARGET;
		}

		UINT32 CanFill = IsInside ? _R(IDC_DROPINSIDEONFILL) : _R(IDC_CANDROPONFILL);

		switch (TargetHit)
		{
			case FILL_TARGET:
			case LINE_TARGET:
			case STARTCOL_TARGET:
			case ENDCOL_TARGET:
			case ENDCOL2_TARGET:
			case ENDCOL3_TARGET:
			case MANY_TARGET:
				return CanFill;

			case NO_TARGET:
				// fill gallery - drop = page fill, CTRL + drop = add bitmap
				return IsInside ? _R(IDC_CANFILLPAGE) : _R(IDC_CANDROPONPAGE);
				//return _R(IDC_CANDROPONPAGE);
		};

		return _R(IDC_CANDROPONPAGE);
	}

	return _R(IDC_CANTDROP);
}

/********************************************************************************************

>	virtual BOOL GalleryFillsDragInfo::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Returns:	Whether String is valid
	Purpose:	provide status line text for this target
   
********************************************************************************************/

BOOL GalleryFillsDragInfo::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)
{
	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");

	//if (TheBitmap == NULL || TheBitmap->ActualBitmap == NULL)
	//	return FALSE;

 	String_256 DragString;
	String_256 ItemName;
	SourceItem->GetNameText(&ItemName);
	PageDropInfo PageDropInfo;
	NodeRenderableInk* pObjectHit = NULL;
	ObjectDragTarget TargetHit;
	if (pDragTarget && pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);
		pObjectHit 	= PageDropInfo.pObjectHit;
		TargetHit 		= PageDropInfo.TargetHit;
	}


	PathName fillPath;
	SourceItem->GetFileName(&fillPath);

	// "Dragging fill '#1%s'"
 	DragString.MakeMsg(_R(IDS_FILLS_DRAGGING), (TCHAR *)ItemName);

	if (pDragTarget && pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		DragString += String_8(_R(IDS_SGDFONTS_STAT_COLON_SEP)); //" : ";

		String_256 ObjectDesc = _R(IDS_SGLFILLS_THIS_OBJECT); // " this object";

		if (pObjectHit)
		{
			ObjectDesc = pObjectHit->Describe(FALSE);
		}

		if (pObjectHit && pObjectHit->IS_KIND_OF(NodeBitmap))
			TargetHit = NO_TARGET;				// Ignore NodeBitmap objects

		ClickModifiers ClickMods = ClickModifiers::GetClickModifiers();
		BOOL IsInside = ClickMods.Constrain;

		if (!IsInside && pObjectHit && pObjectHit->IsCompound())
		{
			TargetHit = MANY_TARGET;
		}

		switch (TargetHit)
		{
			case FILL_TARGET:
			case LINE_TARGET:
			case STARTCOL_TARGET:
			case ENDCOL_TARGET:
			case ENDCOL2_TARGET:
			case ENDCOL3_TARGET:
				DragString += String_64(_R(IDS_SGLFILLS_DROP_TO_APPLY)); //"Drop to apply the fill to this ";
				DragString += ObjectDesc;
				if (IsInside)
					DragString += String_64(_R(IDS_SGLFILLS_ALONE)); // " alone";
				break;
			case MANY_TARGET:
				DragString += String_64(_R(IDS_SGLFILLS_DROP_TO_APPLY)); //"Drop to apply the fill to this ";
				DragString += ObjectDesc;
				DragString += String_64(_R(IDS_SGLFILLS_DROP_INSIDE)); // "; Press 'Control' to drop 'Inside'";
				break;

			case NO_TARGET:
			{
				if (IsInside)
				{
					DragString += String_256(_R(IDS_DRAGBMP_DROP_SETPAGEBACKGROUND)); // "Drop to set the Bitmap fill of the page background";
				}
				else
				{
					// + "Drop to Insert a new Bitmap object on the Page"
					String_64 DropToThis(_R(IDS_FILLS_INSERT_BITMAP_TO_PAGE));
					DragString += DropToThis;
				}
				break;
			}
		}

	 	*TheText = DragString;
		return TRUE;
	}

 	*TheText = DragString;
 	
	return TRUE;
}

/********************************************************************************************

>	BOOL GalleryFillsDragInfo::OnPageDrop(ViewDragTarget* pDragTarget)
 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  
	Inputs:		pDragTarget, the ViewDragTarget that we were dropped onto.
	Returns:	TRUE, if we handled the drop.
	Purpose:	Called when a Fill is dropped onto the page.
				Use pDragTarget->GetDropInfo(), to details of the drop.
	SeeAlso:	FillUsingFillGallerySelection

********************************************************************************************/

BOOL GalleryFillsDragInfo::OnPageDrop(ViewDragTarget* pDragTarget)
{
	enum FillTypeEnum FillType;

	PageDropInfo ThePageDropInfo;
	((ViewDragTarget*)pDragTarget)->GetDropInfo(&ThePageDropInfo);
	NodeRenderableInk* pObjectHit = ThePageDropInfo.pObjectHit;

	PathName FileName;		
	SGLibDisplayItem *FillItem = SourceItem;

	if (!FillItem->GetFileName(&FileName))
		return FALSE;

	if (!FileName.IsValid(FileName.GetPath()))
	{
		ERROR3("LibFillsSGallery::ImportFills - Filename is invalid");
		return FALSE;
	}

	BOOL NeedToDownload;
	if (_taccess(FileName.GetPath(), 0) == -1)
		NeedToDownload = TRUE;
	else
		NeedToDownload = FALSE;

	BOOL Constrain = KeyPress::IsConstrainPressed();
	if (Constrain && !pObjectHit)
		FillType = SetBackground;
	else if (pObjectHit && !pObjectHit->IS_KIND_OF(NodeBitmap))
		FillType = ApplyToObject;
	else
		FillType = AddToPage;

	if (!NeedToDownload)
	// Don't need to download the fill
	{
		TRACEUSER( "Jonathan", _T("Fills: File in cache\n"));
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpBitmapImport));

		if (pOpDesc != 0)
		{
			BitmapImportParam Param;

			Param.File				= &FileName;
			Param.Result			= TRUE;
			Param.pTargetDoc		= Document::GetSelected();
			Param.FillType			= FillType;
			Param.pSpread			= ThePageDropInfo.pSpread;
			Param.DropPos			= ThePageDropInfo.DropPos;
			Param.TagObjectValid	= false;
			Param.pObject			= pObjectHit;
			Param.pObjectValid		= true;

			// Do it...
			pOpDesc->Invoke((OpParam *) &Param);

			if (!Param.Result)
			{
				ERROR3("Problem importing file from cache");
				return FALSE;
			}
		}
	}
	else
	// Need to download the fill
	{
		TRACEUSER( "Jonathan", _T("Fills: File not in cache : downloading\n"));
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpAsynchBitmapImport));

		if (pOpDesc != 0)
		{
			AsynchBitmapImportParam* Param = new AsynchBitmapImportParam;

			if (!Param)
				return FALSE;
			Library* pLibrary		= SourceItem->GetParentLibrary();
			String_256 strFileURL	= FileName.GetPath();
			pLibrary->LocalPath2URL(&strFileURL);
			Param->file				= FileName;
			Param->pTargetDoc		= Document::GetSelected();
			Param->strURL			= strFileURL;
			String_256 strDescription;
			SourceItem->GetNameText(&strDescription);
			Param->strDescription	= _T("'");
			Param->strDescription	+= strDescription;
			Param->strDescription	+= _T("'");
			Param->type				= TYPE_FILL;
			Param->FillType			= FillType;
			Param->priority			= AsynchDownload::PRIORITY_HIGH;
			Param->pSpread			= ThePageDropInfo.pSpread;
			Param->DropPos	 		= ThePageDropInfo.DropPos;
			Param->pObjectValid		= false;
			if (FillType == ApplyToObject || FillType == ApplyToObjectAsTransp)
			{
				Param->TagObjectToFill = pObjectHit->GetTag();
				Param->TagObjectValid = true;
			}

			// Do it...
			pOpDesc->Invoke((OpParam *) Param);
		}
	}
	return TRUE;
}
