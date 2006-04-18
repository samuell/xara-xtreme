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

#include <wx/wxprec.h>
#include <wx/filesys.h>
#include <wx/fs_mem.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_all.h>
#include <wx/fs_zip.h>
#include <wx/filesys.h>
#include <wx/splash.h>
#include <wx/bitmap.h>

//#include "camconfig.h"
#include "camresource.h"
#include "cartctl.h"
#include "drawctl.h"
#include "colourmat.h"

#include "camelot.h"
#include "ccfile.h"

#define PRELOAD_BITMAPS 0

CC_IMPLEMENT_DYNCREATE(CamResource, CCObject)


ResIDToString * CamResource::pHash = NULL;
ResIDToString * CamResource::pObjectNameHash = NULL;
wxFileSystem * CamResource::pwxFileSystem = NULL;
wxBitmap * CamResource::pSplashBitmap= NULL;
wxSplashScreen * CamResource::pSplashScreen = NULL;
wxString * CamResource::pResourcePath = NULL;
CamResourceRemember * CamResource::pFirstRemember=NULL;
BOOL CamResource::HaveCheckedResourcePath = FALSE;
wxLocale * CamResource::m_pLocale = NULL;

ResourceStringToBitmap * CamResource::pBitmapHash = NULL;

TCHAR * CamResource::DefaultString = _T("[Resource string not found]");
TCHAR * CamResource::DefaultObjectName = _T("[Object name not Found]");

wxArrayString CamResource::BitmapExtensions;

// Bodge for the toolbar bitmap
wxImage					imageBevelTool;
wxImage					imageBezTool;
wxImage					imageBlendTool;
wxImage					imageContourTool;
wxImage					imageElipTool;
wxImage					imageFHandTool;
wxImage					imageGradTool;
wxImage					imageLiveEffectTool;
wxImage					imageMouldTool;
wxImage					imagePenTool;
wxImage					imagePush;
wxImage					imageRectTool;
wxImage					imageRegShapeTool;
wxImage					imageSelTool;
wxImage					imageShadrwTool;
wxImage					imageSlicetool;
wxImage					imageTextTool;
wxImage					imageTransTool;
wxImage					imageZoomTool;


/********************************************************************************************

>	static inline ResourceID CamResource::GetResourceID(const TCHAR * Resource)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		Resource - textual name of the resource to lookup
	Outputs:	
	Returns:	the resource ID of the resource
	Purpose:	Converts a textual resource identified (_R(IDS_BLOBBY) or whatever) to a numeric
				resource ID. This ALWAYS succeeds as it creates a new one if there is not
				one in the current table. Can be called from static initialization phase
				so BE CAREFUL with this call. No fancy TRACE() stuff etc. please.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/********************************************************************************************

>	static inline TCHAR * CamResource::GetText(ResourceID Resource)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		Resource - resource ID of text to look up
	Outputs:	
	Returns:	the text of the resource, or a default string
	Purpose:	Converts a ResourceID to a text string, returning a default string on failure
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/********************************************************************************************

>	static inline TCHAR * CamResource::GetTextFail(ResourceID Resource)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		Resource - resource ID of text to look up
	Outputs:	
	Returns:	the text of the resource, or 0 if not found
	Purpose:	Converts a ResourceID to a text string, returning NULL on failure
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/********************************************************************************************

>	static inline TCHAR * CamResource::GetObjectName(ResourceID Resource)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		Resource - resource ID of object name to look up
	Outputs:	
	Returns:	the object name of the resource, or a default string
	Purpose:	Converts a ResourceID to an object name, returning a default string on failure
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/********************************************************************************************

>	static inline TCHAR * CamResource::GetObjectNameFail(ResourceID Resource)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		Resource - resource ID of object name to look up
	Outputs:	
	Returns:	the object name of the resource, or 0 if not found
	Purpose:	Converts a ResourceID to an object name, returning NULL on failure
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#define CRFNH_BUCKETS 1024

class CamResourceFixNameEntry
{
public:
	static UINT32 MakeHash(const TCHAR * k)
	{
		UINT32 hash = 0;

    	while( *k )
    	{
        	hash += *k++;
        	hash += (hash << 10);
        	hash ^= (hash >> 6);
    	}
    	hash += (hash << 3);
    	hash ^= (hash >> 11);

    	return (hash + (hash << 15)) % CRFNH_BUCKETS;
	}

	typedef CamResourceFixNameEntry * pCamResourceFixNameEntry;
	
	static const TCHAR * Find(const TCHAR * from, const TCHAR * to = NULL)
	{
		// Ensure we have an array
		if (!pArray)
		{
			pArray=new pCamResourceFixNameEntry[CRFNH_BUCKETS];
			UINT32 i;
			for (i=0; i<CRFNH_BUCKETS; i++) pArray[i]=NULL;
		}
		if (!pArray) return from;
		UINT32 h = MakeHash(from);
		pCamResourceFixNameEntry * pPoint = &(pArray[h]);
		while (*pPoint)	// while we have a valid pointer to a "next" item
		{
			if (!camStrcmp( (*pPoint)->MapFrom, from ) ) return ( (*pPoint)->MapTo );
			pPoint = &((*pPoint)->pNext);
		}

		// no match - if they have not asked us to insert, return it unaltered
		if (!to) return from;
		// We now knonw pPoint is a pointer to a NULL CamResourceFixedNameEntry *, where we will insert ourselves
		*pPoint = new(CamResourceFixNameEntry);
		if (*pPoint)
		{
			// No need to strdup these as we know this map is only created with constants, because
			// the map is created in init phase
			(*pPoint)->MapFrom = from;
			(*pPoint)->MapTo = to;
		}
		return to;
	}

	static void MakeResources() // Iterates through the hash and calls GetResourceID on them
	{
		if (pArray)
		{
			INT32 i;
			for (i=0; i<CRFNH_BUCKETS; i++)
			{
				pCamResourceFixNameEntry p=pArray[i];
				while (p)
				{
					CamResource::GetResourceID(p->MapFrom);
					p = p->pNext;
				}
			}
		}
	}

	const TCHAR * MapFrom;
	const TCHAR * MapTo;
	CamResourceFixNameEntry * pNext;
	
	static pCamResourceFixNameEntry *pArray;

	CamResourceFixNameEntry() {MapFrom =NULL ; MapTo = NULL ; pNext = NULL;}
};

CamResourceFixNameEntry::pCamResourceFixNameEntry *CamResourceFixNameEntry::pArray = NULL;

/********************************************************************************************

>	static const TCHAR * CamResource::FixObjectName(const TCHAR * ObjectName)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		ObjectName - the object name of the resource to fix up
	Outputs:	
	Returns:	The fixed up object name
	Purpose:	Renames some of the resources from Windows to wx Style
				This must match the mapping in rc2xml.pl. This allows wx to do
				intelligent things with (say) the _R(ID_OK) button on dialogs, but for
				us to still be able to use _R(IDOK) in Camelot rather than having
				to write _R(wxID_OK) which is actually what we mean (but that would
				be unkernel-like. Note this is called at static init time so the
				obvious route of using a wxHash isn't going to work, sadly
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#define FNOMAP(x,y) CamResourceFixNameEntry::Find(_T(x),_T(y))

const TCHAR * CamResource::FixObjectName(const TCHAR * ObjectName)
{
	if (!CamResourceFixNameEntry::pArray)
	{
		// Don't delete the comments or normalize.pl will unhelpfully convert the bits in quotes
		FNOMAP("IDOK", "wxID_OK"); //NORESOURCEFIX
		FNOMAP("IDCANCEL", "wxID_CANCEL"); //NORESOURCEFIX
		FNOMAP("IDAPPLY", "wxID_APPLY"); //NORESOURCEFIX
		FNOMAP("ID_FILE_OPEN", "wxID_OPEN"); //NORESOURCEFIX
		FNOMAP("ID_FILE_CLOSE", "wxID_CLOSE"); //NORESOURCEFIX
		FNOMAP("ID_FILE_SAVE", "wxID_SAVE"); //NORESOURCEFIX
		FNOMAP("ID_FILE_SAVE_AS", "wxID_SAVEAS"); //NORESOURCEFIX
		FNOMAP("ID_APP_EXIT", "wxID_EXIT"); //NORESOURCEFIX
		FNOMAP("ID_FILE_PRINT", "wxID_PRINT"); //NORESOURCEFIX
		FNOMAP("ID_FILE_PRINT_PREVIEW", "wxID_PREVIEW"); //NORESOURCEFIX
		FNOMAP("ID_FILE_PRINT_SETUP", "wxID_PRINT_SETUP"); //NORESOURCEFIX
		FNOMAP("ID_APP_ABOUT", "wxID_ABOUT"); //NORESOURCEFIX
		FNOMAP("ID_EDIT_UNDO", "wxID_UNDO"); //NORESOURCEFIX
		FNOMAP("ID_EDIT_CUT", "wxID_CUT"); //NORESOURCEFIX
		FNOMAP("ID_EDIT_COPY", "wxID_COPY"); //NORESOURCEFIX
		FNOMAP("ID_EDIT_PASTE", "wxID_PASTE"); //NORESOURCEFIX
		FNOMAP("IDYES", "wxID_YES"); //NORESOURCEFIX
		FNOMAP("IDNO", "wxID_NO"); //NORESOURCEFIX
		FNOMAP("IDHELP", "wxID_HELP"); //NORESOURCEFIX
		
		CamResourceFixNameEntry::MakeResources();
	}
	const TCHAR * ret= CamResourceFixNameEntry::Find(ObjectName);
	if (pObjectNameHash && ret !=ObjectName) // don't do during static init
	{
		TRACEUSER("amb",_T("**Mapped %s to %s"),ObjectName,ret);
	}
	return ret;
}

/********************************************************************************************

>	static void CamResource::RememberDuringStaticInit(const TCHAR * ObjectName)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	19/12/2005
	Inputs:		ObjectName - the name of the object to remember
	Outputs:	-
	Returns:	-
	Purpose:	Remembers an objectname that has been GetResourceID'd prior to
				the ObjectHash existing
	Errors:		-
	SeeAlso:	-

As GetResourceID is called during StaticInit, the ObjectHash won't be set up. wxHash cannot
safely be used in static initialization (no, I don't really know why, it seems to work),
so we remember a list of these, then blat them out back through GetResourceID during
::Init.

********************************************************************************************/

