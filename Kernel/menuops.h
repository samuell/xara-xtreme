// $Id: menuops.h 662 2006-03-14 21:31:49Z alex $
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

/*
 *  */

#ifndef INC_MENUOPS
#define INC_MENUOPS

#include "ops.h"
#include "opdesc.h"                            
#include "pathname.h"

// Menu Popup Op Tokens

#define OPTOKEN_FILE_MENU		_T("FileMenu")
#define OPTOKEN_EDIT_MENU		_T("EditMenu")
//#define OPTOKEN_VIEW_MENU		"ViewMenu"
#define OPTOKEN_HELP_MENU		_T("HelpMenu")
//#define OPTOKEN_OPS_MENU		"OpsMenu"
//#define OPTOKEN_DIALOGS_MENU	"DialogsMenu"
#define OPTOKEN_WINDOW_MENU		_T("WindowMenu")
//#define OPTOKEN_PRINT_MENU		"PrintMenu"
#define OPTOKEN_ATTRIBUTE_MENU	_T("AttributeMenu")
#define OPTOKEN_ARRANGE_MENU	_T("ArrangeMenu")
#define OPTOKEN_UTILS_MENU		_T("UtilsMenu")
#define OPTOKEN_DEBUG_MENU		_T("DebugMenu")
#define OPTOKEN_QUALITY_MENU   	_T("QualityMenu")
#define OPTOKEN_BARS_MENU   	_T("BarsMenu")
#define OPTOKEN_EXIT			_T("Exit")
#define OPTOKEN_UPDATE			_T("Update")
#define OPTOKEN_REGISTER		_T("Register")
#define OPTOKEN_FILEOPEN		_T("FileOpen")
#define OPTOKEN_FILECLOSE		_T("FileClose")
#define OPTOKEN_FILENEW			_T("FileNew")
#define OPTOKEN_FILESAVE  		_T("FileSave")
#define OPTOKEN_FILESAVEAS  	_T("FileSaveAs")
#define OPTOKEN_HELPINDEX   	_T("HelpIndex")
#define OPTOKEN_HELPUSING   	_T("HelpUsing")
#define OPTOKEN_HELPTOOLS		_T("HelpTools")
#define OPTOKEN_HELPGALLERIES	_T("HelpGalleries")
#define OPTOKEN_HELPDEMOS		_T("HelpDemos")
#define OPTOKEN_HELPTECHSUPPORT	_T("HelpTechSupport")

//Graham 20/10/97
#define OPTOKEN_FILENEW_DRAWING		_T("FileNewDrawing")
#define OPTOKEN_FILENEW_ANIMATION	_T("FileNewAnimation")
#define OPTOKEN_FILENEW_TEMPLATE1	_T("1FileNewTemplate")
#define OPTOKEN_FILENEW_TEMPLATE2	_T("2FileNewTemplate")
#define OPTOKEN_FILENEW_TEMPLATE3	_T("3FileNewTemplate")
#define OPTOKEN_FILENEW_TEMPLATE4	_T("4FileNewTemplate")
#define OPTOKEN_FILENEW_TEMPLATE5	_T("5FileNewTemplate")
#define OPTOKEN_FILENEW_TEMPLATE6	_T("6FileNewTemplate")
#define OPTOKEN_FILENEW_TEMPLATE7	_T("7FileNewTemplate")
#define OPTOKEN_FILENEW_TEMPLATE8	_T("8FileNewTemplate")
#define OPTOKEN_FILENEW_TEMPLATE9	_T("9FileNewTemplate")
#define OPTOKEN_FILENEW_TEMPLATE10	_T("10FileNewTemplate")


#define HID_FILE_NEW_DRAWING	0
#define HID_FILE_NEW_ANIMATION	0
#define HID_FILE_NEW_TEMPLATE1	0
#define HID_FILE_NEW_TEMPLATE2	0
#define HID_FILE_NEW_TEMPLATE3	0
#define HID_FILE_NEW_TEMPLATE4	0
#define HID_FILE_NEW_TEMPLATE5	0
#define HID_FILE_NEW_TEMPLATE6	0
#define HID_FILE_NEW_TEMPLATE7	0
#define HID_FILE_NEW_TEMPLATE8	0
#define HID_FILE_NEW_TEMPLATE9	0
#define HID_FILE_NEW_TEMPLATE10	0


