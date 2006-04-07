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
#include "filtimop.h"

//#include "resimmap.h"		//For _R(IDS_IMAGEMAP_DEFAULTMAPNAME)
#include "app.h"			//For GetApplication()
#include "bitfilt.h"		//For GetSizeOfDrawing

CC_IMPLEMENT_DYNAMIC(ImagemapFilterOptions, CCObject)

#define new CAM_DEBUG_NEW


/******************************************************

  Constructors

  *****************************************************/

/********************************************************************************************

>	ImagemapFilterOptions::ImagemapFilterOptions()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Purpose:	Default constructor

				Note that this default constructor is used to set up the default
				values on start up.

********************************************************************************************/
ImagemapFilterOptions::ImagemapFilterOptions()
{
	//Initialise our variables
	m_ffApprox=FF_APPROXIMATELY;
	m_fAllRectangles=FALSE;
	m_stExportArea=DRAWING;
	m_dDPI=96.0;

	m_fClipboard=FALSE;
	m_fFile=FALSE;
	m_fInsert=TRUE;
	m_pfileFile=NULL;
	m_fReportErrors=FALSE;
}

/********************************************************************************************

  ImagemapFilterOptions(String_256 strName,
							FlatteningFactor ffApprox,
							BOOL fAllRectangles,
							SelectionType stExportArea,
							double dDPI,
							BOOL fClipboard,
							BOOL fFile=FALSE,
							PathName pthFile,
							BOOL fInsert=FALSE,
							CCLexFile* pfileFile,
							BOOL fReportErrors
							);


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Purpose:	Specific constructor

********************************************************************************************/
ImagemapFilterOptions::ImagemapFilterOptions(String_256 strName,
							FlatteningFactor ffApprox,
							BOOL fAllRectangles,
							SelectionType stExportArea,
							double dDPI,
							BOOL fClipboard,
							BOOL fFile,
							PathName pthFile,
							BOOL fInsert,
							CCLexFile* pfileFile,
							BOOL fReportErrors
							)
{
	//Initialise our variables
	m_strName=strName;
	m_ffApprox=ffApprox;
	m_fAllRectangles=fAllRectangles;
	m_stExportArea=stExportArea;
	m_dDPI=dDPI;

	m_fClipboard=fClipboard;
	m_fFile=fFile;
	m_pthFile=pthFile;
	m_fInsert=fInsert;
	m_pfileFile=pfileFile;
	m_fReportErrors=0;
	
	// init where the graphic is exported to
	// this makes a reasonable guess but should be set later
	m_GraphicPath = pthFile;
	m_GraphicPath.SetType("gif");
}

/******************************************************

  Copy constructor, destructor and assignment operator

  *****************************************************/
/********************************************************************************************

>	ImagemapFilterOptions::ImagemapFilterOptions(const ImagemapFilterOptions& waaCopy)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Inputs:		ImagemapFilterOptions object to copy
	Outputs:	-
	Returns:	*this
	Purpose:	Copy constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ImagemapFilterOptions::ImagemapFilterOptions(const ImagemapFilterOptions& ifoCopy)
{
	//Simply use the assigment operator
	*this=ifoCopy;
					   
}

/********************************************************************************************

>	ImagemapFilterOptions::~ImagemapFilterOptions()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor
	Errors:		-						   
	SeeAlso:	-

********************************************************************************************/

ImagemapFilterOptions::~ImagemapFilterOptions()
{
	//Does nothing
}

/********************************************************************************************

>	ImagemapFilterOptions& operator= (const ImagemapFilterOptions& ifoOther)					

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Inputs:		ifoOther	Object to copy
	Outputs:	-
	Returns:	-
	Purpose:	Assignment operator
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ImagemapFilterOptions& ImagemapFilterOptions::operator= (const ImagemapFilterOptions& ifoOther)					
{
	
	//Simply copy the member variables across
	m_strName=ifoOther.m_strName;
	m_ffApprox=ifoOther.m_ffApprox;
	m_fAllRectangles=ifoOther.m_fAllRectangles;
	m_fClipboard=ifoOther.m_fClipboard;
	m_fFile=ifoOther.m_fFile;
	m_stExportArea=ifoOther.m_stExportArea;
	m_dDPI=ifoOther.m_dDPI;
	m_pthFile=ifoOther.m_pthFile;
	m_fInsert=ifoOther.m_fInsert;
	m_pfileFile=ifoOther.m_pfileFile;
	m_fReportErrors=ifoOther.m_fReportErrors;
	m_GraphicPath = ifoOther.m_GraphicPath;

	return *this;
}

/******************************************************

  Toolkit functions

  *****************************************************/
/********************************************************************************************

	static DocRect ImagemapFilterOptions::GetSizeOfExportArea(SelectionType stExportArea)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		-
	Outputs:	-
	Returns:	The bounding rectangle of the export area
	
	Purpose:	Returns the bounding rectangle of the export area, based
				on the m_stExportArea member variable
	
				Note that this function assumes there are only two types
				of export area: the selection and the drawing.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DocRect ImagemapFilterOptions::GetSizeOfExportArea(SelectionType stExportArea)
{	
	//This is the value we will return
	DocRect rectBounds;

	//If the area to export is the selection
	if (stExportArea==SELECTION)
	{
		//Then get the bounding rectangle of the selection
		rectBounds=GetApplication()->FindSelection()->GetBoundingRect();
	}
	else
	{
		//We assume the selection area is the drawing
		//Get the bounds of the drawing
		rectBounds=BaseBitmapFilter::GetSizeOfDrawing();
	}

	return rectBounds;
 
}

/********************************************************************************************

	DocCoord ImagemapFilterOptions::GetOriginOfExportArea(SelectionType stExportArea)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		-
	Outputs:	-
	Returns:	The top left hand corner coordinate of the export area.
	
	Purpose:	Returns the top left hand corner of the export area - 
				which is the origin from which all bitmap coordinates are
				taken.
	
				Note that this function assumes there are only two types
				of export area: the selection and the drawing.

	Errors:		-
	SeeAlso:	ImagemapFilterOptions::GetSizeOfExportArea

********************************************************************************************/

DocCoord ImagemapFilterOptions::GetOriginOfExportArea(SelectionType stExportArea)
{	
	//Get the export area rectangle
	DocRect rectExport=ImagemapFilterOptions::GetSizeOfExportArea(stExportArea);

	//This is its top left hand corner
	DocCoord coordToReturn(rectExport.lo.x, rectExport.hi.y);
	
	return coordToReturn; 
}