void CamResource::RememberDuringStaticInit(const TCHAR * ObjectName)
{
	TCHAR * text = camStrdup(ObjectName);
	if (!text) return;	// we can't error, we can't trace, so meekly forget it
	CamResourceRemember * pRem = new CamResourceRemember;
	if (!pRem) return;	// we can't error, we can't trace, so meekly forget it
	
	pRem->text = text;	// fill in text
	
	// Now add to our linked list, and return
	pRem->next = pFirstRemember?pFirstRemember:NULL;
	pFirstRemember=pRem;
	return;
}

/********************************************************************************************

>	static void CamResource::AddStringResource(const TCHAR * name, const TCHAR * text, ResourceID r=0)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		name - object name to add
				text - text asssociated with it
				r - numeric ResourceID or 0 to allocate it
	Outputs:	None
	Returns:	None
	Purpose:	Initialize resources
	Errors:		via wxMessageBox
	SeeAlso:	-

Read controls out of this window, and put into string table. Then process children
recursively.

********************************************************************************************/

void CamResource::AddStringResource(const TCHAR * name, const TCHAR * text, ResourceID r)
{
	ResourceID r2=GetResourceID(name);
	if (r)
	{
		if (r != r2)
		{
			TRACE(_T("Possible duplicate label %s\n"),text);
		}
	}
	else
	{
		r = r2;
	}

	if (!name || !text)
	{
		TRACE(_T("Resource ID %d is blank\n"),r);
		ERROR3("Resource is blank");
	}
	else
	{
		ResIDToString::iterator i=pHash->find(r);
		if (i!=pHash->end())
		{
			TRACE(_T("Resource ID %d name <%s> has duplicate definitions: <%s> and <%s>\n"),r,name,text,i->second);
		}
		else
		{
			const TCHAR * tcopy = camStrdup(text);
			(*pHash)[r] = tcopy;

			const TCHAR * t = GetText(r);
			if (t!=tcopy)
			{
				ERROR3("Resource string does not match local copy");
			}		
		}
	}
}

/********************************************************************************************

>	static BOOL CamResource::ReadStringTableFile()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Initialize resources
	Errors:		via wxMessageBox
	SeeAlso:	-

We can't use the CCLexFile stuff because it has not yet been initiated.

********************************************************************************************/

BOOL CamResource::ReadStringTableFile()
{
	TRACET(_T("ReadStringTableFile() called"));
	CamResource res;
	INT32 count=0;

	wxFSFile* f = res.OpenwxFSFile(_T("strings.lst"));	// NB destructor of 'res' deletes this
	if (!f)
	{
		TRACE(_T("Could not load strings.lst"));
		return FALSE;
	}

	wxInputStream * s = f->GetStream();
	if (!s)
	{
		TRACE(_T("Could not load strings.lst - no stream"));
		return FALSE;
	}

	while (!s->Eof())
	{
#define MAX_TAG_LENGTH 1024
		char buf[MAX_TAG_LENGTH];
		
		char c=0;		// Deliberate - file is not Unicode for now
		INT32 l=0;
		while (!s->Eof() && (c=s->GetC()) && (c != '\n') && (c != '\t') && (l<MAX_TAG_LENGTH))
		{
			buf[l++]=c;
		}
		buf[l++]=0;
		buf[MAX_TAG_LENGTH-1]=0;
		if (buf[0])
		{
			wxString name(buf, wxConvUTF8);
	
			if (c == '\n')
			{
				buf[0]=0;
			}
			else
			{
				l=0;
				while (!s->Eof() && (c=s->GetC()) && (c != '\n') && (l<MAX_TAG_LENGTH))
				{
					buf[l++]=c;
				}
				buf[MAX_TAG_LENGTH-1]=0;
			}
			buf[l++]=0;
			wxString text(buf, wxConvUTF8);

			// Now de-XMLize it
			wxString otext;
			UINT32 n=0;
			while (n<text.Length())
			{
				wxChar tc=text.GetChar(n++);
				if (tc==_T('&'))
				{
					// It's an ampersand escape sequence. Read till a semicolon
					wxString seq;
					while (tc !=_T(';') && n<text.Length())
					{
						tc=text.GetChar(n++);
						seq+=tc;
					}
					
					if (seq == _T("amp;"))
					{
						tc=_T('&');
					}
					else if (seq == _T("lt;"))
					{
						tc=_T('<');
					}
					else if (seq == _T("gt;"))
					{
						tc=_T('>');
					}
					else if (seq == _T("quot;"))
					{
						tc=_T('"');
					}
					else if ((seq.Length()>3) && (seq.GetChar(0)==_T('#')) && (seq.GetChar(1)==_T('x')))
					{
						tc=_T('?');
						UINT32 code=tc;
						// It's an alphanumeric form of a unicode character
#if wxUSE_UNICODE
						sscanf((const char*)seq.mb_str(wxConvUTF8), "#x%x;", &tc);
#else
						sscanf((const char*)seq.c_str(), "#x%x;", &tc);						
#endif
						tc=(TCHAR)(code);
					}
					else
					{
						TRACE(_T("CamResource::ReadStringTableFile - Unknown XML escape sequence [&%s] in %s, %s"),seq.c_str(),name.c_str(),text.c_str());
						otext+=_T('&')+seq;
						tc=0; // don't add
					}
				}
				if (tc)
				{
					otext+=tc;
				}
			}

			// Here we could translate it using wxGetTranslation
			// Or we could muck around on non UTF-8 encodings
			// but that would be really boring.
			count++;
			AddStringResource(name.c_str(), ::wxGetTranslation(otext.c_str()));
		}
	}

	TRACE(_T("Added %d strings"),count);
	if (count<1000) return FALSE; // some bad thing has happened.

	return TRUE;
}

