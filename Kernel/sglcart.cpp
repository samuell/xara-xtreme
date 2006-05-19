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
// SGLibClipart.cpp - LibClipart SuperGallery classes - LibClipartSGallery and SGDisplayLibClipart

#include "camtypes.h"
#include "sglcart.h"
#include <io.h>

#include "app.h"		// For GetApplication()
//#include "galstr.h"
#include "pathname.h"
#include "sginit.h"
#include "sglcart.h"	// This .cpp file's corresponding header
#include "sglib.h"
#include "sgmenu.h"
#include "sgtree.h"
#include "thumb.h"
#include "sglfills.h"
#include "thumbmsg.h"
#include "grnddib.h"
//#include "galres.h"
//#include "richard.h"
#include "sgliboil.h"
#include "errors.h"
#include "fixmem.h"
#include "selop.h"
#include "filedlgs.h"
#include "document.h"
#include "progress.h"
#include "camelot.h"
#include "dragmgr.h"
//#include "viewrc.h"		// FOR _R(IDC_CANDROPONPAGE)
//#include "resource.h"
#include "docview.h"
#include "scrcamvw.h"
#include "spread.h"
#include "sgscan.h"
#include "sgscanf.h"
#include "sglbase.h"
#include "sgrmfile.h"	// For Lib settings
//#include "richard2.h"	// For yet more resource strings
#include "keypress.h"	// For adjust checking
#include "fileutil.h"
#include "camdoc.h"
#include "pathnmex.h"
//#include "webster.h"
#include "inetop.h"
#include "camnet.h"
//#include "simon.h"

//#include "richard3.h"	// For _R(IDS_GALLERY_FREEING_MEMORY_ON_CLOSE)

#ifdef _BATCHING
#define SAVEASWELL 1
// For batching...
#include <stdio.h>
#include "spread.h"
#include "document.h"
#include "page.h"
#include "pagesize.h"
#include "optspage.h"
#include "units.h"
#include "grid.h"
#include "zoomops.h"
#include "layer.h"
#include "node.h"
#include "collist.h"
#include "colourix.h"
#include "colormgr.h"
#include "webop.h"
#endif

#include "helpuser.h"		//For the help button
//#include "xshelpid.h"		//For the help button
//#include "helppath.h"
//#include "mario.h"			//For _R(IDE_NOMORE_MEMORY)
#include "resdll.h"

using namespace InetUtils;


// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(LibClipartSGallery, LibraryGallery)
CC_IMPLEMENT_DYNCREATE(OpDisplayLibClipartGallery, Operation)
CC_IMPLEMENT_DYNCREATE(SGClipartItem, SGLibDisplayItem)
CC_IMPLEMENT_DYNCREATE(GalleryClipartDragInfo, BitmapDragInformation)
CC_IMPLEMENT_DYNAMIC(SGClipartDragTarget, SGListDragTarget)
CC_IMPLEMENT_DYNAMIC(SGOilClipartDragTarget, WinoilDragTarget)
CC_IMPLEMENT_DYNCREATE(OpLibGalClipTheme, OpDescriptor);

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

/*
//Static Variables
#ifdef _DEBUG
//String_256 LibClipartSGallery::DefaultLibraryPath = TEXT("\\\\deepthought\\camelotcd\\cd\\clipart");
String_256 LibClipartSGallery::DefaultLibraryPath = TEXT("\\\\jimpc\\corelxra\\clipart");
#else
String_256 LibClipartSGallery::DefaultLibraryPath = TEXT("D:\\clipart");
#endif
*/


// Matt - 03/01/2001
// New statics - we need to remember two directories (one for clipart, one for web themes) + I don't see the need for a debug
// special version of these statics as \\jimpc won't have a '\themes' subdirectory anyway...

// Statics
String_256 LibClipartSGallery::ClipartPath = TEXT("D:\\clipart");
String_256 LibClipartSGallery::WebThemePath = TEXT("D:\\themes");
String_256 LibClipartSGallery::DefaultLibraryPath = ClipartPath;

// For keeping the display modes constant when next loaded
INT32 LibClipartSGallery::DefaultDisplayMode = 0;	// WEBSTER - markn 7/2/97 (changed default display mode)


// For keeping the sort keys constant when next loaded (default == sort alphabetically)
// 1st sort key = DefaultSortKeys & 0x8f
// 2nd sort key = ((DefaultSortKeys>>8) & 0x8f)
// 1st sort key reversed = ((DefaultSortKeys>>7) & 0x01)==1
// 2nd sort key reversed = ((DefaultSortKeys>>15) & 0x01)==1
// So 0 means no sorting at all
// and 1 means sort the gallery alphabetically
UINT32 LibClipartSGallery::DefaultSortKeys = 1;

// Batching system will use this directory to put its output in
String_256 LibClipartSGallery::BatchSaveLocation = "";

BOOL LibClipartSGallery::DoSounds = TRUE;

// Should always be pointing at a clipart gallery
LibClipartSGallery *LibClipartSGallery::ThisGallery = NULL;

LibClipartSGallery* LibClipartSGallery::m_pInstance = 0;

// Quiet status of gallery
BOOL LibClipartSGallery::QuietStatus = FALSE;



//#define _PROFILE_REDRAW

/********************************************************************************************

>	LibClipartSGallery::LibClipartSGallery()
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Purpose:	LibClipartSGallery default constructor

********************************************************************************************/

LibClipartSGallery::LibClipartSGallery()
{
	LibClipartSGallery::ThisGallery = this;
	m_bDiscardWebFolders = FALSE;
	// Default gallery size
	CSize Size(((333 * 2) - 32) - 32, 256);
	SetGallerySize(Size);
	m_pInstance = this;
	m_nClipTheme = 0;
	m_bRedraw = false;
}



/********************************************************************************************

>	LibClipartSGallery::~LibClipartSGallery()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Purpose:	LibClipartSGallery destructor.

********************************************************************************************/

LibClipartSGallery::~LibClipartSGallery()
{
	TRACEUSER( "Matt", _T("~LibClipartSGallery\n"));

	OpenLibFiles.DeleteAll();		// Ensure all open libraries are closed
	LibClipartSGallery::ThisGallery = NULL;
	m_pInstance = 0;
}


/********************************************************************************************
>	static LibClipartSGallery* LibClipartSGallery::Instance()

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/2000
	Returns:	The current LibClipartSGallery object.
********************************************************************************************/

LibClipartSGallery* LibClipartSGallery::Instance()
{
	ERROR3IF(m_pInstance == 0, "LibClipartSGallery::Instance: no gallery");
	return m_pInstance;
}



/********************************************************************************************

>	static BOOL LibClipartSGallery::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95

	Returns:	TRUE if the sgallery initialised successfully
				FALSE if it failed to initialise
	Purpose:

********************************************************************************************/

BOOL LibClipartSGallery::Init(void)
{
	TRACEUSER( "Matt", _T("Init clipart gallery\n"));
#ifdef _BATCHING
	    
	GetApplication()->DeclareSection(TEXT("Batching"), 6);
	GetApplication()->DeclarePref(TEXT("Batching"), TEXT("OutputDirectory"), &LibClipartSGallery::BatchSaveLocation);

#endif

	if (!OpClipartImport::Init())
		return(FALSE);

	if (!OpAsynchClipartImport::Init())
		return(FALSE);


	return(TRUE);
}



/********************************************************************************************

>	BOOL LibClipartSGallery::PreCreate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Returns:	TRUE if the Gallery initialised successfully
				FALSE if it should not be opened due to a failure to initialise

	Purpose:	The LibClipartSGallery PreCreate handler. This overrides the base class
				PreCreate function. It is called at the very beginning of the
				SuperGallery::Create method, before the window has been created.

	Notes:		As this is called before the window is open, it must not attempt to touch
				any of the button gadgets in the window, or force redraws, etc. Also,
				events cannot be passed to the tree, as the tree formatting relies on
				knowing the window size - however, the tree will be reformatted and
				redrawn automatically when the window is opened - this will happen shortly.

********************************************************************************************/

BOOL LibClipartSGallery::PreCreate(void)
{
	TRACEUSER( "Matt", _T("Precreating clipart window\n"));
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
		if(!AddGRMGroups(SGLib_ClipArt))
		{
			String_256 sLoc = ClipartPath;
			String_256 sLoc2 = WebThemePath;
			// Set DefaultLibraryPath to <ExeLocation>\Fills - the user might have installed
			// the fills to his hard disk:
			String_256 sExePath;
			if(CResDll::GetExecutablePath((TCHAR*)sExePath))
			{
				INT32 oldi = GetClipTheme();

				String_256 LibDirName;
				LibDirName.MakeMsg(_R(IDS_LIBRARIES_CLIPART_DIRNAME));
				ClipartPath = sExePath;
				ClipartPath += "\\";
				ClipartPath += LibDirName;

				PathName ThisPath(ClipartPath);
				if(!SGLibOil::FileExists(&ThisPath))	// will also work for paths (not just paths with a filename on the end)
				{
					SetClipTheme(0);
					// Fills not on hard disk. Try the CD location
					if(!ScanForLocation(SGLib_ClipArt, NULL))
					{
						// hard drive and CD location not found
						// put the original path back
						ClipartPath = sLoc;
					}
				}

				String_256 LibDirName2;
				LibDirName2.MakeMsg(_R(IDS_LIBRARIES_WEBTHEMES_DIRNAME));
				WebThemePath = sExePath;
				WebThemePath += "\\";
				WebThemePath += LibDirName2;

				PathName ThisPath2(WebThemePath);
				if(!SGLibOil::FileExists(&ThisPath2))	// will also work for paths (not just paths with a filename on the end)
				{
					SetClipTheme(0);
					// Fills not on hard disk. Try the CD location
					if(!ScanForLocation(SGLib_ClipArt_WebThemes, NULL))
					{
						// hard drive and CD location not found
						// put the original path back
						WebThemePath = sLoc;
					}
				}
				// reset the ClipTheme value
				SetClipTheme(oldi);
			}

			LibraryGallery::AddLibraryGroups(SGLib_ClipArt, &ClipartPath);
			LibraryGallery::AddLibraryGroups(SGLib_ClipArt_WebThemes, &WebThemePath);
		}
	}

	// Use last time's display mode
	DisplayMode = LibClipartSGallery::DefaultDisplayMode;

	return(TRUE);
}



/********************************************************************************************

>	void LibClipartSGallery::SortGallery(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95

	Inputs:		
	Returns:

	Purpose:	Sorts the contents of the gallery in an alphabetical fashion, whilst keeping
				the old sort key status...
	Notes:		
	SeeAlso:

********************************************************************************************/

void LibClipartSGallery::SortGallery(void)
{
	// Matt - 03/01/2001
	// Special case fix ('bodgeus horriblus')- when we are in the clipart gallery we need to remember the default paths for either the web themes or clipart
	// - this method is called just after the defaultlibrarypath is set in the base class, so now's a good time to remember them...
	if (GetClipTheme() == 0)
	{
		LibClipartSGallery::ClipartPath = LibClipartSGallery::DefaultLibraryPath;
	}
	else
	{
		LibClipartSGallery::WebThemePath = LibClipartSGallery::DefaultLibraryPath;
	}

	LibraryGallery::SortGallery();
}


/********************************************************************************************

>	virtual BOOL LibClipartSGallery::ApplyAction(SGActionType Action)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		Action - Indicates what action to apply

	Returns:	TRUE to indicate successful handling of the action, or
				FALSE to indicate failure

	Purpose:	Applies certain conventional gallery actions (usually associated with
				gallery buttons, for new, edit, delete, etc)

	SeeAlso:	SGActionType

********************************************************************************************/

BOOL LibClipartSGallery::ApplyAction(SGActionType Action)
{
	// No display tree? Better forget about it then!
	if (DisplayTree == NULL)
		return(FALSE);

	switch(Action)
	{
		case SGACTION_REDEFINE:
			break;

		case SGACTION_APPLY:
			ImportClipart(TRUE, this);
			break;
		
/*		case SGACTION_APPLYADJUST:
			ImportClipart(FALSE);
			break;*/

		case SGACTION_EDIT:
			// Ignored - libraries are currently read-only
			break;

		case SGACTION_DELETE:
			// Ignored - libraries are currently read-only
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
			LibClipartSGallery::DefaultDisplayMode = DisplayMode;
			InvalidateCachedFormat();
			ReformatAndRedrawIfNecessary();
			//ForceRedrawOfList();
			break;
	
		default:
			return(SuperGallery::ApplyAction(Action));
			break;
	}

	return(TRUE);
}



