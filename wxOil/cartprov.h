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

#ifndef INC_CARTPROV
#define	INC_CARTPROV

#include "ccobject.h"

class CamArtCacheEntry;
class wxCamArtProviderEvent;

enum CamArtFlags
{
	// Caching flags start here - the art provider may have a different bitmap depending on the
	// state of these flags...
	CAF_SELECTED 		= 1<<0,
	CAF_FOCUS			= 1<<1,
	CAF_GREYED			= 1<<2,
	CAF_SMALL			= 1<<3,

	CAF_CACHE_MAX		= 1<<4, // MUST lie at the end of the flags used for caching

	// These flags do not influence the bitmap itself, but influence how it is drawn
	CAF_TOOLBACKGROUND	= 1<<5,
	CAF_PUSHBUTTON		= 1<<6,
	CAF_TOGGLEBUTTON	= 1<<7, // unused here, but for the CACS_ stuff
	CAF_ALLOWHOVER		= 1<<8,
	CAF_TEXT			= 1<<9,
	CAF_EXACTFIT		= 1<<10, // For wxCACS
	CAF_HALFHEIGHT		= 1<<11, // For wxCACS

	CAF_TOP				= 1<<12,
	CAF_BOTTOM			= 1<<13,
	CAF_LEFT			= 1<<14,
	CAF_RIGHT			= 1<<15,

	CAF_BUTTONHOVER		= 1<<16, // this is set if the pointer IS CURRENTLY hovering
	CAF_ALWAYS3D		= 1<<17, // 3D even when not pushed in

	CAF_NOINTERNALBORDER= 1<<18, // Don't paint an internal border (unused, for CACS_)

	CAF_NOAUTOREPEAT	= 1<<19, // Used for CACS_

	CAF_STATUSBARTEXT	= 1<<20, // Used for status bar - render bit before :: in bold, chop on word boundaries

	// And this goes right at the end
	CAF_MAX				= 1<<21,

	// And here is the default
	CAF_DEFAULT			= 0
};

// A very simple class which the hash function hashes to
class ImageAndBitmap : public SimpleCCObject
{
public:
	wxImage * m_pImage;
	wxBitmap * m_pBitmap;
	ImageAndBitmap(wxImage * pImage=NULL, wxBitmap * pBitmap=NULL) : m_pImage(pImage) , m_pBitmap(pBitmap) {}
	~ImageAndBitmap() {if (m_pBitmap) delete (m_pBitmap); m_pBitmap=NULL; m_pImage=NULL; }
};

// Declare the hash map from ResourceID to a bitmap cache entry pointer
// Note we have to set the hash map value to a wx type ... grrr...
//typedef wxULongLong_t ResourceIDWithFlags;
typedef UINT64 ResourceIDWithFlags;

// hashing function
class ResourceIDWithFlagsHash
{
public:
	ResourceIDWithFlagsHash() { }

	unsigned long /*TYPENOTE: Correct*/ operator()( const ResourceIDWithFlags& k ) const
		{ return (/*TYPENOTE: Correct*/unsigned long)(k ^ (k>>32));}

	ResourceIDWithFlagsHash& operator=(const ResourceIDWithFlagsHash&) { return *this; }
};

// comparison operator
class ResourceIDWithFlagsEqual
{
public:
	ResourceIDWithFlagsEqual() { }
	bool /*TYPENOTE: Correct*/ operator()( const ResourceIDWithFlags& a, const ResourceIDWithFlags& b ) const
		{ return (a==b); }

	ResourceIDWithFlagsEqual& operator=(const ResourceIDWithFlagsEqual&) { return *this; }
};

WX_DECLARE_HASH_MAP( ResourceIDWithFlags, ImageAndBitmap, ResourceIDWithFlagsHash, ResourceIDWithFlagsEqual, ResIDWithFlagsToBitmapPtr );

/*******************************************************************************************

>	class CamArtProvider : public wxEvtHandler

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	19/12/2005
	Purpose:	A derived event handler which providers art for bitmap buttons and
				other controls within Camelot
	Notes:		In the OIL
	See Also:	

This class is specificly written so it will provided cached vector-drawn art.

********************************************************************************************/

class CamArtProvider : public wxEvtHandler
{
public:
	CamArtProvider();
	~CamArtProvider(); // Note - do not call the destructor from within an event being handled!

private:
	static CamArtProvider * m_ArtProvider;

public:
	static BOOL Init();
	static void DeInit();
	static CamArtProvider * Get() {return m_ArtProvider;}