/********************************************************************************************

>	static void CamResource::ProcessWindowAndChildren(wxWindow * pWindow)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		pWindow - pointer to window to process
	Outputs:	None
	Returns:	None
	Purpose:	Initialize resources
	Errors:		via wxMessageBox
	SeeAlso:	-

Read controls out of this window, and put into string table. Then process children
recursively.

********************************************************************************************/


void CamResource::ProcessWindowAndChildren(wxWindow * pWindow)
{
	// Process this one
	if (pWindow->IsKindOf(CLASSINFO(wxStaticText)))
	{
		ResourceID r=(ResourceID) pWindow->GetId();

//		Don't be so foolish as to think the following would work. Oh no. What this would
//		do is return a temporary wxString, take the address of it's data member, then
//		mark it as discardable on the stack leaving the data member to be overwritten.
//		Ho de ho de ho.
//		const TCHAR * name = pWindow->GetName().c_str();
//		const TCHAR * text = pWindow->GetLabel().c_str();

		wxString sname = pWindow->GetName();
		wxString stext = pWindow->GetLabel();

		AddStringResource((const TCHAR *)sname.c_str(),(const TCHAR *)stext.c_str(), r);

	}

	// Now process children if any
	wxWindowList::Node * pNode = pWindow->GetChildren().GetFirst();
	while (pNode)
	{
		ProcessWindowAndChildren(pNode->GetData());
		pNode = pNode->GetNext();
	}
	return;
}

/********************************************************************************************

>	static BOOL CamResource::InitXmlResource()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Initialize resources
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

BOOL CamResource::InitXmlResource()
{
	TRACET(_T("CamResource::InitXmlResource() called, doing Load() of strings.xrc"));

	if (!wxXmlResource::Get()->Load(GetResourceFilePath(_T("strings.xrc"))))
	{
		TRACE(_T("Could not load strings.xrc"));
		return FALSE;
	}

	TRACET(_T("CamResource::InitXmlResource() called - loaded strings"));

	return true;
}

/********************************************************************************************

>	static wxString CamResource::GetResourceFilePath( const wxString &str, BOOL AllowOverride = TRUE )


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		str - the string to look up
	Outputs:	
	Returns:	the path to load it from
	Purpose:	Destructor
	Errors:		-
	SeeAlso:	-

By default this will return the MemoryFS path, however, if & when we support overriding
of resources in a resource directory, this will under the appopriate circumstances
use a differnt path. Note this can only be called AFTER the resource system is initialized.

If an empty string is specified, it returns the overridden resource path (if one exists)
(so long as the override is set to TRUE), or failing that the memory resource path

********************************************************************************************/


wxString CamResource::GetResourceFilePath( const wxString &str, BOOL AllowOverride )
{
	// If we are given a full path, then return. For the time being that's anything with a colon or a slash in it
	// as the resources file is flat
	if ( ((str.Find(_T(':')))>=0) || ((str.Find(_T('/')))>=0) || ((str.Find(_T('\\')))>=0) )
	{
		return str;
	}

	wxString mfn = _T("memory:resources#zip:");

	if (AllowOverride && pResourcePath && pwxFileSystem)
	{
		wxString fn = *pResourcePath + _T('/');

		// If we have not run the check on the checksum do it now.
		if (!HaveCheckedResourcePath)
		{
			if (CheckResourcePath(mfn+_T("xrc.check"),fn+_T("xrc.check")))
			{
				HaveCheckedResourcePath = TRUE;
			}
			else
			{
				wxMessageBox(_T("Ignoring resource path specified on command line due to bad resource checksum"), _T("Resource loading error"));
				delete (pResourcePath);
				pResourcePath = NULL;
			}
		}

		if (str.IsEmpty() && pResourcePath)
			return fn;

		fn += str;

		if (pResourcePath) // may have been cleared
		{
			wxFSFile * pTwxFSFile = pwxFileSystem->OpenFile(fn);
			if (pTwxFSFile)
			{
				// we found it, so close it & return
				delete pTwxFSFile;
				return fn;
			}
		}
	}
	return mfn + str;
}

/********************************************************************************************

>	static BOOL CamResource::CheckResourcePath( const wxString &str1, const wxString &str2 )


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		str1, str2 - filenames to check
	Outputs:	
	Returns:	TRUE if the files are identical, else false
	Purpose:	-
	Errors:		-
	SeeAlso:	-

This routine compares two small files. If they are equal, it returns true, else it
returns false. This allows checking of the checksum. This is reasonably resilient to
some fool creating a huge xrc.check as we only read the files if the lengths are
equal, which means we never read more than the size of the file bound into the
executable.

********************************************************************************************/

BOOL CamResource::CheckResourcePath( const wxString &str1, const wxString &str2 )
{
	if (!pwxFileSystem) return FALSE;

	wxFSFile * pTwxFSFile1 = pwxFileSystem->OpenFile(str1);
	wxFSFile * pTwxFSFile2 = pwxFileSystem->OpenFile(str2);
	if (!pTwxFSFile1 || !pTwxFSFile2)
	{
		if (!pTwxFSFile1) delete pTwxFSFile1;
		if (!pTwxFSFile1) delete pTwxFSFile1;
		return FALSE;
	}

	wxInputStream * pStream1 = pTwxFSFile1->GetStream(); // we don't have to delete this ourselves
	wxInputStream * pStream2 = pTwxFSFile2->GetStream(); // we don't have to delete this ourselves
	if (!pStream1 || !pStream2)
	{
		delete (pTwxFSFile1);
		delete (pTwxFSFile2);
		return FALSE;
	}

	UINT32 size1=pStream1->GetSize();
	UINT32 size2=pStream2->GetSize();

	if (!size1 || !size2 || (size1 != size2))
	{
		delete (pTwxFSFile1);
		delete (pTwxFSFile2);
		return FALSE;
	}

	void * mem1 = malloc(size1+4); // safety
	if (!mem1)
	{
		delete (pTwxFSFile1);
		delete (pTwxFSFile2);
		return FALSE;
	}

	void * mem2 = malloc(size2+4); // safety
	if (!mem2)
	{
		free(mem1);
		delete (pTwxFSFile1);
		delete (pTwxFSFile2);
		return FALSE;
	}

	memset(mem1, 0, size1);
	memset(mem2, 0, size2);
	pStream1->Read(mem1, size1); // throws exceptions if can't read
	pStream2->Read(mem2, size2); // throws exceptions if can't read
	
	delete (pTwxFSFile1); // Closes it
	delete (pTwxFSFile2); // Closes it

	BOOL same=(!memcmp(mem1, mem2, size1));

	free (mem1);
	free (mem2);

	return same;
}


