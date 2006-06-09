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
//

#ifndef INC_IMAGEMAPFILTEROPTIONS
#define INC_IMAGEMAPFILTEROPTIONS

//#include "ccobject.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "pathname.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bmpprefs.h"
#include "flatfact.h"
				 
/**************************************************************************************

>	class ImagemapFilterOptions : public CCObject

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Purpose:	An ImagemapFilterOptions object contains all the information
				the ImagemapFilter needs to set itself up ready for export.

**************************************************************************************/
class ImagemapFilterOptions : public CCObject
{
	CC_DECLARE_DYNAMIC(ImagemapFilterOptions);

	//Constructors
public:
	ImagemapFilterOptions();
	ImagemapFilterOptions(String_256 strName,
							FlatteningFactor ffApprox=FF_CLOSELY,
							BOOL fAllRectangles=FALSE,
							SelectionType stExportArea=DRAWING,
							double dDPI=96,
							BOOL fClipboard=FALSE,
							BOOL fFile=FALSE,
							PathName pthFile=PathName(),
							BOOL fInsert=FALSE,
							CCLexFile* pfileFile=NULL,
							BOOL fReportErrors=TRUE
							);


	//Destructor, copy constructor and assignment operator
public:
	~ImagemapFilterOptions();
	ImagemapFilterOptions(const ImagemapFilterOptions& ifoCopy);
	ImagemapFilterOptions& operator=(const ImagemapFilterOptions& ifoCopy);
	

	//Access functions
public:
	String_256 GetImagemapName()
	{
		return m_strName;
	}

	FlatteningFactor GetFlatteningFactor()
	{
		return m_ffApprox;	
	}

	BOOL ExportAllRectangles()
	{
		return m_fAllRectangles;
	}

	BOOL ExportToClipboard()
	{
		return m_fClipboard;
	}

	BOOL ExportToFile()
	{
		return m_fFile;
	}


	SelectionType GetExportArea()
	{
		return m_stExportArea;
	}
	
	double GetDPI()
	{
		return m_dDPI;
	}

	PathName GetImagemapPath()
	{
		return m_pthFile;
	}

	CCLexFile* GetImagemapFile()
	{
		return m_pfileFile;
	}


	BOOL Insert()
	{
		return m_fInsert;
	}



	//Toolkit functions
public:
	static DocRect GetSizeOfExportArea(SelectionType stExportArea);
	static DocCoord GetOriginOfExportArea(SelectionType stExportArea);

	//Member variables
public:
	String_256			m_strName;			//The name of the imagemap
	FlatteningFactor	m_ffApprox;		//How much to flatten the paths
	BOOL				m_fAllRectangles;	//Whether all the clickable areas should be rectangles
	SelectionType		m_stExportArea;		//Area to export.
	double				m_dDPI;				//The DPI by which to scale the imagemap

	BOOL				m_fClipboard;		//Whether to put the imagemap on the clipboard
	BOOL				m_fFile;			//Whether to put the imagemap into a file
											//(Ignored by the ImagemapFilter, which always puts
											//imagemaps into a file)

	PathName			m_pthFile;			//The path of the file to which the imagemap
											//should be exported

	CCLexFile*			m_pfileFile;		//Pointer to the file to export into

	BOOL				m_fInsert;			//If the file specified in m_pthFile exists, 
											//whether to insert the imagemap into this file
											//or replace the entire file.
	BOOL				m_fReportErrors;	//Whether to report errors during the imagemap
											//export
											
	PathName			m_GraphicPath;		//The location of the graphic that the image map refers to
						
};



#endif	// INC_IMAGEMAPFILTER
