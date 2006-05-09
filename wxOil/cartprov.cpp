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

#include "cartprov.h"
#include "camframe.h"
#include "cartctl.h"

CamArtProvider * CamArtProvider::m_ArtProvider = NULL;

/*****************************************************************************

This file contains the class for CamArtProvider, derived from
wxEvtHandler. This contains a mapping allowing dialogs of ANY class
derived from wxDialog to call the relevant bits of DialogManager.

*****************************************************************************/

IMPLEMENT_CLASS( CamArtProvider, wxEvtHandler )
BEGIN_EVENT_TABLE(CamArtProvider, wxEvtHandler)
	EVT_CAMARTPROVIDER_GETBITMAP (wxID_ANY, CamArtProvider::GetBitmapEvent)
	EVT_CAMARTPROVIDER_INVALIDATEART (wxID_ANY, CamArtProvider::InvalidateArtEvent)
END_EVENT_TABLE();

DEFINE_EVENT_TYPE(wxEVT_CAMARTPROVIDER_GETBITMAP)
DEFINE_EVENT_TYPE(wxEVT_CAMARTPROVIDER_INVALIDATEART)
IMPLEMENT_DYNAMIC_CLASS( wxCamArtProviderEvent, wxEvent )

/********************************************************************************************

>	CamArtProvider::CamArtProvider()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CamArtProvider::CamArtProvider()
{
	m_pHash = NULL;
	m_GetBitmapEventPending = FALSE;
	m_InvalidateArtEventPending = FALSE;
	m_pMissingImage = NULL;

	m_pMissingImage = new wxImage; // if this returns NULL, all missing bitmaps will be blank. Oh well
	if (m_pMissingImage) CamResource::LoadwxImage(*m_pMissingImage, _T("missing.png") );
	if (m_pMissingImage) m_pMissingBitmap=new wxBitmap(*m_pMissingImage, -1);

	m_pHash = new ResIDWithFlagsToBitmapPtr; // if this returns NULL, Init will whinge, so don't check
}

/********************************************************************************************

>	CamArtProvider::~CamArtProvider()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CamArtProvider::~CamArtProvider()
{
	if (m_pHash)
	{
		DeleteHashContents();
		if (m_pHash) delete m_pHash;
		m_pHash = NULL;
	}
	if (m_pMissingImage)
	{
		delete(m_pMissingImage);
		m_pMissingImage=NULL;
	}
	if (m_pMissingBitmap)
	{
		delete(m_pMissingBitmap);
		m_pMissingBitmap=NULL;
	}
}

/********************************************************************************************

>	static wxColor CamArtProvider::DarkenColour(const wxColor& c, INT32 amount)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		c - a wxColour
				amount - amount to darken it by
	Outputs:	a darkened colour
	Returns:	-
	Purpose:	Returns a darkened colour
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxColor CamArtProvider::DarkenColour(const wxColor& c, INT32 amount)
{
    INT32 r = c.Red(), g = c.Green(), b = c.Blue();
    return wxColour((r*amount)/100, (g*amount)/100, (b*amount)/100);
}

/********************************************************************************************

>	static wxColor CamArtProvider::LightenColour(const wxColor& c, INT32 amount)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		c - a wxColour
				amount - amount to lighten it by
	Outputs:	a lightened colour
	Returns:	-
	Purpose:	Returns a lightened colour
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxColor CamArtProvider::LightenColour(const wxColor& c, INT32 amount)
{
    INT32 r = 255-c.Red(), g = 255-c.Green(), b = 255-c.Blue();
    return wxColour(255-(r*amount)/100, 255-(g*amount)/100, 255-(b*amount)/100);
}

/********************************************************************************************

>	static BOOL CamArtProvider::Init()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Initialize the class
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

BOOL CamArtProvider::Init()
{
	ERROR3IF(m_ArtProvider, "Double Init of ArtProvider");
	m_ArtProvider=new(CamArtProvider);
	ERROR2IF(!m_ArtProvider, FALSE, "Cannot get an ArtProvider");

	return TRUE;
}

/********************************************************************************************

>	static void CamArtProvider::DeInit()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Initialize resources
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

void CamArtProvider::DeInit()
{
	if (m_ArtProvider)
	{
		delete m_ArtProvider;
		m_ArtProvider = FALSE;
	}
}

/********************************************************************************************

>	void CamArtProvider::DeleteHashContents()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Delete every hash table entry
	Errors:		-
	SeeAlso:	-

Note we don't need to delete the bitmaps themselves - CamResource:: does that.
Thus right now this routine does nothing much with the hash contents.

********************************************************************************************/