/********************************************************************************************

>	virtual MsgResult LibClipartSGallery::Message(Msg* Message)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
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

MsgResult LibClipartSGallery::Message(Msg* Message)
{
	// Added by Craig Hamilton 18/1/01.
	static HANDLE	handle = NULL;
	CString			mutexName = "autorunMutex";
	// End added.

	// If we have no displaytree, then we have not been shown, or something terrible has
	// happened, so we don't bother handling any of these messages.
	if (DisplayTree == NULL)
		return(LibraryGallery::Message(Message));

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

				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYCLIPARTGALLERY, TRUE);
				GalleryAboutToReOpen();
				break;

			case DIM_CANCEL:
				// Added by Craig Hamilton 18/1/01.
				if(handle != NULL)
				{
					CloseHandle(handle);
				}
				// End added.

				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYCLIPARTGALLERY, FALSE);

				// Flush the thumbnail cache
				BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::ThumbState::KILLCACHE, SGLib_ClipArt), DialogOp);
				BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::ThumbState::KILLCACHE, SGLib_ClipArt_WebThemes), DialogOp);
				BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::ThumbState::KILLCACHE, SGLib_Bitmap), DialogOp);

				GalleryAboutToClose();
				
				break;

			case DIM_LFT_BN_CLICKED:
				switch (Msg->GadgetID)
				{
					case _R(IDC_LIBGAL_OPEN):
						ImportClipart(TRUE, this);
						break;

					case _R(IDC_GALLERY_HELP):		// Show help page
						HelpUserTopic(_R(IDS_HELPPATH_Gallery_Clipart));
						break;


					case _R(IDC_LIBGAL_UPDATE):
#ifdef _BATCHING     
						LoadAndSave();
#endif
						break;
					
					case _R(IDC_LIBGAL_IMPORT):
						ImportClipart(FALSE, this);
						break;

					case _R(IDC_LIBGAL_BROWSE):
					case _R(IDC_LIBGAL_ADD_CLIPART):
						BrowseClicked();
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
									Error::SetError(0, TEXT("Renumber ID's ? (Doing this will mess external thumbnails up in a major way !)"), 0); // Debug
									INT32 ButtonPressed = InformWarning(0, _R(IDS_OK), _R(IDS_KEEP), _R(IDS_CANCEL), NULL);

									if(ButtonPressed != 3)
									{
										PathName *Source = Parent->ReturnIndexLocation();
										PathName Dest(*Source);
										Dest.SetType((String_256)"BAK");
										SGLibOil::FileCopy(Source, &Dest);
										Parent->SaveIndexInDisplayedOrder(Source, (ButtonPressed == 1));
									}
								}
							}
						}
#endif
						break;
					
					default:
						break;
				}

#ifdef _PROFILE_REDRAW
			case DIM_REDRAW:
				if (Msg->GadgetID == GetListGadgetID())
				{
					ProfileRedraw((ReDrawInfoType*) Msg->DlgMsgParam);
					return(DialogBarOp::Message(Message));
					break;
				}
				break;
#endif
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
					SuperGallery::ShadeGallery(TRUE);
				}
				else
				{
					// Possibly a new document
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

>	BOOL LibClipartSGallery::BrowseClicked(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95

	Inputs:		
	Returns:	TRUE if the gallery has new stuff in it (FALSE if cancel clicked, etc)...

	Purpose:	Pops up the browse box and lets a new location be set for the clipart
				directory

********************************************************************************************/

BOOL LibClipartSGallery::BrowseClicked(void)
{
	if (GetClipTheme() == 0)
	{
		return (LibraryGallery::BrowseClicked(&DefaultLibraryPath, SGLib_ClipArt, _R(IDS_REMOVE_OLD_GOUPS_CLIPART)));
	}
	else
	{
		return (LibraryGallery::BrowseClicked(&DefaultLibraryPath, SGLib_ClipArt_WebThemes, _R(IDS_REMOVE_OLD_GOUPS_CLIPART)));
	}
}


/********************************************************************************************

>	virtual BOOL LibClipartSGallery::CanCreateIndexes(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Returns:	TRUE to if index generation is possible

	Purpose:	To determine if this gallery can generate indexes or not

********************************************************************************************/