//Graham 20/10/97: The following commands
//do not have specific buttons of their own
#define IDC_BTN_FILENEW_DRAWING		0
#define IDC_BTN_FILENEW_TEMPLATE1	0
#define IDC_BTN_FILENEW_TEMPLATE2	0
#define IDC_BTN_FILENEW_TEMPLATE3	0
#define IDC_BTN_FILENEW_TEMPLATE4	0
#define IDC_BTN_FILENEW_TEMPLATE5	0
#define IDC_BTN_FILENEW_TEMPLATE6	0
#define IDC_BTN_FILENEW_TEMPLATE7	0
#define IDC_BTN_FILENEW_TEMPLATE8	0
#define IDC_BTN_FILENEW_TEMPLATE9	0
#define IDC_BTN_FILENEW_TEMPLATE10	0



#ifdef STANDALONE
#define OPTOKEN_HELPSPEC   		_T("HelpSpec")
#endif

//#define OPTOKEN_WINDOWNEW		"WindowNew"
#define OPTOKEN_WINDOWARRANGE   _T("WindowArrange")
#define OPTOKEN_WINDOWCASCADE	_T("WindowCascade")
#define OPTOKEN_WINDOWTILE		_T("WindowTile")
//#define OPTOKEN_WINDOWTILE_V	"WindowTileVert"
#define OPTOKEN_VIEWTOOLBAR		_T("ViewToolBar")
#define OPTOKEN_VIEWSTATUSBAR	_T("ViewStatusBar")
#define OPTOKEN_VIEWCOLOURBAR	_T("ViewColourBar")
#define OPTOKEN_VIEWFULLSCREEN	_T("ViewFullScreen")
#define OPTOKEN_VIEWSCROLLBARS	_T("ViewScrollBars")
#define OPTOKEN_VIEWRULERS		_T("ViewRulers")

#define OPTOKEN_EDITCUT			_T("EditCut")
#define OPTOKEN_EDITCOPY		_T("EditCopy")
#define OPTOKEN_EDITPASTE		_T("EditPaste")
#define OPTOKEN_EDITPASTESPECIAL	_T("EditPasteSpecial")
#define OPTOKEN_EDITDELETE		_T("EditDelete")

#define OPTOKEN_DO_NOTHING	  	_T("DoNothing")

#define OPTOKEN_FILESAVEALL		_T("FileSaveAll")
#define OPTOKEN_FILEMERGE	  	_T("FileMerge")
#define OPTOKEN_FILEINFO		_T("FileInfo")
#define OPTOKEN_FILEOPTIONS		_T("FileOptions")
#define OPTOKEN_EDITDELETE		_T("EditDelete")
//#define OPTOKEN_EDITSELECTALL	"EditSelectAll"			// Now defined in selall.h
//#define OPTOKEN_EDITSELECTNONE"EditSelectNone"
#define OPTOKEN_EDITDUPLICATE	_T("EditDuplicate")
#define OPTOKEN_EDITCLONE		_T("EditClone")
#define OPTOKEN_EDITOLEINSERT	_T("EditOLEInsert")
#define OPTOKEN_EDITOLELINKS	_T("EditOLELinks")
#define OPTOKEN_EDITOLEOBJECT	_T("EditOLEObject")
#define OPTOKEN_EDITOBJECTINFO	_T("EditObjectInfo")
#define OPTOKEN_ATTRFONT		_T("AttrFont")
#define OPTOKEN_ATTRLINE		_T("AttrLine")
#define OPTOKEN_ATTRFILL		_T("AttrFill")
#define OPTOKEN_ATTRSTYLE		_T("AttrStyle")
#define OPTOKEN_ARRANGEUPLAYER		_T("ArrangeUpLayer")
//#define OPTOKEN_ARRANGEUPTOP		"ArrangeUpTop"
//#define OPTOKEN_ARRANGEUPONE		"ArrangeUpOne"
//#define OPTOKEN_ARRANGEDOWNONE		"ArrangeDownOne"
//#define OPTOKEN_ARRANGEDOWNBOTTOM	"ArrangeDownBottom"
#define OPTOKEN_ARRANGEDOWNLAYER	_T("ArrangeDownLayer")
#define OPTOKEN_ARRANGEALIGNMENT	_T("ArrangeAlignment")
//#define OPTOKEN_ARRANGEPULLGRID		"ArrangePullToGrid"
#define OPTOKEN_ARRANGEJOINSHAPES	_T("ArrangeJoinShapes")
#define OPTOKEN_ARRANGEBREAKSHAPES	_T("ArrangeBreakShapes")
//#define OPTOKEN_ARRANGEMAKESHAPES	"ArrangeMakeShapes"
#define OPTOKEN_UTILCOLOUR		_T("UtilColour")
#define OPTOKEN_UTILCUSTOMIZE	_T("UtilCustomize")
#define OPTOKEN_UTILOPTIONS		_T("UtilOptions")
#define OPTOKEN_WINDOWNEWVIEW	_T("WindowNewView")
#define OPTOKEN_WINDOWEASYMDI	_T("WindowEasyMDI")
#define OPTOKEN_WINDOWTOOLBARS	_T("WindowToolbars")
#define OPTOKEN_WINDOWRULERS	_T("WindowRulers")
//#define OPTOKEN_WINDOWGRIDS		"WindowGrids"
//#define OPTOKEN_WINDOWSNAP		"WindowSnap"
#define OPTOKEN_WINDOWBORDERS	_T("WindowBorders")
#define OPTOKEN_WINDOWQUALITY	_T("WindowQuality")
#define OPTOKEN_WINDOWSEPARATION _T("WindowSeparation")
#define OPTOKEN_QUALITYANTIALIASED	_T("QualityAntiAliased")
#define OPTOKEN_QUALITYNORMAL		_T("QualityNormal")
#define OPTOKEN_QUALITYSIMPLE		_T("QualitySimple")
#define OPTOKEN_QUALITYOUTLINE		_T("QualityOutline")
#define OPTOKEN_MAKEMASK			_T("MakeMask")
#define OPTOKEN_MASKINSIDE			_T("MaskInside")
#define OPTOKEN_XARA_WEB_MENU		_T("XaraWebMenu")
#define OPTOKEN_XARAX_FORUM			_T("XaraXForum")
#define OPTOKEN_WEBSTER_HOME_PAGE	_T("WebsterHomePage")
#define OPTOKEN_XARA_HOME_PAGE		_T("XaraHomePage")
#define OPTOKEN_XARA_PURCHASE_PAGE	_T("XaraPurchasePage")
#define OPTOKEN_WEBLINK				_T("WebLink")
#define OPTOKEN_HINTS_AND_TIPS_PAGES	_T("HintsAndTipsPages")