void CamArtProvider::DeleteHashContents()
{
	if (m_pHash)
	{
		ResIDWithFlagsToBitmapPtr::iterator current;

		while (!m_pHash->empty())
		{
			current = m_pHash->begin();
			// wxImage * s=current->second.m_pImage;
			m_pHash->erase(current);
			// delete s;  -- we don't need to do this, CamResource:: does this
		}
		// for good measure
		m_pHash->clear();
		
		delete (m_pHash);
		m_pHash = NULL;
	}
}

/********************************************************************************************

>	void CamArtProvider::ArtLoad(BOOL newbitmaps = FALSE, BOOL defer = TRUE)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		newbitmaps - set TRUE if we should forget cache of all bitmaps that were
							 missing last time. Also causes existing pending event flag
							 to be ignored in case an event got lost (should never happen).
				defer      - set TRUE if the art load is to be deferred
	Outputs:	-
	Returns:	-
	Purpose:	Schedule a GetBitmap event, unless one is already scheduled
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CamArtProvider::ArtLoad(BOOL newbitmaps, BOOL defer)
{
	// If we already have an event pending, don't generate a new one
	if (m_GetBitmapEventPending && !newbitmaps) return;

	if (newbitmaps && m_pMissingImage && m_pHash && !m_pHash->empty())
	{
		ResIDWithFlagsToBitmapPtr::iterator current;

		for( current = m_pHash->begin(); current != m_pHash->end(); ++current )
		{
			// we don't need to worry about memory management as deleting the
			// bitmaps are CamResource::'s responsibility
			if (current->second.m_pImage == m_pMissingImage)
				m_pMissingImage = NULL; // mark as required to be loaded on the event
		}
	}

	// Send ourselves a deferred event
	m_GetBitmapEventPending = TRUE;
	wxCamArtProviderEvent event (wxEVT_CAMARTPROVIDER_GETBITMAP, 0, defer);
	if (defer)
		AddPendingEvent(event);
	else
		ProcessEvent(event);	
}


/********************************************************************************************

>	CamArtProvider::GetBitmapEvent(wxCamArtProviderEvent& event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Called when we need to load some art
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void CamArtProvider::GetBitmapEvent(wxCamArtProviderEvent& event)
{
	if (m_pHash && !m_pHash->empty())
	{
		BOOL FoundBitmapToLoad = FALSE;
		ResIDWithFlagsToBitmapPtr::iterator current;
		for( current = m_pHash->begin(); current != m_pHash->end(); ++current )
		{
			if (!current->second.m_pImage) // we have no blank bitmap
			{
				FoundBitmapToLoad = TRUE;

//				ResourceID r = GetResourceID(current->first);
//				CamArtFlags f = GetFlags(current->first);
//				TRACE(_T("Hash for bitmap 0x%x:%d %s"),f,r,CamResource::GetObjectName(r));

				wxImage * pBitmap = MakeBitmap(current->first);
				current->second.m_pImage = pBitmap?pBitmap:m_pMissingImage;
			}
		}

		if (FoundBitmapToLoad)
		{
			if (!m_InvalidateArtEventPending)
			{
				m_InvalidateArtEventPending = TRUE;
				BOOL defer=!event.m_deferred; //Defer the invalidate if this is immediate
				wxCamArtProviderEvent event (wxEVT_CAMARTPROVIDER_INVALIDATEART, 0, defer);
				if (defer)
					AddPendingEvent(event);
				else
					ProcessEvent(event);	
			}
		}
	}
	m_GetBitmapEventPending=FALSE;
}

/********************************************************************************************

>	CamArtProvider::GetBitmapEvent(wxCamArtProviderEvent& event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Called when we need to load some art
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void CamArtProvider::InvalidateArtEvent(wxCamArtProviderEvent& event)
{
	m_InvalidateArtEventPending = FALSE;
	InvalidateAllArt();
}

/********************************************************************************************

>	void CamArtProvider::InvalidateAllArt()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Invalidates every window which has art in it
	Errors:		-
	SeeAlso:	-

Note this function should do a broadcast to all, which will allow others to invalidate
themselves.

********************************************************************************************/