BOOL LibClipartSGallery::CanCreateIndexes(void)
{
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL LibClipartSGallery::GetDefaults(String_256 *DefaultIndex, String_256 *IndexDesc, SGLibType *Type)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Outputs:	DefaultIndex	- The filename for the default index file (Xaraclip.txt)
				IndexDesc		- Description of the index / gallery (Clipart)
				Type			- Default library type associated with this gallery

	Returns:	TRUE if this was overridden successfully 

	Purpose:	To determine various library gallery default properties

********************************************************************************************/

BOOL LibClipartSGallery::GetDefaults(String_256 *DefaultIndex, String_256 *IndexDesc, SGLibType *Type)
{
	TRACEUSER( "Matt", _T("Getting defaults for clipart gallery\n"));
	if(DefaultIndex != NULL)
	{
		//Set the correct info filename for the current gallery style
		if (GetClipTheme() == 0)
		{
			*DefaultIndex = _R(IDS_LIBRARIES_CLIPART_FILENAME);	// "XaraClip.txt";
		}
		else
		{
			*DefaultIndex = _R(IDS_LIBRARIES_WEBTHEMES_FILENAME);	// "XaraThem.txt";
		}
	}

	if(IndexDesc != NULL)
		*IndexDesc = _R(IDS_LIBRARIES_CLIPART_DESC);		// "Clipart";

	if(Type != NULL)
	{
		//Set the type of the gallery to be 'Clipart' or 'Web Themes'
		//so that the base classes attempt to load file info from the
		//correct URL - real handy...
		if (GetClipTheme() == 0)
		{
			*Type = SGLib_ClipArt;
		}
		else
		{
			*Type = SGLib_ClipArt_WebThemes;
		}
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL LibClipartSGallery::GetLibraryDirectoryName(String_256 *LibDirName)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95

	Outputs:	LibDirName	- Returns the Default directory (on the clipart CD) for the gallery
	Returns:	TRUE if overridden and directory obtained...

	Purpose:	Get the default CD directory name for the gallery

********************************************************************************************/

BOOL LibClipartSGallery::GetLibraryDirectoryName(String_256 *LibDirName)
{
	TRACEUSER( "Matt", _T("Getting library directory name for clipart gallery\n"));
	if (GetClipTheme() == 0)
	{
		LibDirName->MakeMsg(_R(IDS_LIBRARIES_CLIPART_DIRNAME));
	}
	else
	{
		LibDirName->MakeMsg(_R(IDS_LIBRARIES_WEBTHEMES_DIRNAME));
	}
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL LibClipartSGallery::CheckForIndexMatch(StringBase *Txt)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Inputs:		Txt		- Last column entry in the index.txt file (" A")
	Returns:	TRUE if this signifies the gallery in question...

	Purpose:	To see whether we should add this line of the index.txt file to this gallery

********************************************************************************************/

BOOL LibClipartSGallery::CheckForIndexMatch(StringBase *Txt)
{
	BOOL Match = FALSE;

	// Artwork, and Bitmaps...
	if(((Txt->Sub(String_8("A"))!=-1) || (Txt->Sub(String_8("a"))!=-1)) ) Match = TRUE;
	if(((Txt->Sub(String_8("B"))!=-1) || (Txt->Sub(String_8("b"))!=-1)) ) Match = TRUE;				

	return Match;
}


/********************************************************************************************

>	virtual BOOL LibClipartSGallery::GetQuietStatus(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Returns:	TRUE if Quiet has been pressed (SetQuiet status called with TRUE)
	Purpose:	Get the Quiet status of the gallery

********************************************************************************************/

BOOL LibClipartSGallery::GetQuietStatus(void)
{
	return LibClipartSGallery::QuietStatus;
}

/********************************************************************************************

>	virtual void LibClipartSGallery::SetQuietStatus(BOOL Status)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Inputs:		Status - Set to TRUE and call if Quiet has just been pressed on an 'adding
						 indexes' dialog
	Purpose:	Set the Quiet status of the gallery

********************************************************************************************/
		
void LibClipartSGallery::SetQuietStatus(BOOL Status)
{
	LibClipartSGallery::QuietStatus = Status;
}


/***********************************************************************************************

>	virtual BOOL LibClipartSGallery::ScanForLocation(SGLibType Type, StringBase *Result = NULL);

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

BOOL LibClipartSGallery::ScanForLocation(SGLibType Type, StringBase *Result)
{
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
			case SGLib_ClipArt:
			case SGLib_ClipArt_WebThemes:
			case SGLib_Bitmap:
			{
				LibClipartSGallery::DefaultLibraryPath = DriveName;
#ifdef _DEBUG
				if(Adjust)
				{
					LibClipartSGallery::DefaultLibraryPath += TEXT("Graphics\\XaraSt~1");
				}
				else
#endif
				{
					String_256 LibDirName;
					GetLibraryDirectoryName(&LibDirName);
					LibClipartSGallery::DefaultLibraryPath += LibDirName;
				}

				if(Result)
					*Result = LibClipartSGallery::DefaultLibraryPath;

				if (GetClipTheme() == 0)
				{
					LibClipartSGallery::ClipartPath = LibClipartSGallery::DefaultLibraryPath;
				}
				else
				{
					LibClipartSGallery::WebThemePath = LibClipartSGallery::DefaultLibraryPath;
				}

				return TRUE;
			}

		}
	}
	return FALSE;
}



/********************************************************************************************

>	virtual void LibClipartSGallery::HandleDragStart(DragMessage *DragMsg)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95

	Inputs:		DragMsg - The DRAGSTARTED message that we've just recieved, indicating
				the type of drag being started

	Purpose:	Checks a DragMessage to see if it is a colour drag.
				If it is, then it creates a drag target for this gallerys listbox.

	Notes:		Overrides the default base-class action. Calls down to the base class
				if it is not a colour drag, so that dragging of gallery groups is allowed

********************************************************************************************/

void LibClipartSGallery::HandleDragStart(DragMessage *DragMsg)
{
	// If it's a bitmap drag, add a target for our window. If not, let the base class
	// have a look at it (to see if it is a gallery item being dragged)
	if (DragMsg->pInfo->IsKindOf(CC_RUNTIME_CLASS(GalleryClipartDragInfo)))
		SGClipartDragTarget *NewTarget = new SGClipartDragTarget(this, GetListGadgetID());
	else
		SuperGallery::HandleDragStart(DragMsg);
}


/* for profiling reasons */

void LibClipartSGallery::ProfileRedraw(ReDrawInfoType *Param)
{
#ifdef _PROFILE_REDRAW
	RenderListBox(Param);
#endif
}

/********************************************************************************************

>	BOOL LibClipartSGallery::ImportClipart(BOOL NewDocument)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
					Asynchronous import code added by Adrian, 12/96
	Inputs:		NewDocument - Set to TRUE if you don't want the import to be added ontopof
				the currently selected document.
	Outputs:
	Returns:	TRUE if things went OK, false otherwise

	Purpose:	To load/import all the selected clipart items one by one into camelot.				
	Notes:
	SeeAlso:

*********************************************************************************************/

BOOL LibClipartSGallery::ImportClipart(BOOL NewDocument, LibraryGallery* pSourceGallery)
{
	TRACEUSER( "Matt", _T("Importing clipart in clipart gallery\n"));
	if (pSourceGallery->DisplayTree == NULL)
	{
		ERROR3("LibClipartSGallery::ImportClipart No display tree - bad !");
		return FALSE;
	}

	if(Document::GetSelected() == NULL && !NewDocument)
	{
		return FALSE;
	}
									  
	BOOL ok = TRUE;


	// Find the first selected item (if any) and get the next item to start searching from
	// If there is no selection, then get the first item
	SGDisplayNode *Item = pSourceGallery->DisplayTree->FindNextSelectedItem(NULL);
	String_256 ItemText;

	while (Item != NULL)
	{
		if(Item->IsKindOf(CC_RUNTIME_CLASS(SGClipartItem)) || Item->IsKindOf(CC_RUNTIME_CLASS(SGFillsItem)))
		{
			Item->GetNameText(&ItemText);

			if(Item->Flags.Selected)
			{
				PathName FileName;		
				SGLibDisplayItem *ClipItem = (SGLibDisplayItem *) Item;
			
				//if (Lib != NULL)
				ok = ClipItem->GetFileName(&FileName);

				if(ok)
				{
					// Bodge for clipart CD - 'Cow' in LScape should point to "Moo.tif", not just "Moo"
					if(FileName.GetFileName(FALSE) == (String_256)"Moo")
					{
						INT32 Size = ClipItem->GetFileSize(NULL);
						if(Size == 1080904)
							FileName.SetType("TIF");
					}

					if(!FileName.IsValid(FileName.GetPath()))
					{
						ERROR3("LibClipartSGallery::ImportClipart - Filename is invalid");
						return FALSE;
					}

					// Check if file has a long filename, and use that if it does...
					String_256 FilePath((const TCHAR *)FileName.GetPath());
					String_256 LongFileName;
					BOOL LongPath = FileUtil::GetLongFileName((LPTSTR)FilePath, (LPTSTR)LongFileName, 255);
					if(LongPath)
					{
						FilePath = FileName.GetLocation(TRUE);
						FilePath += LongFileName;
					}

					// Check if the file really exists on the local drive
					Library* pLibrary = ClipItem->GetParentLibrary();
					if (!pLibrary)
					{
						ERROR3("Illegal NULL pointer");
						return FALSE;
					}
					BOOL bIsLocalFile;
					if (!pLibrary->IsWebLibrary())
						bIsLocalFile = TRUE;
					else 
						bIsLocalFile = (_access((TCHAR*) FilePath, 0) != -1);
						
					if(NewDocument)
					{
						if (bIsLocalFile)
						{
							CWinApp* pApp = AfxGetApp();
							BaseFileDialog::SelectedFilter = 0;
			
							// Open a document
							CCamDoc* pDoc = (CCamDoc*) pApp->OpenDocumentFile((TCHAR *)FilePath);

							// And redraw the imported document
							if(pDoc) pDoc->GetKernelDoc()->ForceRedraw();
						}
						else // do an asynch import
						{
							goto ASYNCH_IMPORT;
						}
					}
					else
					{
						// Invoke the clipart import operation so that we get Undo.
						if (bIsLocalFile) // if the file is local we do a normal import Op
						{

							OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpClipartImport));

							if (pOpDesc != NULL)
							{
								ClipartImportParam Param;

								PathName FullPathName(FilePath);

								// Set up the parameters which we require to do the import operation
								Param.File = &FullPathName;
								Param.Import = !NewDocument;
								Param.Result = TRUE;

								// Import the specified file
								pOpDesc->Invoke((OpParam *) &Param);

								ok = Param.Result;
								if(!ok)
								{
									ERROR3("LibClipartSGallery::ImportClipart - Problem importing file");
									return FALSE;
								}
							}
						}
						else // try an asynch import
						{
ASYNCH_IMPORT:
							String_256 strFileURL = FilePath;
							pLibrary->LocalPath2URL(&strFileURL);
							OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpAsynchClipartImport));

							if (pOpDesc != NULL)
							{
								AsynchClipartImportParam* Param = new AsynchClipartImportParam;
								ERROR2IF(!Param, FALSE, "Memory allocation error");
								// Set up the parameters which we require to import the clipart
								Param->File.SetPathName((TCHAR *)FilePath);
								String_256 strDescription;
								ClipItem->GetNameText(&strDescription);
								Param->strDescription = _T("'");
								Param->strDescription += strDescription;
								Param->strDescription += _T("'");
								Param->strURL = strFileURL;
								Param->Import = !NewDocument;
								Param->bDropped = FALSE;
								// Invoke the operation
								pOpDesc->Invoke((OpParam*) Param);
							}
						}
					}
				}
			}
		}
		Item = SGDisplayRoot::FindNextItemInTree(Item);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL LibClipartSGallery::LoadAndSave(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Inputs:
	Outputs:
	Returns:	TRUE if things went OK, false otherwise

	Purpose:	To load all the selected clipart items one by one into camelot and then
				to save them out again - useful for updating documents.
				
	Notes:		Code for this should only be compiled in DEBUG, and calling this is
				not guaranteed to leave XS in a stable state on exit.
	SeeAlso:

********************************************************************************************/

BOOL LibClipartSGallery::LoadAndSave(void)
{

#ifdef _BATCHING

	if (DisplayTree == NULL)
	{
		ERROR3("LibClipartSGallery::LoadAndSave No display tree - bad !");
		return FALSE;
	}

	// For centering items...
	PageTab PTab;
	if(!PTab.Init())
	{
		ERROR3("Can't init page resizing stuff...");
		return FALSE;
	}

	// Find the first selected item (if any) and get the next item to start searching from
	// If there is no selection, then get the first item
	SGDisplayNode *Item = DisplayTree->FindNextSelectedItem(NULL);

	String_256 ItemText;

	while (Item != NULL)
	{
		if(Item->IsKindOf(CC_RUNTIME_CLASS(SGClipartItem)))
		{
			Item->GetNameText(&ItemText);

			// Update selected ones, or all after first selected ?
			if(Item->Flags.Selected || TRUE)
			{
				PathName FileName;		
				SGLibDisplayItem *ClipItem = (SGLibDisplayItem *) Item;

				BOOL ok = TRUE;
			
				//if (Lib != NULL)
				ok = ClipItem->GetFileName(&FileName);

				// Check if we're an ai file, don't do it if we're not !
				String_8 Ending;
				if(ok) Ending = FileName.GetType();
				if(ok) Ending.toLower();
				if(/*Ending == (String_8)"art" &&*/ ok)
				{
					// Load a file
					CWinApp* pApp = AfxGetApp();
					BaseFileDialog::SelectedFilter = 0;

					if(!FileName.IsValid(FileName.GetPath()))
					{
						ERROR3("Filename is invalid");
					}

					String_256 Path(FileName.GetPath());
					
					// Batching system will use this directory to put its output in
					String_256 OutputPath(LibClipartSGallery::BatchSaveLocation);
					
					// Unless it == "" of course...
					if(OutputPath.Length() == 0)
						OutputPath = Path;
					else
					{
						if(OutputPath[OutputPath.Length()-1] != '\\')
							OutputPath += TEXT("\\");
						OutputPath += FileName.GetFileName(TRUE);
					}

					PathName OutputPathART(OutputPath);
					OutputPathART.SetType((String_256)"XAR");
					String_256 SavePath(OutputPathART.GetPath());

		 			Error::ClearError();
	
					// Only do it if there isn't an art file there already...
				//	if(!SGLibOil::FileExists(&OutputPathART))
				//	{			
				
						MonotonicTime Time;
						INT32 OpenTime = Time.Sample();
						INT32 InitialSize = SGLibOil::FileSize(&FileName);

// If this is not defined, we just load each file in turn, and spool any problems to 'load.txt'
//#define SAVEASWELL

#ifndef SAVEASWELL
						FILE *FP = _tfopen("C:\\load.txt", "a"); // Debug
						if(FP != NULL)
						{
							_ftprintf(FP, "Loading: %s\r\n", (TCHAR *)Path); // Debug
							fclose(FP);
						}
#endif

						// Open a document
						CCamDoc* pDoc = (CCamDoc*) pApp->OpenDocumentFile((TCHAR *)Path);
						/*Document *CurDoc = Document::GetSelected();
							CCamDoc* pDoc = CurDoc->GetOilDoc();*/
#ifndef SAVEASWELL

						if(pDoc == NULL)
						{
							FP = _tfopen("C:\\batch.txt", "a"); // Debug
							if(FP != NULL)
							{
								_ftprintf(FP, "Problems: %s\r\n", (TCHAR *)Path); // Debug
								fclose(FP);
							}
						}
						else
						{
				 			Error::ClearError();
							// Close a document
				 		 	((CCamDoc *) (((CMDIFrameWnd *)                                      
				 				((AfxGetApp())->m_pMainWnd))->MDIGetActive()	//Get Active Child Window
			 					)->GetActiveDocument()							//Get Active Document
								)->OnFileClose();								//Close Active Document
						}	
			 			Error::ClearError();

#else
						OpenTime = Time.Sample() - OpenTime;

						if (pDoc != NULL)
						{
							INT32 Pass = 0;

							/*do {*/
								/*******************/

								// Remove empty layers and rename clipart layer to 'Clipart'

#if 0
								Document* OurDocument = pDoc->GetKernelDoc();
								Spread* OurSpread = OurDocument->FindFirstSpread();
								Page* CurrentPage = OurSpread->FindFirstPageInSpread(); 

								if(OurDocument == NULL || OurSpread == NULL || CurrentPage == NULL)
								{
									Error::SetError(0, String_256("NULL Page/Spread/Doc in batcher..."), 0); // Debug
									InformWarning(0, _R(IDS_OK), NULL);
									Error::ClearError();
								}
								else
								{
									OurDocument->SetCurrent();

									/* remove messed up layers from tree */
									Layer* pLayer = OurSpread->FindFirstLayer();

									INT32 Count = 0;
									while(pLayer)
									{
										Node* pNode = pLayer->FindFirstChild();
										BOOL ChildFound = FALSE;

										while (pNode != NULL && !ChildFound)
										{
											ChildFound = pNode->IsBounded(); //IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded));
											pNode = pNode->FindNext();
										}

										Layer *tLayer = pLayer->FindNextLayer();

										if(!ChildFound)
											pLayer->UnlinkNodeFromTree();
										else
										{
											pLayer->SetVisible(TRUE);
											pLayer->SetLocked(FALSE);
											Count ++;
										}

										pLayer = tLayer;
									}


									if(Count == 1)
									{
										Layer* pLayer = OurSpread->FindFirstLayer();
										if(pLayer)
											pLayer->SetLayerID(String_256("Clipart")); // Debug
									}


									/**************************************/

									// Sort the page orientation out, centre the clipart and zoom to it
		
									DocRect	OurItemRect = OurSpread->GetBoundingRect();
									DocRect OurPageRect = CurrentPage->GetPageRect();

									if (AppPrefsDlg::pPageSizesList != NULL)
									{
										PageId PageChosen = A4;
										MILLIPOINT Width;
										MILLIPOINT Height;
										AppPrefsDlg::pPageSizesList->GetSizeFromPageId(PageChosen, &Width, &Height);

										// Bodge it to landscape / portrait...
										if(    ((OurItemRect.Width() > OurItemRect.Height()) && (OurPageRect.Width() < OurPageRect.Height())) 
											|| ((OurItemRect.Width() < OurItemRect.Height()) && (OurPageRect.Width() > OurPageRect.Height())) )
										{
											MILLIPOINT Tmp = Width;
											Width = Height;
											Height = Tmp;
										}

										TRACEUSER( "Richard", _T("width %d height %d in MP\n"),Width,Height);

										// Set up the parameters which we require to do the page resizing operation 
										PageResizeInfo Param;

										// Set up the new page parameters that we want
										Param.Height = Height;
										Param.Width = Width;

										MILLIPOINT Margin = Width / 2;
										if(Height < Width)
											Margin = Height / 2;

										Param.Margin = Margin;
										Param.Bleed = 0; //Bleed;
										Param.Portrait = (Width < Height); //Portrait;
										Param.Dps = FALSE; //Dps;
										Param.ShowDropShadow = TRUE; //DropShadow;
										// Set up the new default grid settings that we want 
										//Param.GridSpacing = CM_MP_VAL;
										//Param.GridSubDivisions = 2;
										//Param.GridUnits = CENTIMETRES;
										//Param.TypeOfGrid = RECTANGULAR;

										Param.pSpread = OurSpread;
										OptionsTabs::SetCurrentDocAndSpread(OurDocument); // set up the spread that we are currently workign on

										// Invoke the page centre / resize function...
										PTab.InvokeResizeAndCentre(&Param);

										// BODGE BODGE BODGE to make fakeinvoke work when the spread's not in view...
										LibClipartSGallery::DefaultSortKeys = TRUE;

										//OpZoomDescriptor::FakeInvoke(OPTOKEN_ZOOMSPREAD);
										OpZoomDescriptor::FakeInvoke(OPTOKEN_ZOOMDRAWING);

										LibClipartSGallery::DefaultSortKeys = FALSE;

										OurDocument->ForceRedraw();
									}
								}
/*								Pass ++;
							} while(Pass < 5);*/

							/*Document**/ OurDocument = pDoc->GetKernelDoc();
							/*Spread**/ OurSpread = OurDocument->FindFirstSpread();
							/*Page* */ CurrentPage = OurSpread->FindFirstPageInSpread(); 

							DocRect	OurItemRect = OurSpread->GetBoundingRect();
							DocRect OurPageRect = CurrentPage->GetPageRect();

							BOOL PossiblyOutsidePage = FALSE;
							/*if(    (OurItemRect.lo.x < OurPageRect.lo.x)
								|| (OurItemRect.hi.x > OurPageRect.hi.x)
								|| (OurItemRect.lo.y < OurPageRect.lo.y)
								|| (OurItemRect.hi.y > OurPageRect.hi.y) )
								PossiblyOutsidePage = TRUE;*/

							/*******************/

							// Remove duplicate colours

							Document *TheDoc = OurDocument;

							ColourList *ParentList = TheDoc->GetIndexedColours();

							INT32 TotalItems = ParentList->GetCount();
							INT32 DeletedItems = 0;

							IndexedColour **KillList = new IndexedColourPtr[TotalItems+1];
							if (KillList != NULL)
							{

								DWORD KillIndex = 0;
								IndexedColour *Ptr = ParentList->GetUndeletedHead();

								while (Ptr != NULL)
								{
									if(!Ptr->IsInUse())
									{
										DeletedItems++;
										KillList[KillIndex++] = Ptr;
									}

									Ptr = ParentList->GetUndeletedNext(Ptr);
								}
	
								KillList[KillIndex] = NULL;		// NULL terminate the list

								// Delete (hide, with undo actually) the given colours
								ColourManager::HideColours(ParentList, KillList, TRUE);

								// HideColours has made a copy of this list for itself, so we are responsible
								// for deleting our original list
								delete [] KillList;
							}

							// Only add a new palette if there are now no colours...
							if(TotalItems == DeletedItems)
							{
								/*******************/

								// Import a blank document containing the 'default' colours

								PathName FileName((String_256)"C:\\pal.art");
								if(SGLibOil::FileExists(&FileName))
								{
									// Invoke the clipart import operation so that we get Undo.
									OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpClipartImport));

									if (pOpDesc != NULL)
									{
										ClipartImportParam Param;

										// Set up the parameters which we require to do the page resizing operation 
										Param.File = &FileName;
										Param.Import = TRUE;
										Param.Result = TRUE;

										// Go for it!
										pOpDesc->Invoke((OpParam *) &Param);

										ok = Param.Result;

										if(!ok)
										{
											ERROR3("Batching... Problem importing file");
											return FALSE;
										}
									}
								}

								/*******************/
							}
	
#endif
							INT32 CloseTime = Time.Sample();

							BOOL Worked = TRUE;
#if 0	//#ifndef NEW_NATIVE_FILTER
		// Save a document
							Worked = pDoc->OnSaveDocument((TCHAR *)SavePath) && Worked;
				 			Error::ClearError();

							INT32 FinalSize = SGLibOil::FileSize(&OutputPathART);
							CloseTime = Time.Sample() - CloseTime;
#endif
#if 0	// Save a document, but ensure it is in the old format!
							Worked = pDoc->OnSaveVersion1Document((TCHAR *)SavePath) && Worked;
				 			Error::ClearError();

							INT32 FinalSize = SGLibOil::FileSize(&OutputPathART);
							CloseTime = Time.Sample() - CloseTime;

#endif

							PathName path(SavePath);

#if 0	// Do an Export of the file as a version 2 Native file
							path.SetType(TEXT("CXN"));
							path.SetType(TEXT("xar"));	// new files have this extension now
							CCDiskFile file(path, ios::out | ios::binary | ios::trunc);
							BOOL NativeExportWorked = InvokeNativeFileOp(OPTOKEN_SAVEASNATIVEV2, &file, FILTERID_NATIVE);
							Error::ClearError();
							Worked = NativeExportWorked && Worked;
#endif
#if 0	// Do an Export of the file as a Web file
							path.SetType(TEXT("web"));
							CCDiskFile webfile(path, ios::out | ios::binary | ios::trunc);
							BOOL WebExportWorked = InvokeNativeFileOp(OPTOKEN_NATIVESAVE, &webfile, FILTERID_WEB);
							Error::ClearError();
							Worked = WebExportWorked && Worked;
#endif
#if 1	// Rename the file as a munged 8.3 file if saving in the same place - none of this is wheat germ and shouldn't
		// be in the kernel, however it only apears in _BATCHING debug builds
							if (LibClipartSGallery::BatchSaveLocation.Length() == 0)
							{
								// To perform the rename is a bit tricky 'cos we don't know the long filename,
								// but we can reference the original file through its short name, renaming it
								// to a temporary name then renaming the temporary back to the short name. We
								// can not simply rename the long to the short 'cos the operating system will
								// complain that the short name (i.e. the long one) already exists.
								BOOL RenameWorked = FALSE;
								String_256 TempName = FileName.GetLocation();	// temp file in this location
								// create a temp filename with an obvious prefix
								char * temmmmmp = _tempnam( TempName, TEXT("~XARA") );
								if( temmmmmp != NULL )	// pointer returned by _tempnam
								{
									// copy the name we created
									TempName = temmmmmp;
									// free memory allocated by _tempnam
									free( temmmmmp );
									temmmmmp = NULL;
									// rename our original by referencing it through the munged name
									if ( 0 == rename( FileName.GetPath(), TempName ) )
									{
										// THIS DOESN'T WORK just get original name back
										// rename the temp file back to the munged (original) name
										RenameWorked = !rename( TempName, FileName.GetPath() );
									}
								}
								Worked = RenameWorked && Worked;
							}
							else
							{
								// we are not saving in the same place so save as a web file like above
								path.SetType(TEXT("web"));
								CCDiskFile webfile(path, ios::out | ios::binary | ios::trunc);
								BOOL WebExportWorked = InvokeNativeFileOp(OPTOKEN_NATIVESAVE, &webfile, FILTERID_WEB);
								Error::ClearError();
								Worked = WebExportWorked && Worked;
							}
#endif
#if 1	// Do an Export of the file as a png thumbnail file to the "XaraInfo" directory

							Document* OurDocument = pDoc->GetKernelDoc();
							Spread* OurSpread = OurDocument->FindFirstSpread();
							Layer* pLayer = OurSpread->FindFirstLayer();
							DocRect Bounds;
							while (pLayer != NULL)
							{
								Bounds = Bounds.Union(pLayer->GetBoundingRect());
								pLayer = pLayer->FindNextLayer();
							}		

							INT32 d = Bounds.Width();
							if (Bounds.Height() > d)
								d = Bounds.Height();

							if (d < (72000*2))
							{
								CCamDoc* pCCamDoc = OurDocument->GetOilDoc();
								pCCamDoc->ScaleContents(5.0,5.0);
							}

							
							String_256 ThumbPathStr = path.GetLocation();
							ThumbPathStr += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
							ThumbPathStr += TEXT("\\");
							ThumbPathStr += path.GetFileName();

							path = PathName(ThumbPathStr);

							path.SetType(TEXT("png"));
							CCDiskFile thumbfile(path, ios::out | ios::binary | ios::trunc);
							BOOL ThumbExportWorked = InvokeNativeFileOp(OPTOKEN_NATIVESAVE, &thumbfile, FILTERID_PREVIEW_PNG);
				 			Error::ClearError();
							Worked = ThumbExportWorked && Worked;
#endif


							// Close a document
							if(Worked)
					 		 	((CCamDoc *) (((CMDIFrameWnd *)                                      
					 				((AfxGetApp())->m_pMainWnd))->MDIGetActive()	//Get Active Child Window
				 					)->GetActiveDocument()							//Get Active Document
									)->OnFileClose();								//Close Active Document
	
				 			Error::ClearError();
							//ERROR3IF(!Worked, "Ok, so it didn't work...");

							/*FILE *FP = _tfopen("C:\\times.txt", "a");
							if(FP != NULL)
							{
								_ftprintf(FP, "%s\tLoad - %d\tSave - %d\tSize - %d to %d (%d%%)\r\n", (TCHAR *)SavePath, OpenTime, CloseTime, InitialSize, FinalSize, (FinalSize / InitialSize)*100);
								fclose(FP);
							} */

							if(!Worked)
							{
								FILE *FP = _tfopen("C:\\batch.txt", "a"); // Debug
								if(FP != NULL)
								{
									_ftprintf(FP, "Problems  saving: %s\r\n", (TCHAR *)Path); // Debug
									fclose(FP);
								}
							}
							else
							{
#if 0
								if(PossiblyOutsidePage)
								{
									FILE *FP = _tfopen("C:\\batch.txt", "a"); // Debug
									if(FP != NULL)
									{
										_ftprintf(FP, "Possibly outside: %s\r\n", (TCHAR *)Path); // Debug
										fclose(FP);
									}
								}
#endif
							}
						}
						else
						{
							FILE *FP = _tfopen("C:\\batch.txt", "a"); // Debug
							if(FP != NULL)
							{
								_ftprintf(FP, "Problems loading: %s\r\n", (TCHAR *)Path); // Debug
								fclose(FP);
							}
						}
#endif  //SAVEASWELL
				//	}
				}
			}

		}

		Item = SGDisplayRoot::FindNextItemInTree(Item);
	}

#endif
	return TRUE;
}


/********************************************************************************************

>	virtual SGDisplayItem *LibClipartSGallery::AddLibraryItem(SGDisplayGroup *LibraryGroup,
												Library *ParentLib,
												LibraryIndex ItemIndex)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/95

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
				in the library (e.g. a clipart library gallery will have to create items
				that display clipart thumbnails)

	SeeAlso:	SuperGallery::AddLibraryGroup; SuperGallery::RemoveLibraryGroup

********************************************************************************************/

SGDisplayItem *LibClipartSGallery::AddLibraryItem(SGDisplayGroup *LibraryGroup, Library *ParentLib, LibraryIndex ItemIndex, BOOL bNew)
{
	TRACEUSER( "Matt", _T("Add library item called in clipart gallery\n"));
	ERROR3IF(LibraryGroup == NULL || ParentLib == NULL,
				"SuperGallery::AddLibraryItem - NULL params are illegal");

	ERROR3IF(LibraryGroup->GetParentLibrary() != ParentLib,
				"SuperGallery::AddLibraryitem - The DisplayGroup is not for the same library!");

	// Create a clipart library item

	SGClipartItem *NewItem = new SGClipartItem(ItemIndex, bNew);

	// Check for, and don't include if found in the Studio group, the line:
	// Rolex.art,Rather expensive watch,36,Rolex,Object|Time|Watch|Machine,113852
	if((ParentLib->Title != NULL) && (ParentLib->Title->Sub((String_8)"Studio") == 0))
	{
		String_256 TmpPath;
		BOOL ok = ParentLib->GetFilename(ItemIndex, &TmpPath, FALSE);

		if(ok)
		{
			INT32 Size = NewItem->GetFileSize(ParentLib);
			if(Size == 113852)
			{
				String_256 TmpPath;
				BOOL ok = ParentLib->GetFilename(ItemIndex, &TmpPath, FALSE);

				if(TmpPath == (String_16)"Rolex.art")
				{
					delete NewItem;
					NewItem = FALSE;
				}
			}
		}
	}

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

>	void LibClipartSGallery::ModeHasChanged(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/09/2000

	Purpose:	To toggle the visible/selectable items of this gallery between
				clipart and web themes
	Notes:

********************************************************************************************/

void LibClipartSGallery::ModeHasChanged(void)
{
	TRACEUSER( "Matt", _T("MODE has changed called in clipart gallery\n"));
	if (DisplayTree == NULL)
	{
		return;
	}

	INT32 libindex = 0;

	//Search through our constructed library of clipart and themes...
	LibraryFile* pLibFile = (LibraryFile*) OpenLibFiles.GetHead();
	while (pLibFile)
	{
		Library* pLib = (Library*) pLibFile->Libraries.GetHead();

		while (pLib)
		{
			if ((pLib->Type == SGLib_ClipArt) && (GetClipTheme() != 0))
			{
				pLib->ParentGroup->Flags.Invisible = true;
				pLib->ParentGroup->Flags.CanSelect = false;
				pLib->ParentGroup->Flags.Selected = false;
				pLib->ParentGroup->Flags.Folded = true;
			}
			else if ((pLib->Type == SGLib_ClipArt_WebThemes) && (GetClipTheme() != 1))
			{
				pLib->ParentGroup->Flags.Invisible = true;
				pLib->ParentGroup->Flags.CanSelect = false;
				pLib->ParentGroup->Flags.Selected = false;
				pLib->ParentGroup->Flags.Folded = true;
			}
			else
			{
				pLib->ParentGroup->Flags.Invisible = false;
				pLib->ParentGroup->Flags.CanSelect = true;
			}

			pLib = (Library*) pLibFile->Libraries.GetNext(pLib);
		}

		pLibFile = (LibraryFile*) OpenLibFiles.GetNext(pLibFile);
	}

	ReformatNow(true);
}



/********************************************************************************************

>	void LibClipartSGallery::SelectionHasChanged(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> (based on Jason code)
	Created:	28/3/95

	Purpose:	To inform the gallery that the selection has changed in some way.
				We need to grey different buttons on different occasions
	Notes:

********************************************************************************************/

void LibClipartSGallery::SelectionHasChanged(void)
{
	TRACEUSER( "Matt", _T("Selection has changed called in clipart gallery\n"));
	if (DisplayTree == NULL)
		return;

	INT32 Count = DisplayTree->GetSelectedItemCount();

	EnableGadget(_R(IDC_LIBGAL_OPEN), (Count > 0));
#ifdef _BATCHING
	EnableGadget(_R(IDC_LIBGAL_UPDATE), (Count > 0));
#endif

#ifndef STANDALONE
	EnableGadget(_R(IDC_LIBGAL_IMPORT), (Count > 0) && Document::GetSelected() != NULL);
#else
	EnableGadget(_R(IDC_LIBGAL_IMPORT), FALSE);
#endif

	EnableGadget(_R(IDC_LIBGAL_ADD_CLIPART), TRUE);


	// Standard library buttons
	LibraryGallery::SelectionHasChanged();
}

/********************************************************************************************

>	void LibClipartSGallery::DoShadeGallery(BOOL ShadeIt)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> (based on Jason code)
	Created:	28/3/95

	Purpose:	To un-grey the options... button when there are no docs
	Notes:

********************************************************************************************/

void LibClipartSGallery::DoShadeGallery(BOOL ShadeIt)
{
	if (DisplayTree == NULL)
		return;

	// Keep this here always
	EnableGadget(_R(IDC_GALLERY_MENU), TRUE);

	if(ShadeIt)
	{
		EnableGadget(_R(IDC_LIBGAL_IMPORT), FALSE);
	}

	EnableGadget(_R(IDC_LIBGAL_ADD_CLIPART), TRUE);

	// Standard library buttons
	LibraryGallery::DoShadeGallery(ShadeIt);
}

/********************************************************************************************

>	virtual BOOL LibClipartSGallery::CanSearchKeywords(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95

	Returns:	TRUE

	Purpose:	Used to determine if this type of gallery supports keyword searching.
				This one does
				
********************************************************************************************/

BOOL LibClipartSGallery::CanSearchKeywords(void)
{
	return(TRUE);
}



/********************************************************************************************

>	virtual void LibClipartSGallery::WorkOutSectionName(String_256 *Section);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/95

	Outputs:	Section	- String used to return the section name

	Purpose:	Returns the section name to use in the grm file

********************************************************************************************/

void LibClipartSGallery::WorkOutSectionName(String_256 *Section)
{
	if(Section)
		*Section = _R(IDS_SGLCART_SECTION_NAME); // "ClipartGallery";
}



/***********************************************************************************************

>	virtual SGDisplayGroup *LibClipartSGallery::AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		LibraryToDisplay - Pointer to the library associated with the group
				NumItems - Number of items in the group
	Returns:	The newly created library group, or NULL if there were problems
	Purpose:	Create a library group, as opposed to a display group
	Notes:

***********************************************************************************************/

SGDisplayGroup *LibClipartSGallery::AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems)
{
	TRACEUSER( "Matt", _T("Add library group called in clipart gallery\n"));
	ERROR3IF(LibraryToDisplay == NULL, "LibClipartSGallery::AddLibraryGroup - NULL parameter is illegal");

	if (DisplayTree == NULL)
	{
		ERROR3("LibClipartSGallery::AddLibraryGroup called before the DisplayTree was initialised!");
		return(NULL);
	}

	//Flag the gallery to redraw and check the group against the state of the web themes / clipart box
	m_bRedraw = true;
	SGLibGroup *TheGroup = (SGLibGroup *)DisplayTree->FindSubtree(this, NULL, LibraryToDisplay);

	if (TheGroup == NULL)
	{
		// No existing group for that library, so create a new one
		TheGroup = new SGLibGroup(this, NULL, LibraryToDisplay);

		if (TheGroup == NULL)				// Failed!
			return(NULL);
		
		// And add it to our display tree
		if (((GetClipTheme() == 0) && (LibraryToDisplay->Type == SGLib_ClipArt_WebThemes)) || ((GetClipTheme() == 1) && (LibraryToDisplay->Type == SGLib_ClipArt)))
		{
			TheGroup->Flags.Invisible = true;
			TheGroup->Flags.CanSelect = false;
		}
		DisplayTree->AddItem(TheGroup);
		TRACEUSER( "Matt", _T("...Add library group has just added the item to the display tree\n"));
	}
	else
	{
		if (((GetClipTheme() == 0) && (LibraryToDisplay->Type == SGLib_ClipArt_WebThemes)) || ((GetClipTheme() == 1) && (LibraryToDisplay->Type == SGLib_ClipArt)))
		{
			TheGroup->Flags.Invisible = true;
			TheGroup->Flags.CanSelect = false;
		}

		TheGroup->DestroySubtree(FALSE);	// Delete all items in the group
	}

	return(TheGroup);
}




/********************************************************************************************

>	virtual SGDisplayItem *LibClipartSGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
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

SGDisplayItem *LibClipartSGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
								SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition)
{
	TRACEUSER( "Matt", _T("Copy display item called in clipart gallery\n"));
	if(SourceItem == NULL || DestGroup == NULL)
	{
		ERROR3("LibClipartSGallery::CopyDisplayItem -> Illegal NULL param");
		return NULL;
	}

	// Check if we've dragged from one group to another and warn if that's the case
	if(DestGroup->IsKindOf(CC_RUNTIME_CLASS(SGLibGroup)))
	{
		SGDisplayNode *SourceGroup = SourceItem->GetParent();
		if((SGDisplayGroup *)SourceGroup != DestGroup)
		{
			// "Can't move clipart between groups."
			InformError(_R(IDS_CLIPART_BETWEEN_GROUPS), _R(IDS_OK));
			Error::ClearError();
		}
	}

	return(NULL);
}



/********************************************************************************************

>	virtual BOOL LibClipartSGallery::InitMenuCommands(void)
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95

	Returns:	TRUE for success

	Purpose:	Initialises any menu commands that this gallery needs.

	Notes:		Will only create the menu commands once - further calls in the future
				will return TRUE immediately wihtout doing anything.

********************************************************************************************/

BOOL LibClipartSGallery::InitMenuCommands(void)
{
	TRACEUSER( "Matt", _T("Clipart Gallery InitMenuCommands() called\n"));
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
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Add, _R(IDS_SGMENU_ADDCLIPART));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_EmptyClipartCache, _R(IDS_SGMENU_EMPTYCLIPARTCACHE));

		// "Special" entries for over-list menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Open, _R(IDS_SGMENU_OPEN));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Import, _R(IDS_SGMENU_IMPORT));
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

>	virtual BOOL LibClipartSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		TheMenu - The menu to add commands to
				MenuID  - The type of menu (over-list or from-options-button) to create

	Returns:	TRUE if it succeeded

	Purpose:	To build a menu of commands to be popped up over the gallery.
	
	Notes:		Override this method to stop the default menus being built

********************************************************************************************/

BOOL LibClipartSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)
{
 	BOOL ok = TRUE;

	if (MenuID == SGMENU_OPTIONS)
	{
		// Options menu
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Add);	
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_EmptyClipartCache, TRUE);

		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Find);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Sort);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Properties);
	}
	else
	{
		// Over-list menu
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Open);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Import);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Remove, TRUE);		// With separator

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

