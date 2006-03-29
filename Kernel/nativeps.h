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
// The Camelot Native File format for Studio (version 1)

#ifndef INC_NATIVEEPS
#define INC_NATIVEEPS

#include "cameleps.h"
class KernelBitmapRef;



// All the commands used by Camelot Native EPS files.
enum
{
	// Bitmap objects/bitmap fills
	EPSC_cbmp = EPSC_cameps_last,
};




/********************************************************************************************

>	class CamelotNativeEPSFilter : public CamelotEPSFilter

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Purpose:	This filter will be almost the same as Camelot EPS, only it will hold a
				little more information and store data slightly more efficiently. Mainly,
				it will save out bitmaps as binary. It won't really be valid EPS as it
				will not always be ascii. The Native file format is being done this way as
				it will take little or no effort to do this.

********************************************************************************************/

class CamelotNativeEPSFilter : public CamelotEPSFilter
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(CamelotNativeEPSFilter);

public:
	// Construction and initialisation
	CamelotNativeEPSFilter();
	~CamelotNativeEPSFilter();
	BOOL Init();

	// Exporting
	BOOL PrepareToExport(CCLexFile* pFile, Spread *pSpread);
	virtual EPSExportDC* CreateExportDC();
	virtual BOOL ExportBitmap(KernelBitmap&);

	// Importing
	BOOL PrepareToImport();
	virtual INT32 ImportBinary(ADDR pData, INT32 Length);
	virtual BOOL ProcessFilterComment();
	virtual void CleanUpAfterImport(BOOL Successful);
	BitmapFilterSupport GetBitmapSupportLevel();
	virtual void BitmapPoolAttach(KernelBitmap* pBitmap);

	// Flag to show if the pool is being saved or not	
	BOOL IsSavingBitmapPool;

	double SmartGetBuildNumber();
	double GetBuildNumber() { return BuildVersionNumber; }

protected:
	virtual void LookUpToken();
	virtual BOOL ProcessToken();
	INT32  EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize);

	// Function to get export options, if any
	virtual BOOL GetExportOptions( );

	// Function to load in bitmaps
	virtual BOOL ReadBitmap();

	// Stuff to handle the bitmap pool
	INT32 BitmapCount;
	INT32 PendingBitmapNum;
	KernelBitmapRef* BitmapPoolRefs;

	// The old value of the Import With Layers Flag
	BOOL OldImportWithLayers;
	BOOL OldOpenWithLayers;

	static CommandMap NativeCommands[];

	// Items to handle the file compression
	INT32 CompressionType;	// type of compression in use (0 at present)
	BOOL CompressionOn;		// on/off flag

	virtual BOOL SetFileCompressionState(BOOL NewState); 
	virtual BOOL GetFileCompressionState(); 

	double FileVersionNumber;
	double BuildVersionNumber;

	INT32 OldMinLineWidth;		// because we need somewhere to store it as we set it to zero

public:
	// Control over whether we compress native files or not. 
	static BOOL SetNativeCompression(BOOL NewState);
	static BOOL GetNativeCompression();
	static BOOL CompressNative;	// compression preference flag
};





/********************************************************************************************

>	class NativeRenderRegion : public CamelotEPSRenderRegion

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Purpose:	The render region class for the native file save.

********************************************************************************************/

class NativeRenderRegion : public CamelotEPSRenderRegion
{
	CC_DECLARE_DYNAMIC(NativeRenderRegion)
	
public:
	NativeRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale);

	virtual void ConditionalSuicide ( void );	// Graeme (24-0-00).

	void GetRenderRegionCaps(RRCaps* pCaps);

protected:
	virtual BOOL WriteProlog(KernelDC*);
	virtual BOOL WriteSetup(KernelDC*);

	// File version saving. In anything but Native format does nothing
	virtual BOOL WriteFileVersion(KernelDC*);

	// Compressed saving. In anything but Native format does nothing
	virtual BOOL WriteCompressionState(KernelDC*);
	virtual BOOL WriteEndCompressionState(KernelDC*);

	// Flag to say that we started up the compressor ok
	BOOL CompressionInitedOk;

public:
	void GetValidPathAttributes();
	void GetValidTransparencyAttributes();
	virtual void GetValidTextAttributes();
	virtual BOOL RenderChar(WCHAR ch, Matrix* pMatrix);

// virtual Functions to help with rendering
	virtual BOOL WantsGrids();

	// (ChrisG - 30/01/01) Added so that we don't use the CamelotEPS (Xara X EPS) 
	//	version (which copes with RGB colours, not just CMYK ones).
	virtual void OutputFillColour ();
	virtual void OutputStrokeColour ();
	virtual BOOL OutputGradFillColours(DocColour* StartCol, DocColour* EndCol, ColourContext* pContext);
};




/********************************************************************************************

>	class NativeExportDC : public EPSExportDC

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Purpose:	This class makes use of the base class for almost everything. Its real
				purpose is just to overide the Binary and Bitmap areas of the DC. In
				EPSExportDC these are converted to Hex strings and here they are output
				directly as binary...
	SeeAlso:	EPSExportDC

********************************************************************************************/

class NativeExportDC : public EPSExportDC
{
public:
	NativeExportDC(Filter*);

	// This class want to overide the bitmap functions from the base class
	virtual INT32 OutputRawBinary(BYTE *Data, UINT32 Length, UINT32 Alignment = 1);
};


#endif  // INC_NATIVEEPS


