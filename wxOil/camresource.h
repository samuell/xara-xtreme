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

#ifndef INC_CAMRESOURCE
#define	INC_CAMRESOURCE

#include <wx/wxprec.h>

#include <wx/filesys.h>
#include <wx/fs_mem.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_all.h>
#include <wx/hashmap.h>


// Define _R() before we include other Camelot headers
// NOTE THIS MACRO IS EXPANDABLE IN A STATIC INITIALIZER
#if 0 != wxUSE_UNICODE
#define _RESQUOTE(x) L ## #x
#else
#define _RESQUOTE(x) #x
#endif
#define _R(x) ( CamResource::GetResourceID( _RESQUOTE(x)) )

#include "ccobject.h"

class CCLexFile;
class wxFSFile;
class wxBimtap;
class wxSplashScreen;

typedef UINT32 ResourceID;

// Need a type for the hash data or we get duplicate const error
typedef const TCHAR * ResourceString;

// Declare the hash map from ResourceID to String
WX_DECLARE_HASH_MAP( ResourceID, ResourceString, wxIntegerHash, wxIntegerEqual, ResIDToString );
WX_DECLARE_HASH_MAP( ResourceString, wxImage *, wxStringHash, wxStringEqual, ResourceStringToBitmap);

// Some bodgy stuff from tbbitmap which we will fix some time
extern wxImage			imageBevelTool;
extern wxImage			imageBezTool;
extern wxImage			imageBlendTool;
//extern wxImage			imagelbuttonst32_xpm );
//extern wxImage			lbuttst232_xpm );
//extern wxImage			lbutts_liveeffect_xpm );
extern wxImage			imageContourTool;
extern wxImage			imageElipTool;
extern wxImage			imageFHandTool;
extern wxImage			imageGradTool;
extern wxImage			imageLiveEffectTool;
extern wxImage			imageMouldTool;
extern wxImage			imagePenTool;
extern wxImage			imagePush;
extern wxImage			imageRectTool;
extern wxImage			imageRegShapeTool;
extern wxImage			imageSelTool;
extern wxImage			imageShadrwTool;
extern wxImage			imageSlicetool;
extern wxImage			imageTextTool;
extern wxImage			imageTransTool;
extern wxImage			imageZoomTool;

class CamResourceRemember;

/*******************************************************************************************

>	class CamResource : public CCObject

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Purpose:	To control the translation of resources within Camelot
	Notes:		In the OIL
	See Also:	

This is a class composed of static member functions which rapidly translates resources,
and a dynamic object which provides capabilities of loading from the disk.

String tables

The GetText() member functions look up the integer ID's passed to it in a hash map, via inline
functions, so should be pretty rapid in execution. Thus the actual mechanism of loading
the strings is effectively encapsulated.

Dialogs and other resources

The GetResourceID() member function implements the _R() macro, which allows a resource in
a dialog (or a stringtable resource) to be translated into the numeric ID's Camelot likes.

Example usage:

	ResourceID	myResource;
	myResource = _R("_R(IDS_BLOBBYSTRING)"); // Uses CamResource::GetResourceID()

	... pass around the integer value ...

	const TCHAR * myString = CamResource::GetText(myResource); // Can't fail
	... do something with the text of the entry ...

Note that the _R() macros is clevere than you think. That's because it can be used
as a static initializer (e.g. in wxWidgets event tables). What is happening here is
that the GetXRCID() hash table gets initialized at static initialization time creating
a map from the resource string (IDS_ etc.) to a unique integer value, and when the XML
resources are loaded later, they are assigned that integer value.

Note this is a static class, and thus no constructors and destructors are offered.

********************************************************************************************/


class CamResource : public CCObject
{
	CC_DECLARE_DYNCREATE( CamResource )

// The per-object bits

public:
	CamResource();
	virtual ~CamResource();

private:
	CCLexFile * pFile;
	void * pMemory;
	UINT32 Size;
	wxFSFile * pwxFSFile;
	static wxFileSystem * pwxFileSystem;
	static wxLocale * m_pLocale;

public:
	CCLexFile * Open ( ResourceID ID, BOOL ErrorReporting=TRUE, BOOL ExceptionThrowing=FALSE);
	CCLexFile * Open ( const TCHAR * pFileName, BOOL ErrorReporting=TRUE, BOOL ExceptionThrowing=FALSE);
	wxFSFile * OpenwxFSFile (ResourceID ID);
	wxFSFile * OpenwxFSFile (const TCHAR * pFileName);
	void Close();
	CCLexFile * Get () {return pFile;}
	wxFSFile * GetwxFSFile () {return pwxFSFile;}

// The static stuff

private:
	static void AddStringResource(const TCHAR * name, const TCHAR * text, ResourceID r=0);
	static void RememberDuringStaticInit(const TCHAR * ObjectName);
	static BOOL ReadStringTableFile();
	static void ProcessWindowAndChildren(wxWindow * pWindow);
	static void GetBinaryFileInfo (void **pPtr, UINT32 *pSize);
	static BOOL InitXmlResource();
	static BOOL Splash();
	static BOOL CheckResourcePath( const wxString &str1, const wxString &str2 );
	static const TCHAR * FixObjectName(const TCHAR * ObjectName);
	static BOOL AddBitmaps(wxString &Path);
	static BOOL LoadBitmaps();
	static void DeleteBitmapHashEntries();
	static void MakeVariantBitmaps(ResourceStringToBitmap::iterator * it=NULL);