>	virtual OpState LibClipartSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)

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

OpState LibClipartSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)
{
	OpState State;

	if (*CommandID == SGCmd_Add)											// --- Add (always available)
		return(State);

	if (*CommandID == SGCmd_EmptyClipartCache)
	{
		if (m_bDiscardWebFolders)
			State.Greyed = TRUE;
		return (State);
	}

	if (*CommandID == SGCmd_Open || *CommandID == SGCmd_Import)				// --- Open/Import
	{
		if (GetSelectedItemCount() < 1)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSEL));
		}
	}
	else if (*CommandID == SGCmd_Remove)									// --- Remove
	{
		if (GetSelectedGroupCount() < 1)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSELGROUP));
		}
	}
	else if (*CommandID == SGCmd_Properties)									// --- Properties - disabled if the gallery is currently downloading thumnails
	{
		if (!m_lstPendingThumbs.empty())
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_DOWNLOADING));
		}
	}
	else
		return(SuperGallery::GetCommandState(CommandID, ShadeReason));		// Unknown command- pass to baseclass

	return(State);
}



/********************************************************************************************

>	virtual void LibClipartSGallery::DoCommand(StringBase *CommandID)

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

void LibClipartSGallery::DoCommand(StringBase *CommandID)
{
	if (*CommandID == SGCmd_Open)											// --- Open
		ImportClipart(TRUE, this);
	else if (*CommandID == SGCmd_Import)									// --- Import
		ImportClipart(FALSE, this);
	else if (*CommandID == SGCmd_Add)										// --- Add clipart
		BrowseClicked();
	else if (*CommandID == SGCmd_EmptyClipartCache)					// --- Remove web folders
	{
		RemoveWebFolders(SGLib_ClipArt); //This will actually remove both clipart and web themes!
	}
	else if (*CommandID == SGCmd_Remove)									// --- Remove
	{
		RemoveSelectedLibraries(TRUE);
		UpdateGRMFile();
	}
	else
		SuperGallery::DoCommand(CommandID);		// Unknown command- pass to the base class
}


/********************************************************************************************
>	INT32 LibClipartSGallery::GetClipTheme() const

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/2000
	Returns:	The index of the theme/clipart currently shown in the selector.
	SeeAlso:	LibClipartSGallery::SetPropertyIndex
********************************************************************************************/

