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

#include "camtypes.h"

#include "app.h"
#include "colormgr.h"
#include "document.h"
#include "rndrgn.h"
#include "ensure.h"
#include "prefs.h"

#include "pump.h"
#include "ops.h"
#include "dialogop.h"
#include "bars.h"
#include "opdesc.h"
#include "range.h"
#include "objreg.h"
#include "pen.h"
#include "blobs.h"
#include "docview.h"
//#include "clipint.h"
#include "doccomp.h"
//#include "keypress.h"
#include "tool.h"
//#include "keymsg.h"
#include "hotkeys.h"
#include "statline.h"
#include "camelot.h"
#include "dbugtree.h"
//#include "atminfo.h"
#include "fontman.h"
//#include "coldlog.h"
//#include "xaracms.h"
//#include "prnmks.h"
#include "camelot.h"
//#include "ralphcri.h"
#include "ralphdoc.h"
//#include "registry.h"

//#include "resimmap.h"	//For _R(IDS_NEWTEMPLATES_RELATIVEPATH)

#ifdef FREEHANDPLUGINS
// Only add in if required - general plug-in removal at present
#include "apiobj.h"		// APIObject
#include "handman.h"	//	HandleManager
#endif
//#include "plugmngr.h"	// Plug-in Manager
#include "filtrmgr.h"	// Filter Manager
#include "tmpltarg.h"	// WizOp(s)

#include "noisebas.h"
#ifdef AUTOMATION
#include "ccc.h"		// ConcurrencyController
#endif

#include "objcache.h"	// the objectcache header
#include "cachrand.h"	// ObjectCache (random)
#include "cachfifo.h"	// ObjectCache (fifo)
#include "cachweak.h"	//
//#include "bitmapcache.h"

#include "sgliboil.h"	// AppendSlashIfNotPresent

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_MEMDUMP(Application, CCObject)

Application Camelot;

/********************************************************************************************

	Preference:		Path
	Section:		Templates
	Range:			0 .. 256 characters
	Purpose:		The path to load the templates from and save the templates to.
					Defaults to blank which means alongside	the exe.
	SeeAlso:		CCamDoc::LoadDefaultDocument; OpMenuSave::Do;

********************************************************************************************/

String_256 Application::m_TemplatesPath = TEXT("");

/***********************************************************************************************

> 	Application::Application()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	20/5/93
	
	Inputs:		-
    Outputs:    -
    Returns:   	-
                 
    Purpose:    Initialise the application.
               
	Errors:		None.
                   			                                     
***********************************************************************************************/

Application::Application()
{
// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Now taken out via vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	PressurePen = NULL;
	PenInitialised = FALSE;
#endif // VECTOR_STROKING

	CamelotPrefs = NULL;
	Selection = NULL;
	TheColourManager = NULL;
	BlobMgr = NULL;
	DocComponentClasses = NULL;
	pATMIniCache = NULL;

	FontMgr						= NULL;
	CMSManager					= NULL;
	TheMarksManager				= NULL;
	ThePlugInManager			= NULL;
	TheFilterManager			= NULL;
	TheAPIObject				= NULL;
	TheHandleManager			= NULL;
	TheNoiseManager				= NULL;
	TheWizardOfOps				= NULL;
	DisabledRealDownload		= FALSE;
PORTNOTE("other","Removed HKEY usage")
#ifndef EXCLUDE_FROM_XARALX
	key							= NULL;
	key2						= NULL;
#endif
#ifdef AUTOMATION
	TheConcurrencyController	= NULL;
#endif

	// This variable will get set to TRUE when Camelot starts to shut down
	CamelotShuttingDown = FALSE; 
	BgRendering = FALSE;

	// Temporary: Remove when Kernel StatusLine is actually created by Oil statusline
	m_pStatusLine = NULL;
}

/***********************************************************************************************

> 	Application::~Application()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	20/5/93
	
	Inputs:		- 
    Outputs:    -
    Returns:   	-
                 
    Purpose:    Destroys any documents associated with this object.
               
	Errors:		None.
                   			                                     
***********************************************************************************************/

Application::~Application()
{
	// Delete any documents still associated with this application
	Document *pDocument = (Document *) Documents.GetHead();
	
	while (pDocument != NULL)
	{
		delete Documents.RemoveItem(pDocument);
		pDocument = (Document *) Documents.GetHead();
	}

	if (m_pStatusLine)
	{
		delete m_pStatusLine;
		m_pStatusLine = NULL;
	}

//	ERROR3IF(DocComponentClasses != NULL,
//				"Application was not properly de-initialised");
	if (DocComponentClasses != 0) TRACE( wxT("WARNING: Application was not properly de-initialised\n") );

	RegenList.DeleteAll();
}

#if _AFXDLL

// these must be an out-of-line function in DLL builds

Application *GetApplication()
{
	return &Camelot;
}

#endif

/***********************************************************************************************

> 	BOOL Application::Init()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	26/8/93
	
	Inputs:		-
    Outputs:    -
    Returns:   	TRUE if the object initialised ok.
                 
    Purpose:    Initialise the application, including the user preferences system. This is
				the earliest kernel-level code to be called - do NOT ADD THINGS HERE. Almost
				everything that needs to be initialised should go in InitKernel, which gets
				called after this and some OIL initialisation. If you think you qualify to
				add something here, speak to Andy first.
               
	Errors:		None.
                   			                                     
***********************************************************************************************/

