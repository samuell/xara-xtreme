// $Id: clicarea.h 662 2006-03-14 21:31:49Z alex $
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
// Imagemap

#ifndef IMAGEMAPCLICKABLEAREA
#define IMAGEMAPCLICKABLEAREA

#include "overitem.h"
#include "docrect.h"
#include "paths.h"
#include "flatfact.h"

class CCLexFile;
class ImagemapFilter;

/********************************************************************************************

>	class ImagemapClickableArea : public OverrideListItem

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Purpose:	This class represents a clickable area.

				It's a pure virtual class, so use one of the three
				derived classes (below) instead.

********************************************************************************************/

class ImagemapClickableArea : public OverrideListItem
{
	CC_DECLARE_DYNAMIC(ImagemapClickableArea)

	//Constructors
	ImagemapClickableArea();
	ImagemapClickableArea(TCHAR* pcURL, TCHAR* pcFrame);

	//Destructor, copy constructor and assignment operator
	~ImagemapClickableArea();
	ImagemapClickableArea(const ImagemapClickableArea& icaCopy);
	ImagemapClickableArea& operator=(const ImagemapClickableArea& icpCopy);

	//Writing out the clickable area
public:
	virtual INT32 Write(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL)=0;
	
	INT32 WriteURLAndFrame(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	

	//Information functions
public:
	virtual DocRect GetBoundingRect()=0;

	BOOL IsAClickableArea()
	{
		return TRUE;
	}

	//Toolkit functions
public:
	ImagemapFilter* GetImagemapFilter();
	
	//Member variables
	TCHAR* m_pcURL;
	TCHAR* m_pcFrame;
};

/********************************************************************************************

>	class ImagemapClickableRectangle : public ImagemapClickableArea

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Purpose:	This class represents a clickable rectangle.

********************************************************************************************/

class ImagemapClickableRectangle : public ImagemapClickableArea
{
	CC_DECLARE_DYNAMIC(ImagemapClickableRectangle)

	//Constructors
public:
	ImagemapClickableRectangle();

	ImagemapClickableRectangle(DocRect rectToStore, TCHAR* pcURL=NULL, TCHAR* pcFrame=NULL);
	
	//Writing out the clickable area
public:
	INT32 Write(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	

	//Override functions
public:
	BOOL OverrideFromBelow(OverrideListItem* picaOther);

	BOOL OverrideFromAbove(OverrideListItem* picaOther)
	{
		return OverrideFromBelow(picaOther);
	}

	//Information function
public:
	DocRect GetBoundingRect()
	{
		return m_rect;
	}
	 
	//Member variable:
public:
	DocRect m_rect;

};

/********************************************************************************************

>	class ImagemapClickableCircle : public ImagemapClickableArea

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Purpose:	This class represents a clickable circle.

********************************************************************************************/

class ImagemapClickableCircle : public ImagemapClickableArea
{
	CC_DECLARE_DYNAMIC(ImagemapClickableCircle)

	//Constructors
public:
	ImagemapClickableCircle();

	ImagemapClickableCircle(Path* pthToCopy, TCHAR* pcURL=NULL, TCHAR* pcFrame=NULL);
	
	//Writing out the clickable area
public:
	INT32 Write(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	

	//Information function
public:
	DocRect GetBoundingRect();
	
	//Member variable:
public:
	DocCoord m_dcCentre;
	INT32 m_lRadius;
	
};

/********************************************************************************************

>	class ImagemapClickablePolygon : public ImagemapClickableArea

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Purpose:	This class represents a clickable n-sided polygon.

********************************************************************************************/

class ImagemapClickablePolygon : public ImagemapClickableArea
{
	CC_DECLARE_DYNAMIC(ImagemapClickablePolygon)

	//Constructors
public:
	ImagemapClickablePolygon();
	
	ImagemapClickablePolygon(Path* pthToCopy, TCHAR* pcURL=NULL, TCHAR* pcFrame=NULL);

	//Destructor, copy constructor and assignment operator
public:
	~ImagemapClickablePolygon();
	ImagemapClickablePolygon(const ImagemapClickablePolygon& icpCopy);
	ImagemapClickablePolygon& operator=(const ImagemapClickablePolygon& icpCopy);
	
	//Writing out the clickable area
public:
	INT32 Write(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	

	//Information function
public:
	DocRect GetBoundingRect()
	{
		if (m_ppth)
			return m_ppth->GetBoundingRect();
		else
			return DocRect(0,0,0,0);
	}

	//Member variable
public:
	Path* m_ppth;

};



#endif // IMAGEMAPCLICKABLEAERA