	static ResIDToString * pHash;
	static ResIDToString * pObjectNameHash;
	static TCHAR * DefaultString;
	static TCHAR * DefaultObjectName;
	static wxString * pResourcePath;
	static BOOL HaveCheckedResourcePath;
	static CamResourceRemember * pFirstRemember;

	static wxBitmap * pSplashBitmap;
	static wxSplashScreen * pSplashScreen;

	static ResourceStringToBitmap * pBitmapHash;

public:
	static BOOL LoadwxImage (wxImage & rImage, const TCHAR * pFileName, BOOL Grey=FALSE);
	static BOOL LoadwxBitmap (wxBitmap & rBitmap, const TCHAR * pFileName, BOOL Grey=FALSE);

	static wxImage* GetCachedBitmap(const TCHAR * pName);

	static void MakeGreyImage (wxImage & rImage);
	static void MakeHotImage (wxImage & rImage);

	static inline ResourceID GetResourceID(const TCHAR * ObjectName) // Implement the _R macro
	{
		const TCHAR * fObjectName=FixObjectName(ObjectName);
		ResourceID Resource = wxXmlResource::GetXRCID(fObjectName);
		// need to record it in the hash that goes the other way lest we be asked. Note this will always
		// work as the caller to GetObjectName can't have a ResourceID without _R()'ing it (or at least
		// cannot legally have one).
		if (pObjectNameHash)	// skip on static initialization phase or we'll be dead
		{
			ResIDToString::iterator i=pObjectNameHash->find(Resource);
			if (i==pObjectNameHash->end()) (*pObjectNameHash)[Resource]=camStrdup(fObjectName);
		}
		else RememberDuringStaticInit(fObjectName); // add it to the hash later
		return Resource;
	} 	
	static inline const TCHAR * GetTextFail(ResourceID Resource)
	{
		if (!pHash) return NULL;
		ResIDToString::iterator i=pHash->find(Resource);
		return (const TCHAR *)((i==pHash->end())?NULL:i->second);
	};
	static inline const TCHAR * GetText(ResourceID Resource)
	{
		if (!pHash) return DefaultString;
		ResIDToString::iterator i=pHash->find(Resource);
		const TCHAR * text = (const TCHAR *)((i==pHash->end())?NULL:i->second);
		return text?text:DefaultString;
	};
	static inline const TCHAR * GetObjectNameFail(ResourceID Resource)
	{
		if (!pObjectNameHash) return NULL;
		ResIDToString::iterator i=pObjectNameHash->find(Resource);
		return (const TCHAR *)((i==pObjectNameHash->end())?NULL:i->second);
	};
	static inline const TCHAR * GetObjectName(ResourceID Resource)
	{
		if (!pObjectNameHash) return DefaultString;
		ResIDToString::iterator i=pObjectNameHash->find(Resource);
		const TCHAR * text = (const TCHAR *)((i==pObjectNameHash->end())?NULL:i->second);
		return text?text:DefaultString;
	};

	static wxString GetResourceFilePath( const wxString &str, BOOL AllowOverride=TRUE );
	static void SetResourceFilePath( const wxString &str );

	// String manipulation functions for bitmaps
	static BOOL IsBitmapName(const wxString &str);
	static wxString GetBitmapExtension(const wxString &str);
	static wxString GetBitmapBaseName(const wxString &str);
	static wxString GetBitmapFlagString(const wxString & str);
	static wxString MakeBitmapString(const wxString base, const wxString flags, const wxString ext)
	{
		if (flags.IsEmpty())
			return base+wxString(_T("."))+ext;
		else
			return base+wxString(_T("-"))+flags+wxString(_T("."))+ext;
	}

	static BOOL Init();
	static BOOL DeInit();

	static BOOL DoneInit();

	static wxLocale * GetLocale() {return m_pLocale;}

	static wxArrayString BitmapExtensions;

};


class CamResourceRemember
{
	friend class CamResource;

private:
	CamResourceRemember * next;
	TCHAR * text;
};



#endif

