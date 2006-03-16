// $Id: bitfopts.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_BITFOPTS
#define INC_BITFOPTS

/********************************************************************************************

  >	class BitmapImportOptions : public CC_CLASS_MEMDUMP

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Purpose:   	Base class for storing import options
	Scope:		public

********************************************************************************************/
class BitmapImportOptions : public CC_CLASS_MEMDUMP		// public ImportOptions
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(BitmapImportOptions);
public:
	BitmapImportOptions();

	SelOperation*	m_pSelectionOp;		// Operation used to do import
	BOOL			m_bAutoChosen;		// er?...
	ImportPosition*	m_pImportPosition;	// Where to stick it

	CCLexFile*		m_pFile;			// File to be used for import
	Document*		m_pDestinationDoc;	// Docuement in which to place bitmap

	UINT32			m_uBitmapToImport;	// The number of the bitmap to import

	BOOL			m_bCancelled;		// user cancelled

	BOOL	m_bProvideProgress;	// should BaseBitmapFilter class provide the progress bar?
								// Of course it should (or filter should), but provides backwards
								// compatability with derived classes who provide their own.

	BOOL	m_bValid;			// TRUE if constructed OK
};




/********************************************************************************************

  >	class BitmapImportResults : public CC_CLASS_MEMDUMP

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Purpose:   	Provides the results for the import
	Notes:		m_bValid should be checked for TRUE after any construction because the
				constructor can fail
	Scope:		public

********************************************************************************************/
class BitmapImportResults : public CC_CLASS_MEMDUMP		// ...public ImportResults
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(BitmapImportResults);
public:
	BitmapImportResults();		// CAN FAIL!!
	virtual ~BitmapImportResults();

	BitmapSequence*		m_pImportedBitmaps;
	INT32					m_nNextAvailableBitmap;	// the next bitmap that can be read

	BOOL	m_bValid;			// TRUE if constructed OK
};



/********************************************************************************************

  >	class GIFImportOptions : public CC_CLASS_MEMDUMP

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Purpose:   	Import options for the GIF filter. Allows a clean method of passing
				assorted parameters for the GIF filter. Specifically, that the
				BaseBitmapFilter should give us a progress bar.
	Scope:		public
	See Also:	BitmapImportOptions

********************************************************************************************/
class GIFImportOptions : public BitmapImportOptions
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(GIFImportOptions);
public:
	GIFImportOptions() : BitmapImportOptions() {m_bProvideProgress = TRUE;}

	// 
};


/********************************************************************************************

  >	class GIFImportResults : public BitmapImportResults

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Purpose:   	Provides the results for the import of a GIF
	Notes:		m_bValid should be checked for TRUE after any construction because the
				constructor can fail
	Scope:		public

********************************************************************************************/
class GIFImportResults : public BitmapImportResults
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(GIFImportResults);
public:
	GIFImportResults();

	OILBitmap*			m_pBitmap;
	Filter*				m_pFilter;					// used for progress

	INT32					m_nTransparentColourIndex;	// index of the GIF's transparent colour
													// -1 if none
	UINT32				m_uRepeats;					// for those animated doo-dahs

	enum DISPOSAL_METHOD
	{
		NONE,
		DONT,
		TOBACKGROUND,
		TOPREVIOUS,
	};
	DISPOSAL_METHOD		m_DisposalMethod;			// animation idea

};


#endif	// INC_BITFOPTS
