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
// Imagemap class

#include "camtypes.h"
#include "imagemap.h"

#include "clicarea.h"	//For ImagemapClickableArea and derived classes
#include "filtimop.h"	//For ImagemapFilterOptions::GetOriginOfExportArea
#include "paths.h"		//For Path::Scale
#include "doccoord.h"	//For DocCoord::Scale

CC_IMPLEMENT_DYNAMIC(Imagemap, List)

/*************************************************

  Constructors

  ***********************************************/

/********************************************************************************************

>	Imagemap::Imagemap() : List()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Purpose:	Default constructor
	
********************************************************************************************/

Imagemap::Imagemap() : OverrideList()
{
	//Does nothing
}

/*************************************************

  Destructor

  ***********************************************/

  /********************************************************************************************

>	Imagemap::~Imagemap()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Purpose:	Destructor
	
********************************************************************************************/

Imagemap::~Imagemap()
{
	//Delete all our list items
	DeleteAll();
}

/*************************************************

  Adding to the imagemap

  ***********************************************/

  /********************************************************************************************

  >	Imagemap::AddRectangle(DocRect rectToAdd, TCHAR* pcURL=NULL, TCHAR* pcFrame=NULL);


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		rectToAdd	The rectangle to add to the imagemap
	Purpose:	Adds a rectangular clickable region to the imagemap
	
********************************************************************************************/

void Imagemap::AddRectangle(DocRect rectToAdd, TCHAR* pcURL, TCHAR* pcFrame)
{
	//Create a new ImagemapClickableRectangle with the rectangle we've been given
	ImagemapClickableRectangle* pRectangle=new ImagemapClickableRectangle(rectToAdd, pcURL, pcFrame);

	//And add it to the imagemap
	//Note that we add it to the head of the list. That's because, in imagemaps,
	//the first clickable area found in the file is the one nearest the front.
	AddHead(pRectangle);
}

/********************************************************************************************

  >	Imagemap::AddCircle(Path* pthToAdd, TCHAR* pcURL=NULL, TCHAR* pcFrame=NULL);


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		pthToAdd	The circular path to add to the imagemap
	Purpose:	Adds a circular clickable region to the imagemap
	
********************************************************************************************/

void Imagemap::AddCircle(Path* pthToAdd, TCHAR* pcURL, TCHAR* pcFrame)
{
	//Create a new ImagemapClickableCircle with our scaled path
	ImagemapClickableCircle* pCircle=new ImagemapClickableCircle(pthToAdd, pcURL, pcFrame);

	//And add it to the imagemap
	//Note that we add it to the head of the list. That's because, in imagemaps,
	//the first clickable area found in the file is the one nearest the front.
	AddHead(pCircle);
}

/********************************************************************************************

  >	Imagemap::AddPolygon(Path* pthToAdd, TCHAR* pcURL=NULL, TCHAR* pcFrame=NULL);


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		pthToAdd	The path to add to the imagemap
				
	Purpose:	Adds a straight-sided polygon to the imagemap
	
********************************************************************************************/

void Imagemap::AddPolygon(Path* pthToAdd, TCHAR* pcURL, TCHAR* pcFrame)
{
	//Create a new ImagemapClickableCircle with the path we've been given
	ImagemapClickablePolygon* pPolygon=new ImagemapClickablePolygon(pthToAdd, pcURL, pcFrame);
		
	//And add it to the imagemap
	//Note that we add it to the head of the list. That's because, in imagemaps,
	//the first clickable area found in the file is the one nearest the front.
	AddHead(pPolygon);
}

/*************************************************

  Writing out the imagemap

  ***********************************************/


/********************************************************************************************

 >	INT32 Imagemap::WriteHelper(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL)
										   
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		pfileToWrite - The file to write to
				pcBuffer	 - Pointer to a text buffer to write to

				Either of the above may be NULL.

  Returns:		The number of TCHARs written.

  Purpose:		Writes out the imagemap.

				This function simply iterates through the list and calls on
				the members to write themselves out.
	
SeeAlso:		ImagemapRenderRegion::Write()
	
********************************************************************************************/

INT32 Imagemap::Write(CCLexFile* pfileToWrite, TCHAR* pcBuffer)
{
	//Set up a variable to remember how many bytes we have written
	INT32 lBytesWritten=0;
	
	//Now, get the first clickable area in the imagemap
	ImagemapClickableArea* pAreaToWrite=(ImagemapClickableArea*) GetHead();

	//And while we're still looking at a valid clickable area
	while (pAreaToWrite!=NULL)
	{
		//Tell that area to write itself out to the imagemap.
		//This function will return the number of TCHARs it writes
		lBytesWritten+=pAreaToWrite->Write(pfileToWrite, pcBuffer);

		//And move on to the next clickable area
		pAreaToWrite=(ImagemapClickableArea*) GetNext(pAreaToWrite);
	}

	//And return the number of TCHARs we have written
	return lBytesWritten;
	
}