#define OPTOKEN_GALLERIES_SUBMENU	_T("Galleries")
#define OPTOKEN_ANIMATION_SUBMENU	_T("Animation")

/********************************************************************************************

>	class OpExit : public Operation

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/93
	Purpose:	This class represents the Exit Application operation.
	SeeAlso:	Operation

********************************************************************************************/

class CCAPI OpExit: public Operation
{                           

	CC_DECLARE_DYNCREATE( OpExit )    
    
public:
	OpExit();								// Constructor

	static BOOL		Init();					// Register an OpDescriptor
	                                        // Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		

	void			Do(OpDescriptor*);		// "Do" function

};

/****************************************************************************

>	class OpRegister: public Operation

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/2004
	Purpose:	Manage registration of CD 

****************************************************************************/

class CCAPI OpRegister: public Operation
{                           
	CC_DECLARE_DYNCREATE( OpRegister )    
public:
	OpRegister();
	static BOOL		Init();					
	static OpState	GetState(String_256*, OpDescriptor*){return OpState();}
	void			Do(OpDescriptor*);
	void			DoWithParam ( OpDescriptor*,  OpParam* Params );

	static String_256	GetSerialNumber();

	static BOOL		get_HasRegistered();
	static void		put_HasRegistered(BOOL);

protected:
	BOOL RegisterOnline();
	static	BOOL	RegisteredOnline;

PORTNOTE("other","Removed DATE usage")
#ifndef EXCLUDE_FROM_XARALX
	static  DATE	LastCheck ;
#endif

	static  INT32		DaysBetweenAutoChecks;

};

/********************************************************************************************

>	class OpUpdate : public Operation

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/4/2004
	Purpose:	This class represents the Update operation.
	SeeAlso:	Operation

********************************************************************************************/

class CCAPI OpUpdate: public Operation
{                           

	CC_DECLARE_DYNCREATE( OpUpdate )    
    
public:
	OpUpdate();							// Constructor

	static BOOL		Init();					// Register an OpDescriptor
	                                        // Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*){return OpState();}

	void			Do(OpDescriptor*);		// "Do" function
	void			DoWithParam ( OpDescriptor*,  OpParam* Params );
	void			CheckForNewFiles();
	static String_256	GetSpecialsFile();
	static BOOL UpdateFiles(BOOL bForeground); 
	static String_256 GetWebFilePath(LPCTSTR p);
	static String_256 GetWebFileRoot();
	static String_256 GetUpdateURL();
	static BOOL UpdateFiles(BOOL bForeground,
							const String_256& strClientFolder,
							const String_256& strURL,
							const String_256& strIndexLeafName,
							const String_256& strRegKeyName
							);