INT32 LibClipartSGallery::GetClipTheme() const
{
	return m_nClipTheme;
}



/********************************************************************************************
>	INT32 LibClipartSGallery::SetClipTheme(INT32 nNewIndex)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/2000
	Inputs:		nNewIndex	---		the new index to show.
	Returns:	The old value of the index.
	SeeAlso:	LibClipartSGallery::GetPropertyIndex
********************************************************************************************/

INT32 LibClipartSGallery::SetClipTheme(INT32 nNewIndex)
{
	ERROR3IF(0 > nNewIndex, "LibClipartSGallery::SetClipTheme: index out of range");
	INT32 n = m_nClipTheme;
	m_nClipTheme = nNewIndex;
	return n;
}




/********************************************************************************************

>	BOOL OpDisplayLibClipartGallery::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDisplayLibClipartGallery initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDisplayLibClipartGallery::Init()
{

	ERRORIF(new OpLibGalClipTheme == 0, _R(IDE_NOMORE_MEMORY), FALSE);

	return (RegisterOpDescriptor(
	 							0,
	 							_R(IDS_DISPLAY_CLIPART_GALLERY),
								CC_RUNTIME_CLASS(OpDisplayLibClipartGallery),
	 							OPTOKEN_DISPLAYCLIPARTGALLERY,
	 							OpDisplayLibClipartGallery::GetState,
	 							0,	/* help ID */
	 							_R(IDBBL_DISPLAY_CLIPART_GALLERY),
	 							0	/* bitmap ID */));
}               