BOOL Application::Init()
{
	
	// Initialise the message handler class groups
	if(!(
		MessageHandler::RegisterClassGroup(CC_RUNTIME_CLASS(MessageHandler)) && 
		MessageHandler::RegisterClassGroup(CC_RUNTIME_CLASS(Operation))		&&
		MessageHandler::RegisterClassGroup(CC_RUNTIME_CLASS(DialogOp))		&&
		MessageHandler::RegisterClassGroup(CC_RUNTIME_CLASS(OpDescriptor))	&&
		MessageHandler::RegisterClassGroup(CC_RUNTIME_CLASS(DialogBarOp))	&&
		MessageHandler::RegisterClassGroup(CC_RUNTIME_CLASS(SelRangeMessageHandler))
	  ))
	{
		return FALSE; // Failed
	}

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
PORTNOTE("colourmanager","Removed XaraCMS usage")
	// Create the Xara CMS manager - we have to get this in there
	// before a colour manager initialise otherwise there'll be trouble.
	// Note that if this fails in any way, we continue happily, simply disabling
	// the advanced colour management functions.
	CMSManager = new XaraCMS;
	if (CMSManager != NULL)
	{
		// We got one. Let's initialise it
		if (!CMSManager->Initialise(1))
		{
			delete CMSManager;
			CMSManager = NULL;
		}
	}
	CMSManager = NULL;
#else 
	CMSManager = NULL;
#endif

	// startup the noise system
	TheNoiseManager = new NoiseMan;
	if (TheNoiseManager == NULL)
		return FALSE;

	if (!TheNoiseManager->Initialise())
		return FALSE;

	// Initialise the colour system
	if (!ColourManager::Init())
		return FALSE;

	// Create and initialise the Camelot Selection object
	// This must be done AFTER registering the SelRangeMessageHandler class above,
	// as the Selection tries to register a message handler when it is constructed.
	// Thus it is necessary to defer creation of the object until the msg system
	// has been appropriately set up.
	Selection = new SelRange;
	if (Selection == NULL)
		return(FALSE);

	// startup the noise system
	// Create and initialise the colour manager
	// THis is done here for the same reasons as described for the SelRange, above.
	TheColourManager = new ColourManager;
	if (TheColourManager == NULL)
		return(FALSE);

	// create and initialise a new Blob Manager
	BlobMgr = new BlobManager;
	if (BlobMgr == NULL)
		return FALSE;

PORTNOTE("other","Removed init of stuff that is not supported")
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// create the ATM ini cache.
	pATMIniCache = new ATMIniCache;
	if (pATMIniCache == NULL)
		return FALSE;
#endif

	// Create the font mangler
	FontMgr = new FontManager;
	if (FontMgr == NULL)
		return FALSE;

	// And make sure its initialised;
	if (!FontMgr->Init())
		return FALSE;

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
#ifndef STANDALONE
#ifndef WEBSTER
// Do not do do on Webster as this conflicts with the CleanUpAfterExport code in BaseCamelotFilter

	// Create a print marks manager please
	TheMarksManager = new PrintMarksMan;
	if (TheMarksManager == NULL)
		return FALSE;
#endif // WEBSTER

#ifdef FREEHANDPLUGINS
// Only add in if required - general plug-in removal at present
	// Create the external (Freehand Xtra) API object
	TheAPIObject = new APIObject;
	if (TheAPIObject == NULL)
		return FALSE; // Actually we can probably run without this...
#endif // FREEHANDPLUGINS

#ifdef PLUGINSUPPORT
	// Create a plug-in manager please
	ThePlugInManager = new PlugInManager;
	if (ThePlugInManager == NULL)
		return FALSE;
#endif // PLUGINSUPPORT

#ifdef FREEHANDPLUGINS
	TRACEUSER( "claude", _T("new handlemanager\n"));
	TheHandleManager = new HandleManager;
	if (TheHandleManager == NULL)
		return FALSE;
#endif // FREEHANDPLUGINS

	if (!InitFilterManager())
	{
		return FALSE;
	}
	
#endif // STANDALONE
#endif // EXCLUDE_FROM_RALPH, EXCLUDE_FROM_XARALX

#ifndef EXCLUDE_FROM_XARALX
#ifdef AUTOMATION
	if (!InitCCC())
	{
		return FALSE;
	}
#endif
#endif

	// Initialise the object registry. 
	if (!ObjectRegistry::Init())
	{
		// Error has been set
		return (FALSE);  
	}

	if (!InitWizOps())
	{
		return FALSE;
	}  

#if !defined(EXCLUDE_FROM_RALPH)
	// Create and initialise the CamelotPrefs object.
	// This object is dynamically created rather than being embedded in the Appliation object
	// because the Application destructor is called after the SimpleCCObject::MemoryDump()
	// function is called.  Hence if the CamelotPrefs object were embedded, the MemoryDump
	// would list all the preference-related objects on application termination.
	CamelotPrefs = new Preferences;

	if (!((CamelotPrefs != NULL) && CamelotPrefs->Init()))
		return FALSE;
#endif

#ifdef VECTOR_STROKING // Neville 6/8/97
	m_pObjCache = new ObjectCacheFIFO(50);		// set a cache of 50 bytes

	if (m_pObjCache == NULL)
		return FALSE;
	// Set the cache size with an arbitrary value (in bytes)
	//m_ObjCache->SetMaxCacheSize(50);
#endif // VECTOR_STROKING

	// Set up the Bitmap Cache...
//#pragma message( __LOCMSG__ "Removed BitmapCache usage" )
	TheBitmapCache = new CBitmapCache();
	TheBitmapCache->Initialise();					// Initialise with default settings

	// Get an empty list object to hold all the registered doc components
	DocComponentClasses = new List;
	if (DocComponentClasses == NULL)
		return FALSE;

	// Temporary: Remove when Kernel StatusLine is actually created by Oil statusline
	m_pStatusLine = new StatusLine;

	// Eveything worked (gasp!)
	return TRUE; 
}


/***********************************************************************************************

>	BOOL Application::InitFilterManager()

    Author: 	Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
    Created:	26/12/96
    Returns:   	TRUE if OK, FALSE if failed
    Purpose:    Support function to initialize this Application's FilterManager
                   			                                     
***********************************************************************************************/
BOOL Application::InitFilterManager()
{
	BOOL Success = FALSE;

	TheFilterManager = new FilterManager;
	if (TheFilterManager != NULL)
	{
		Success = TheFilterManager->Init();
	}
	else
	{
		Success = FALSE;
	}

	return Success;
}


/***********************************************************************************************

>	BOOL Application::InitWizOps()

    Author: 	Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
    Created:	26/12/96

    Purpose:    Support function to initialize this Application's WizardOfOps from the local
				registry.

    Returns:   	TRUE if OK, FALSE if failed
                   			                                     
***********************************************************************************************/
BOOL Application::InitWizOps()
{
	BOOL Ok = TRUE;

	TheWizardOfOps = new WizOps;
	if (TheWizardOfOps != NULL)
	{
		Ok = TheWizardOfOps->InitializeFromRegistry();
	}
	else
	{
		Ok = FALSE;
	}

	return Ok;
}


#ifdef AUTOMATION
/***********************************************************************************************

>	BOOL Application::InitCCC()

    Author: 	Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
    Created:	26/03/97

    Returns:   	TRUE if OK, FALSE if failed
    Purpose:    Support function to initialize this Application's ConcurrencyController
                   			                                     
***********************************************************************************************/
BOOL Application::InitCCC()
{
	BOOL Success = FALSE;

	TheConcurrencyController = new ConcurrencyController;
	if (TheConcurrencyController != NULL)
	{
		Success = TheConcurrencyController->Init();
	}
	else
	{
		Success = FALSE;
	}

	return Success;
}
#endif	// AUTOMATION


/***********************************************************************************************

>	BOOL Application:LateInit()

    Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
    Created:	26/9/94
    Returns:   	TRUE if OK, FALSE if failed->SetError.
    Purpose:    Initialise the application object bits that require the preferences system
				to be fully up & running. Currently that means the pen only.
	Errors:		None.
                   			                                     
***********************************************************************************************/

BOOL Application::LateInit()
{
#ifdef PLUGINSUPPORT
// Only add in if required - general plug-in removal at present
	// We need the preferences system present so we must wait until here
	// before we ask the PlugInManager to initialise iteself;
	if (ThePlugInManager == NULL || !ThePlugInManager->Init())
		return FALSE;
#endif // PLUGINSUPPORT

	// As the preference system is up, declare our preference to it
	// This is the pathname that the templates should be loaded from and saved to
	// If blank, the default, then it should use the exe path
	GetApplication()->DeclareSection(TEXT("Templates"), 2);
	GetApplication()->DeclarePref(TEXT("Templates"), TEXT("Path"), &m_TemplatesPath);

	//Graham 21/10/97: If it is blank, then we should use the
	//exe path with "\templates\" on the end
	if (m_TemplatesPath.IsEmpty())
	{
		PORTNOTETRACE("other","Application::LateInit - do nothing");
#ifndef EXCLUDE_FROM_XARALX
		TCHAR Pathname[MAX_PATH];
		if( GetModuleFileName(NULL, Pathname, MAX_PATH) == 0 )
			return FALSE;

		// Put the path name into a string
		String_256 strPathname(Pathname);
		PathName ModulePath(strPathname);

		m_TemplatesPath = ModulePath.GetLocation(TRUE);

		//And add "templates\" to the end
		String_256 strRelativePath(_R(IDS_NEWTEMPLATES_RELATIVEPATH));

		m_TemplatesPath+=strRelativePath;
#endif
	}
	
	// Everything went ok
	return TRUE;
}