	static String_256	IndexBaseURL;
	static String_32	IndexLeafName;


protected:
	static	BOOL	UpdateDontShowAgain;
	static	BOOL	SpecialsDontShowAgain;
	static	INT32		UpdateFileDate;
	static	INT32		SpecialsFileDate;

};


/********************************************************************************************

>	class DocOps : public Operation

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/93
	Purpose:	This class represents All the File operations.
	SeeAlso:	Operation

********************************************************************************************/

class CCAPI DocOps: public Operation
{                           

	CC_DECLARE_DYNCREATE( DocOps )    
    
public:
	DocOps();								// Constructor

	static BOOL		Init();					// Register an OpDescriptor
	                                        // Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		

	void			Do(OpDescriptor*);		// "Do" function

public:
	static String_256 ms_strDefaultAnimationTemplate;
	static String_256 ms_strDefaultDrawingTemplate;

	static PathName GetDefaultAnimationTemplate();
	static PathName GetDefaultDrawingTemplate();
	
};

/********************************************************************************************

>	class MenuPopupOps : public Operation

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Purpose:	This class represents All the Menu Popup operations.
	SeeAlso:	Operation

********************************************************************************************/

class CCAPI MenuPopupOps: public Operation
{                           

	CC_DECLARE_DYNCREATE( MenuPopupOps )    
    
public:
	MenuPopupOps();								// Constructor

	static BOOL		Init();					// Register an OpDescriptor
	                                        // Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		

	void			Do(OpDescriptor*);		// "Do" function

};

/********************************************************************************************

>	class OpToolSelect : public Operation

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/93
	Purpose:	This class is used for Tool Selection.
	SeeAlso:	OpToolSelect

********************************************************************************************/

class CCAPI OpToolSelect: public Operation
{                           

	CC_DECLARE_DYNCREATE( OpToolSelect )    
    
public:
	OpToolSelect();						// Constructor

	static BOOL		Init();							// Register an ToolOpDescriptor
	                                        		// Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor* ToolOp);		

	void			Do(OpDescriptor* ToolOp);		// "Do" function

}; 

/********************************************************************************************

>	class HelpOps : public Operation

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/93
	Purpose:	This class is used for all the Help Operations found on the help menu.
	SeeAlso:	operation

********************************************************************************************/

class CCAPI HelpOps: public Operation
{                           

	CC_DECLARE_DYNCREATE( HelpOps )    
    
public:
	HelpOps();										// Constructor

	static BOOL		Init();							// Register the Help OpDescriptors
	                                        		// Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		

	void			Do(OpDescriptor* WhichOp);		// "Do" function
	static BOOL	GetCDHelpOn()					{ return CDHelpOn;  }
	static void	SetCDHelpOn(BOOL value)			{ CDHelpOn = value; }

protected:
	static BOOL CDHelpOn;		// Is the CD Help Otption enabled
}; 

/********************************************************************************************

>	class WindowOps : public Operation

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/93
	Purpose:	This class is used for all the Window operations found on the Window Menu.
	SeeAlso:	Operation

********************************************************************************************/

class CCAPI WindowOps: public Operation
{                           

	CC_DECLARE_DYNCREATE( WindowOps )    
    
public:
	WindowOps();										// Constructor

	static BOOL		Init();							// Register Window OpDescriptors
	                                        		// Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		

	void			Do(OpDescriptor* WhichOp);		// "Do" function

}; 

/********************************************************************************************

>	class ViewOps : public Operation

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/93
	Purpose:	This class is used for all the View operations found on the View Menu.
	SeeAlso:	Operation

********************************************************************************************/

class CCAPI ViewOps: public Operation
{                           

	CC_DECLARE_DYNCREATE( ViewOps )    
    
public:
	ViewOps();										// Constructor

	static BOOL		Init();							// Register Window OpDescriptors
	                                        		// Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);
			
	virtual BOOL OnIdleEvent(void);		// Overriden idle event processing method
	static BOOL DoFullScreen ;
	static BOOL IsFullScreenPending(){return DoFullScreen;};
	void			Do(OpDescriptor* WhichOp);		// "Do" function

}; 

/********************************************************************************************

>	class DummyOp : public Operation

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/94
	Purpose:	This class represents the Dummy operation.
	SeeAlso:	Operation

********************************************************************************************/

class CCAPI DummyOp: public Operation
{

	CC_DECLARE_DYNCREATE( DummyOp )

public:
	DummyOp();								// Constructor

	static BOOL		Init();					// Register an OpDescriptor
	                                        // Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		

	void			Do(OpDescriptor*);		// "Do" function

};

#endif  