void CamArtProvider::InvalidateAllArt()
{
	wxWindow * pWindow = CCamFrame::GetMainFrame();
	if (pWindow) InvalidateAllArtInChildren(pWindow);

	// Broadcast an art changing message here!

}

/********************************************************************************************

>	void CamArtProvider::InvalidateAllArtInChildren(wxWindow * pWindow)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Invalidates every window which has art in it
	Errors:		-
	SeeAlso:	-

Note this function should do a broadcast to all, which will allow others to invalidate
themselves.

********************************************************************************************/


void CamArtProvider::InvalidateAllArtInChildren(wxWindow * pWindow)
{
	// Process this one
	if (pWindow->IsKindOf(CLASSINFO(wxCamArtControl)))
		((wxCamArtControl *)pWindow)->NewBitmap();

	// Now process children if any
	wxWindowList::Node * pNode = pWindow->GetChildren().GetFirst();
	while (pNode)
	{
		InvalidateAllArtInChildren(pNode->GetData());
		pNode = pNode->GetNext();
	}
	return;
}

/********************************************************************************************

>	void CamArtProvider::EnsureBitmapLoaded(ResourceID Resource, BOOL SkipArtLoad=FALSE)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		Resource - the ResourceID of the art to ensure is loaded
	Outputs:	-
	Returns:	-
	Purpose:	Passes an event to DialogManager::Event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void CamArtProvider::EnsureBitmapLoaded(ResourceID Resource, BOOL SkipArtLoad)
{
	UINT32 f;
	for (f = 0; f<CAF_CACHE_MAX; f++)
	{
		FindBitmap(Resource, (CamArtFlags)f, SkipArtLoad);	// Find all the variants of the bitmap, thus ensuring a load
	}
}

/********************************************************************************************

>	void CamArtProvider::EnsureChildBitmapsLoaded(wxWindow * pWindow = NULL, BOOL SkipArtLoad=FALSE)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Invalidates every window which has art in it
	Errors:		-
	SeeAlso:	-

Note this function should do a broadcast to all, which will allow others to invalidate
themselves. Note that we bunch up all the art loading until the end by ensuring that
internally we call with SkipArtLoad set.

********************************************************************************************/


void CamArtProvider::EnsureChildBitmapsLoaded(wxWindow * pWindow, BOOL SkipArtLoad)
{
	if (!pWindow)
		pWindow = CCamFrame::GetMainFrame();

	if (!pWindow) return;

	// Process this one
	if ( (pWindow->IsKindOf(CLASSINFO(wxCamArtControl))) &&
		 !(((wxCamArtControl*)pWindow)->GetStyle() & wxCACS_TEXT) ) // don't bother finding bitmaps for text ones, as there isn't one!
		EnsureBitmapLoaded((ResourceID)(pWindow->GetId()), TRUE); // always skip the art load

	// Now process children if any
	wxWindowList::Node * pNode = pWindow->GetChildren().GetFirst();
	while (pNode)
	{
		EnsureChildBitmapsLoaded(pNode->GetData(), TRUE); // Always skip the art load
		pNode = pNode->GetNext();
	}

	if (!SkipArtLoad)
		ArtLoad(FALSE, FALSE);

	return;
}

/********************************************************************************************

>	void CamArtProvider::ReloadAllArt()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Reloads all the art
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CamArtProvider::ReloadAllArt()
{
	m_GetBitmapEventPending=FALSE; // to be sure
	DeleteHashContents();
	EnsureChildBitmapsLoaded();
}

/********************************************************************************************

>	ResIDWithFlagsToBitmapPtr::iterator CamArtProvider::Find(ResourceID Resource, CamArtFlags Flags = 0, BOOL SkipArtLoad=FALSE)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		Resource - the resource ID to find
				Flags - the flags to find
	Outputs:	-
	Returns:	a pointer to the bitmap that has been foudn
	Purpose:	Finds the cached bitmap with the relevant resource ID & flags
	Errors:		-
	SeeAlso:	-

may return NULL if there is no default "missing bitmap" that could be loaded

********************************************************************************************/