/********************************************************************************************

> 	void Application::Deinit()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	26/8/93
    Purpose:    Close down the kernel application object, including the user preferences 
    			system.
                   			                                     
********************************************************************************************/

void Application::Deinit()
{
#ifdef PLUGINSUPPORT
// Only add in if required - general plug-in removal at present
	// We need to ask the plug-in manager to save out its list of pathnames.
	// So before the prefs system is vaped.
	PlugInManager * pPlugMan = GetApplication()->GetPlugInManager();
	if (pPlugMan)
		pPlugMan->WritePathNameList();
#endif // PLUGINSUPPORT

#ifdef FREEHANDPLUGINS
	//	clean up the memory used by MOA
	TRACEUSER( "claude", wxT("Free up frmo the handle manager\n") );
	HandleManager* pHandleCleaning = GetApplication()->GetHandleManager();
	if(pHandleCleaning)
		pHandleCleaning->FreeAll();
#endif // FREEHANDPLUGINS

	if (CamelotPrefs!=NULL)
	{
		CamelotPrefs->Deinit();
		delete CamelotPrefs;
		CamelotPrefs = NULL;
	}

	if (Selection!=NULL)
	{
		delete Selection;
		Selection = NULL;
	}
		
	if (DocComponentClasses != NULL)
	{
		DocComponentClasses->DeleteAll();
		delete DocComponentClasses;
		DocComponentClasses = NULL;
	}

	if (TheColourManager!=NULL)
	{
		ColourManager::Deinit();
		delete TheColourManager;
		TheColourManager = NULL;
	}

	if (TheNoiseManager!=NULL)
	{
		TheNoiseManager->Destroy();
		delete TheNoiseManager;
		TheNoiseManager=NULL;
	}

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
PORTNOTE("colourmanager","Removed XaraCMS usage")
	if (CMSManager!=NULL)
	{
		CMSManager->Finalise();
		delete CMSManager;
		CMSManager = NULL;
	}

// VECTOR_STROKING - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	if (PressurePen != NULL)
	{
		delete PressurePen;
		PressurePen = NULL;
	}
#endif // VECTOR_STROKING

	// Get rid of the blob manager
	if (BlobMgr!=NULL)
	{
		delete BlobMgr;
		BlobMgr = NULL;
	}
#endif // EXCLUDE_FROM_RALPH, EXCLUDE_FROM_XARALX

PORTNOTE("other","Removed un-=impl. stuff")
#ifndef EXCLUDE_FROM_XARALX
	// Get rid of the ATM Ini cache object
	if (pATMIniCache!=NULL)
	{
		delete pATMIniCache;
		pATMIniCache = NULL;
	} */
#endif

	// Get rid of the font manager
	if (FontMgr!=NULL)
	{
		delete FontMgr;
		FontMgr = NULL;
	}

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
#ifndef STANDALONE
#ifndef WEBSTER
// Do not do do on Webster as this conflicts with the CleanUpAfterExport code in BaseCamelotFilter

	// Toast the marks manager
	if (TheMarksManager!=NULL)
	{
		delete TheMarksManager;
		TheMarksManager = NULL;
	}
#endif // WEBSTER
#endif
#endif

#ifdef PLUGINSUPPORT
// Only add in if required - general plug-in removal at present
	// Toast the plug-in manager
	if (ThePlugInManager != NULL)
	{
		delete ThePlugInManager;
		ThePlugInManager = NULL;
	}
#endif // PLUGINSUPPORT

#ifdef FREEHANDPLUGINS
	// Nuke the API interfaces (must be done after killing the plugin manager). Don't delete
	// it afterwards, it's all handled by the reference count...
	if(TheAPIObject != NULL)
		TheAPIObject->ShutDown();

	//	Delete the Moa handle manager
	if(TheHandleManager != NULL)
	{
		TRACEUSER( "claude", wxT("deleting the moa handle manager\n") );
		delete TheHandleManager;
		TheHandleManager = NULL;
	}
#endif // FREEHANDPLUGINS

	DeinitFilterManager();

	if (m_pObjCache != NULL)
	{
		delete m_pObjCache;
		m_pObjCache = NULL;
	}

//#pragma message( __LOCMSG__ "Remove BitmapCache usage" )
	if (TheBitmapCache)
	{
		TheBitmapCache->DeInitialise();
		delete TheBitmapCache;
		TheBitmapCache = NULL;
	}

	DeinitWizOps();

#ifdef AUTOMATION
	DestroyCCC();
#endif

	ObjectRegistry::Delete();
}


/***********************************************************************************************

>	BOOL Application::DeinitFilterManager()

    Author: 	Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
    Created:	26/12/96
    Returns:   	TRUE if OK, FALSE if failed
    Purpose:    Support function to deinitialize & destruct this Application's FilterManager
                   			                                     
***********************************************************************************************/
BOOL Application::DeinitFilterManager()
{
	BOOL Success = FALSE;

	FilterManager* pFilterManager = GetFilterManager();
	if (pFilterManager != NULL)
	{
		Success = TheFilterManager->DeInit();
		// ...ignore any errors
		if (!Success)
		{
			TRACE( wxT("Deinit of FilterManager not successful") );
		}
		delete TheFilterManager;
		TheFilterManager = NULL;
		Success = TRUE;
	}
	else
	{
		TRACE( wxT("No Filter Manager") );
		Success = FALSE;
	}

	return Success;
}


/***********************************************************************************************

>	BOOL Application::DeinitWizOps()

    Author: 	Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
    Created:	26/12/96

    Purpose:    Support function to deinitialize & destruct this Application's WizOps

    Returns:   	TRUE if OK, FALSE if failed
                   			                                     
***********************************************************************************************/
BOOL Application::DeinitWizOps()
{
	BOOL Ok = TRUE;

	if (TheWizardOfOps != NULL)
	{
		delete TheWizardOfOps;
		TheWizardOfOps = NULL;
	}
	else
	{
		TRACE( wxT("No WizOps") );
		Ok = FALSE;
	}

	return Ok;
}


#ifdef AUTOMATION
/***********************************************************************************************

>	BOOL Application::DestroyCCC()

    Author: 	Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
    Created:	26/12/96
    Returns:   	TRUE if OK, FALSE if failed
    Purpose:    Support function to deinitialize & destruct this Application's 
				ConcurrencyController.
                   			                                     
***********************************************************************************************/
BOOL Application::DestroyCCC()
{
	BOOL Success = FALSE;

	if (TheConcurrencyController != NULL)
	{
		delete TheConcurrencyController;
		TheConcurrencyController = NULL;
		Success = TRUE;
	}
	else
	{
		TRACE( wxT("No ConcurrencyController") );
		Success = FALSE;
	}

	return Success;
}
#endif