/********************************************************************************************

>	OpState	OpDisplayLibClipartGallery::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDisplayLibClipartGallery operation
	Purpose:	For finding the OpDisplayLibClipartGallery's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDisplayLibClipartGallery::GetState(String_256* UIDescription, OpDescriptor*)
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

>	void OpDisplayLibClipartGallery::Do(OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Displays the LibCliparts gallery
				Updates the button state for this Op (the button sticks down while the
				gallery is open)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDisplayLibClipartGallery::Do(OpDescriptor*)
{
	TRACEUSER( "Matt", _T("OpDisplayLibClipartGallery::Do called\n"));
	DialogBarOp *pDialogBarOp = FindGallery();

	if (pDialogBarOp != NULL)
	{
		// Toggle the visible state of the gallery window
		pDialogBarOp->SetVisibility( !pDialogBarOp->IsVisible() );

		// And update our button state
		SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYCLIPARTGALLERY, pDialogBarOp->IsVisible());

		// If we're closing the gallery, flush the thumbnail caches
		if(pDialogBarOp->IsVisible() == FALSE)
		{
			// Flush the thumbnail cache
			BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::ThumbState::KILLCACHE, SGLib_ClipArt), DialogOp);
			BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::ThumbState::KILLCACHE, SGLib_ClipArt_WebThemes), DialogOp);
			BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::ThumbState::KILLCACHE, SGLib_Bitmap), DialogOp);

			// Ensure all open libraries are closed and memory reclaimed when the gallery closes
			if(LibClipartSGallery::ThisGallery != NULL)
			{
				LibClipartSGallery::ThisGallery->GalleryAboutToClose();
			}
			else
			{
				ERROR3("Clipart gallery seems to have been closed before I could reclaim my memory");
			}

		}
/*		else
		{
			if(LibClipartSGallery::ThisGallery != NULL)
			{
				LibClipartSGallery::ThisGallery->GalleryAboutToReOpen();
			}
		}*/
	}

	End();
}



/********************************************************************************************

>	static DialogBarOp *OpDisplayLibClipartGallery::FindGallery(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)

	Returns:	NULL or a pointer to the LibClipart gallery instance

	Purpose:	Finds the LibClipart gallery class instance

	Notes:		The bars system always keeps one LibClipart gallery alive for us.
				If one is not found, this usually indicates that it can't be found
				in bars.ini: Check that the 'Name' string *exactly* matches the
				title string given in bars.ini.
				Also check that bars.ini indicates the bar is of the LibClipartSGallery class

********************************************************************************************/

DialogBarOp *OpDisplayLibClipartGallery::FindGallery(void)
{
	String_32 Name = _R(IDS_SGLCART_GALLERY_NAME); // "Clipart gallery";
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

	if (pDialogBarOp != NULL)
	{
		if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(LibClipartSGallery))
			return(pDialogBarOp);

		ERROR3("Got the Clipart gallery but it's not of the LibClipartSGallery class");
	}
	else
	{
		ERROR3("Can't find the Clipart gallery in bars.ini!\n");
	}

	return(NULL);
}

/***********************************************************************************************

>	virtual LibDisplayType SGClipartItem::GetDisplayType(SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/95

	Inputs:		MiscInfo - Contains a few useful bits of info that may be
				needed for all event types.
	Outputs:	
	Returns:	The display mode type to use (position of text, and size of thumb)

	Purpose:	Return the display type to use - clipart gallery override
	Notes:
	SeeAlso:

***********************************************************************************************/

LibDisplayType SGClipartItem::GetDisplayType(SGMiscInfo *MiscInfo)
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

>	SGClipartItem::SGClipartItem(LibraryIndex LibraryIndexToDisplay);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/95
	Inputs:		LibraryIndex - Offset into index file which describes this item
	Outputs:	
	Returns:
	Purpose:	Creates and initialises a new item for the clipart gallery
	Notes:
	SeeAlso:

***********************************************************************************************/

SGClipartItem::SGClipartItem(LibraryIndex LibraryIndexToDisplay, BOOL bNew) :
	SGLibDisplayItem(LibraryIndexToDisplay, bNew)
{
}

/* default constructor and destructor */

SGClipartItem::SGClipartItem()
{
}

SGClipartItem::~SGClipartItem()
{
	TRACEUSER( "Matt", _T("~SGClipartItem called\n"));
}







/***********************************************************************************************

>	virtual BOOL SGClipartItem::HandleEvent(SGEventType EventType, void *EventInfo,
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

BOOL SGClipartItem::HandleEvent(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo)
{
	switch (EventType)
	{
		case SGEVENT_MOUSECLICK:
		{
			SGMouseInfo *Mouse = GetMouseInfo(EventType, EventInfo);

			if (Mouse != NULL && FormatRect.ContainsCoord(Mouse->Position))
			{	
				if(LibClipartSGallery::DoSounds)
				{
					Library *Lib = GetParentLibrary();
				 	if (Lib != NULL)
					{
						String_64 Key("Sample");
						String_256 SampleString;
						Lib->GetSingleField(TheLibraryIndex, &Key, &SampleString);
						if(SampleString.Length() > 2)
						{
							String_256 SubPath = Lib->SubLibPath->GetPath();
							BOOL Done = SGLibOil::PlayRandomSample(&SampleString, &SubPath);
							ERROR3IF(!Done, "Problems playing sample");
						}
					}
				}

				INT32 XSize=0, YSize=0;
				if(SGLibDisplayItem::StartDrag(EventType, EventInfo, MiscInfo, &XSize, &YSize))
				{
					GalleryClipartDragInfo *DragClipart;
					DragClipart = new GalleryClipartDragInfo(this, Mouse, MiscInfo,
														Mouse->MenuClick, XSize, YSize);
					if (DragClipart != NULL)
						DragManagerOp::StartDrag(DragClipart, GetListWindow());
					else
					{
						if(LibClipartSGallery::TmpDraggingBitmap != NULL)
						{
							delete LibClipartSGallery::TmpDraggingBitmap;
							LibClipartSGallery::TmpDraggingBitmap = NULL;
						}
					}
				}
				return TRUE;		// Claim this event - nobody else can own this click
			}
		}
	}
	
	return(SGLibDisplayItem::HandleEvent(EventType, EventInfo, MiscInfo));
}


/********************************************************************************************

>	SGClipartDragTarget::SGClipartDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL)
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Inputs:		TheDialog - The kernel dialog in which the target exists
				TheGadget - The gadget within that dialogue which is the target

	Purpose:	Constructor

********************************************************************************************/

SGClipartDragTarget::SGClipartDragTarget(DialogOp *TheDialog, CGadgetID TheGadget)
					: SGListDragTarget(TheDialog, TheGadget)
{
	ERROR3IF(!TheDialog->IsKindOf(CC_RUNTIME_CLASS(LibClipartSGallery)),
			"You can only use SGClipartDragTargets with LibClipartSGallery dialogues!");
}



/********************************************************************************************

	BOOL SGClipartDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
											OilCoord *pMousePos, KeyPress* pKeyPress)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Inputs:		Event - Indicates what has happened
				pDragInfo - points to drag information describing this drag. This
				should be a ClipartDragInformation or derivation thereof
				pMousePos - points to information on the current mouse position, in OIL coords
				pKeyPress - NULL, or if for a keypress event, keypress information

	Returns:	TRUE to claim the event, FALSE to let it through to other targets

	Purpose:	Event Handler for SuperGallery listitem drag events. Overrides the
				base class handler to enable it to sort out the node being dragged
				for Clipart drags.

********************************************************************************************/

