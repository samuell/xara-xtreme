// $Id: clicarea.cpp 662 2006-03-14 21:31:49Z alex $
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
// Imagemap class

#include "camtypes.h"
#include "clicarea.h"

#include "webattr.h"		//For WebAddressAttribute::CopyString

//#include "resimmap.h"		//The HTML Export resources
//#include "hotlink.h"		//For _R(IDS_WEBADDRESS_SHORT_SELF)
#include "filtimag.h"		//Because everything that's written is done by the ImagemapFilter
#include "app.h"			//For the pointer to the application
#include "filtrmgr.h"		//To find the ImagemapFilter


CC_IMPLEMENT_DYNAMIC(ImagemapClickableArea, OverrideListItem)
CC_IMPLEMENT_DYNAMIC(ImagemapClickableRectangle, ImagemapClickableArea)
CC_IMPLEMENT_DYNAMIC(ImagemapClickableCircle, ImagemapClickableArea)
CC_IMPLEMENT_DYNAMIC(ImagemapClickablePolygon, ImagemapClickableArea)

/*************************************************

  ImagemapClickableArea

  ***********************************************/
/********************************************************************************************

	ImagemapClickableArea::ImagemapClickableArea()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		-
	Outputs:	-
	Purpose:	Default contructor
	
********************************************************************************************/

ImagemapClickableArea::ImagemapClickableArea()
{
	//Set our member variable pointers to NULL
	m_pcURL=m_pcFrame=NULL;
}

/********************************************************************************************

	ImagemapClickableArea::ImagemapClickableArea(TCHAR* pcURL, TCHAR* pcFrame)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		pcURL, pcFrame - the strings to put in our member variables
	Outputs:	-
	Purpose:	Specific contructor
	
********************************************************************************************/

ImagemapClickableArea::ImagemapClickableArea(TCHAR* pcURL, TCHAR* pcFrame)
{
	//First set our member variables to NULL
	m_pcURL=m_pcFrame=NULL;

	//And copy the strings across
	WebAddressAttribute::CopyString(&m_pcURL, pcURL);
	WebAddressAttribute::CopyString(&m_pcFrame, pcFrame);
}

/********************************************************************************************

	ImagemapClickableArea::~ImagemapClickableArea()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		-
	Outputs:	-
	Purpose:	Destructor
	
********************************************************************************************/

ImagemapClickableArea::~ImagemapClickableArea()
{
	//Delete our member variables if they exist
	if (m_pcURL)
		delete m_pcURL;

	if (m_pcFrame)
		delete m_pcFrame;
}

/********************************************************************************************

  ImagemapClickableArea::ImagemapClickableArea(const ImagemapClickableArea& icaCopy)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		-
	Outputs:	-
	Purpose:	Copy constructor
	
********************************************************************************************/

ImagemapClickableArea::ImagemapClickableArea(const ImagemapClickableArea& icaCopy)
{
	//Simply use the assignment operator
	*this=icaCopy;

}

/********************************************************************************************

  ImagemapClickableArea& ImagemapClickableArea::operator=(const ImagemapClickableArea& icpCopy)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		icpCopy
	Outputs:	-
	Purpose:	Assignment operator
	
********************************************************************************************/

ImagemapClickableArea& ImagemapClickableArea::operator=(const ImagemapClickableArea& icaCopy)
{
	//First set our member variables to NULL
	m_pcURL=m_pcFrame=NULL;

	//And copy the strings across
	WebAddressAttribute::CopyString(&m_pcURL, icaCopy.m_pcURL);
	WebAddressAttribute::CopyString(&m_pcFrame, icaCopy.m_pcFrame);

	return *this;

}

/********************************************************************************************

	INT32 ImagemapClickableArea::WriteURLAndFrame(CCLexFile* pfileToWrite, TCHAR* pcBuffer)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		pfileToWrite	The file to write to
				pcBuffer		The text buffer to write to

				Either of the above may be NULL.
				
	Returns:	The number of TCHARs written.

	Purpose:	Writes the URL and the frame targetting information.

				If the frame is "_self", it will not be written out.
	
********************************************************************************************/