/***********************************************************************************************

>	RenderRegionList* Application::GetRegionList()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/95
	Purpose:	Gets a pointer to the Applications RenderRegion list.

***********************************************************************************************/

//#pragma message( __LOCMSG__ "Removed RenderRegionList usage" )
#if 1

RenderRegionList* Application::GetRegionList()
{
 	return &RenderList;
}

/***********************************************************************************************

>	void Application::AddRenderRegion(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/93
	Inputs:		pRegion - pointer to the RenderRegion to add to the list.
	Returns:	Region - the render region to add.
	Purpose:	To add a RenderRegion object to the RenderList. Automatically sets a flag
				to do immediate rendering if the render region is an immediate one.
	SeeAlso:	RenderRegion; List

***********************************************************************************************/

void Application::AddRenderRegion(RenderRegion *pRegion)
{
	// Is this render region an immediate one?

	View *pView = pRegion->GetRenderView();
	ENSURE(pView != NULL, "No View in AddRenderRegion!");

	if (!pView->GetForeBackMode())
	{
//		TRACE(_T("ImmediateRender\n"));
		// Yes - set the flag to indicate we should do some immediate rendering on the
		// next redraw event.
		ImmediateRenderPending = TRUE;
	}

	// Add this render region to the list.
	RenderList.Merge(pRegion);
}

/***********************************************************************************************

>	BOOL Application::DeleteRenderRegion(RenderRegion *Region)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/93
	Inputs:		Region - pointer to the RenderRegion to remove from the list.
	Returns:	TRUE if the region was in the list, FALSE otherwise.
	Purpose:	To remove a RenderRegion object from the RenderList.  If the
				region in not in the list, no errors occur.  The RenderRegion object will
				be destroyed (i.e. delete will be called on it).
	Errors:		-
	SeeAlso:	RenderRegion; List

***********************************************************************************************/

BOOL Application::DeleteRenderRegion(RenderRegion *Region)
{
#ifdef RALPH
		// Wait until it's safe to enter
		CCamApp::EnterGDrawCriticalSection();
#endif
	if (RenderList.FindPosition(Region) == NOT_IN_LIST)
	{
	#ifdef RALPH
		// Let another process have a go
		CCamApp::ExitGDrawCriticalSection();
	#endif
		ENSURE( FALSE, "Couldn't find Region in List" );
		return FALSE;
	}
	
	RenderList.Remove(Region);
	#ifdef RALPH
		// Let another process have a go
		CCamApp::ExitGDrawCriticalSection();
	#endif
	return TRUE;
}

#endif

/********************************************************************************************

>	BOOL Application::ServiceRendering(BOOL bImmediate = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/94
	Returns:	TRUE if more rendering needs to be done, FALSE if there are no more
				regions left to render.
	Purpose:	Render some objects!  This function will either render any outstanding 
				'immediate' render regions to completion, or render a background render
				regions for one time-slice.
	SeeAlso:	RenderRegionList::ImmediateRender; RenderRegionList::BackgroundRender

********************************************************************************************/

BOOL Application::ServiceRendering(BOOL bImmediate)
{ 
//#pragma message( __LOCMSG__ "Application::ServiceRendering - do nothing" )
//	TRACE( wxT("Warning - Application::ServiceRendering called\n") );
#ifdef RALPH
#ifdef _DEBUG
	if(::GetCurrentThreadId() == RalphDocument::GetImportingThreadID())
	{
		// If we get in here, it's extremely BAD !
		AfxDebugBreak();
	}
#endif

	// Fixes splodgy text problems...
	RalphCriticalSection RCS;
#endif

#ifdef RALPH
		// Wait until it's safe to enter
		CCamApp::EnterGDrawCriticalSection();
#endif

	// DMc - render the list of nodes to regenerate
	RegenerateNodesInList();

//	TRACEUSER( "Gerry", _T("Service Rendering - Enter\n"));
	if (ImmediateRenderPending || bImmediate)
	{
		// Render all immediate render regions to completion, and then clear the
		// flag so we do some background rendering the next time around.
		RenderList.ImmediateRender(bImmediate);
		ImmediateRenderPending = FALSE;

	#ifdef RALPH
		// Let another process have a go
		CCamApp::ExitGDrawCriticalSection();
	#endif

		// Try background rendering next
//		TRACEUSER( "Gerry", _T("Service Rendering - Exit - More\n"));
		return TRUE;
	}
	else
	{
		BOOL ok = RenderList.BackgroundRender();
	#ifdef RALPH
		// Let another process have a go
		CCamApp::ExitGDrawCriticalSection();
	#endif

//		TRACEUSER( "Gerry", _T("Service Rendering - Exit - %s\n"), ok ? _T("More") : _T("Finished"));
		return ok;
	}

//	TRACEUSER( "Gerry", _T("Service Rendering - Exit - More\n"));
	return TRUE;
}


/********************************************************************************************

>	BOOL Application::IsBgRendering()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	Returns:	TRUE if Background rendering is in progress.
	Purpose:	Find out if there is Background rendering in progress or not.

********************************************************************************************/

BOOL Application::IsBgRendering()
{
	return BgRendering;
}



/********************************************************************************************

>	void Application::RegisterDocComponent(DocComponentClass *pClass)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pClass - pointer to the Document component class thatis to be added.
	Purpose:	Register a new Document Component class with the application.  This class
				will be asked to add an instance to a document every time a new document
				is created. 
	SeeAlso:	DocComponentClass; DocComponent

********************************************************************************************/

void Application::RegisterDocComponent(DocComponentClass *pClass)
{
	// Check for multiple registrations in debug build...
	#if _DEBUG
	DocComponentClass *pItem = (DocComponentClass *) DocComponentClasses->GetHead();

	while (pItem != NULL)
	{
		// Already registered?
		if (pItem->GetRuntimeClass() == pClass->GetRuntimeClass())
		{
			ENSURE(FALSE, "Document component registered more than once!");
			return;
		}
		// Try the next one
		pItem = (DocComponentClass *) DocComponentClasses->GetNext(pItem);
	}
	#endif

	// Simple - just add it to the list
	DocComponentClasses->AddTail(pClass);
}


/********************************************************************************************

>	DocComponentClass *Application::EnumerateDocComponents(DocComponentClass *pContext)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pContext: NULL => Get the first document component
					  non-NULL => Get the next document component after the one pointed to
								  by pContext.
	Returns:	Pointer to the required document component.
	Purpose:	Allow the caller to examine/use all the registered document components.
				pContext should be NULL for the first call, which will return the first
				registered document component.  After that, the returned pointer should
				be passed back in to this function, which will return the next one, and
				so on.
				For example:
				MonoOn
					DocComponentClass *pDocComp = NULL;
					do
					{
						pDocComp = Camelot.EnumerateDocComponents(pDocComp);

						// Do something groovy with pDocComp...
					}
					while (pDocComp != NULL);
				MonoOff

	SeeAlso:	Application::RegisterDocComponent;
				DocComponent; 
				DocComponentClass

********************************************************************************************/