BOOL SGClipartDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
										OilCoord *pMousePos, KeyPress* pKeyPress)
{
	TRACEUSER( "Matt", _T("SGClipartDragTarget::ProcessEvent called\n"));
	if (!pDragInfo->IsKindOf(CC_RUNTIME_CLASS(BitmapDragInformation)))
		return(FALSE);

	SGDisplayNode *DraggedNode = NULL;
	BOOL IsSimpleBitmapDrag = TRUE;

	if (IS_A(pDragInfo, GalleryClipartDragInfo))
	{
		DraggedNode = ((GalleryClipartDragInfo *)pDragInfo)->GetDraggedClipart();
		IsSimpleBitmapDrag = FALSE;
	}

	if (DraggedNode != NULL)
	{
		switch(Event)
		{
			case DRAGEVENT_COMPLETED:
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

>	void GalleryClipartDragInfo::GalleryClipartDragInfo() 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  

	Purpose:	Default constructor - do not call this constructor

********************************************************************************************/

GalleryClipartDragInfo::GalleryClipartDragInfo()
{
	ERROR3("Default GalleryClipartDragInfo constructor called");	
}


/********************************************************************************************

>	void GalleryClipartDragInfo::~GalleryClipartDragInfo() 
	 
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/95		  

	Purpose:	Destructor - need to unlock thumbnail

********************************************************************************************/

GalleryClipartDragInfo::~GalleryClipartDragInfo()
{
	TRACEUSER( "Richard", _T("~GalleryClipartDragInfo called\n"));

	if(LibraryGallery::TmpDraggingBitmap != NULL)
	{
		delete LibraryGallery::TmpDraggingBitmap;
		LibraryGallery::TmpDraggingBitmap = NULL;
	}
}

/********************************************************************************************

>	GalleryClipartDragInfo::GalleryClipartDragInfo(SGClipartItem *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust = FALSE,
 											INT32 XSize = 0, INT32 YSize = 0)
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  

	Inputs:		pSourceItem - The gallery item from which the drag originated
				pMouseInfo	- The mouse info which made the item start the drag
				pMiscInfo	- The MiscInfo which accompanied the mouse event
				IsAdjust	- TRUE if this is an adjust (line-Bitmap) drag
				XSize  		- Width in pixels of preview to drag
				YSize 		- Height in pixels of preview to drag

	Purpose:	Constructor

********************************************************************************************/

GalleryClipartDragInfo::GalleryClipartDragInfo(SGClipartItem *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust, INT32 XSize, INT32 YSize)
 								  : BitmapDragInformation(LibraryGallery::TmpDraggingBitmap,
										/*pSourceItem->GetDisplayedKernelBitmap(pMiscInfo, TRUE)*/
			  							XSize, YSize, 0, 0, IsAdjust)
{
	SourceItem	= pSourceItem;	// Copy the source item pointer
	MouseInfo	= *pMouseInfo;	// Duplicate the structures (they may cease to exist
	MiscInfo	= *pMiscInfo;	// soon after the drag is started)
}



/********************************************************************************************

>	void GalleryClipartDragInfo::OnClick(INT32 Flags,POINT Point) 
	 
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

void GalleryClipartDragInfo::OnClick(INT32 Flags ,POINT Point)
{
	TRACEUSER( "Matt", _T("GalleryClipartDragInfo::OnClick called\n"));
	if (SourceItem != NULL)
		SourceItem->DragWasReallyAClick(&MouseInfo, &MiscInfo);
}

/********************************************************************************************

>	void GalleryClipartDragInfo::GetCursorID(DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Purpose:	Set cursor over this target


********************************************************************************************/

UINT32 GalleryClipartDragInfo::GetCursorID(DragTarget* pDragTarget)
{
	if (pDragTarget && pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		PageDropInfo PageDropInfo;
		((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);

		NodeRenderableInk* pObjectHit 	= PageDropInfo.pObjectHit;
		ObjectDragTarget TargetHit 		= PageDropInfo.TargetHit;

		if (pObjectHit)
			TargetHit = NO_TARGET;

		switch (TargetHit)
		{
			case NO_TARGET:
				return _R(IDC_CANDROPONPAGE);
		};

		return _R(IDC_CANDROPONPAGE);
	}

	return _R(IDC_CANTDROP);
}

/********************************************************************************************

>	virtual BOOL GalleryClipartDragInfo::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Returns:	Whether String is valid
	Purpose:	provide status line text for this target
   
********************************************************************************************/

BOOL GalleryClipartDragInfo::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)
{
	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");

	if (TheBitmap == NULL || TheBitmap->ActualBitmap == NULL)
		return FALSE;

 	String_256 DragString;
	String_256 ItemName;
	SourceItem->GetNameText(&ItemName);

	// "Dragging clipart '#1%s'"
 	DragString.MakeMsg(_R(IDS_CLIPART_DRAGGING), (TCHAR *)ItemName);

	if (pDragTarget && pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		DragString += String_8(_R(IDS_SGDFONTS_STAT_COLON_SEP)); //" : ";

		PageDropInfo PageDropInfo;
		((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);

		NodeRenderableInk* pObjectHit 	= PageDropInfo.pObjectHit;
		ObjectDragTarget TargetHit 		= PageDropInfo.TargetHit;

		if (pObjectHit)
			TargetHit = NO_TARGET;				// Ignore NodeBitmap objects

		switch (TargetHit)
		{
			case NO_TARGET:
			{
				// + "Drop to Insert the clipart at this point on the Page";
				String_64 DropToThis(_R(IDS_CLIPART_DROP));
				DragString += DropToThis;
				break;
			}
		};

	 	*TheText = DragString;
		return TRUE;
	}
 	
 	*TheText = DragString;
	return TRUE;
}

/********************************************************************************************

>	BOOL GalleryClipartDragInfo::OnPageDrop(ViewDragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  
	Inputs:		pDragTarget, the ViewDragTarget that we were dropped onto.
	Returns:	TRUE, if we handled the drop.
	Purpose:	Called when Clipart is dropped onto the page.
				Use pDragTarget->GetDropInfo(), to details of the drop.

********************************************************************************************/

BOOL GalleryClipartDragInfo::OnPageDrop(ViewDragTarget* pDragTarget)
{
	PageDropInfo PageDropInfo;
	((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);

	PathName FileName;		
	SGLibDisplayItem *ClipItem = SourceItem;

	BOOL ok = ClipItem->GetFileName(&FileName);

	if(ok)
	{
		// Bodge for clipart CD - 'Cow' in LScape should point to "Moo.tif", not just "Moo"
		if(FileName.GetFileName(FALSE) == (String_256)"Moo")
		{
			INT32 Size = ClipItem->GetFileSize(NULL);
			if(Size == 1080904)
				FileName.SetType("TIF");
		}

		if(!FileName.IsValid(FileName.GetPath()))
		{
			ERROR3("LibClipartSGallery::ImportClipart - Filename is invalid");
			return FALSE;
		}

		// Check if file has a long filename, and use that if it does...
		String_256 FilePath((const TCHAR *)FileName.GetPath());
		String_256 LongFileName;
		BOOL LongPath = FileUtil::GetLongFileName((LPTSTR)FilePath, (LPTSTR)LongFileName, 255);
		if(LongPath)
		{
			FilePath = FileName.GetLocation(TRUE);
			FilePath += LongFileName;
		}

#ifdef STANDALONE
		CWinApp* pApp = AfxGetApp();
		BaseFileDialog::SelectedFilter = 0;

		// Open a document
		CCamDoc* pDoc = (CCamDoc*) pApp->OpenDocumentFile((TCHAR *)FilePath);

		// And redraw the imported document
		if(pDoc) pDoc->GetKernelDoc()->ForceRedraw();
#else
		// Invoke the clipart import operation so that we get Undo.

		Library* pLibrary = ClipItem->GetParentLibrary();
		if (!pLibrary)
		{
			ERROR3("Illegal NULL pointer");
			return FALSE;
		}
		BOOL bIsLocalFile;
		if (!pLibrary->IsWebLibrary())
			bIsLocalFile = TRUE;
		else 
			bIsLocalFile = (_access((TCHAR*) FilePath, 0) != -1);

		
		
		if (bIsLocalFile) // local file, do a normal import Op
		{
			OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpClipartImport));

			if (pOpDesc != NULL)
			{
				ClipartImportParam Param;

				// Set up the parameters which we require to import the clipart
				PathName FullPathName((TCHAR *)FilePath);
				Param.File = &FullPathName;

				Param.Import = TRUE;
				if(PageDropInfo.pDoc == NULL)
					Param.Import = FALSE;

				Param.Result = TRUE;
				Param.DropInfo =  &PageDropInfo; // for positioning and scaling

				// Go for it!
				pOpDesc->Invoke((OpParam *) &Param);

				ok = Param.Result;

				if(!ok)
				{
					ERROR3("LibClipartSGallery::ImportClipart - Problem importing file");
					return FALSE;
				}
			}
		}
		else // try an asynch import
		{
			String_256 strFileURL = FilePath;
			pLibrary->LocalPath2URL(&strFileURL);
			OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpAsynchClipartImport));
			if (pOpDesc != NULL)
			{
				AsynchClipartImportParam* Param = new AsynchClipartImportParam;
				ERROR2IF(!Param, FALSE, "Memory allocation error");
				// Set up the parameters which we require to import the clipart
				Param->File.SetPathName((TCHAR *)FilePath);
				Param->strURL = strFileURL;
				String_256 strDescription;
				ClipItem->GetNameText(&strDescription);
				Param->strDescription = _T("'");
				Param->strDescription += strDescription;
				Param->strDescription += _T("'");
				Param->bDropped = TRUE;
				Param->Import = (PageDropInfo.pDoc == NULL) ? FALSE : TRUE;
				((ViewDragTarget*)pDragTarget)->GetDropInfo(&Param->DropInfo);
				pOpDesc->Invoke((OpParam *) Param);
			}
		}
#endif
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL GalleryClipartDragInfo::OnMainFrameDrop(DragTarget* pDragTarget)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/95		  
	Inputs:		pDragTarget, the Target that this drag finished over.
	Returns:	TRUE, if we handled the drop.
				FALSE, if we want to let the target handle it.
	Purpose:	Called when this DragInfo is dropped onto the MainFrame. We open the file
				into a new document...

********************************************************************************************/

BOOL GalleryClipartDragInfo::OnMainFrameDrop(DragTarget* pDragTarget)
{
	SGLibDisplayItem* ClipItem = (SGLibDisplayItem*) SourceItem;
	String_256 ItemText;

	if (ClipItem->IsKindOf(CC_RUNTIME_CLASS(SGClipartItem)))
	{
		ClipItem->GetNameText(&ItemText);
		PathName FileName;		
	
		//if (Lib != NULL)
		BOOL ok = ClipItem->GetFileName(&FileName);

		if(ok)
		{
			// Bodge for clipart CD - 'Cow' in LScape should point to "Moo.tif", not just "Moo"
			if(FileName.GetFileName(FALSE) == (String_256)"Moo")
			{
				INT32 Size = ClipItem->GetFileSize(NULL);
				if(Size == 1080904)
					FileName.SetType("TIF");
			}

			if(!FileName.IsValid(FileName.GetPath()))
			{
				ERROR3("LibClipartSGallery::ImportClipart - Filename is invalid");
				return FALSE;
			}

			// Check if file has a long filename, and use that if it does...
			String_256 FilePath((const TCHAR *)FileName.GetPath());
			String_256 LongFileName;
			BOOL LongPath = FileUtil::GetLongFileName((LPTSTR)FilePath, (LPTSTR)LongFileName, 255);
			if(LongPath)
			{
				FilePath = FileName.GetLocation(TRUE);
				FilePath += LongFileName;
			}

			// Check if the file really exists on the local drive
			Library* pLibrary = ClipItem->GetParentLibrary();
			if (!pLibrary)
			{
				ERROR3("Illegal NULL pointer");
				return FALSE;
			}
			BOOL bIsLocalFile;
			if (!pLibrary->IsWebLibrary())
				bIsLocalFile = TRUE;
			else 
				bIsLocalFile = (_access((TCHAR*) FilePath, 0) != -1);
				
			if (bIsLocalFile)
			{
				CWinApp* pApp = AfxGetApp();
				BaseFileDialog::SelectedFilter = 0;

				// Open a document
				CCamDoc* pDoc = (CCamDoc*) pApp->OpenDocumentFile((TCHAR *)FilePath);

				// And redraw the imported document
				if(pDoc) pDoc->GetKernelDoc()->ForceRedraw();
			}
			else // do an asynch import
			{
				String_256 strFileURL = FilePath;
				pLibrary->LocalPath2URL(&strFileURL);
				OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpAsynchClipartImport));
				if (pOpDesc != NULL)
				{
					AsynchClipartImportParam* Param = new AsynchClipartImportParam;
					ERROR2IF(!Param, FALSE, "Memory allocation error");
					// Set up the parameters which we require to import the clipart
					Param->File.SetPathName((TCHAR *)FilePath);
					String_256 strDescription;
					ClipItem->GetNameText(&strDescription);
					Param->strDescription = _T("'");
					Param->strDescription += strDescription;
					Param->strDescription += _T("'");
					Param->strURL = strFileURL;
					Param->Import = FALSE;
					Param->bDropped = FALSE;
					// Invoke the operation
					pOpDesc->Invoke((OpParam*) Param);
				}
			}
		}
		return TRUE;
	}	
	return FALSE;
}





/********************************************************************************************

>	SGOilClipartDragTarget::SGOilClipartDragTarget(HWND TheWindow, CRect *ClientArea = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/95		  
	Inputs:		TheWindow 	- Window to use for drag target
				ClientArea 	- Rectangle in window to use for drag target

	Purpose:	Constructor

********************************************************************************************/

SGOilClipartDragTarget::SGOilClipartDragTarget(HWND TheWindow, CRect *ClientArea)
					: WinoilDragTarget(TheWindow, ClientArea)
{
}
	
/********************************************************************************************

>	UINT32 SGOilClipartDragTarget::GetCursorID()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/95		  
	Returns:	Cursor ID for clipart drags over the mainframe
	Purpose:	Get cursor ID

********************************************************************************************/

UINT32 SGOilClipartDragTarget::GetCursorID()
{
	return _R(IDC_CANDROPONPAGE);
}

/********************************************************************************************

>	BOOL SGOilClipartDragTarget::GetStatusLineText(String_256 * TheText)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/95		  
	Outputs:	Text for status line
	Returns:	TRUE if we extracted a wee tad of status line text
	Purpose:	Obtains status line text for when we're dragging an item of clipart over the
				blank mainframe area...

********************************************************************************************/

BOOL SGOilClipartDragTarget::GetStatusLineText(String_256 * TheText)
{
	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");

	DragInformation *pDragInfo = DragManagerOp::GetCurrentDragInfo();
	if (pDragInfo == NULL || !pDragInfo->IsKindOf(CC_RUNTIME_CLASS(GalleryClipartDragInfo)))
		return(FALSE);
	SGClipartItem *pClipartItem = ((GalleryClipartDragInfo *)pDragInfo)->SourceItem;

 	String_256 DragString;
	String_256 ItemName;
	pClipartItem->GetNameText(&ItemName);

	// "Dragging clipart '#1%s' : Drop to open a new document containing the clipart"
 	DragString.MakeMsg(_R(IDS_CLIPART_DRAGGING), (TCHAR *)ItemName);
	DragString += String_8(_R(IDS_SGDFONTS_STAT_COLON_SEP));
	DragString += String_256(_R(IDS_CLIPART_OPEN_DROP));

 	*TheText = DragString;
	return TRUE;
}

/********************************************************************************************

>	BOOL SGOilClipartDragTarget::ProcessEvent(DragEventType Event,
			DragInformation *pDragInfo, CPoint *pMousePos, KeyPress* pKeyPress)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/95		  
	Inputs:		Standard drag target stuff...
	Returns:	TRUE if we processed our event ok...
	Purpose:	Process an event such as a completed drag...

********************************************************************************************/

BOOL SGOilClipartDragTarget::ProcessEvent(DragEventType Event,
			DragInformation *pDragInfo, CPoint *pMousePos, KeyPress* pKeyPress)
{
	if (!pDragInfo->IsKindOf(CC_RUNTIME_CLASS(GalleryClipartDragInfo)))
		return(FALSE);

	SGDisplayNode *DraggedNode = NULL;
	BOOL IsSimpleClipartDrag = TRUE;

	if (IS_A(pDragInfo, GalleryClipartDragInfo))
	{
		DraggedNode = ((GalleryClipartDragInfo *)pDragInfo)->GetDraggedClipart();
		IsSimpleClipartDrag = FALSE;
	}

	if (DraggedNode != NULL)
	{
		switch(Event)
		{
			case DRAGEVENT_COMPLETED:
			{
				// Open the clipart file into a brand-spanking-new document
				return(((GalleryClipartDragInfo *)pDragInfo)->OnMainFrameDrop(this));
			}

			case DRAGEVENT_MOUSESTOPPED:
			case DRAGEVENT_MOUSEMOVED:
			case DRAGEVENT_MOUSEIDLE:
				return(TRUE);		// Claim the event so the mouse cursor changes
		}
	}

	// Otherwise, we aren't interested in the event, so we don't claim it
	return(FALSE);
}



/**********************************************************************************************
>	OpLibGalClipTheme::OpLibGalClipTheme()

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/2000
	Purpose:	Constructs an OpLibGalClipTheme object.
**********************************************************************************************/

/*
	OpDescriptor constructor's arguments:-

				 UINT32 toolID,                      	// Module Identifier
				 UINT32 txID,                         // String Resource ID
				 CCRuntimeClass* Op,				// pointer to the Op's runtime class
				 TCHAR* tok,						// pointer to the token string
				 pfnGetState gs,					// pointer to the GetState function
				 UINT32 helpId = 0,					// help identifier 
				 UINT32 bubbleID = 0,					// string resource for bubble help
				 UINT32 resourceID = 0,				// bitmap ("icon") or resource ID
				 UINT32 controlID =0,					// control ID
				 BOOL ReceiveMessages = FALSE,
				 BOOL Smart = FALSE,
				 BOOL Clean = TRUE,
				 UINT32 OneOpenInstID = 0,			// When not 0 only one live instance
				 									// of the operation is permitted, and
													// OneOpenInstID is the string resource
													// ID describing why.This will be useful
				 									// for 1 open instance dialogs.
				 UINT32 AutoStateFlags = 0			// Flags which indicate conditions when
				 									// the operation should automatically be
													// greyed/ticked etc..(This cuts down the 
													// number of tests that need to be made
													// in the GetState function).
*/

OpLibGalClipTheme::OpLibGalClipTheme()
  : OpDescriptor(0,
				 _R(IDS_LIBGAL_CLIPTHEME),
				 CC_RUNTIME_CLASS(OpLibGalClipTheme),
				 OPTOKEN_LIBGAL_CLIPTHEME,
				 OpDisplayLibClipartGallery::GetState,
				 0,
				 _R(IDBBL_LIBGAL_CLIPTHEME),
				 0,
				 _R(IDC_LIBGAL_CLIPTHEME),
				 TRUE)
{
	// Empty.
}




/***********************************************************************************************
>	virtual void OpLibGalClipTheme::OnControlCreate(OpDescControlCreateMsg* pCreateMsg)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/2000
	Inputs:		pCreateMsg		---		message describing this creation event
	Purpose:	Called when the combo-boxes associated with this descriptor are created.
***********************************************************************************************/

void OpLibGalClipTheme::OnControlCreate(OpDescControlCreateMsg* pCreateMsg)
{
	// Fill up the gadget's list with the names of properties.  These must be in
	// ascending order of property indices.
	pCreateMsg->pDlgOp->SetStringGadgetValue(
							pCreateMsg->SetGadgetID, &String(_R(IDS_LIBGAL_CLIPTHEME_CLIPART)));
	pCreateMsg->pDlgOp->SetStringGadgetValue(
							pCreateMsg->SetGadgetID, &String(_R(IDS_LIBGAL_CLIPTHEME_WEBTHEME)));

	// Resize the list to fit and update associated gadgets.
	pCreateMsg->pDlgOp->SetComboListLength(pCreateMsg->SetGadgetID);
	UpdateGadgets();
}



/**********************************************************************************************
>	void OpLibGalClipTheme::OnSelectionChange(OpDescControlMsg* pSelChangedMsg,
													   List* pGadgetList)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/2000
	Inputs:		pSelChangedMsg		---		message describing the change event
				pGadgetList			---		list of gadgets associated with this
	Purpose:	Called when the combo-boxes associated with this descriptor are changed.
**********************************************************************************************/

void OpLibGalClipTheme::OnSelectionChange(OpDescControlMsg* pSelChangedMsg, List*)
{
	LibClipartSGallery::Instance()->m_bRedraw = TRUE;

	// Extract the index of the item selected from the list, if any.
	INT32 i = pSelChangedMsg->pDlgOp->GetSelectedValueIndex(pSelChangedMsg->SetGadgetID);
	if (i == -1)
	{
		LibClipartSGallery::Instance()->m_bRedraw = FALSE;
		i = LibClipartSGallery::Instance()->GetClipTheme();
	}

	//Only redraw if the mode has actually been changed...
	if (i != LibClipartSGallery::Instance()->GetClipTheme())
	{
		// Matt - 02/01/2001 - in search of a quick fix...
		// Store DefaultLibraryPath information in the appropriate static...
		if (i == 1)
		{
			LibClipartSGallery::ClipartPath = LibClipartSGallery::DefaultLibraryPath;
			LibClipartSGallery::DefaultLibraryPath = LibClipartSGallery::WebThemePath;
		}
		else
		{
			LibClipartSGallery::WebThemePath = LibClipartSGallery::DefaultLibraryPath;
			LibClipartSGallery::DefaultLibraryPath = LibClipartSGallery::ClipartPath;
		}

		// Update the gallery's property index.
		LibClipartSGallery::Instance()->SetClipTheme((INT32) i);
		UpdateGadgets();

		//Now Redraw the gallery after changing between Clipart and Web Themes...
		if (LibClipartSGallery::Instance()->m_bRedraw)
		{
			LibClipartSGallery::Instance()->ModeHasChanged();
			LibClipartSGallery::Instance()->SelectionHasChanged();
			LibClipartSGallery::Instance()->ForceRedrawOfList();
		}
	}
}


/**********************************************************************************************
>	void OpLibGalClipTheme::UpdateGadgets()

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/2000
	Purpose:	Sets the text in all the combos associated with this descriptor to the
				currently selected clipart/web theme selection.
	SeeAlso:	OpDescriptor::BuildGadgetList
**********************************************************************************************/

void OpLibGalClipTheme::UpdateGadgets()
{
	// Create a list of all the gadget IDs
	List theGadgets;
	if (BuildGadgetList(&theGadgets))
	{
		// Iterate over each control in the list.
		INT32 nVal = LibClipartSGallery::Instance()->GetClipTheme();
		for (GadgetListItem* pgli = (GadgetListItem*) theGadgets.GetHead(); 
			 pgli != 0; 
			 pgli = (GadgetListItem*) theGadgets.GetNext(pgli))
		{
			// Set each control to display the text.
			pgli->pDialogBarOp->SetSelectedValueIndex(pgli->gidGadgetID, nVal);
		}

		// Tidy up.
		theGadgets.DeleteAll();
	}
} 



/********************************************************************************************

>	BOOL LibClipartSGallery::OnGetButtonClicked()

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> - quite blatantly ripped from Adrian - then changed a lot...
	Created:	19/09/2000

	Inputs:		None
	Returns:	TRUE if web folders have been added successfully

	Purpose:	Attempts to download the main web resource index file from our web site,
				parse it and create corresponding gallery folders

********************************************************************************************/

BOOL LibClipartSGallery::OnGetButtonClicked()
{
	// Connect to the Net if not yet connected
	if (!InternetManager::AttemptConnection())
	{
		InformError(_R(IDS_NOTCONNECTED), _R(IDS_OK));
		return FALSE;
	}

	// (re)enable thumbnail downloading in case it has been stopped
	ResumeThumbnailDownloading();
	ForceRedrawOfList();

	if (OpAddWebFolders::Success[SGLib_ClipArt] && OpAddWebFolders::Success[SGLib_ClipArt_WebThemes] && !m_bDiscardWebFolders) // nothing else to do
	{
		//Now I've loaded ALL of the clipart and web themes... Redraw everything just in case...
		ModeHasChanged();
		SelectionHasChanged();
		ReformatAndRedrawIfNecessary();
		

		return TRUE;
	}
	m_bDiscardWebFolders = FALSE;


	//Now make a list of Clipart
	String_256 strIndexURL(GetStringField((UINT32) SGLib_ClipArt, _R(IDS_MAININDEXURL)));

	// We initiate the download of the web clipart index file to a temporary file in $WINDIR\Temporary Internet Files
	String_256 strTempFile;
	GetTempInetDir(&strTempFile);
	strTempFile += GetStringField((UINT32) SGLib_ClipArt, _R(IDS_TMPFILES));

	OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpAddWebFolders));
	if (pOpDesc != NULL)
	{
		TRACEUSER( "Matt", _T("Starting Download of Clipart\n"));
		AddWebFoldersParam* Param = new AddWebFoldersParam;
		ERROR2IF(!Param, FALSE, "Memory allocation error");
		// Set up the op's param
		Param->localFile.SetPathName((TCHAR *) strTempFile); // local path of index file
		Param->strURL = strIndexURL; // URL of index file on our web server
		Param->pLibGal = this; // pointer to the gallery to add folders to
		Param->type = SGLib_ClipArt;
		// Invoke the operation - this will initiate the download of the index file
		// No result is returned, but the Op will report itself any error that might occur
		pOpDesc->Invoke((OpParam*) Param);
		TRACEUSER( "Matt", _T("Invoked Download of Clipart\n"));
	}
	else
	{
		ERROR3("OpAddWebFolders not found");
		return FALSE;
	}

	//Now make a list of Web Themes
	String_256 strIndexWebThemesURL(_R(IDS_CLIPART_WEBTHEMES_URL));
	String_256 strTempFileWebThemes;
	GetTempInetDir(&strTempFileWebThemes);
	strTempFileWebThemes += String_256(_R(IDS_CLIPART_WEBTHEMES_TEMPFILE));

	OpDescriptor* pOpDescWebThemes = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpAddWebFolders));
	if (pOpDescWebThemes != NULL)
	{
		TRACEUSER( "Matt", _T("Starting Download of Web Themes\n"));
		AddWebFoldersParam* ParamWebThemes = new AddWebFoldersParam;
		ERROR2IF(!ParamWebThemes, FALSE, "Memory allocation error");
		// Set up the op's param
		ParamWebThemes->localFile.SetPathName((TCHAR *) strTempFileWebThemes); // local path of index file
		ParamWebThemes->strURL = strIndexWebThemesURL; // URL of index file on our web server
		ParamWebThemes->pLibGal = this; // pointer to the gallery to add folders to
		ParamWebThemes->type = SGLib_ClipArt_WebThemes;
		// Invoke the operation - this will initiate the download of the index file
		// No result is returned, but the Op will report itself any error that might occur
		pOpDescWebThemes->Invoke((OpParam*) ParamWebThemes);
		TRACEUSER( "Matt", _T("Invoked Download of Web Themes\n"));
	}
	else
	{
		ERROR3("OpAddWebFolders not found");
		return FALSE;
	}


	//All clipart and web themes now downloaded...

	return TRUE;
}