/********************************************************************************************

>	void  CamResource::SetResourceFilePath( const wxString &str )


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		str - the string to set
	Outputs:	
	Returns:	the path to load it from
	Purpose:	Destructor
	Errors:		-
	SeeAlso:	-

This sets the override resource path - this function can be called before
CamResource::Init()

********************************************************************************************/


void CamResource::SetResourceFilePath( const wxString &str )
{
	if (pResourcePath)
	{
		delete (pResourcePath);
		pResourcePath = NULL;
	}

	HaveCheckedResourcePath=FALSE;

	if (!str.IsEmpty())
	{
		pResourcePath = new(wxString);
		if (pResourcePath) *pResourcePath = str;
	}
	return;
}

/********************************************************************************************

>	static BOOL CamResource::AddBitmaps(wxString &Path)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	01/01/2005
	Inputs:		Path - the path to add
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Add all bitmaps in the path
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

BOOL CamResource::AddBitmaps(wxString &Path)
{
	ERROR2IF(!pBitmapHash, FALSE, "No bitmap hash");
	ERROR2IF(!pwxFileSystem, FALSE, "No filesystem");

	TRACEUSER("amb",_T("Adding bitmaps from %s"),Path.c_str());

	wxString fn = pwxFileSystem->FindFirst(Path+_T("*.*"), wxFILE); // This isn't an MSDOS-ism - we are looking for files with an extension
	while (!fn.IsEmpty())
	{
		wxString LeafName = fn.Mid(Path.Length());

		if (IsBitmapName(LeafName))
		{
	
			ResourceStringToBitmap::iterator i=pBitmapHash->find(LeafName);
	
			if (i==pBitmapHash->end())	// ignore if we already have an entry
			{
#if PRELOAD_BITMAPS	
				wxImage * pBitmap=new wxImage;
				if (!pBitmap)
					return FALSE;
	
	//			TRACE(_T("Loading bitmap %s"),fn.c_str());
				if (LoadwxImage(*pBitmap, LeafName))
					(*pBitmapHash)[camStrdup(LeafName.c_str())]=pBitmap;
				else
				{
					ERROR3_PF((_T("Could not load bitmap %s"),fn.c_str()));
					delete pBitmap;
				}
#else
				(*pBitmapHash)[camStrdup(LeafName.c_str())]=NULL; // Mark as there, but needs to be loaded
#endif
			}
		}
	
		fn = pwxFileSystem->FindNext();
	}
	return TRUE;
}

/********************************************************************************************

>	static BOOL CamResource::IsBitmapName(const wxString &str)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/01/2005
	Inputs:		str - sting to look at
	Outputs:	None
	Returns:	TRUE if the extension is a bitmap type
	Purpose:	Determine whether or not a path points to a bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamResource::IsBitmapName(const wxString &str)
{
	wxString ext=GetBitmapExtension(str);
	return (BitmapExtensions.Index(ext) != wxNOT_FOUND);
}

/********************************************************************************************

>	static wxString CamResource::GetBitmapExtension(const wxString &str);


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/01/2005
	Inputs:		str - sting to look at
	Outputs:	None
	Returns:	Empty string if none found, else the extenstion
	Purpose:	Determines the extension of a bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxString CamResource::GetBitmapExtension(const wxString &str)
{
	// Find the rightmost dot
	INT32 i=str.Find(_T('.'), TRUE);
	if (i<0)
		return wxEmptyString;
	else
		return str.Mid(i+1);
}

/********************************************************************************************

>	static wxString CamResource::GetBitmapBaseName(const wxString &str);


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/01/2005
	Inputs:		str - sting to look at
	Outputs:	None
	Returns:	Empty string if none found, else the BaseName (without flags or extension)
	Purpose:	Determines the extension of a bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxString CamResource::GetBitmapBaseName(const wxString &str)
{
	// Find the rightmost dot
	INT32 i=str.Find(_T('.'), TRUE);
	if (i<0)
		return wxEmptyString;
	wxString barestr = str.Left(i);

	// Find the rightmost "-";
	i=barestr.Find(_T('-'), TRUE);
	if (i<0)
		return barestr;

	// Now check all characters after the "-" are alphanumeric
	INT32 j;
	for (j=i+1; j<(INT32)barestr.Length(); j++)
	{
		wxChar c=barestr[j];
		if ((c<_T('a')) || (c>_T('z')))
			return barestr;
	}
	return barestr.Left(i);
}

/********************************************************************************************

>	static wxString CamResource::GetBitmapFlagString(const wxString &str)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/01/2005
	Inputs:		str - sting to look at
	Outputs:	None
	Returns:	Empty string if none found, else the flags string (without the '-')
	Purpose:	Determines the extension of a bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxString CamResource::GetBitmapFlagString(const wxString &str)
{
	// Find the rightmost dot
	INT32 i=str.Find(_T('.'), TRUE);
	if (i<0)
		return wxEmptyString;
	wxString barestr = str.Left(i);

	// Find the rightmost "-";
	i=barestr.Find(_T('-'), TRUE);
	if (i<0)
		return wxEmptyString;

	// Now check all characters after the "-" are alphanumeric
	INT32 j;
	for (j=i+1; j<(INT32)barestr.Length(); j++)
	{
		wxChar c=barestr[j];
		if ((c<_T('a')) || (c>_T('z')))
			return wxEmptyString;
	}
	return barestr.Mid(i+1);
}


/********************************************************************************************

>	static void CamResource::MakeVariantBitmaps(ResourceStringToBitmap::iterator * it=NULL)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	01/01/2005
	Inputs:		it - iterator
	Outputs:	None
	Returns:	None
	Purpose:	Make variant copies of all bitmaps
	Errors:		-
	SeeAlso:	-

If "it" is set, it will look at a single source bitmap, and make variants of that. Else it will walk
the table.

********************************************************************************************/