DocComponentClass *Application::EnumerateDocComponents(DocComponentClass *pContext)
{
	if (pContext == NULL)
	{
		// Return the first item in the list...
		return (DocComponentClass *) (DocComponentClasses->GetHead());
	}
	else
	{
		// Return the next document component...
		return (DocComponentClass *) (DocComponentClasses->GetNext(pContext));
	}
}


/********************************************************************************************

>	BOOL Application::ApplyDocComponents(Document *pDocument)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pDocument - the document to add the components to.
	Returns:	TRUE if all the components were added successfully;
				FALSE if not.
	Purpose:	Given a document, apply all the currently registered document components
				to it.  i.e., add a colour table, units information, etc.
	Errors:		Out of memory, or other errors depending on implementations of each
				document component.
	SeeAlso:	DocComponent; 
				DocComponentClass

********************************************************************************************/

BOOL Application::ApplyDocComponents(BaseDocument *pDocument)
{
	// Go through the registered components and add their virtual function to add an
	// instance of the class to the Document object...
	DocComponentClass *pItem = (DocComponentClass *) DocComponentClasses->GetHead();

	while (pItem != NULL)
	{
		if (!pItem->AddComponent(pDocument))
		{
			// Component could not be added - probably out of memory.
			return FALSE;
		}
		// Try the next one
		pItem = (DocComponentClass *) DocComponentClasses->GetNext(pItem);
	}

	// All the components were successfully added
	return TRUE;
}

/********************************************************************************************
>	BOOL Application::OnKeyPress(KeyPress* pKeyPress)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (simplified by JustinF)
	Created:	26/8/94
	Inputs:		pKeyPress =Ptr to platform-indy key press object
	Returns:	TRUE  if the key press was processed, so do not pass on.
				FALSE if not processed, so pass on to other interested parties
	Purpose:	This is where key presses are handled in Camelot. It does the following in this order -

					1) Lets the current tool have a go
					2) See if a temporary tool switch has occurred
					3) Check for a hot-key combination
					[ 4) Broadcast key press to all live ops - removed as it slows text typing by 20% ]

				If at any of the stages the key is processed, this function exits immediately with TRUE,
				i.e. it doesn't let the next stage try and deal with a key press that's been processed
				
	Errors:		-
	SeeAlso:	KeyPress::TranslateMessage
********************************************************************************************/

BOOL Application::OnKeyPress(KeyPress* pKeyPress)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
PORTNOTE("other","Removed XaraCMS usage")
	// First, pass the keypress to the Colour Editor.
	// This is used to stop keypresses while dragging in the editor, so must be done first
	if (ColourEditDlg::OnKeyPress(pKeyPress))
		return(TRUE);
#endif

	// Pass this keypress on to selected DocView, if we have one, otherwise
	// pass on to tools, hotkeys etc.
	DocView* pDocView = DocView::GetSelected();
	return ( (pDocView != NULL && pDocView->HandleKeyPress(pKeyPress))
			|| Tool::SwitchTool(pKeyPress)
			|| HotKey::OnKeyPress(pKeyPress)
// removed by Ed 27/4/95 - no-one uses this and it slows text typing by 20%!
//			|| (BROADCAST_TO_ALL(KeyMsg(KeyMsg::KEYPRESS, pKeyPress)) == EAT_MSG )
			);
}



/********************************************************************************************

>	void Application::RegisterIdleProcessor(INT32 Priority, Operation* pOp)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		Priority - either IDLEPRIORITY_HIGH or IDLEPRIORITY_LOW.
				pOp - Pointer to an operation.
	Returns:	-
	Purpose:	If an operation wants to do processing on idle events then it must register
				to receive them.  It registers by calling this function with a pointer to
				itself.  On idle events the operations OnIdle function is called.

	Notes:		If the operator is already registered for idles of the given priority,
				this call does nothing (i.e. registering twice will give you only one
				entry in the list)

				HIGH priority handlers are called on every Idle event. LOW priority
				handlers will only be called on idle events if there were no high
				priority handlers which returned TRUE from their OnIdle method. 
				i.e. If any high priority handler is active, all low-priority idles
				cease to occur.

	Errors:		-
	SeeAlso:	Operation::OnIdleEvent, Application::CallIdleProcessors, 
				Application::RemoveIdleProcessor

********************************************************************************************/

void Application::RegisterIdleProcessor(INT32 Priority, Operation* pOp)
{
	List* Current = (Priority==IDLEPRIORITY_HIGH) ? &IdleHighPriorityOps : &IdleLowPriorityOps;

	// Ensure the given Op is only registered once on the list
	ListItemOpPtr* Entry = (ListItemOpPtr *)Current->GetHead();
	while (Entry != NULL)
	{
		if (Entry->pOp == pOp)		// Hey, you're already registered!
			return;

		Entry = (ListItemOpPtr *) Current->GetNext(Entry);
	}

	// Have not found an entry in this list for this Op, so add one
	Entry = new ListItemOpPtr;
	Entry->pOp = pOp;
	Current->AddTail(Entry);
}



/********************************************************************************************

>	void Application::RemoveIdleProcessor(INT32 Priority, Operation* pOp)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		Priority - either IDLEPRIORITY_HIGH or IDLEPRIORITY_LOW.
				pOp - Pointer to an operation.
	Returns:	-
	Purpose:	Stops an operation from receiving idle events.

	Notes:		If the Operation was not registered for idles, this call returns
				quietly. This allows you to ensure you are deregistered without
				having to remember if you did/didn't register for idles.

				It is safe to call this method when processing an idle event - the event
				processing code ensures that if you remove yourself from the list, it does
				not give a toss (or an access violation!).

	Errors:		-
	SeeAlso:	Application::CallIdleProcessors

********************************************************************************************/

void Application::RemoveIdleProcessor(INT32 Priority, Operation* pOp)
{
	List* Current = (Priority==IDLEPRIORITY_HIGH) ? &IdleHighPriorityOps : &IdleLowPriorityOps;
	ListItemOpPtr* Entry = (ListItemOpPtr *)Current->GetHead();

	while (Entry != NULL)
	{
		if (Entry->pOp == pOp)
		{
			Current->RemoveItem(Entry);
			delete Entry;
			return;
		}

		Entry = (ListItemOpPtr *) Current->GetNext(Entry);
	}
}



/********************************************************************************************

>	BOOL Application::CallIdleProcessors()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		-
	Returns:	TRUE if the registered operations require more idle events.
				FALSE if no operations need further idle events.

	Purpose:	To call registered operations OnIdle functions.

	Notes:		It is assumed that if there are any processes registered, we still 
				require idle events.

				When idle processsor methods are called, they return the following values:
				HIGH PRIORITY:	TRUE  = (claim) DO NOT call low-priority processors
								FALSE = (don't claim) DO call low-priority processors

				LOW PRIORITY:	The return code is ignored.

				This allows us to drag in the DragManager, updating fequently on high-priority
				idles, but when the mouse is not moving, allow low-priority idles to be
				called (which allows scrolling in galleries to be highly responsive, but
				background redraws will kick into action very fast while the mouse stays still)

				Most high-priority handlers will just override all events while they're running,
				though there should never be very many high-priority handlers around.

				[TODO: Perhaps low priority handlers should always be called once in a while (e.g.
				always call them if they have not been called for a second or so), so that
				we can never get inot a complete-lock-out situation.]

	Errors:		-
	SeeAlso:	Application::RegisterIdleProcessor

********************************************************************************************/