INT32 ImagemapClickableArea::WriteURLAndFrame(CCLexFile* pfileToWrite, TCHAR* pcBuffer)
{
	PORTNOTETRACE("other","ImagemapClickableArea::WriteURLAndFrame - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	//Keep a count of the characters we write out
	INT32 lCharsWritten=0;

	ImagemapFilter * pFilter = GetImagemapFilter();
	ERROR2IF(pFilter == NULL,lCharsWritten,"ImagemapClickableArea::WriteURLAndFrame no imagemap filter");

	//Write out HREF="..."
	if (m_pcURL)
		lCharsWritten += pFilter->WriteParameterInQuotes(_R(IDS_HTMLEXPORT_HREF), m_pcURL, pfileToWrite, pcBuffer);

	//Now, is the frame something other than "_self"?
	// Neville 21/8/97 removed the is self test as it is wrong, assumes default setting is self
	// Changed so that we output blank when a new default setting is seen
	if (m_pcFrame && !WebAddressAttribute::AreStringsEqual(m_pcFrame, String_256(_R(IDS_WEBADDRESS_SHORT_DEFAULT))))
	{
		//Yes. So write out TARGET="..."
		lCharsWritten += pFilter->WriteParameterInQuotes(_R(IDS_HTMLEXPORT_TARGET), m_pcFrame, pfileToWrite, pcBuffer);
	}

	//And return the number of characters we've written
	return lCharsWritten;
#else
	return 0;
#endif
}

/********************************************************************************************

	ImagemapFilter* ImagemapClickableArea::GetImagemapFilter()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		-
				
	Returns:	A pointer to the imagemap filter

	Purpose:	Finds the imagemap filter
	
********************************************************************************************/

ImagemapFilter* ImagemapClickableArea::GetImagemapFilter()
{
	//First find the filter manager
	FilterManager* pFilterManager=Camelot.GetFilterManager();

	//If we can't find it, return NULL
	if (pFilterManager==NULL)
		return NULL;

	//Otherwise, find the imagemap filter
	return (ImagemapFilter*) pFilterManager->FindFilterFromID(FILTERID_IMAGEMAP);
}



/*************************************************

  ImagemapClickableRectangle

  ***********************************************/

/********************************************************************************************

	ImagemapClickableRectangle::ImagemapClickableRectangle() : ImagemapClickableArea()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		-
	Outputs:	-
	Purpose:	Default constructor
	
********************************************************************************************/

ImagemapClickableRectangle::ImagemapClickableRectangle() : ImagemapClickableArea()
{
	m_rect=DocRect(0,0,0,0);
}

/********************************************************************************************

	ImagemapClickableRectangle::ImagemapClickableRectangle(DocRect rectNew, TCHAR* pcURL, TCHAR* pcFrame) 
		: ImagemapClickableArea(pcURL, pcFrame)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		rectNew		The new rectangle to use
	Outputs:	-
	Purpose:	Specific constructor
	
********************************************************************************************/

ImagemapClickableRectangle::ImagemapClickableRectangle(DocRect rectNew, TCHAR* pcURL, TCHAR* pcFrame) 
: ImagemapClickableArea(pcURL, pcFrame)
{
	m_rect=rectNew;
}

/********************************************************************************************

	INT32 Write(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		pfileToWrite	The file to write to
				pcBuffer		The text buffer to write to

				Either of the above may be NULL.
				
	Returns:	The number of TCHARs written.

	Purpose:	Writes this clickable area, in client-side imagemap form,
				into the buffer or the file or both.

				This function may be called with all parameters NULL, to find out how
				many TCHARs will be written into the imagemap, and hence to find out
				how long the text buffer must be
	
********************************************************************************************/

INT32 ImagemapClickableRectangle::Write(CCLexFile* pfileToWrite, TCHAR* pcBuffer)
{
	PORTNOTETRACE("other","ImagemapClickableArea::Write - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	//Keep a count of the characters we write out
	INT32 lCharsWritten = 0;

	ImagemapFilter * pFilter = GetImagemapFilter();
	ERROR2IF(pFilter == NULL,lCharsWritten,"ImagemapClickableRectangle::Write no imagemap filter");

	//First indent the text
	pFilter->Indent();
	lCharsWritten += pFilter->WriteIndent(pfileToWrite, pcBuffer);

	//And write out <AREA
	lCharsWritten += pFilter->WriteStartOfTag(_R(IDS_HTMLEXPORT_AREA), pfileToWrite, pcBuffer);

	//Now write out SHAPE=RECTANGLE
	lCharsWritten += pFilter->WriteParameter(_R(IDS_HTMLEXPORT_SHAPE), _R(IDS_HTMLEXPORT_RECTANGLE), pfileToWrite, pcBuffer);

	//Write out the URL and the FRAME
	lCharsWritten += WriteURLAndFrame(pfileToWrite, pcBuffer);
		
	//And write out our coords
	lCharsWritten += pFilter->WriteCoords(m_rect, m_pcFrame, pfileToWrite, pcBuffer);

	//And the end of the tag
	lCharsWritten += pFilter->WriteEndOfTag(pfileToWrite, pcBuffer);

	//Then reset the indent
	pFilter->Unindent();

	//And write out a new line
	lCharsWritten += pFilter->WriteEOL(pfileToWrite, pcBuffer);

	//And return the number of characters we've written
	return lCharsWritten;
#else
	return 0;
#endif
}

/********************************************************************************************

  BOOL ImagemapClickableRectangle::OverrideFromBelow(OverrideListItem* picaOther)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		picaOther	The imagemap clickable area which we want to know
							if it's overridden or not
				
	Returns:	TRUE if this ImagemapClickableRectangle overrides picaOther
				FALSE otherwise

	Purpose:	If this ImagemapClickableRectangle encloses (or is equal to) 
				the bounding box of the other ImagemapClickableArea, and
				both have the same URL and frame information, then we override
				the other.
	
********************************************************************************************/

BOOL ImagemapClickableRectangle::OverrideFromBelow(OverrideListItem* poliOther)
{
	ERROR2IF(poliOther==NULL, FALSE, "ImagemapClickableRectangle::OverrideFromAbove passed NULL parameter");

	if (!poliOther->IsAClickableArea())
		return FALSE;

	ImagemapClickableArea* picaOther=(ImagemapClickableArea*) poliOther;

	//Get the bounding box of the other ImagemapClickableArea
	DocRect rectOther=picaOther->GetBoundingRect();

	//We override the other ImagemapClickableArea if we contain
	//its bounding box and if URL and frame strings are equal
	return (m_rect.ContainsRect(rectOther)
		&& WebAddressAttribute::AreStringsEqual(m_pcURL, picaOther->m_pcURL)
		&& WebAddressAttribute::AreStringsEqual(m_pcFrame, picaOther->m_pcFrame));

}

/*************************************************

  ImagemapClickableCircle

  ***********************************************/

  /********************************************************************************************

  ImagemapClickableCircle::ImagemapClickableCircle():ImagemapClickableArea()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Purpose:	Default constructor
	
********************************************************************************************/

ImagemapClickableCircle::ImagemapClickableCircle() : ImagemapClickableArea()
{
	m_dcCentre=DocCoord(0,0);
	m_lRadius=0;
}

  
  /********************************************************************************************

	ImagemapClickableCircle::ImagemapClickableCircle(Path* ppthPath, TCHAR* pcURL, TCHAR* pcFrame) 
: ImagemapClickableArea(pcURL, pcFrame)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Purpose:	Specific constructor
	
********************************************************************************************/

ImagemapClickableCircle::ImagemapClickableCircle(Path* ppthPath, TCHAR* pcURL, TCHAR* pcFrame) 
: ImagemapClickableArea(pcURL, pcFrame)
{
	//If we have been passed a path
	if (ppthPath)
	{
		//Get its bounding box
		DocRect rectBounds=ppthPath->GetBoundingRect();
		
		//Then the centre of the circle is the centre of the bounding box
		m_dcCentre=DocCoord(rectBounds.lo.x/2 + rectBounds.hi.x/2, rectBounds.lo.y/2 + rectBounds.hi.y/2);

		//And the radius of the circle is half the width of the rectangle
		m_lRadius=rectBounds.Width()/2;
	}
	else
	{
		m_dcCentre=DocCoord(0,0);
		m_lRadius=0;
	}

}
  
  /********************************************************************************************

	INT32 Write(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		pfileToWrite	The file to write to
				pcBuffer		The text buffer to write to

				Either of the above may be NULL.
				
	Returns:	The number of TCHARs written.

	Purpose:	Writes this clickable area, in client-side imagemap form,
				into the buffer or the file or both.

				This function may be called with all parameters NULL, to find out how
				many TCHARs will be written into the imagemap, and hence to find out
				how long the text buffer must be
	
********************************************************************************************/

INT32 ImagemapClickableCircle::Write(CCLexFile* pfileToWrite,  TCHAR* pcBuffer)
{
	PORTNOTETRACE("other","ImagemapClickableArea::Write - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	//Keep a count of the characters we write out
	INT32 lCharsWritten=0;

	ImagemapFilter * pFilter = GetImagemapFilter();
	ERROR2IF(pFilter == NULL,lCharsWritten,"ImagemapClickableCircle::Write no imagemap filter");

	//First indent the text
	pFilter->Indent();
	lCharsWritten += pFilter->WriteIndent(pfileToWrite, pcBuffer);

	//And write out <AREA
	lCharsWritten += pFilter->WriteStartOfTag(_R(IDS_HTMLEXPORT_AREA), pfileToWrite, pcBuffer);

	//Now write out SHAPE=CIRCLE
	lCharsWritten += pFilter->WriteParameter(_R(IDS_HTMLEXPORT_SHAPE), _R(IDS_HTMLEXPORT_CIRCLE), pfileToWrite, pcBuffer);

	//Write out the URL and the FRAME
	lCharsWritten += WriteURLAndFrame(pfileToWrite, pcBuffer);

	//And write out our coords
	lCharsWritten += pFilter->WriteCircleCoords(m_dcCentre, m_lRadius, m_pcFrame, pfileToWrite, pcBuffer);

	//And the end of the tag
	lCharsWritten += pFilter->WriteEndOfTag(pfileToWrite, pcBuffer);

	//Then reset the indent
	pFilter->Unindent();

	//And write out a new line
	lCharsWritten += pFilter->WriteEOL(pfileToWrite, pcBuffer);
	
	//And return the number of characters we've written
	return lCharsWritten;
#else
	return 0;
#endif
}

/********************************************************************************************

	DocRect ImagemapClickableCircle::GetBoundingRect()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/97	- VOTE LABOUR!
	Inputs:		-
				
	Returns:	The bounding rectangle of the circle

	Purpose:	Gets the bounding rectangle
	
********************************************************************************************/

DocRect ImagemapClickableCircle::GetBoundingRect()
{
	return DocRect(m_dcCentre.x-m_lRadius, m_dcCentre.y-m_lRadius, 
		m_dcCentre.x+m_lRadius, m_dcCentre.y+m_lRadius);
}


/*************************************************

  ImagemapClickablePolygon

  ***********************************************/

/********************************************************************************************

	ImagemapClickablePolygon::ImagemapClickablePolygon() : ImagemapClickableArea()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		-
	Outputs:	-
	Purpose:	Default contructor
	
********************************************************************************************/

ImagemapClickablePolygon::ImagemapClickablePolygon() : ImagemapClickableArea()
{
	m_ppth=NULL;
}

/********************************************************************************************

	ImagemapClickablePolygon::ImagemapClickablePolygon(Path* ppthPath)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		ppthPath - pointer to the path to copy
	Outputs:	-
	Purpose:	Specific contructor
	
********************************************************************************************/

ImagemapClickablePolygon::ImagemapClickablePolygon(Path* ppthPath, TCHAR* pcURL, TCHAR* pcFrame) 
	: ImagemapClickableArea(pcURL, pcFrame)
{
	//Create a new path member variable
	m_ppth=new Path();

	//Initialise it
	m_ppth->Initialise(ppthPath->GetNumCoords());

	//And copy the path we have been given
	m_ppth->CopyPathDataFrom(ppthPath);
}

/********************************************************************************************

	ImagemapClickablePolygon::~ImagemapClickablePolygon()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		-
	Outputs:	-
	Purpose:	Destructor
	
********************************************************************************************/

ImagemapClickablePolygon::~ImagemapClickablePolygon()
{
	//If we have a path member variable, delete it
	if (m_ppth)
		delete m_ppth;
}

/********************************************************************************************

  ImagemapClickablePolygon::ImagemapClickablePolygon(const ImagemapClickablePolygon& waaCopy);

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		-
	Outputs:	-
	Purpose:	Copy constructor
	
********************************************************************************************/

ImagemapClickablePolygon::ImagemapClickablePolygon(const ImagemapClickablePolygon& icpCopy)
:ImagemapClickableArea(icpCopy)
{
	//Simply use the assignment operator
	*this=icpCopy;

}

/********************************************************************************************

  ImagemapClickablePolygon& ImagemapClickablePolygon::operator=(const ImagemapClickablePolygon& icpCopy)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		icpCopy
	Outputs:	-
	Purpose:	Assignment operator
	
********************************************************************************************/

ImagemapClickablePolygon& ImagemapClickablePolygon::operator=(const ImagemapClickablePolygon& icpCopy)
{
	//Delete our member variable path
	if (m_ppth)
		delete m_ppth;
		
	//Now, does the other ICP have a member path variable?
	if (icpCopy.m_ppth!=NULL)
	{
		//Yes. So create a new path member variable
		m_ppth=new Path();
		
		//Initialise it
		m_ppth->Initialise(icpCopy.m_ppth->GetNumCoords());

		//And copy the path we have been given
		m_ppth->CopyPathDataFrom(icpCopy.m_ppth);
	}
	else
	{
		//No. So set our member path pointer to NULL
		m_ppth=NULL;
	}

	return *this;

}


/********************************************************************************************

	INT32 ImagemapClickablePolygon::Write(CCLexFile* pfileToWrite, TCHAR* pcBuffer)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		pfileToWrite	The file to write to
				pcBuffer		The text buffer to write to

				Either of the above may be NULL.
				
	Returns:	The number of TCHARs written.

	Purpose:	Writes this clickable area, in client-side imagemap form,
				into the buffer or the file or both.

				This function may be called with all parameters NULL, to find out how
				many TCHARs will be written into the imagemap, and hence to find out
				how long the text buffer must be
	
********************************************************************************************/

INT32 ImagemapClickablePolygon::Write(CCLexFile* pfileToWrite, TCHAR* pcBuffer)
{
	PORTNOTETRACE("other","ImagemapClickableArea::Write - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	//Keep a count of the characters we write out
	INT32 lCharsWritten=0;

	ImagemapFilter * pFilter = GetImagemapFilter();
	ERROR2IF(pFilter == NULL,lCharsWritten,"ImagemapClickablePolygon::Write no imagemap filter");

	//First indent the text
	pFilter->Indent();
	lCharsWritten += pFilter->WriteIndent(pfileToWrite, pcBuffer);

	//And write out <AREA
	lCharsWritten += pFilter->WriteStartOfTag(_R(IDS_HTMLEXPORT_AREA), pfileToWrite, pcBuffer);

	//Now write out SHAPE=PATH
	lCharsWritten += pFilter->WriteParameter(_R(IDS_HTMLEXPORT_SHAPE), _R(IDS_HTMLEXPORT_POLYGON), pfileToWrite, pcBuffer);

	//Write out the URL and the FRAME
	lCharsWritten += WriteURLAndFrame(pfileToWrite, pcBuffer);

	//And write out our coords
	lCharsWritten += pFilter->WriteCoords(m_ppth, m_pcFrame, pfileToWrite, pcBuffer);

	//And the end of the tag
	lCharsWritten += pFilter->WriteEndOfTag(pfileToWrite, pcBuffer);

	//Then reset the indent
	pFilter->Unindent();

	//And write out a new line
	lCharsWritten += pFilter->WriteEOL(pfileToWrite, pcBuffer);

	//And return the number of characters we've written
	return lCharsWritten; */
#else
	return 0;
#endif
}
