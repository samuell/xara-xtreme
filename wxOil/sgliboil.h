// $Id: sgliboil.h 662 2006-03-14 21:31:49Z alex $
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
// SGLibOil.h - oily library code

#ifndef INC_SGLIBOIL
#define INC_SGLIBOIL

#include "selop.h"
#include "opdesc.h"
#include "ops.h"
#include "ccobject.h"
#include "thumb.h"
//#include "scrvw.h"

class Library;
class PathName;
class PageDropInfo;
class KernelBitmap;
class LibraryGallery;
class CXaraFileRecordHandler;
class CXaraFile;




/****************************************************************************

>	class OpClipartImport: public OpMenuImport

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Purpose:	This class represents the Undo-able operation to import and
				open clipart files.
	SeeAlso:

****************************************************************************/

class CCAPI ClipartImportParam
{
public:
	ClipartImportParam();

public:
	PathName *File;
	BOOL Import;
	BOOL Result;
	PageDropInfo* DropInfo;
};
				  
#define OPTOKEN_OPCLIPARTIMPORT _T("ClipartImport")	// Optoken for the clipart import / open

PORTNOTE("other","Removed OpClipartImport defn.")
#ifndef EXCLUDE_FROM_XARALX
class OpClipartImport: public SelOperation
{         
	CC_DECLARE_DYNCREATE( OpClipartImport )

public:
	OpClipartImport();	
	~OpClipartImport();	

	static	BOOL Init();                        
	static	OpState GetState(String_256*, OpDescriptor*);


	virtual void	Do(OpDescriptor *);
	virtual void	DoWithParam(OpDescriptor* pOp, OpParam* pClipartImportParam);

};
#endif

enum FillTypeEnum
{
	ApplyToObject,			// Apply to specified object (as a fill)
	ApplyToObjectAsTransp,	// Apply to specified object (as a transparency)
	SetBackground,			// Set as background
	AddToPage				// Add as a new object on the page
};

class CCAPI BitmapImportParam
{
public:
	BitmapImportParam();

public:
	PathName		*File;
	BOOL			Result;

	// The next four params mirror those of AsyncBitmapImport
	Document*			pTargetDoc;			// Document the bitmap is going into
	FillTypeEnum		FillType;			// The type of fill (ApplyToObject, SetBackground, AddToPage)
	UINT32				TagObjectToFill;	// Object to apply the fill to (if fill type == ApplyToObject)
	bool				TagObjectValid;		// Use Tag? (if !TagObjectValid && !pObjectValid use selection)
	NodeRenderableInk*	pObject;			// Pointer to object to fill
	bool				pObjectValid;		// Use pObject? (if !TagObjectValid && !pObjectValid use selection)
	Spread* 			pSpread;			// Spread (for FillType == SetBackground)
	DocCoord 			DropPos;			// DropPos (for FillType == AddToPage)
};

#define OPTOKEN_OPBITMAPIMPORT _T("BitmapImport")	// Optoken for the Bitmap load & open

PORTNOTE("other","Removed OpBitmapImport")
#ifndef EXCLUDE_FROM_XARALX
class OpBitmapImport: public Operation
{         
	CC_DECLARE_DYNCREATE( OpBitmapImport )

public:
	OpBitmapImport();	
	~OpBitmapImport();	

	static	BOOL Init();                        
	static	OpState GetState(String_256*, OpDescriptor*);

	void	Do(OpDescriptor *);
	void	DoWithParam(OpDescriptor* pOp, OpParam* pBitmapImportParam);

private:
	bool	ApplyFill(BitmapImportParam *pInfo, KernelBitmap* pBitmap);
};
#endif

/********************************************************************************************

>	SGLibOil

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/3/95
	Purpose:	Initially, general OILy utilities for the library, but then the Add Library
				dialog code was added in...

********************************************************************************************/

class SGLibOil: public CCObject
{
	CC_DECLARE_DYNCREATE( SGLibOil )

public:
	SGLibOil() {}
	~SGLibOil() {}

	static BOOL FileCopy(PathName *Source, PathName *Destination);
	static BOOL FileExists(PathName *FileName);
	static INT32 FileSize(PathName *FileName);
	static BOOL FileDelete(PathName *FileName);
 	static UINT32 FileModified(PathName *FileName);
	static BOOL IsRootDirectory(String_256 *Path);
	static void AppendSlashIfNotPresent(String_256 *Str);
	static DWORD GetDriveSerialNumber(String_256 *PathStr);

	static BOOL LocateCDROMDrive(LibraryGallery *LG, SGLibType LibType, String_256 *DriveName, BOOL *OnXaraNet, String_256 *XaraMount, BOOL AdjustPressed = FALSE);
	static BOOL MakeSound(PathName *WaveFile);
	static BOOL PlayRandomSample(String_256 *SampleString, String_256 *SubPath);
	static BOOL GetLibPath(LibraryGallery *LG, PathName *Path, BOOL CanGenerate, SGLibType Type, BOOL *Updated = NULL);
	static BOOL GenerateClicked(LibraryGallery *LG, PathName *GenDirectory);
	static BOOL CheckForIndex(String_256 *Path, LibraryGallery *ParentGallery);
	static BOOL LoadSoundDefinition(CXaraFileRecordHandler *pXFileRecHandler, CXaraFile * pCXFile, INT32 Tag, UINT32 Size, UINT32 RecordNumber);
	static BOOL ExportWav(BaseCamelotFilter *pFilter);
	static BOOL TidyUp(void);
protected:
	// For use by GetLibPath
	static UINT32 CALLBACK FileHook( wxWindow *hDlg, UINT32 Msg, UINT32 wParam, INT32 lParam );
#if _MFC_VER >= 0x400
	static UINT32 CALLBACK ExplorerFileHook(HWND hDlg, UINT32 Msg, UINT32 wParam, INT32 lParam);
	static BOOL ExplorerFileHookNotify(HWND hDlg, LPOFNOTIFY pofn);
#endif
	static BOOL SearchButtonClicked;
	static String_256 SearchDirectory;
	static String_256 BrowsePath;
	static SGLibType LibType;
	static LibraryGallery *ParentLibraryGallery;
	static BOOL GenerateButtonClicked;
	static TCHAR *TmpSndName;

public:
	static BOOL UseExplorerForAdd;
};


// a home for Internet utility functions
namespace InetUtils
{
	// Utility function - will extract a single string from a composite indexed string
	// The components of the composite string must be separated by vertical bars (|) and the
	// length less than 1024
	TCHAR* GetStringField(UINT32 nIndex, UINT32 nCompositeStringID);
	// Returns the location of a file on a remote HTTP server, given the local path of the file relative to the 
	// application's directory and a root URL on the server (currently www.xara.com). Works on the assumption
	// that the directory structure on the server is identical to the local one
	String_256 MakeURL(const String_256& rRelativePath, const String_256& rRootURL);
	String_256 MakeLocalPath(const String_256& rRelPath);
	BOOL GetTempInetDir(String_256* pTempDir); 
	BOOL GetAppDataPath(String_256* pPath);
	BOOL GetAppCachePath(String_256* pPath);
	BOOL DeleteDirectoryTree(const String_256& rPath);
}


#endif