BOOL Application::CallIdleProcessors()
{
	// If the system is disabled (due to an error box being up) we must not do anything,
	// as we definitely do not want to cause a re-entrant error situation!
	if( CCamApp::IsDisabled() )
		return(TRUE);

	BOOL CallLowPriorityHandlers = TRUE;

	ListItemOpPtr *CurrentOp = (ListItemOpPtr*)IdleHighPriorityOps.GetHead();
	ListItemOpPtr *NextOp;

	// If there are no registered processors, we'll return FALSE as idles aren't needed
	BOOL MoreIdlesNeeded = CurrentOp != NULL;

	while (CurrentOp != NULL)
	{
		// Remember the next item in the list now, in case this one deregisters itself
		NextOp = (ListItemOpPtr *) IdleHighPriorityOps.GetNext(CurrentOp);

		// Call the handler, and if it claims idles, disable calling of low-priority handlers
		if (CurrentOp->pOp->OnIdleEvent())
			CallLowPriorityHandlers = FALSE;

		CurrentOp = NextOp;
	}

	// If nobody objects to a low-priority pass, do one
	if (CallLowPriorityHandlers)
	{
		CurrentOp = (ListItemOpPtr*)IdleLowPriorityOps.GetHead();
		if (CurrentOp != NULL)
			MoreIdlesNeeded = TRUE;

		while (CurrentOp != NULL)
		{
			// Remember the next item in the list now, in case this one deregisters itself
			NextOp = (ListItemOpPtr *) IdleLowPriorityOps.GetNext(CurrentOp);				
			CurrentOp->pOp->OnIdleEvent();
			CurrentOp = NextOp;
		}
	}

	return MoreIdlesNeeded;
}



/********************************************************************************************

>	BOOL Application::OnIdle(BOOL IdleRedraw)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		IdleRedraw - TRUE if background redrawing should be performed.
	Returns:	TRUE if more idle messages are needed.
				FALSE if no more idle processing is needed 
	Purpose:	This is where idle events are recived by the kernel.
	Errors:		-
	SeeAlso:	CCamApp::OnIdle

********************************************************************************************/

BOOL Application::OnIdle(BOOL IdleRedraw)
{
	// If the system is disabled (due to an error box being up) we must not do anything,
	// as we definitely do not want to cause a re-entrant error situation!
	if( CCamApp::IsDisabled() )
		return(TRUE);

	Tool* CurTool=Tool::GetCurrent();
	BOOL  Idles  =FALSE;

	if (IdleRedraw)
		if (ServiceRendering())
			Idles=TRUE;

	if (CurTool!=NULL)
		if (CurTool->OnIdle())
			Idles=TRUE;

	if (CallIdleProcessors())
		Idles=TRUE;

//	ControlList::Idle();

	StatusLine* pStatusLine=GetpStatusLine();
	if (pStatusLine!=NULL)
		pStatusLine->OnIdle();
//	else
//		ERROR3("Application::OnIdle() - pStatusLine==NULL");

//	Idles=TRUE;

	return Idles;
}

/***********************************************************************************************

>	BOOL Application::CreateDragTargets()
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	tell all documents to create a drag target for each of their views 
	SeeAlso:	-
*********************************************************************************************/


BOOL Application::CreateDragTargets(DragInformation * DragInfo)
{
	Document* pDocument = (Document*) Documents.GetHead();
	while (pDocument != NULL)
	{
		pDocument->CreateDragTargets(DragInfo);

		pDocument = (Document*) Documents.GetNext(pDocument);
	}
	return TRUE;
}

/***********************************************************************************************

>	void Application::ShowViewScrollers(BOOL fIsVisible)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Show or Hide all Scrollers on all Views on all Documents
	SeeAlso:	-
*********************************************************************************************/


void Application::ShowViewScrollers(BOOL fIsVisible)
{
	Document* pDocument = (Document*) Documents.GetHead();
	while (pDocument != NULL)
	{
		pDocument->ShowViewScrollers(fIsVisible);
		pDocument = (Document*) Documents.GetNext(pDocument);
	}

}


/********************************************************************************************
>	BOOL Application::UpdateStatusBarText(String_256* text, BOOL PrefixSelDesc=TRUE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		ptext         - text to go in the specified status bar pane
				PrefixSelDesc - TRUE to prefix the displayed text with a selection description
	Returns:	FALSE if falied
	Purpose:	Just interfaces to similar StatusLine funtion
	Notes:		This has been bodged by Jason to return without an error if the status line has
				not yet been initialised as the progress system is tries to use it in this state!
********************************************************************************************/

BOOL Application::UpdateStatusBarText(String_256* ptext, BOOL PrefixSelDesc)
{
	StatusLine* pStatusLine = GetpStatusLine();

//	ERROR2IF(pStatusLine==NULL,FALSE,"Application::UpdateStatusBarText() - pStatusLine==NULL");
	if (pStatusLine==NULL)
		return FALSE;	
	
	return pStatusLine->UpdateText(ptext,PrefixSelDesc);
}



/********************************************************************************************
>	BOOL Application::LockOutControlHelp ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/2000
	Inputs:		N/A
	Returns:	Whatever our helper function feels like.
	Purpose:	Just interfaces to similar StatusLine funtion
	Notes:		This is really a bodge to fix the annoying features of camelot that persist
				in infinitely updating the status bar with different text.  At present, this
				occurs in two places (known to me):  the transparency slider and when typing
				text within the text tool.  This function locks the status bar to prevent
				these guys such annoying things.  NOTE:  if one calls this - then you MUST
				also have a matching Application::UnlockControlHelp () somewhere; cause
				otherwise camelots status bar will never again update itself ....

				NOTE:  this function can also be used to prevent the status bar from updating
					   at all (as in the solution to the text tool probs) ....	
	See Also:	Application::UnlockControlHelp ()
********************************************************************************************/

BOOL Application::LockOutControlHelp ()
{
	StatusLine* pStatusLine = GetpStatusLine ();

	if (pStatusLine==NULL)
		return FALSE;	

	return (pStatusLine->LockOutControlHelp ());
}



/********************************************************************************************
>	BOOL Application::UnlockControlHelp ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/2000
	Inputs:		N/A
	Returns:	Whatever our helper function feels like.
	Purpose:	Just interfaces to similar StatusLine funtion
	Notes:		See Application::LockOutControlHelp ()
	See Also:	Application::LockOutControlHelp ()
********************************************************************************************/

BOOL Application::UnlockControlHelp ()
{
	StatusLine* pStatusLine = GetpStatusLine ();

	if (pStatusLine==NULL)
		return FALSE;	

	return (pStatusLine->UnlockControlHelp ());
}



