// $Id: app.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_APP
#define INC_APP

#include <limits.h>
#include <float.h>

#include "list.h"
#include "rgnlist.h"
#include "ccobject.h"
#include "prefs.h"
//#include "range.h"
//#include "sentryop.h"
#include "bmplist.h"
#include "fraclist.h"
//#include "bitmapcachekey.h"

class CCPen;
class RenderRegion;		// #include "rndrgn.h"
class RenderRegionList;
class BlobManager;		// #include "blobs.h"
class DocComponentClass; // #include "doccomp.h"
class KeyPress;
class CCStatusBar;
class StatusLine;
class DragInformation;
class ATMIniCache;
class FontManager;
class ColourManager;
class XaraCMS;
class PrintMarksMan;
class PlugInManager;
class FilterManager;
class APIObject;
class HandleManager;
class NoiseMan;
class ObjectCache;
class SelRange;
class ConcurrencyController;
class WizOps;
class CBitmapCache;

#define FONTMANAGER (GetApplication()->GetFontManager())
#define NOISEMANAGER (GetApplication()->GetNoiseManager())

/*********************************************************************************************

>	class Application : public SimpleCCObject

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/1993
	Base Class:	None
	Purpose:	Represents an instance of the Camelot application.
	
				\b General \b0
				
				There is only one Application object; it is a global, and it is called
				Camelot.
				
				The data member 'Documents' is a list of 'Document' objects associated
				with this instance of the application.  Any documents left in this list
				are deleted by the Application's destructor.
				
				The data member 'RenderRegions' is a RenderRegionList object, and is the
				list of RenderRegions outstanding for this instance of the application.

				The data member 'Selection' is a SelRange object which caches information
				about the current selection for quick access. NOTE: There is only one 
				Selection object and it is used by all documents because the selection 
				follows the input focus - when the user selects something in a document 
				any existing selection in any other document is cleared.
				So that the inline functions get fast access to the SelRange object the 
				Application class is a friend of the SelRange class.
                
                \b User Preferences \b0

				The Camelot object is responsible for controlling the application 
				preferences.
				
				When a client wishes to use a preference, they should allocate some space to
				hold its value, and then use DeclarePref to inform the preferences system
				of its existence.
				
				e.g.
				MonoOn
					INT32 AutoSaveDelay;
					Camelot.DeclarePref("Global", "AutoSaveDelay", &AutoSaveDelay);
				MonoOff
				
				This will inform the preferences system that there is a preference called
				\b AutoSaveDelay \b0 in the section called \b Global \b0 (the names of the 
				sections and preferences are always case-insensitive).  The \b DeclarePref \b0
				call will also attempt to read this preference from the application's 
				preference file - if it exists, it will read the value and store it in the 
				\i AutoSaveDelay \i0 variable. The \b DeclarePref \b0 function is currently
				overloaded for INTs, UINTs,	doubles, and Camelot String objects.
				
				\b NB! You should NOT pass the address of an \i auto \i0 variable to 
				DeclarePref, because this will obviously be invalid as soon as your function
				exits, and will cause interesting values to be written to the .ini file - hence
				all preference variables should be declared as \i static \i0 . \b0
				(This should be fairly obvious though, because actually using a preference
				variable that isn't static wouldn't really work all that well.)
				
				Another effect of declaring a preference is that when the application saves
				the preferences out (e.g. on normal application close-down), the preference
				is automatically saved out under its correct name and section, using the
				value contained in (assuming the above example) \i AutoSaveDelay \i0 when the 
				save occurs.
				
				Note that before a preference can be declared, its parent section \i must \i0
				have been declared via a call to \b DeclareSection \b0:
				
				MonoOn
					Application::DeclareSection("Global", 100);
				MonoOff
				
				The value of 100 indicates that the caller expects there to about 100
				preferences stored in this section - this is used as a guideline by the
				preferences system when allocating memory for preferences.  This is not an
				absolute limit - the section is automatically extended when necessary, but
				the more accurate the initial estimate, the more efficient the memory
				usage will be.  Obviously for some sections, the accuracy is limited - for
				example, it may depend on how many tools the user loads/uses.
                
                
	Errors:		None.
				
*********************************************************************************************/

class CCAPI Application : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(Application);
	
public:       

	Application();
	~Application();

	BOOL Init();
	BOOL LateInit();
	void Deinit();
	
	List Documents;

	BOOL CreateDragTargets(DragInformation * DragInfo);