	void GetBitmapEvent(wxCamArtProviderEvent &event);
	void InvalidateArtEvent(wxCamArtProviderEvent &event);
	void Draw (wxDC& dc, const wxRect & rect, ResourceID Resource, CamArtFlags Flags = CAF_DEFAULT, const wxString &text = wxEmptyString);

	wxSize GetSize(ResourceID r, CamArtFlags Flags=CAF_DEFAULT, const wxString &text = wxEmptyString);
	
	void EnsureBitmapLoaded(ResourceID Resource, BOOL SkipArtLoad = FALSE);
	void EnsureChildBitmapsLoaded(wxWindow * pWindow = NULL, BOOL SkipArtLoad = FALSE);
	void ReloadAllArt();
	wxBitmap * FindBitmap(ResourceID Resource, CamArtFlags Flags = CAF_DEFAULT, BOOL SkipArtLoad=FALSE);
	wxImage * FindImage(ResourceID Resource, CamArtFlags Flags = CAF_DEFAULT, BOOL SkipArtLoad=FALSE);

	static CamArtFlags GetBitmapFlags(const wxString &str);
	static wxString MakeBitmapFlagString(const CamArtFlags flags);

protected:
	ResIDWithFlagsToBitmapPtr::iterator Find(ResourceID Resource, CamArtFlags Flags, BOOL SkipArtLoad);

	ResourceIDWithFlags CombineFlags(ResourceID Resource, CamArtFlags Flags) { return Resource | (((UINT64)Flags)<<32) ; }
	CamArtFlags GetFlags(ResourceIDWithFlags ResWithFlags) { return (CamArtFlags)(ResWithFlags>>32); }
	ResourceID GetResourceID(ResourceIDWithFlags ResWithFlags) { return (ResourceID)(ResWithFlags & 0xFFFFFFFF); } // mask unnecessary but for clarity

	static wxColor DarkenColour(const wxColor& c, INT32 amount);
	static wxColor LightenColour(const wxColor& c, INT32 amount);

	wxImage * MakeBitmap(ResourceIDWithFlags ResWithFlags);
	void InvalidateAllArt();
	void InvalidateAllArtInChildren(wxWindow * pWindow);
	void DeleteHashContents();
	void ArtLoad(BOOL newbitmaps = FALSE, BOOL defer=TRUE);

	wxString GetTextInfoOrDraw(ResourceID r, CamArtFlags f, wxDC &dc, BOOL Draw=FALSE, wxCoord *w=NULL, wxCoord *h=NULL,
								wxCoord x=0, wxCoord y=0, const wxString &text = wxEmptyString);

	ResIDWithFlagsToBitmapPtr * m_pHash;
	BOOL m_GetBitmapEventPending;
	BOOL m_InvalidateArtEventPending;
	wxImage * m_pMissingImage;
	wxBitmap * m_pMissingBitmap;

	DECLARE_CLASS(wxEvtHandler)
	DECLARE_EVENT_TABLE()

};


/*******************************************************************************************

>	class wxCamArtProviderEvent : public wxEvent

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	19/12/2005
	Purpose:	A derived event to allow for postprocessing of dialog events
	Notes:		In the OIL
	See Also:	

********************************************************************************************/

class wxCamArtProviderEvent : public wxEvent
{
public:
	// Aargh note wxEvent's constructor has paramaters the otherway around from this - designed to match wxCommandEvent
	wxCamArtProviderEvent(wxEventType commandType = wxEVT_NULL, INT32 id=0, BOOL deferred=FALSE ) : wxEvent(id, commandType) {m_deferred=deferred;}

	wxCamArtProviderEvent(const wxCamArtProviderEvent& event) : wxEvent(event) {m_deferred=event.m_deferred;}

	virtual wxEvent *Clone() const {return new wxCamArtProviderEvent(*this); }

	BOOL m_deferred;

	DECLARE_DYNAMIC_CLASS(wxCamArtProviderEvent);

};


typedef void (wxEvtHandler::*wxCamArtProviderEventFunction)(wxCamArtProviderEvent &);

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_CAMARTPROVIDER_GETBITMAP, 1101)
	DECLARE_EVENT_TYPE(wxEVT_CAMARTPROVIDER_INVALIDATEART, 1102)
END_DECLARE_EVENT_TYPES()

#define EVT_CAMARTPROVIDER_GETBITMAP(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_CAMARTPROVIDER_GETBITMAP, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCamArtProviderEventFunction) &fn, (wxObject *) NULL),
#define EVT_CAMARTPROVIDER_INVALIDATEART(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_CAMARTPROVIDER_INVALIDATEART, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCamArtProviderEventFunction) &fn, (wxObject *) NULL),

#endif