/********************************************************************************************
>	CCStatusBar* Application::GetpCCStatusBar()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Purpose:	return pointer to CMainFrame::CCStatusBar
	Returns:	pointer to CMainFrame::CCStatusBar else NULL if error (see Errors)
********************************************************************************************/

CCStatusBar* Application::GetpCCStatusBar()
{
	PORTNOTETRACE("other","Application::GetpCCStatusBar - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	CMainFrame* mfwindow = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
	ERROR2IF(mfwindow==NULL,NULL,"Application::GetpCCStatusBar() - mfwindow==NULL");

	return mfwindow->GetpCCStatusBar();
#else
	return NULL;
#endif
}


/********************************************************************************************
>	StatusLine* Application::GetpStatusLine()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Returns:	Pointer to CMainFrame::StatusLine else NULL if error (or not initialised)
	Notes:		This has been bodged by Jason to return without an error if the mainframe has
				not yet been initialised as the progress system is tries to use it in this state!
********************************************************************************************/

StatusLine* Application::GetpStatusLine()
{
PORTNOTE("statusline", "getpStatusLine returning disconnected Kernel StatusLine while there is no Oil version");
#ifndef EXCLUDE_FROM_XARALX
	CMainFrame* mfwindow=(CMainFrame*)(AfxGetApp()->m_pMainWnd);

//	ERROR2IF(mfwindow==NULL,NULL,"Application::GetpStatusLinet() - mfwindow==NULL");
	if (mfwindow==NULL)
		return NULL;

	return mfwindow->GetpStatusLine();
#else
	// This is only temporary
	return m_pStatusLine;

#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Comments for the help system...

/***********************************************************************************************

>	inline ColourManager *Application::FindTheColourManager(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the system Colour manager object
	Purpose:	To find the single 'global' ColourManager instance in Camelot
	SeeAlso:	ColourManager

***********************************************************************************************/



/***********************************************************************************************

>	void Application::DumpAllOps(Document* pDoc = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/95
	Inputs:		pDoc points to the document containing the operation histor to dump, NULL for
				selected doc.
	Outputs:	Displays debug info
	Returns:	-
	Purpose:	Dumps information about all the operations in the history list to the debugger
	SeeAlso:	Application::DumpLastOp

***********************************************************************************************/
void Application::DumpAllOps(Document* pDoc)
{
	PORTNOTETRACE("other","Application::DumpAllOps - do nothing");
#ifndef EXCLUDE_FROM_XARALX
#if DEBUG_TREE
	if (pDoc == NULL)
		pDoc = Document::GetSelected();

	pDoc->GetOpHistory().DumpAll();
#endif
#endif
}



/***********************************************************************************************

>	void Application::DumpLastOp(Document* pDoc = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/95
	Inputs:		pDoc points to the document containing the operation histor to dump, NULL for
				selected doc.
	Outputs:	Displays debug info
	Returns:	-
	Purpose:	Dumps information about the last operation in the history list to the debugger
	SeeAlso:	Application::DumpAllOps

***********************************************************************************************/
void Application::DumpLastOp(Document* pDoc)
{
	PORTNOTE("other","Application::DumpLastOp - do nothing")
#ifndef EXCLUDE_FROM_XARALX
#if DEBUG_TREE
	if (pDoc == NULL)
		pDoc = Document::GetSelected();

	pDoc->GetOpHistory().DumpLast();
#endif
#endif
}



/***********************************************************************************************

>	void Application::DumpTree(BaseDocument* pDoc = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/95
	Inputs:		pDoc points to the document containing the operation histor to dump, NULL for
				selected doc.
	Outputs:	Displays debug info
	Returns:	-
	Purpose:	Dumps information about the document tree to the debugger
	SeeAlso:	Application::DumpAllOps

***********************************************************************************************/
void Application::DumpTree(BaseDocument* pDoc)
{
PORTNOTE("other","Removed DebugTreeDlg usage")
#ifndef EXCLUDE_FROM_XARALX
#if DEBUG_TREE
	DebugTreeDlg::TweeDump(pDoc);
#endif
#endif
}



/***********************************************************************************************

>	CCPen* Application::GetPressurePen()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/97

	Returns:	NULL, or a pointer to the application wide instance of the pressure pen
	Purpose:	Dumps information about the document tree to the debugger
	SeeAlso:	Application::DumpAllOps

***********************************************************************************************/

// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING

CCPen* Application::GetPressurePen()
{
	// If we haven't tried initialising the pen, then do so now.
	// (i.e. this is demand-initialised)
	if (!PenInitialised)
	{
		PenInitialised = TRUE;
		PressurePen = CCPen::Init();	// This will return NULL if it fails, but that's OK
	}

	return(PressurePen);
}

#endif // VECTOR_STROKING


/***********************************************************************************************

>	ObjectCache* Application::GetObjectCache()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/97
	Inputs:		-
	Outputs:	-
	Returns:	a pointer to the cache object
	Purpose:	return a pointer to the cache object so that external objects can add object in 
				the cache.
	SeeAlso:	Application::Init(), Application::DeInit()

***********************************************************************************************/

ObjectCache* Application::GetObjectCache()
{
	return m_pObjCache;
}


/***********************************************************************************************

>	String_256& Application::GetTemplatesPath()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/03/97
	Inputs:		-
	Outputs:	-
	Returns:	The specfied templates path in a string
	Purpose:	To find out the user's preference for a path to the template file.
	SeeAlso:	CCamDoc::LoadDefaultDocument; OpMenuSave::Do;

***********************************************************************************************/

String_256& Application::GetTemplatesPath()
{
	// Add a trailing slash if it hasn't got one
	SGLibOil::AppendSlashIfNotPresent(&m_TemplatesPath);

	return m_TemplatesPath;
}

/***********************************************************************************************

>	void Application::SetTemplatesPath(String_256& strToSet)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/20/97
	Inputs:		strToSet - the new value
	Returns:	-
	Purpose:	To set the default templates path
	SeeAlso:	-

***********************************************************************************************/

void Application::SetTemplatesPath(String_256& strToSet)
{
	m_TemplatesPath=strToSet;
}

/********************************************************************************************

>	void DocView::RegenerateNodesInList()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/99
	Inputs:		The node to add to the list
	Outputs:	-
	Returns:	-
	Purpose:	Calls all of the regeneration functions of the nodes in the list
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Application::RegenerateNodesInList()
{
	if (RegenList.IsEmpty())
		return;

	NodeListItem *pItem = (NodeListItem *)RegenList.GetHead();

	while (pItem)
	{
		if (pItem->pNode)
		{
			if (pItem->pNode->GetHiddenCnt()==0)
			{
				if (pItem->pNode->IsBounded())
				{
					((NodeRenderableBounded *)pItem->pNode)->InvalidateBoundingRect();
				}

				pItem->pNode->RegenerateNode(NULL, FALSE, FALSE);
				
				if (pItem->pNode->IsBounded())
				{
					((NodeRenderableBounded *)pItem->pNode)->InvalidateBoundingRect();
				}
			}
		}

		pItem = (NodeListItem *)RegenList.GetNext(pItem);
	}

	RegenList.DeleteAll();
}

/********************************************************************************************

>	BOOL Application::AddNodeToRegenList(Node * pNode)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/99
	Inputs:		The node to add to the list
	Outputs:	-
	Returns:	-
	Purpose:	Adds a node to the regeneration list
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Application::AddNodeToRegenList(Node * pNode)
{
	// check to see if the node is already in the list
	NodeListItem * pItem = (NodeListItem *)RegenList.GetHead();

	BOOL bAdd = TRUE;

	while (pItem)
	{
		if (pItem->pNode == pNode)
		{
			bAdd = FALSE;
			break;
		}

		pItem = (NodeListItem *)RegenList.GetNext(pItem);
	}

	if (!bAdd)
		return TRUE;

	pItem = new NodeListItem(pNode);

	ERROR2IF(!pItem, FALSE, "Memory Error !");

	RegenList.AddTail(pItem);

	return TRUE;
}


/********************************************************************************************

>	BOOL Application::ChangeRealDownloadBrowserOtion (HKEY rootKey, HKEY key, BOOL changeVal)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/2000
	Inputs:		rootKey - HKEY_LOCAL_MACHINE or HKEY_CURRENT_USER (not checked)
				key		- already opened subpath key (to realdownload directory)
				changeVal - value to change the realdownload "EnableBrowserWatch" key to
	Outputs:	-
	Returns:	TRUE if changed value, FALSE otherwise
	Purpose:	Change the RealDownload "EnableBrowserWatch" key value
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PORTNOTE("other","Removed HKEY usage")
#ifndef EXCLUDE_FROM_XARALX
BOOL Application::ChangeRealDownloadBrowserOtion (HKEY rootKey, HKEY key, BOOL changeVal)
{
	LPCTSTR path = "software\\realnetworks\\realdownload\\";
	
	const INT32 buf_size = 256;
	DWORD dwIndex	= 0;				// index of subkey to enumerate 
	TCHAR Name[buf_size];				// buffer for subkey name
	DWORD cbName	= buf_size;			// size of subkey buffer 
	TCHAR Class[buf_size];				// buffer for class string 
	DWORD cbClass	= buf_size;			// size of class buffer 
	FILETIME ftLastWriteTime;			// time key last written to

	BOOL returnVal = FALSE;

	INT32 nResult = ERROR_SUCCESS;
//	String_256 KeyName;

	do
	{
		// reset the string sizes to their maximum for receiving data
		cbName	= buf_size;
		cbClass	= buf_size;

		// Does another sub-key exist?
		nResult = ::RegEnumKeyEx(key, dwIndex, Name, &cbName, NULL, Class, &cbClass, &ftLastWriteTime);

		if (nResult == ERROR_SUCCESS)
		{
			// weve hit a real download version number - scan it ....

			char subpath [100];
			char newpath [200];
			
			sprintf (subpath, "%s", Name);
			sprintf (newpath, "%s%s", path, subpath);

			HKEY subkey = OpenRegKey (rootKey, newpath);

			DWORD newval = (DWORD) changeVal;

			if (subkey)
			{	
				DWORD type, dataVal, dataSize;

				if (RegQueryValueEx (subkey, "EnableBrowserWatch", NULL, &type, (unsigned char*) &dataVal, &dataSize) == ERROR_SUCCESS)
				{
					BOOL currentVal = (BOOL) dataVal;

					if (currentVal == TRUE)
					{
						if (RegSetValueEx (subkey, "EnableBrowserWatch", NULL, REG_DWORD, (unsigned char*) &newval, sizeof (newval)) == ERROR_SUCCESS)
						{
							RegCloseKey (subkey);
							return (TRUE);
						}
						else
						{
							RegCloseKey (subkey);
							return (FALSE);
						}
					}
					else if ((currentVal == FALSE) && (changeVal == TRUE))		// were resetting to TRUE
					{
						if (RegSetValueEx (subkey, "EnableBrowserWatch", NULL, REG_DWORD, (unsigned char*) &newval, sizeof (newval)) == ERROR_SUCCESS)
						{
							RegCloseKey (subkey);
							return (TRUE);
						}
						else
						{
							RegCloseKey (subkey);
							return (FALSE);
						}
					}
				}

				RegCloseKey (subkey);
			}
			
			//			KeyName = Name;
		}

		// increment our indexing item and try again
		dwIndex ++;

	} while (nResult == ERROR_SUCCESS);

	if (DisabledRealDownload)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}

	return false;
}
#endif

/********************************************************************************************

>	void Application::CheckRealDownload ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Disables RealDownload browser integration if necessary - to prevent camelot
				from dying when downloading files that are registered with RealDownload.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Application::CheckRealDownload ()
{
	PORTNOTE("other","Application::CheckRealDownload - do nothing")
#ifndef EXCLUDE_FROM_XARALX
	key = OpenRegKey (HKEY_CURRENT_USER, "software\\realnetworks\\realdownload\\");

	if (key)
	{
		// disable it ....
		DisabledRealDownload = ChangeRealDownloadBrowserOtion (HKEY_CURRENT_USER, key, FALSE);

		if (DisabledRealDownload)
		{
			key2 = OpenRegKey (HKEY_LOCAL_MACHINE, "software\\realnetworks\\realdownload\\");

			if (key2)
			{
				// disable it ....
				ChangeRealDownloadBrowserOtion (HKEY_LOCAL_MACHINE, key2, FALSE);
			}
		}
	}
	else
	{
		key2 = OpenRegKey (HKEY_LOCAL_MACHINE, "software\\realnetworks\\realdownload\\");

		if (key2)
		{
			// disable it ....
			DisabledRealDownload = ChangeRealDownloadBrowserOtion (HKEY_LOCAL_MACHINE, key2, FALSE);
		}
	}
#endif
}


/********************************************************************************************

>	void Application::ResetRealDownload ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Re-enables RealDownload browser integration if we disabled it.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Application::ResetRealDownload ()
{
	PORTNOTE("other","Application::ResetRealDownload - do nothing")
#ifndef EXCLUDE_FROM_XARALX
	if (DisabledRealDownload)
	{
		if (key)
		{
			// both HKEY_CURRENT_USER and HKEY_LOCAL_MACHINE could have been used, reset both ....

			ChangeRealDownloadBrowserOtion (HKEY_CURRENT_USER, key, TRUE);
			RegCloseKey (key);

			if (key2)
			{
				ChangeRealDownloadBrowserOtion (HKEY_LOCAL_MACHINE, key2, TRUE);
				RegCloseKey (key2);
			}
			
			DisabledRealDownload = FALSE;
		}
		else if (key2)
		{
			// only HKEY_LOCAL_MACHINE will have been used, reset it ....

			ChangeRealDownloadBrowserOtion (HKEY_LOCAL_MACHINE, key2, TRUE);
			RegCloseKey (key2);

			DisabledRealDownload = FALSE;
		}
	}
#endif
}




/********************************************************************************************

>	BOOL Application::GetConvertToEditableShapesDPI (INT32* pValue)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/11/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Wrapper function for gaining access to convert to editable shapes DPI
				(as requested by karim).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Application::GetConvertToEditableShapesDPI (INT32* pValue)
{
	return (Camelot.GetPrefValue(TEXT("Displays"),TEXT("CompCToEDPI"),pValue));
}