ResIDWithFlagsToBitmapPtr::iterator CamArtProvider::Find(ResourceID Resource, CamArtFlags Flags, BOOL SkipArtLoad)
{
	ResourceIDWithFlags f = CombineFlags(Resource, (CamArtFlags)(Flags & (CAF_CACHE_MAX-1)));
	if (!m_pHash) return m_pHash->end();

	// get the bitmap from the hash
	ResIDWithFlagsToBitmapPtr::iterator i=m_pHash->find(f);
	if (i==m_pHash->end())
	{
		// Oh dear, we haven't found an entry. We thus create one and schedule a load
		ImageAndBitmap IandB;
		(*m_pHash)[f]=IandB;
		if (!SkipArtLoad)
			ArtLoad(FALSE, FALSE);
		// See if it's appeared
		i=m_pHash->find(f);
	}

	return i;
}

/********************************************************************************************

>	wxImage * CamArtProvider::FindImage(ResourceID Resource, CamArtFlags Flags = 0, BOOL SkipArtLoad=FALSE)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		Resource - the resource ID to find
				Flags - the flags to find
	Outputs:	-
	Returns:	a pointer to the bitmap that has been foudn
	Purpose:	Finds the cached bitmap with the relevant resource ID & flags
	Errors:		-
	SeeAlso:	-

may return NULL if there is no default "missing bitmap" that could be loaded

********************************************************************************************/


wxImage * CamArtProvider::FindImage(ResourceID Resource, CamArtFlags Flags, BOOL SkipArtLoad)
{
	ResIDWithFlagsToBitmapPtr::iterator i=Find(Resource, Flags, SkipArtLoad);
	if (i==m_pHash->end())
	{
		return m_pMissingImage;
	}
	wxImage *pImage = i->second.m_pImage;

	return pImage?pImage:m_pMissingImage;
}

/********************************************************************************************

>	wxImage * CamArtProvider::FindBitmap(ResourceID Resource, CamArtFlags Flags = 0, BOOL SkipArtLoad=FALSE)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		Resource - the resource ID to find
				Flags - the flags to find
	Outputs:	-
	Returns:	a pointer to the bitmap that has been foudn
	Purpose:	Finds the cached bitmap with the relevant resource ID & flags
	Errors:		-
	SeeAlso:	-

may return NULL if there is no default "missing bitmap" that could be loaded

We look for an image, and if there is an attached bitmap, use that, else make one

********************************************************************************************/


wxBitmap * CamArtProvider::FindBitmap(ResourceID Resource, CamArtFlags Flags, BOOL SkipArtLoad)
{
	ResIDWithFlagsToBitmapPtr::iterator i=Find(Resource, Flags, SkipArtLoad);
	if (i==m_pHash->end())
	{
		return m_pMissingBitmap;
	}
	wxBitmap *pBitmap = i->second.m_pBitmap;
	if (pBitmap) return pBitmap;

	// No bitmap. Do we have an image?
	wxImage *pImage = i->second.m_pImage;

	if (!pImage || (pImage==m_pMissingImage))
		return m_pMissingBitmap;

	// OK make a bitmap
	pBitmap = new wxBitmap(*pImage, -1); // for now, always use screen depth, not least as GDK doesn't support anything except this & 1bpp
	if (!pBitmap) return m_pMissingBitmap;

	i->second.m_pBitmap=pBitmap;
	return pBitmap;
}

/********************************************************************************************

>	void CamArtProvider::Draw(wxDC& dc, const wxRect & rect, ResourceID Resource, CamArtFlags Flags = 0, const wxString &text = wxEmptyString)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		dc - the wxDC to draw into
				Resource - the ResourceID to load
				CamArtFlags - the flags to use for drawing
				InternalBorder - the internal border (in pixels) - see note
	Outputs:	-
	Returns:	pointer to a string containing the event number
	Purpose:	Draws the art into the appropriate DC
	Errors:		-
	SeeAlso:	-

Note that the internal border is the TOTAL internal border (i.e. left plus right). Border
pixels will be allocated preferentially on left and top, to right and bottom. Rect should
already have been inflated (wxCamArtControl does this by increasing its client area).

********************************************************************************************/