void CamResource::MakeVariantBitmaps(ResourceStringToBitmap::iterator * it)
{
	if (!pBitmapHash)
		return;

	// Do this entire lot for each flag
	CamArtFlags flag=CAF_CACHE_MAX;
	while ((flag=(CamArtFlags)(flag>>1)) !=0 )
	{
		if ((flag==CAF_GREYED) || (flag==CAF_SELECTED)) // We don't know how to produce anything else, so ignore
		{

			// a convenient place to store the new grey bitmaps without breaking the iterator
			ResourceStringToBitmap NewHash;
			NewHash.clear();
		
			ResourceStringToBitmap::iterator current;
			if (it)
				current=*it;
			else
				current = pBitmapHash->begin();
	
			// iterate through the old hash (if needs be)
			while (current != pBitmapHash->end())
			{
				if (current->second) // don't bother with bitmaps we have yet to demand load
				{
					wxString fn(current->first);
					//TRACEUSER("amb",_T("Trying to grey bitmap %s"),fn.c_str());
		
					// If it's a .png, but without the particular flag set
					if (IsBitmapName(fn) && !(CamArtProvider::GetBitmapFlags(fn) & flag))
					{
						// Find the greyed name of the bitmap
						wxString gfn=MakeBitmapString(GetBitmapBaseName(fn),
													CamArtProvider::MakeBitmapFlagString((CamArtFlags)
																						(CamArtProvider::GetBitmapFlags(fn) |
																							flag)),
																							GetBitmapExtension(fn));
						ResourceStringToBitmap::iterator i=pBitmapHash->find(gfn.c_str());
						if (i==pBitmapHash->end())	// ignore if we already have an entry
						{
//							TRACEUSER("amb",_T("Greying bitmap %s to bitmap %s"),fn.c_str(),gfn.c_str());
							// We want to do this, but it doesn't actually copy the image data (boo hiss)
							// wxImage *pBitmap=new wxImage(*(current->second));
							//wxBitmap b(*(current->second));
							wxImage *pBitmap = new wxImage;
							if (pBitmap)
							{
								*pBitmap=current->second->Copy();
								// WX bug - on 2.6.2 this does not copy the alpha channel. Work aroud it
								if (current->second->HasAlpha() && !pBitmap->HasAlpha())
								{
									// This will never happen if wxWidgets is fixed
									pBitmap->SetAlpha();
									memcpy( pBitmap->GetAlpha(), current->second->GetAlpha(), pBitmap->GetWidth()*pBitmap->GetHeight());
								}
								switch (flag)
								{
									case CAF_GREYED:
										MakeGreyImage(*pBitmap);
										break;
									case CAF_SELECTED:
										MakeHotImage(*pBitmap);
										break;
									default:
										ERROR3("Unsupported automatic variant");
										break;
								}
								NewHash[camStrdup(gfn.c_str())] = pBitmap;
							}
						}	
					}
				}
		
				if (it)
				{
					break; // jump out loop
				}
				else
				{
					++current;
				}
			}
		
			// Now dump the new hash into the old hash
			for( current = NewHash.begin(); current != NewHash.end(); ++current )
				(*pBitmapHash)[current->first]=current->second;
		}
	}

	return;
}

