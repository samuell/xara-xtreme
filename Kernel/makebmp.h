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

#ifndef	INC_MAKEBMP
#define	INC_MAKEBMP

//#include "selop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "giffiltr.h"

#define OPTOKEN_CONVERTTOBITMAP _T("ConvertToBitmap")	

/********************************************************************************************

>	class OpConvertToBitmap: public SelOperation

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95
	Purpose:	Converts objects into a bitmap
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpConvertToBitmap: public SelOperation
{         
	CC_DECLARE_DYNCREATE( OpConvertToBitmap );

public:
	OpConvertToBitmap();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }
};  



/********************************************************************************************

>	class MakeBitmapExportOptions : public MaskedFilterExportOptions

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/96
	Purpose:   	Allows "Create Bitmap Copy" to provide own defaults

********************************************************************************************/
class MakeBitmapExportOptions : public MaskedFilterExportOptions
{
CC_DECLARE_DYNCREATE(MakeBitmapExportOptions)

public:
	static BOOL Declare();

	// default constructor, fairly useful at the best of times.
	MakeBitmapExportOptions() {} // Do nothing for now

    MakeBitmapExportOptions(const FilterType FilterID, const StringBase* pFilterName);

	virtual BOOL	RetrieveDefaults();
	virtual BOOL	SetAsDefaults() const;

	// for object copying and comparing
	BitmapExportOptions *MakeCopy();
	virtual BOOL CopyFrom(BitmapExportOptions *pOther);
//	virtual BOOL Equal(BitmapExportOptions *pOther);

protected:
	// Persistant buffer
	static DPI			g_Dpi;
	static BMP_DEPTH	g_Depth;
	static DITHER		g_Dither;
	static PALETTE		g_Palette;
	static BOOL			g_TranspBkGnd;
};


/********************************************************************************************

>	class MakeBitmapFilter : public TI_GIFFilter

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Purpose:	Special filter used to convert the selection into a bitmap

********************************************************************************************/

class MakeBitmapFilter : public TI_GIFFilter
{
	CC_DECLARE_DYNAMIC(MakeBitmapFilter);
	
public:
	MakeBitmapFilter();

	// Check if this Bitmap filter can cope with saving at this Bpp/Colour depth
	virtual BOOL IsThisBppOk(UINT32 Bpp);
	virtual UINT32 GetExportMsgID();

	BOOL DoCreateBitmap(Operation *pOp, Document *pDoc, KernelBitmap** ppBitmap);
	BOOL InsertBitmapIntoDocument(UndoableOperation *pOp, KernelBitmap* KernelBmp, Document* DestDoc);

	static BOOL FindCentreInsertionPosition(Spread** Spread, DocCoord* Position);
	static BOOL Initialise();

protected:
	//
	// Export related functions
	//
	virtual BitmapExportOptions*	CreateExportOptions() const;
	virtual BOOL GetExportOptions(BitmapExportOptions* pOptions);
	virtual void CleanUpAfterExport();
	virtual BOOL WriteToFile( BOOL End );
	virtual BOOL EndWriteToFile( );

	virtual KernelBitmap* GetTheBitmap(BitmapExportOptions *pExportOptions);

	KernelBitmap* pTheBitmap;
	static FilterType s_FilterType;
};

#endif