void CamArtProvider::Draw(wxDC& dc, const wxRect & rect, ResourceID Resource, CamArtFlags Flags, const wxString &text)
{
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	dc.SetPen(*wxTRANSPARENT_PEN);
//	dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
	
//	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	dc.SetBrush(*wxTRANSPARENT_BRUSH); // Needed in case we need to paint 3D furniture, but not the button top (ALWAYS3D)

	wxRect brect = rect;

	UINT32 bitmapoffsetX=0;
	UINT32 bitmapoffsetY=0;

	if ((Flags & CAF_PUSHBUTTON) && !rect.IsEmpty())
	{
		// 3Dness
		brect.width-=1;
		brect.height-=1;
		
		if (Flags & CAF_SELECTED)
		{
			// It's pushed in
			brect.x++;
			brect.y++;
		}

		UINT32 InternalBorderX = (Flags & CAF_NOINTERNALBORDER)?0:2;
		UINT32 InternalBorderY = (Flags & CAF_NOINTERNALBORDER)?0:1;

		bitmapoffsetX=1+((InternalBorderX+1)>>1); // this is a completely independent offset to the brect stuff, which gives the bitmap a border
		bitmapoffsetY=1+((InternalBorderY+1)>>1); // this is a completely independent offset to the brect stuff, which gives the bitmap a border

//		BOOL FaceSelected = ( (Flags & CAF_SELECTED) || (Flags & CAF_BUTTONHOVER) );
//		BOOL Border3D = ( (Flags & CAF_SELECTED) || (Flags & CAF_ALWAYS3D));

		BOOL FaceSelected = ( (Flags & CAF_SELECTED) || ((Flags & CAF_ALWAYS3D) && (Flags & CAF_BUTTONHOVER)) ); // highlight face on hover if always3D
		BOOL Border3D = ( (Flags & CAF_SELECTED) || (Flags & CAF_ALWAYS3D) || (Flags & CAF_BUTTONHOVER));

		// Change the brush if the face is selected
		if (FaceSelected)
		{
			if (Flags & CAF_TOOLBACKGROUND)
				dc.SetBrush(wxColour(255,255,192)); // A light yellow - we should load this from somewhere else
			else
			{	
				if (Flags & CAF_BUTTONHOVER)
					dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHIGHLIGHT));
				else	
					dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
			}
		}

		if (Border3D)
		{
			// draw the light highlights
			dc.SetPen(wxSystemSettings::GetColour((Flags & CAF_SELECTED)?wxSYS_COLOUR_3DHIGHLIGHT:wxSYS_COLOUR_3DDKSHADOW));
			// draw the background behind the button undisplaced
			dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
			// now draw the left & top bits
			dc.SetPen(wxSystemSettings::GetColour((Flags & CAF_SELECTED)?wxSYS_COLOUR_3DDKSHADOW:wxSYS_COLOUR_3DHIGHLIGHT));
			dc.DrawLine(rect.x, rect.y, rect.x+rect.width-1, rect.y);
			dc.DrawLine(rect.x, rect.y, rect.x, rect.y+rect.height-1);
		}
		else if (FaceSelected) // if not FaceSelected, we've already drawn it
		{
			// draw the background behind the button displaced
			dc.DrawRectangle(brect.x, brect.y, brect.width, brect.height);
		}
	}

	// Now apply justification to the object
	wxRect BestSize(GetSize(Resource, Flags, text));
	if (BestSize.width != rect.width)
	{
		if (Flags & CAF_RIGHT)
		{
			bitmapoffsetX += (rect.width-BestSize.width);
		}
		else if (Flags & CAF_LEFT)
		{
			// do nothing
		}
		else
		{
			bitmapoffsetX += (rect.width-BestSize.width)/2;
		}	
	}	
	if (BestSize.height != rect.height)
	{
		if (Flags & CAF_BOTTOM)
		{
			bitmapoffsetY += (rect.height-BestSize.height);
		}
		else if (Flags & CAF_TOP)
		{
			// do nothing
		}
		else
		{
			bitmapoffsetY += (rect.height-BestSize.height)/2;
		}	
	}	

	if (Flags & CAF_TEXT)
	{
		wxString ctext = GetTextInfoOrDraw(Resource, Flags, dc, TRUE, NULL, NULL, brect.x+bitmapoffsetX, brect.y+bitmapoffsetY, text);
	}
	else
	{
		// Get the bitmap with the correct flags. We here pretend the button is selected if we are
		// hovering. We can't do this in the control (grrr...) because else by the time the flags get
		// here we cannot distinguish selection from hover, which means we slab wrongly
		wxBitmap *pBitmap = FindBitmap(Resource, (CamArtFlags)(Flags|((Flags & CAF_BUTTONHOVER)?CAF_SELECTED:0) ));
		
		// plot the bitmap
		if (pBitmap)
			dc.DrawBitmap(*pBitmap, brect.x+bitmapoffsetX, brect.y+bitmapoffsetY, TRUE);
	}
}