//#pragma message( __LOCMSG__ "Removed RenderRegionList usage" )
	void AddRenderRegion(RenderRegion *);
	BOOL DeleteRenderRegion(RenderRegion *);
	void DeleteRenderRegions(Document *pDoc) 
		{ RenderList.Remove(pDoc); };
	void DeleteRenderRegions(View *pView) 
		{ RenderList.Remove(pView); };
	RenderRegionList* GetRegionList();
	// CGS:  used by the brush system.
	// allows an extra check prior to doing the screen capture inside freehand.cpp (OnIdle ())
	BOOL IsRenderingComplete () { return (RenderList.IsEmpty ()); }
	BOOL ServiceRendering(BOOL bImmediate = FALSE);

	BOOL IsBgRendering();
	BOOL BgRendering;

	inline ColourManager *FindTheColourManager(void)
		{ return(TheColourManager); };

	SelRange *Selection;				// Cached information about the current selection

// These global functions provide VERY fast access to the global Selection object...
	inline void UpdateSelection()		// Inform the global Selection object that
		{ Selection->Update(); };		// the selection may have changed.

	inline SelRange* FindSelection()	// Retreive a ptr to the global Selection
		{ return(Selection); };	 		// SelRange object.

// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	// This functions allows access to the global pressure pen.
	CCPen* GetPressurePen();
#endif // VECTOR_STROKING

	// Functions to gain access to the Blob Manager
	BlobManager* GetBlobManager() { return BlobMgr; }

	// Function to gain access to the ATM ini cache manager
	ATMIniCache* GetATMIniCache() { return pATMIniCache; }

	// Function to gain access to Camelots Font Manager
	FontManager* GetFontManager() { return FontMgr; }

	// Function to gain access to Camelots CMS Manager
	XaraCMS* GetCMSManager() const { return CMSManager; }

	// Function to gain access to the PrintMarks Manager
	PrintMarksMan* GetMarksManager() const { return TheMarksManager; }

	// Function to gain access to the Camelot Plug-in Manager
	PlugInManager* GetPlugInManager() const { return ThePlugInManager; }

	// And one for the one & only Camelot Filter Manager
	FilterManager* GetFilterManager() const { return TheFilterManager; }
	
	// Function to gain access to the plugin API host object
	APIObject* GetAPIObject() const { return TheAPIObject; }

	// Function to gain access to the noise generator
	NoiseMan* GetNoiseManager() const { return TheNoiseManager; }

	// Function to gain access to the IMoaHandleManager
	HandleManager* GetHandleManager() const {return TheHandleManager;}

	// Here's one for the WizOp's hanging around in your registry (don't forget the 's')
	WizOps* GetWizOps() const	{	return TheWizardOfOps;	}

	// Function to gain access to the Bitmap Cache
	CBitmapCache* GetBitmapCache() const { return TheBitmapCache; }

#ifdef AUTOMATION
	// Use this for the one true ConcurrencyController
	ConcurrencyController* GetConcurrencyController() const	{	return TheConcurrencyController;	}
#endif

	// Functions to gain access to the Global Bitmap List
	GlobalBitmapList* GetGlobalBitmapList() { return &GlobalBitmaps; }
	GlobalFractalList* GetGlobalFractalList() { return &GlobalFractals; }

	// Function to register document components with the application.
	void RegisterDocComponent(DocComponentClass *);

	// Find out which document components have been registered.
	DocComponentClass *EnumerateDocComponents(DocComponentClass*);

	// Ask each registered document component to add an instance of the component to
	// a specified document.
	BOOL ApplyDocComponents(BaseDocument *);


	// Keith may not be needed now we have the Message handler class
	// Keith is Camelot's main sentry operation. His job is to hang out on the live
	// operation's list waiting for messages. When he receives a message he informs 
	// all interested subsystems. 
	/*
	SentryOperation Keith; 
	*/						 

	BOOL OnKeyPress(KeyPress* pKeyPress);
	BOOL OnIdle(BOOL IdleRedraw);
	void RemoveIdleProcessor(INT32 Priority, Operation* pOp);
	void RegisterIdleProcessor(INT32 Priority, Operation* pOp);

	// Dubugging dump ops
	void DumpAllOps(Document* pDoc);			// Dumps entire op history
	void DumpLastOp(Document* pDoc);			// Dumps last operation
	void DumpTree(BaseDocument* pDoc);				// Dumps document tree
	void DumpAllOps() {DumpAllOps(NULL);}			// The stupid QuickWatch window dosent allow default params!
	void DumpLastOp() {DumpLastOp(NULL);}			
	void DumpTree()   {DumpTree(NULL);}			

protected:
	BOOL InitFilterManager();
	BOOL DeinitFilterManager();

	BOOL InitWizOps();
	BOOL DeinitWizOps();

#ifdef AUTOMATION
	BOOL InitCCC();
	BOOL DestroyCCC();
#endif

private:

