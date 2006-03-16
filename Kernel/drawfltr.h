// $Id: drawfltr.h 662 2006-03-14 21:31:49Z alex $
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

// An input filter for importing RISC OS Draw format files.

#ifndef INC_DRAWFLTR
#define INC_DRAWFLTR

#include "ccfile.h"
#include "filters.h"

class Node;

/********************************************************************************************

>	class DrawFileHeader

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Purpose:	Holds the information contained in the header of an Acorn Draw file.
	SeeAlso:	DrawObjectHeader; AcornDrawFilter

********************************************************************************************/

class DrawFileHeader
{
public:
	// Should be the string "Draw"
	char 		Ident[4];

	// version number and name of program that created the Draw file
	UINT32		MajorVersion;
	UINT32		MinorVersion;
	char		Creator[12];

	// Bounding box of the whole draw file
	DocCoord	BBoxLo;
	DocCoord	BBoxHi;
};

/********************************************************************************************

	typedef DrawObjectType

	This is a simple type to decode the object types in an Acorn Draw file.

********************************************************************************************/

typedef INT32 DrawObjectType;


/********************************************************************************************

>	class DrawObjectHeader

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Purpose:	Contains the information contained in an object header of an Acorn Draw file.
	SeeAlso:	DrawFileHeader; AcornDrawFilter

********************************************************************************************/

class DrawObjectHeader
{
public:
	// Type and size of object
	DrawObjectType 	Type;
	UINT32			Size;

	// Bounding box of this object
	DocCoord		BBoxLo;
	DocCoord		BBoxHi;
};



// Various forward declarations for the AcornDrawFilter class
class PathName;
class Document;
class NodePath;
class Layer;
class AttrLineWidth;
class AttrStrokeColour;
class AttrFillColour;
class OpImportAcornDraw;
class Page;
class DrawPathHeader;

/********************************************************************************************

>	class AcornDrawFilter : public VectorFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Purpose:	Encapsulates an RISC OS Draw input filter system.

********************************************************************************************/

class AcornDrawFilter : public VectorFilter
{
	CC_DECLARE_DYNAMIC(AcornDrawFilter);
	
public:
	AcornDrawFilter();
	~AcornDrawFilter();
	BOOL Init();	

	// Works out if opening a file of this type requires a default document to be loaded
	// or does the file format supply the document.
	virtual BOOL IsDefaultDocRequired(const TCHAR* pcszPathName);

	INT32 HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize);

	BOOL DoImport(SelOperation *Op, CCLexFile*, Document *DestDoc, 
				  BOOL AutoChosen, ImportPosition *Pos = NULL,
				  KernelBitmap** ppImportedBitmap = NULL,
				  DocCoord* pPosTranslate = NULL, String_256* = NULL);

	BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);

private:
	BOOL PrepareToImport();
	void CleanUpAfterImport();
		
	// The file we are reading
	CCLexFile *DrawFile;
	
	// TRUE if the EOF has been reached
	BOOL eof;
	
	// The layer that objects are placed on
	Layer *pLayer;
	
	// The page that objects appear on
	Page *pPage;
	
	// The Node to which new objects are attached (may be a Layer or a Group node)
	Node *pNode;
	
	// TRUE if a complex path is being constructed
	BOOL ComplexPath;
	
	// TRUE if the path being constructed has not had any attributes added yet
	BOOL NoAttributes;

	// Co-ordinate transformations
	DocCoord Origin;

	// TransformCoord() - converts a DocCoord from Draw units (1/640 of a point) to
	// Camelot units (millipoints). (the factor 1000/640 is reduced to 25/16)
	void TransformCoord(DocCoord& C) 
		{	C.x *= 25; C.x /= 16; C.x += Origin.x;
			C.y *= 25; C.y /= 16; C.y += Origin.y; };

	// Buffers to hold data read from the Draw file.
	DrawFileHeader	 FileHeader;
	DrawObjectHeader ObjectHeader;
	ADDR DataBuf; // Dynamically allocated when the file is read.
	UINT32 DataBufSize;
	UINT32 HeaderSize; // Not necessarily sizeof(DrawObjectHeader)! (Thanks, Acorn...)
	UINT32 DataSize;

	// Functions to read in and process the Draw objects
	BOOL ReadFileHeader();
	BOOL ReadObjectHeader();
	BOOL ReadObjectData();
	BOOL ProcessObject();
	BOOL ProcessPath();
	BOOL ProcessGroup();
	BOOL ProcessTaggedObject();
	BOOL SkipObject();
	BOOL AddAttributes(NodePath *pPath, DrawPathHeader *pHeader);

	// Used to update progress bar etc
	INT32 BytesRead;
	INT32 LastProgressUpdate;
};


#endif