/********************************************************************************************

>	wxSize CamArtProvider::GetSize(ResourceID r, CamArtFlags Flags=CAF_DEFAULT, const wxString &text = wxEmptyString)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	25/01/2005
	Inputs:		r - the resource
				Flags - the flags
	Outputs:	-
	Returns:	The size of the control
	Purpose:	-
	Errors:		-
	SeeAlso:	-

Note that the internal border is the TOTAL internal border (i.e. left plus right). Border
pixels will be allocated preferentially on left and top, to right and bottom. Rect should
already have been inflated (wxCamArtControl does this by increasing its client area).

********************************************************************************************/

wxSize CamArtProvider::GetSize(ResourceID r, CamArtFlags Flags, const wxString &text)
{
	wxCoord w=24;
	wxCoord h=24;

	if (Flags & CAF_TEXT)
	{
		wxScreenDC dc;
		wxString ctext = GetTextInfoOrDraw(r, Flags, dc, FALSE, &w, &h, 0, 0, text);
	}
	else
	{
		wxBitmap * pBitmap=FindBitmap(r, Flags);
		if (pBitmap)
		{
			w=pBitmap->GetWidth();
			h=pBitmap->GetHeight();
		}
	}

	UINT32 InternalBorderX = (Flags & CAF_NOINTERNALBORDER)?0:2;
	UINT32 InternalBorderY = (Flags & CAF_NOINTERNALBORDER)?0:1;
	// 3 extra pixels for push buttons, 2 border (one on each side) and one for push displacement
	UINT32 extraX=InternalBorderX+3*((Flags & CAF_ALWAYS3D) || (Flags & CAF_PUSHBUTTON)); // 3 or 0
	UINT32 extraY=InternalBorderY+3*((Flags & CAF_ALWAYS3D) || (Flags & CAF_PUSHBUTTON)); // 3 or 0

	return wxSize(w+extraX, h+extraY);
}

/********************************************************************************************

>	wxString CamArtProvider::GetTextInfoOrDraw(ResourceID r, wxDC &dc,  BOOL Draw=FALSE, wxCoord *w=NULL, wxCoord *h=NULL,
											   wxCoord x=0, wxCoord y=0, const wxString &text = wxEmptyString)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/04/2006
	Inputs:		r - the resource
				Flags - the flags
				Draw = TRUE to draw the text
				x, y - location to draw the text (if Draw is set)
	Outputs:	w, h, pointers to fill in (can be NULL) for the text extent. Either both or neither must be NULL
	Returns:	The text
	Purpose:	Sets up the DC and returns the text
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxString CamArtProvider::GetTextInfoOrDraw(ResourceID r, CamArtFlags f, wxDC &dc, BOOL Draw/*=FALSE*/, wxCoord *w, wxCoord *h, wxCoord x, wxCoord y, const wxString &text)
{
	// find the name by looking up the ID as a string
	const TCHAR * tcname=text;

	if (w) *w=0;
	if (h) *h=0;

	if (text.IsEmpty())
	{
		tcname = CamResource::GetTextFail(r);
		if (!tcname || ( (tcname[0]==_T('-')) && !tcname[1]) )
		{
			// default to object name
			tcname = CamResource::GetObjectNameFail(r);
		}
		
		if (!tcname)
		{
			// If this goes off, then somehow we've been passed a resource ID without a reverse
			// bitmap lookup. This can normally only happen if the resource ID is corrupted, or
			// the resources are corrupted
			ERROR3_PF(("Cannot get text for resource %d",r));
			return wxEmptyString;
		}
	}

	dc.SetTextForeground(wxSystemSettings::GetColour((f & CAF_GREYED)?wxSYS_COLOUR_GRAYTEXT:wxSYS_COLOUR_BTNTEXT));
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetPointSize((f & CAF_HALFHEIGHT)?7:8);
	dc.SetFont(font);

	if (h && w)
		dc.GetTextExtent(wxString(tcname), w, h);

	if (Draw)
		dc.DrawText(wxString(tcname), x, y);

	return wxString(tcname);
}