//#pragma message( __LOCMSG__ "Removed RenderRegionList usage" )
	RenderRegionList RenderList;
	BOOL ImmediateRenderPending;

	Preferences *CamelotPrefs;							

	// The Colour System Manager. Call him Bob.
	ColourManager *TheColourManager;

// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	// A real or pretend pressure pen
	CCPen *PressurePen;
	BOOL PenInitialised;
#endif // VECTOR_STROKING

	// The blob manager - He controls which blobs get displayed and when.
	BlobManager* BlobMgr;

	// List of registered document component classes.
	List* DocComponentClasses;

	// List of global bitmaps (OILBitmaps)
	GlobalBitmapList GlobalBitmaps;

	// List of global fractals
	GlobalFractalList GlobalFractals;

	// Lists for operation idle processing registery
	List IdleHighPriorityOps;
	List IdleLowPriorityOps;

	BOOL CallIdleProcessors();

	// The cache for stuff in the ATM ini file.
	ATMIniCache* pATMIniCache;

	// The Font Manager inside Camelot, Yes there's only one!
	FontManager* FontMgr;

	// The CMS (Colour Management system)
	XaraCMS* CMSManager;

	// The Print Marks manager
	PrintMarksMan* TheMarksManager;

	// The Plug-in Manager
	PlugInManager* ThePlugInManager;

	// Hmm...what could this be
	FilterManager*	TheFilterManager;

	// Camelot's plugin interface object
	APIObject *TheAPIObject;

	// The IMoaHandle Manager
	HandleManager* TheHandleManager;

	// The noise generation manager
	NoiseMan* TheNoiseManager;

	// The cache object
	ObjectCache *m_pObjCache;

	// The Bitmap Cache
	CBitmapCache* TheBitmapCache;

	// The WizOps local to this application
	WizOps*	TheWizardOfOps;
#ifdef AUTOMATION
	// For controlling all your concurrency needs...
	ConcurrencyController*	TheConcurrencyController;
#endif

	// Temporary
	StatusLine* m_pStatusLine;

	// At some point this may well go to a template manager but for the present
	// moment the most logical places is here as loading is in CCamDoc and saving
	// is in OpMenuSave::Do.
	static String_256 m_TemplatesPath;

public:
	// Public access to the user's defined template path
	String_256& GetTemplatesPath();
	void SetTemplatesPath(String_256& strToSet);


	// return a pointer to the objectcache
	ObjectCache* GetObjectCache();

	// Call this to hide or show all view scroll-bars in all documents.
	void ShowViewScrollers(BOOL fIsVisible);

	// Preferences functions - just veneers to the Preferences object
	void WritePreferences()
		{ CamelotPrefs->Write(); };

	void WipePreferenceFile()
		{ CamelotPrefs->WipePreferenceFile(); };
	void WipeDangerousPrefs()
		{ CamelotPrefs->WipeDangerousPrefs (); };

	BOOL DeclareSection(LPTCHAR Section, UINT32 InitialSize)
		{ return CamelotPrefs->DeclareSection(Section, InitialSize); };
	
	BOOL DeclarePref(LPTCHAR Section, LPTCHAR Pref, 
					 INT32 *PrefVar, INT32 Min = INT_MIN, INT32 Max = INT_MAX)
		{ return CamelotPrefs->DeclarePref(Section, Pref, PrefVar, Min, Max); };

	BOOL DeclarePref(LPTCHAR Section, LPTCHAR Pref, 
					 UINT32 *PrefVar, UINT32 Min = 0, UINT32 Max = UINT_MAX)
		{ return CamelotPrefs->DeclarePref(Section, Pref, PrefVar, Min, Max); };

	BOOL DeclarePref(LPTCHAR Section, LPTCHAR Pref, 
					 double *PrefVar, double Min = DBL_MIN, double Max = DBL_MAX)
		{ return CamelotPrefs->DeclarePref(Section, Pref, PrefVar, Min, Max); };

	BOOL DeclarePref(LPTCHAR Section, LPTCHAR Pref, StringBase *PrefVar)
		{ return CamelotPrefs->DeclarePref(Section, Pref, PrefVar); };

	// Used by the application options/preferences dialog box
	BOOL GetPrefValue(LPTCHAR Section, LPTCHAR Pref, INT32 *PrefVar)
		{ return CamelotPrefs->GetPrefValue(Section, Pref, PrefVar); };
	BOOL GetPrefValue(LPTCHAR Section, LPTCHAR Pref, UINT32 *PrefVar)
		{ return CamelotPrefs->GetPrefValue(Section, Pref, PrefVar); };
	BOOL GetPrefValue(LPTCHAR Section, LPTCHAR Pref, double *PrefVar)
		{ return CamelotPrefs->GetPrefValue(Section, Pref, PrefVar); };

	BOOL SetPrefValue(LPTCHAR Section, LPTCHAR Pref, INT32 *PrefVar)
		{ return CamelotPrefs->SetPrefValue(Section, Pref, PrefVar); };
	BOOL SetPrefValue(LPTCHAR Section, LPTCHAR Pref, UINT32 *PrefVar)
		{ return CamelotPrefs->SetPrefValue(Section, Pref, PrefVar); };
	BOOL SetPrefValue(LPTCHAR Section, LPTCHAR Pref, double *PrefVar)
		{ return CamelotPrefs->SetPrefValue(Section, Pref, PrefVar); };

	// Used for direct partying on the preference file - mainly used for 'dynamic'
	// preferences that are interrogated on the fly as and when required.
	BOOL SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, const char *pValue, BOOL Force = FALSE)
		{ return CamelotPrefs->SetPrefDirect(Section, Pref, pValue, Force); }
	BOOL SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, INT32 *pValue)
		{ return CamelotPrefs->SetPrefDirect(Section, Pref, pValue); }
	BOOL SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, UINT32 *pValue)
		{ return CamelotPrefs->SetPrefDirect(Section, Pref, pValue); }
	BOOL SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, double *pValue)
		{ return CamelotPrefs->SetPrefDirect(Section, Pref, pValue); }

	BOOL GetPrefDirect(LPTCHAR Section, LPTCHAR Pref,StringBase *pValue)
		{ return CamelotPrefs->GetPrefDirect(Section, Pref, pValue); }
	BOOL GetPrefDirect(LPTCHAR Section, LPTCHAR Pref,INT32 *pValue)
		{ return CamelotPrefs->GetPrefDirect(Section, Pref, pValue); }
	BOOL GetPrefDirect(LPTCHAR Section, LPTCHAR Pref,UINT32 *pValue)
		{ return CamelotPrefs->GetPrefDirect(Section, Pref, pValue); }
	BOOL GetPrefDirect(LPTCHAR Section, LPTCHAR Pref,double *pValue)
		{ return CamelotPrefs->GetPrefDirect(Section, Pref, pValue); }

	BOOL GetConvertToEditableShapesDPI (INT32* pValue);