/********************************************************************************************

>	BOOL LibClipartSGallery::RemoveWebFolders(SGLibType type)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> - ripped from Adrian
	Created:	20/09/2000

	Inputs:		type - one of 	SGLib_ClipArt,	SGLib_Font,	SGLib_Fractal
	Returns:	TRUE if the folders are removed, FALSE otherwise 

	Purpose:	Reclaims space taken up by downloaded files of a certain type
						on the local machine

********************************************************************************************/
BOOL LibClipartSGallery::RemoveWebFolders(SGLibType type)
{
	TRACEUSER( "Matt", _T("REMOVING WEB FOLDERS! in Clipart gallery\n"));
	if (AskQuestion(_R(IDS_WARNREMOVE), _R(IDS_EMPTY), _R(IDS_CANCEL)) == _R(IDS_CANCEL))
		return FALSE;

	String_256 strLocation;
	GetAppDataPath(&strLocation);
	strLocation += GetStringField(SGLib_ClipArt, _R(IDS_CACHEDIR));

	BOOL ok = true;

	if (_access((TCHAR*) strLocation, 0) != -1) // found it
	{
		DownloadCache::Traverse(strLocation, TRUE);
		DownloadCache::Refresh();
		SelectionHasChanged();
	}
	else
	{
		ok = FALSE;
	}

	GetAppDataPath(&strLocation);
	strLocation += GetStringField(SGLib_ClipArt_WebThemes, _R(IDS_CACHEDIR));
	if (_access((TCHAR*) strLocation, 0) != -1) // found it
	{
		DownloadCache::Traverse(strLocation, TRUE);
		DownloadCache::Refresh();
		SelectionHasChanged();
	}
	else
	{
		ok = FALSE;
	}

	return ok;
}