/********************************************************************************************

>	static wxImage* CamResource::GetCachedBitmap(const TCHAR * pName)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	01/01/2005
	Inputs:		pName - pointer to name of bitmap (leaf name)
	Outputs:	None
	Returns:	a pointer to the bitmap
	Purpose:	Returns a pointer to the preloaded bitmap
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

wxImage* CamResource::GetCachedBitmap(const TCHAR * pName)
{
	if (!pBitmapHash) return NULL;
	ResourceStringToBitmap::iterator i=pBitmapHash->find(pName);
	if (i==pBitmapHash->end())
	{
		// We know this file does not exist in the resources. However, we might be able to
		// make one if it's greyed and we can get the ungreyed version

		wxString fn(pName);

		// Clear each of the flags in sequence
		BOOL Found=FALSE;
		// Do this entire lot for each flag
		CamArtFlags flag=CAF_CACHE_MAX;
		while ((flag=(CamArtFlags)(flag>>1)) !=0 )
		{
			if ((flag==CAF_GREYED) || (flag!=CAF_SELECTED)) // We don't know how to produce anything else, so ignore
			{

				if (IsBitmapName(fn) && (CamArtProvider::GetBitmapFlags(fn) & flag))
				{
					// Find the ungreyed name of the bitmap
					wxString ugfn=MakeBitmapString(GetBitmapBaseName(fn),
												CamArtProvider::MakeBitmapFlagString((CamArtFlags)
																						(CamArtProvider::GetBitmapFlags(fn) &
																						~flag)),
																						GetBitmapExtension(fn));
					Found = TRUE; // we should at least look for another bitmap
	
					// Recursively call ourself to load the ungreyed bitmap. We don't care about the result, but this
					// will insert the greyed version into the cache as a byproduct
	
#ifdef _DEBUG
					static INT32 recursion=0;
					recursion++;
					ERROR3IF(recursion>16,"Too much recursion in CamResource::GetCachedBitmap");
#endif
					GetCachedBitmap(ugfn.c_str());
#ifdef _DEBUG
					recursion--;
#endif
				}
			}
		}
		if (!Found)
			return NULL;

		i=pBitmapHash->find(pName); // reset iterator
		if (i==pBitmapHash->end())
			return NULL; // we couldn't find a greyed version
	}

	if (!i->second)
	{
		// There is no bitmap there, but the name is there. We must be demand loading

		wxImage * pBitmap = new wxImage;
		if (!pBitmap) return NULL;

		if (!LoadwxImage(*pBitmap, pName))
		{
			ERROR3_PF((_T("Could not load bitmap %s"), pName));
			delete pBitmap;
			return NULL;
		}

		// Add it to the cache
		i->second=pBitmap;

		// Generate variant versions if we should
		MakeVariantBitmaps(&i);

		return pBitmap;
	}
	return i->second;
}


/********************************************************************************************

>	static BOOL CamResource::LoadBitmaps()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	01/01/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Add internal and external bitmaps
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

BOOL CamResource::LoadBitmaps()
{
	TRACET(_T("LoadBitmaps() called"));
	wxString ExternalRes = GetResourceFilePath(wxString(_T("")), TRUE);
	wxString InternalRes = GetResourceFilePath(wxString(_T("")), FALSE);
	TRACET(_T("LoadBitmaps() adding External bitmaps"));
	if (!AddBitmaps(ExternalRes)) return FALSE;
	TRACET(_T("LoadBitmaps() adding Internal bitmaps"));
	if (InternalRes != ExternalRes)
		if (!AddBitmaps(InternalRes)) return FALSE;
	TRACET(_T("LoadBitmaps() making grey copies"));
	MakeVariantBitmaps(); // make grey copies of each bitmap
	TRACET(_T("LoadBitmaps() done"));
	return TRUE;
}

/********************************************************************************************

>	static BOOL CamResource::Init()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Initialize resources
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

BOOL CamResource::Init()
{
	void * pFile=NULL;
	UINT32 Length=0;

	TRACET(_T("CamResource::Init() called"));

	pHash = new (ResIDToString);
	if (!pHash) return FALSE;

	pObjectNameHash = new (ResIDToString);
	if (!pObjectNameHash) return FALSE;

	pBitmapHash = new (ResourceStringToBitmap);
	if (!pBitmapHash) return FALSE;

	// Clear our own hash tables
	pHash->clear();
	pObjectNameHash->clear();
	pBitmapHash->clear();

	// Now go through ID's translated before we got here
	CamResourceRemember * pRem = pFirstRemember;
	pFirstRemember = NULL;
	while (pRem)
	{
		if (pRem->text)
		{
//			ResourceID r = GetResourceID(pRem->text);
//			TRACEUSER("amb",_T("Fixed up static init translation from %s to %d"),pRem->text,r);
			free (pRem->text);
		}
		CamResourceRemember * pORem = pRem;
		pRem = pRem->next;
		delete pORem;
	}


	// Initialize the handlers
	wxXmlResource::Get()->InitAllHandlers();
	wxXmlResource::Get()->AddHandler(new wxCamArtControlXmlHandler);
	wxXmlResource::Get()->AddHandler(new wxCamDrawControlXmlHandler);

	if (!pwxFileSystem) pwxFileSystem = new wxFileSystem;
	if (!pwxFileSystem)
	{
		TRACE(_T("Failed to allocate file system"));
		return FALSE;
	}

	// Check for memory FS. If not present, load the handler:
	// No idea for the reason for the additional braces, but let's keep them ther
	{
		wxMemoryFSHandler::AddFile(_T("XRC_resource/dummy_file"), _T("dummy one"));
		wxFSFile *f = pwxFileSystem->OpenFile(_T("memory:XRC_resource/dummy_file"));
		wxMemoryFSHandler::RemoveFile(_T("XRC_resource/dummy_file"));
		if (f) delete f;
		else wxFileSystem::AddHandler(new wxMemoryFSHandler);
	}

	TRACET(_T("CamResource::Init() added MemoryFSHandler"));

	wxFileSystem::AddHandler(new wxZipFSHandler);

	TRACET(_T("CamResource::Init() added Zip handler, now doing AddFile() on resources"));

	GetBinaryFileInfo(&pFile, &Length);

	// Do not use a file extension, as unbelievably this adds 3 seconds to startup time pre-splash box
	// This is because it tries to initialize the MIME system (sigh)
	wxMemoryFSHandler::AddFile(_T("resources"), pFile, Length); // Irritatingly does not return errors

	// Add bitmap extenstions
	BitmapExtensions.Add(_T("png"));
	BitmapExtensions.Add(_T("cur"));

	// Initialize locale
	m_pLocale = new wxLocale();
	if (!m_pLocale) return FALSE;
	m_pLocale->Init(wxLANGUAGE_DEFAULT, wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING);
	m_pLocale->AddCatalogLookupPathPrefix(GetResourceFilePath(_T("")));
	m_pLocale->AddCatalog(_T("XaraLX"));

	TRACET(_T("CamResource::Init() launching splash screen"));
	if (!Splash()) return FALSE;

#ifdef XML_STRINGS
	TRACET(_T("CamResource::Init() now loading internal resources, calling InitXmlResource"));
	// Load the internal resources
	// We should offer the possibility of loading them from a non-XML file here
	if (!InitXmlResource()) return FALSE;

	wxYield(); // yield again to allow repaint

	TRACET(_T("CamResource::Init() starting to load string table dialog"));

	// Read the string table into the hash map
	wxDialog * pStringTable = wxXmlResource::Get()->LoadDialog(NULL, _T("STRINGTABLE"));
	if (!pStringTable)
	{
		// We can hardly internationalize the message box, as we have no strings to play with yet.
		wxMessageBox(_T("Could not load resources: Failed to locate string table"), _T("Resource loading error"));
		return FALSE;
	}	
	wxYield(); // yield again to allow repaint

	TRACET(_T("CamResource::Init() starting to process string table"));

	// Now do a depth-first search of this window's children
	ProcessWindowAndChildren(pStringTable);

	wxYield(); // yield again to allow repaint

	TRACET(_T("CamResource::Init() and deleting the dialog"));

	delete pStringTable;
#else

	TRACET(_T("CamResource::Init() now loading internal resources, calling ReadStringTableFile()"));
	if (!ReadStringTableFile())
	{
		// We can hardly internationalize the message box, as we have no strings to play with yet.
		wxMessageBox(_T("Could not load resources: Failed to locate string table"), _T("Resource loading error"));
		return FALSE;
	}	

#endif

	TRACET(_T("CamResource::Init() done processing string table, starting to load bitmaps"));

	wxYield(); // yield again to allow repaint

	// Note this won't actually load the bitmaps if we are demand loading, but it will at least get a list of them
	if (!LoadBitmaps())
		return FALSE;

	TRACET(_T("CamResource::Init() done processing bitmaps, starting to load toolbar images"));

	wxYield(); // yield again to allow repaint

	LoadwxImage(imageBevelTool, _T("lbeveltool32.png") );
	LoadwxImage(imageBezTool, _T("lbeztool32.png") );
	LoadwxImage(imageBlendTool, _T("lblendtool32.png") );
	LoadwxImage(imageContourTool, _T("lcontourtool32.png") );
	LoadwxImage(imageElipTool, _T("leliptool32.png") );
	LoadwxImage(imageFHandTool, _T("lfhandtool32.png") );
	LoadwxImage(imageGradTool, _T("lgradtool32.png") );
	LoadwxImage(imageLiveEffectTool, _T("lliveeffecttool32.png") );
	LoadwxImage(imageMouldTool, _T("lmouldtool32.png") );
	LoadwxImage(imagePenTool, _T("lpentool32.png") );
	LoadwxImage(imagePush, _T("lpushtool32.png") );
	LoadwxImage(imageRectTool, _T("lrecttool32.png") );
	LoadwxImage(imageRegShapeTool, _T("lregshapetool32.png") );
	LoadwxImage(imageSelTool, _T("lselrtool32.png") );
	LoadwxImage(imageShadrwTool, _T("lsoftshadowtool32.png") );
	LoadwxImage(imageSlicetool, _T("lslicetool32.png") );
	LoadwxImage(imageTextTool, _T("ltexttool32.png") );
	LoadwxImage(imageTransTool, _T("ltransptool32.png") );
	LoadwxImage(imageZoomTool, _T("lzoomtool32.png") );

	TRACET(_T("CamResource::Init() Added images"));

	wxYield(); // yield again to allow repaint

	if (!wxXmlResource::Get()->Load(GetResourceFilePath(_T("dialogs.xrc"))))
	{
		TRACE(_T("Could not load dialogs.xrc"));
		return FALSE;
	}

	TRACET(_T("CamResource::Init - loaded dialogs"));

	wxYield(); // yield again to allow repaint

	return TRUE;
}

/********************************************************************************************

>	static void CamResource::DeleteBitmapHashEntries()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Delete every bitmap hash table entry
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CamResource::DeleteBitmapHashEntries()
{
	if (pBitmapHash)
	{
		ResourceStringToBitmap::iterator current;

		while (!pBitmapHash->empty())
		{
			current = pBitmapHash->begin();
			wxImage * b=current->second;
			TCHAR * s=(TCHAR *)current->first;
			pBitmapHash->erase(current);
			if (b)
				delete b;
			free(s);
		}
		// for good measure
		pBitmapHash->clear();
	}
}


/********************************************************************************************

>	static BOOL CamResource::DeInit()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Initialize resources
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

BOOL CamResource::DeInit()
{
	if (pwxFileSystem)
	{
		delete (pwxFileSystem);
		pwxFileSystem = NULL;
	}

	if (pBitmapHash)
	{
		DeleteBitmapHashEntries();
		delete pBitmapHash;
		pBitmapHash = NULL;
	}

	if (pHash)
	{
		ResIDToString::iterator current;

		while (!pHash->empty())
		{
			current = pHash->begin();
			ResourceString s=current->second;
			pHash->erase(current);
			free((void *)s);
		}
		// for good measure
		pHash->clear();
		
		delete (pHash);
		pHash = NULL;
	}

	if (pObjectNameHash)
	{
		ResIDToString::iterator current;

		while (!pObjectNameHash->empty())
		{
			current = pObjectNameHash->begin();
			ResourceString s=current->second;
			pObjectNameHash->erase(current);
			free((void *)s);
		}
		// for good measure
		pObjectNameHash->clear();
		
		delete (pObjectNameHash);
		pObjectNameHash = NULL;
	}

	if (pSplashScreen)
	{
		delete (pSplashScreen);
		pSplashScreen = NULL;
	}

	if (pSplashBitmap)
	{
		delete (pSplashBitmap);
		pSplashBitmap = NULL;
	}

	if (m_pLocale)
	{
		delete (m_pLocale);
		m_pLocale = NULL;
	}

	return TRUE;
}

/********************************************************************************************

>	static BOOL CamResource::Splash()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Put up a splash screen
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

BOOL CamResource::Splash()
{
	TRACET(_T("CamResource::Splash() called"));

	
	wxImage Image;

	if (pSplashBitmap) delete pSplashBitmap;
	pSplashBitmap=NULL;

	// We'd like to get the bitmap name from the resources, but, urm, we haven't yet
	// loaded them
	if (!LoadwxImage(Image, _T("startup-lx.png") ))
	{
		TRACE(_T("Cannot load splash bitmap - possible resource compilation error?"));
		return TRUE;
	}
	
	pSplashBitmap = new wxBitmap(Image);
	if (!pSplashBitmap) return FALSE;

	pSplashScreen = new wxSplashScreen(*pSplashBitmap,
          wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_NO_TIMEOUT,
          0, NULL, -1, wxDefaultPosition, wxDefaultSize,
          wxSIMPLE_BORDER
#if !defined (_DEBUG)
		  |wxSTAY_ON_TOP // Only stay on top in non-debug builds - too annoying for preinit debugging
#endif
		  );
	if (!pSplashScreen) return FALSE;

  	wxYield();

	return TRUE;
}

/********************************************************************************************

>	static BOOL CamResource::DoneInit()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Signals to the resouce system initialization is completed. The splah
				screen can thus be removed
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

BOOL CamResource::DoneInit()
{
	TRACET(_T("CamResource::DoneInit() called"));
	if (pSplashScreen)
	{
		delete pSplashScreen;
		pSplashScreen = NULL;
	}
	if (pSplashBitmap)
	{
		delete (pSplashBitmap);
		pSplashBitmap = NULL;
	}
	return TRUE;
}

/********************************************************************************************

>	CamResource::CamResource()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor
	Errors:		-
	SeeAlso:	-

Example of how to use this class:

	CamResource myResource;								// Allocate a new resource object

	CCLexFile * myFile = myResource.Open(_R(IDB_BLOBBY));	// Open the BLOBBY bitmap
														// Could have specified a string
														// instead

	myFile->Read( ... )									// or whatever

The user can either close the file, or not. However, operations must NOT be done on the
file once the corresponding resource object is destroyed (as the resource object may have
allocated a memory copy of the image). To prevent damage, the destructor will ALWAYS
close the file and free the resources.

The caller should NOT delete the open CCLexFile object that is returned. Either use
our Close mechanism, or let the destructor take its course.

********************************************************************************************/