private:
	BOOL CamelotShuttingDown; 	// Set to TRUE at the point of no return when we are shutting
								// down camelot.  Note that if the user cancels the shut-down
								// this will be reset to FALSE again.
	BOOL DisabledRealDownload;	// did we disable real download?
PORTNOTE("other","Removed HKEY usage")
#ifndef EXCLUDE_FROM_XARALX
	HKEY key;					// used for HKEY_CURRENT_USER
	HKEY key2;					// used for HKEY_LOCAL_MACHINE
	BOOL ChangeRealDownloadBrowserOtion (HKEY rootKey, HKEY key, BOOL changeVal);	// actually changes the value
#endif
public:

	// Used to test if camelot is in the process of shutting itself down
	BOOL CamelotIsDying()	{ return CamelotShuttingDown; }
	
	// Called in CCamApp::SaveAllModified to indicate that Camelot is shutting down.
	void ShuttingDown(BOOL fState = TRUE) { CamelotShuttingDown = fState; }

	void CheckRealDownload ();	// we need to check whether RealDownload exists!
	void ResetRealDownload ();	// we need to reset if we changed its state

// StatusBar/StatusLine ...
public:
	BOOL UpdateStatusBarText(String_256* text, BOOL PrefixSelDesc=TRUE);
	
	BOOL LockOutControlHelp ();				// status-line overide - no control help please
	BOOL UnlockControlHelp ();				// status-line overide - control help please
	
	CCStatusBar* GetpCCStatusBar();
	StatusLine*  GetpStatusLine();

	// regeneration list stuff to be regenerated at next click
	void RegenerateNodesInList();
	BOOL AddNodeToRegenList(Node * pNode);
private:
	List RegenList;
};


/********************************************************************************************

>	inline BOOL CCamApp CamelotIsDying()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if camelot is in the process of shutting down
	Purpose:	To determine if we are currently shutting down Camelot. 
	Errors:		-

********************************************************************************************/


// This is the application object itself:
extern Application Camelot;

// This is a global function which returns a pointer to the Camelot object. Try to use
// this function instead of directly accessing the Camelot object, especially if you are
// in a tool
#if _AFXDLL
	Application* GetApplication();
#else
	inline Application *GetApplication() { return &Camelot; }
#endif

const INT32 IDLEPRIORITY_HIGH = 1;
const INT32 IDLEPRIORITY_LOW = 2;


#endif
													   