/********************************************************************************************

>	wxImage * CamArtProvider::MakeBitmap(ResourceIDWithFlags ResWithFlags, BOOL Grey=FALSE)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		ResWithFlags - the hash key to make a bitmap for (ResourceID combined with flags)
				Grey - true if we should grey (in software) the image we find. Don't set this
				       when calling, set CAF_GREYED which will use a greyed preprepared version
					   if available
	Outputs:	-
	Returns:	a pointer to the bitmap, or NULL if one could not be made
	Purpose:	Makes a bitmap from the appropraite resource
	Errors:		-
	SeeAlso:	-

This routine is abstracted to allow an easy switch to vector art. Simlarly, it is only
called from the main event loop, so we are safe to muck arround with the current document
etc. so we can do a true render.

********************************************************************************************/

wxImage * CamArtProvider::MakeBitmap(ResourceIDWithFlags ResWithFlags)
{
	ResourceID r = GetResourceID(ResWithFlags);
	CamArtFlags f = GetFlags(ResWithFlags);
	wxImage * pBitmap = NULL;

	// find the name by looking up the ID as a string
	const TCHAR * tcname = CamResource::GetTextFail(r);
	if (!tcname || ( (tcname[0]==_T('-')) && !tcname[1]) )
	{
		// default to object name
		tcname = CamResource::GetObjectNameFail(r);
	}

	if (!tcname)
	{
		// If this goes off, then somehow we've been passed a resource ID without a reverse
		// bitmap lookup. This can normally only happen if the resource ID is corrupted, or
		// the resources are corrupted
		ERROR3_PF(("Cannot get bitmap name for resource %d",r));
		delete pBitmap;
		return FALSE;
	}

	wxString basename(tcname);
	wxString resname;

	// Iterate over all extensions
	INT32 ext;
	for (ext=0; ext<(INT32)(CamResource::BitmapExtensions.GetCount()); ext++)
	{
		resname=CamResource::MakeBitmapString(basename, MakeBitmapFlagString(f), CamResource::BitmapExtensions[ext]);
	
		pBitmap=CamResource::GetCachedBitmap(resname.c_str());
		if (pBitmap) return pBitmap;
	
		// OK, it didn't work. Try deleting the flags in order and seeing whether
		// that works. Note each of these will itself try deleting flags. Thus recursively
		// we work our way down to the base bitmap.
	
#ifdef _DEBUG
		static INT32 recursion=0;
		recursion++;
		ERROR3IF(recursion>16,"Too much recursion in CamArtProvider::MakeBitmap");
#endif
		if (!pBitmap && (f & CAF_SELECTED)) pBitmap=MakeBitmap(CombineFlags(r, (CamArtFlags)(f & ~CAF_SELECTED)));
		if (!pBitmap && (f & CAF_FOCUS)) pBitmap=MakeBitmap(CombineFlags(r, (CamArtFlags)(f & ~CAF_FOCUS)));
		if (!pBitmap && (f & CAF_SMALL)) pBitmap=MakeBitmap(CombineFlags(r, (CamArtFlags)(f & ~CAF_SMALL)));
		if (!pBitmap && (f & CAF_GREYED)) pBitmap=MakeBitmap(CombineFlags(r, (CamArtFlags)(f & ~CAF_GREYED)));
#if _DEBUG
		recursion--;
#endif
		if (pBitmap) return pBitmap;
	}	

	return NULL;
}

/********************************************************************************************

>	static CamArtFlags CamArtProvider::GetBitmapFlags(const wxString &str);


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/01/2005
	Inputs:		str - sting to look at
	Outputs:	None
	Returns:	Flags associated with a bitmap string
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CamArtFlags CamArtProvider::GetBitmapFlags(const wxString &str)
{
	wxString ext=CamResource::GetBitmapFlagString(str);
	return (CamArtFlags)(
			((ext.Find('s')>=0)?CAF_SMALL:0) |
			((ext.Find('f')>=0)?CAF_FOCUS:0) |
			((ext.Find('x')>=0)?CAF_SELECTED:0) |
			((ext.Find('g')>=0)?CAF_GREYED:0) |
			0);
}

/********************************************************************************************

>	static CamArtFlags CamArtProvider::MakeBitmapFlagString(const CamArtFlags flags)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/01/2005
	Inputs:		flags - the flags to look at
	Outputs:	None
	Returns:	Flags string associated with a bitmap
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxString CamArtProvider::MakeBitmapFlagString(const CamArtFlags flags)
{
	wxString mod;

	if (flags & CAF_SMALL) mod+=_T("s");
	if (flags & CAF_FOCUS) mod+=_T("f");
	if (flags & CAF_SELECTED) mod+=_T("x");
	if (flags & CAF_GREYED) mod+=_T("g");
	return mod;
}