CamResource::CamResource()
{
	Size = 0;
	pMemory = NULL;
	pFile = NULL;
	pwxFSFile = NULL;
	return;
}


/********************************************************************************************

>	CamResource::~CamResource()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CamResource::~CamResource()
{
	Close(); // releases pFile and pMemory
}

/********************************************************************************************

>	void CamResource::Close()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Close the relevant file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CamResource::Close()
{
	if (pFile)
	{
		pFile->close();
		delete (pFile);
		pFile = NULL;
	}

	if (pwxFSFile)
	{
		delete (pwxFSFile);
		pwxFSFile=NULL;
	}

	if (pMemory) free(pMemory);
	pMemory = NULL;

	Size = 0; // leave in tidy state
}

/********************************************************************************************

>	CCLexFile * CamResource::Open( const TCHAR * pFileName, BOOL ErrorReporting=TRUE, BOOL ExceptionThrowing=FALSE)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		pFileName - **LEAF** element of filename - e.g. "deffill.bmp"
	Outputs:	-
	Returns:	Pointer to the open file, or NULL for a failure
	Purpose:	Return a pointer to the open file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CCLexFile * CamResource::Open( const TCHAR * pFileName, BOOL ErrorReporting, BOOL ExceptionThrowing)
{
	Close();

	// For the time being, we always use a memory file. We load the resource in using the
	// wxWidgets VFS code (allowing us to load it out the Zip file), and keep the memory
	// around till we need to zap it.

	wxFSFile * pTwxFSFile = pwxFileSystem->OpenFile(GetResourceFilePath(pFileName));
	if (!pTwxFSFile)
	{
		return NULL;
	}

	wxInputStream * pStream = pTwxFSFile->GetStream(); // we don't have to delete this ourselves
	if (!pStream)
	{
		delete (pTwxFSFile);
		return NULL;
	}

	UINT32 newsize=pStream->GetSize();

	if (!newsize)
	{
		delete (pTwxFSFile);
		return NULL;
	}

	void * newmem = malloc(newsize+4); // safety
	if (!newmem)
	{
		delete (pTwxFSFile);
		return NULL;
	}

	memset(newmem, 0, newsize);
	pStream->Read(newmem, newsize); // throws exceptions if can't read
	
	delete (pTwxFSFile);	// which closes it

	CCMemFile * pMemFile=new CCMemFile (newmem, newsize, CCMemRead, ErrorReporting, ExceptionThrowing);
	if (!pMemFile)
	{
		free(newmem);
		return NULL;
	}

	// OK, we've done it. So we can store the variables we have
	pFile = pMemFile;
	Size = newsize;
	pMemory = newmem;

	return (pFile);
}


/********************************************************************************************

>	CCLexFile * CamResource::Open( ResourceID ID, BOOL ErrorReporting=TRUE, BOOL ExceptionThrowing=FALSE);


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		ResourceID of the file in question
	Outputs:	-
	Returns:	Pointer to the open file, or NULL for a failure
	Purpose:	Return a pointer to the open file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CCLexFile * CamResource::Open( ResourceID ID, BOOL ErrorReporting, BOOL ExceptionThrowing)
{
	return Open(GetText(ID), (BOOL) ErrorReporting, (BOOL) ExceptionThrowing);
}

/********************************************************************************************

>	wxFSFile * CamResource::OpenwxFSFile( ResourceID ID);


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		ResourceID of the file in question
	Outputs:	-
	Returns:	Pointer to the open file stream, or NULL for a failure
	Purpose:	Return a pointer to the open file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxFSFile * CamResource::OpenwxFSFile( ResourceID ID )
{
	return OpenwxFSFile(GetText(ID));
}

/********************************************************************************************

>	wxFSFile * CamResource::OpenwxFSFile( const TCHAR * pFileName );


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		pFileName - pointer to the LEAF filename
	Outputs:	-
	Returns:	Pointer to the open file stream, or NULL for a failure
	Purpose:	Return a pointer to the open file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxFSFile * CamResource::OpenwxFSFile( const TCHAR * pFileName )
{
	wxFSFile * pTwxFSFile = pwxFileSystem->OpenFile(GetResourceFilePath(pFileName));
	if (!pTwxFSFile)
	{
		return NULL;
	}

	pwxFSFile=pTwxFSFile;

	return pwxFSFile;
}

/********************************************************************************************

>	static void CamResource::MakeGreyImage (wxImage & rImage)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		Image - Image to make grey
	Outputs:	
	Returns:	-
	Purpose:	Static function to grey an image
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CamResource::MakeGreyImage (wxImage & rImage)
{
	// now we want to grey the thing ourselves
	INT32 x;
	INT32 y;

	BOOL hasalpha = rImage.HasAlpha();

	// give it an alpha channel
	if (!hasalpha) rImage.InitAlpha(); // copies alpha into the mask if it had one.
	rImage.SetMask(FALSE); // ignore the mask - we now have alpha

	UINT8 * p = rImage.GetData();
	UINT8 * alpha = rImage.GetAlpha();
	
	INT32 w=rImage.GetWidth();
	INT32 h=rImage.GetHeight();

	ColourMatrix Whiten = ColourMatrix::Whiten();
	ColourMatrix Grey   = ColourMatrix::Grey();

	// If we do the copy backwards we can avoid copying the data
	// Start off pointing at the last pixel
	const INT32 size = w * h;
	INT32 m = size - 1;
	INT32 n = (size - 1) * 3;

	// Don't use SetRGB - it does funny things to the alpha channel
	for (y = h-1; y>=0; y--) for (x = w-1 ; x>=0; x--)
	{
		// packed bytes, same order on each platform
		ColourVector togrey;
		togrey.SetRGBA (p[n], p[n+1], p[n+2], alpha[m]);

		togrey=togrey.Apply(Grey);

		ColourVector towhiten(0.0,0.0,0.0,0.0); // transparent
		// whitened image is displaced by one pixel down and right. So we sample
		// from a pixel left and up (this is width + 1 pixels earlier in the image)
		if ((y>0) && (x>0))
		{
			towhiten.SetRGBA(p[n-(w+1)*3],
							 p[n-(w+1)*3+1],
							 p[n-(w+1)*3+2],
							 alpha[m-(w+1)]);
		}

		towhiten=towhiten.Apply(Whiten);
		ColourVector result = towhiten.ComposeOnTop(togrey);

		// Write the colour back into the image
		p[n]=result.GetR();
		p[n+1]=result.GetG();
		p[n+2]=result.GetB();
		alpha[m]=result.GetA();

		// Go to "next" pixel (going backwards)
		n-=3;
		m--;
	}
}

/********************************************************************************************

>	static void CamResource::MakeHotImage (wxImage & rImage)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		Image - Image to make hot
	Outputs:	
	Returns:	-
	Purpose:	Static function to make a hot version of an image
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CamResource::MakeHotImage (wxImage & rImage)
{
	// now we want to grey the thing ourselves
	INT32 x;
	INT32 y;
	INT32 n=0;
	INT32 m=0;

	BOOL hasalpha = rImage.HasAlpha();

	// give it an alpha channel
	if (!hasalpha) rImage.InitAlpha(); // copies alpha into the mask if it had one.
	rImage.SetMask(FALSE); // ignore the mask - we now have alpha

	UINT8 * p = rImage.GetData();
	UINT8 * alpha = rImage.GetAlpha();
	
	INT32 w=rImage.GetWidth();
	INT32 h=rImage.GetHeight();

	ColourMatrix Boost = ColourMatrix::Boost();

	// Don't use SetRGB - it does funny things to the alpha channel
	for (y = 0; y<h; y++) for (x = 0 ; x<w; x++)
	{
		// packed bytes, same order on each platform
		ColourVector result;
		result.SetRGBA (p[n], p[n+1], p[n+2], alpha[m]);

		result=result.Apply(Boost);

		p[n]=result.GetR();
		p[n+1]=result.GetG();
		p[n+2]=result.GetB();
		alpha[m]=result.GetA();

		n+=3;
		m++;
	}

}

/********************************************************************************************

>	static BOOL CamResource::LoadwxImage (wxImage & rImage, const TCHAR * pFileName, BOOL Grey=FALSE)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		Image - Image to alter. pFileName - pointer to the LEAF filename
				Grey - to grey the image
	Outputs:	
	Returns:	Success or otherwise
	Purpose:	Static function to load a bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamResource::LoadwxImage (wxImage & rImage, const TCHAR * pFileName, BOOL Grey)
{
	CamResource Resource;
	wxFSFile* f=Resource.OpenwxFSFile(pFileName);
	if (!f) return FALSE;
	wxInputStream * pStream=f->GetStream();
	if (!pStream) return FALSE;
	if (!rImage.LoadFile(*pStream, wxBITMAP_TYPE_ANY)) return FALSE;

	// Grey the image if required
	if (Grey) MakeGreyImage(rImage);

	return TRUE;
}

/********************************************************************************************

>	static BOOL CamResource::LoadwxBitmap (wxBitmap & rBitmap, const TCHAR * pFileName, BOOL Grey=FALSE)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		Image - Image to alter. pFileName - pointer to the LEAF filename
				Grey - to Grey the image
	Outputs:	
	Returns:	Success or otherwise
	Purpose:	Static function to load a bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamResource::LoadwxBitmap (wxBitmap & rBitmap, const TCHAR * pFileName, BOOL Grey)
{
	// Annoyingly, wxBitmap does not support the stream class
	wxImage i;
	if (!LoadwxImage(i, pFileName)) return FALSE;
	wxBitmap b(i, -1); // for now, always use screen depth, not least as GDK doesn't support anything except this & 1bpp
	rBitmap = b;
	return TRUE;
}




#if 0
	// OLD CODE FROM CAMELOT.CPP

	m_strResourcePath = CamResource::GetResourcePath();
	// Load strings into our string table
	// InitStringTable();


	// Calculate the absolute path to resources
	m_strResourcePath = argv[0];
#if defined(__WXMSW__)
	m_strResourcePath = m_strResourcePath.BeforeLast( _T('\\') ) + _T("\\Resources\\");
#else
	if( -1 == m_strResourcePath.Find('/') )
		m_strResourcePath = _T("./Resources/");
	else
		m_strResourcePath = m_strResourcePath.BeforeLast( _T('/') ) + _T("/Resources/");
	{
		wxString	strRes( m_strResourcePath + _T("deffill.bmp") );
		
#if 0 == wxUSE_UNICODE
		FILE *fpResFile = _tfopen( (const TCHAR *)strRes.c_str(), "rb");
#else
		// GNU\Linux seems to have no unicode filesystem name support, so we
		// just get on and do the needed conversion as efficiently as possible
		size_t				cchTmp = camWcstombs( NULL, strRes.c_str(), 0 ) + 1;
		PSTR				pszTmp = PSTR( alloca( cchTmp ) );
		camWcstombs( pszTmp, strRes.c_str(), cchTmp );
	
		FILE *fpResFile = fopen( pszTmp, "rb" );
#endif

		if( NULL == fpResFile )
		{
			wxMessageBox( _T("In order for the  toolbar resources to be locatable "
				"you must either run the XaraLX executable from the current directory or "
				"specify the entire path to it."), _T("Resource loading error") );
			
			return FALSE;
		}
		
		fclose( fpResFile );
	}
#endif

#endif
