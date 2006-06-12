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

#ifndef INC_SAVEEPS
#define INC_SAVEEPS

//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "kerneldc.h"
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "vectrndr.h"
//#include "rrcaps.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class EPSExportDC;
class KernelBitmap;
class ColourPlate;
class ColourContext;

/********************************************************************************************

>	class EPSRenderRegion : public VectorFileRenderRegion

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Purpose:	Encapsulates an EPS render region.  This is used to export EPS files
				from Camelot.  This class is specifically for Adobe Illustrator EPS files,
				although other EPS filters may derive their render region from this one.

********************************************************************************************/

class EPSRenderRegion : public VectorFileRenderRegion
{
	CC_DECLARE_DYNAMIC(EPSRenderRegion)

	friend class NodeBitmap;
	
public:

	EPSRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale);
	BOOL Init();

	BOOL AttachDevice(View* ViewToAttach, CNativeDC *DCToAttach, Spread* SpreadToAttach, bool fOwned = false );
	BOOL InitDevice();

	virtual BOOL RenderChar(WCHAR ch, Matrix* pMatrix);

	virtual BOOL SaveAttribute(UINT32 Index, AttributeValue *pAttr, BOOL Temp);

	virtual BOOL CloseDown();

	// Write the path to the output file.
	virtual void DrawPathToOutputDevice ( Path		*PathToRender,
										  PathShape	shapePath );

	void ExportPath ( Path	*DrawPath,
					  BOOL	DataOnly,
					  BOOL	PureDataOnly = FALSE );

	// Used to delay changing attributes until absolutely necessary.
	virtual void GetValidPathAttributes();

	// Used to delay changing attributes until absolutely necessary.
	virtual void GetValidTextAttributes();

	virtual void GetRenderRegionCaps(RRCaps* pCaps);

	// Virtual Functions to help with rendering
	virtual BOOL WantsGrids();
	virtual BOOL WriteNewLine ( void );

	/////////////////////////////////////////////////////////////////////////////////////////
	//	Karim 22/05/2000
	//	Implementation of clipping within an EPSRenderRegion.
	virtual void SetClipRegion		(ClipRegionAttribute * pClipAttr, BOOL Temp);
	virtual void RestoreClipRegion	(ClipRegionAttribute * pClipAttr, BOOL Temp);
	/////////////////////////////////////////////////////////////////////////////////////////

protected:

	virtual void OutputFontName( void );
	INT32 GetFontStyle( void );

	virtual BOOL WriteEPSVersion ( void );
	virtual BOOL WriteEPSBoundingBox ( void );
	virtual BOOL WriteEPSProcessColours ( void );
	virtual BOOL WriteEPSResources ( EPSFilter	*pFilter,
									 Document	*pDocument );
	virtual BOOL WriteEPSProlog ( EPSFilter	*pFilter,
								  Document	*pDocument );
	virtual BOOL WriteEPSTrailerComments ( void );
	virtual BOOL WriteProlog(KernelDC*);
	virtual BOOL WriteSetup(KernelDC*);
	virtual BOOL WriteSepTables(KernelDC*);
	virtual BOOL WriteSepFunctions(KernelDC*);

	BOOL SetLastOutputAttribute(INT32 AttrID);
	void ResetOutputAttributes( void );

	// File version saving. In anything but Native format does nothing
	virtual BOOL WriteFileVersion(KernelDC*);

	// Compressed saving. In anything but Native format does nothing
	virtual BOOL WriteCompressionState(KernelDC*);
	virtual BOOL WriteEndCompressionState(KernelDC*);

	// Extract the gradient fills from the tree, and save them to file.
	virtual BOOL WriteGradientFills ( Document *pDocument );
	
	// Determine whether or not to write the font definitions to file.
	virtual BOOL WriteDocumentSetup ( Document	*pDocument,
									  EPSFilter	*pFilter );

	// Write out the end of layer tags (if necessary).
	virtual BOOL EndLayer ( void );

	ColourPlate* GetOutputColourPlate(ColourModel ColModel);
	void GetOutputColourPlate(ColourModel ColModel, 
							  ColourContext **ppContext, 
							  ColourPlate **ppPlate);

	// ChrisG (19/12/00)
	// This function has been added mainly for AI export, but can be used for any EPS
	//	export.
	virtual void OutputWindingRule ();

	// Graeme (29/3/00) - These functions were originally private, with a comment that
	// they could be made virtual protected in the future. I've taken the hint, and
	// changed their scoping because it suits my needs. :)
	virtual void OutputStrokeColour ();
	virtual void OutputLineWidth ();
	virtual void OutputJoinType ();
	virtual void OutputDashPatternInit ();
	virtual void OutputDashPattern ();
	virtual void OutputStartCap ();
	virtual void OutputMitreLimit ();
	virtual void OutputFillColour ();

	// (ChrisG - 29/11/00)
	// New functions to allow for use of RGB colour models in the export.
	virtual void OutputStrokeRGBColour ();
	virtual void OutputFillRGBColour ();
	virtual void OutputStrokeCMYKColour ();
	virtual void OutputFillCMYKColour ();

	// (ChrisG - 3/11/00)
	// These new functions have been added so that GetValidTextAttributes works the same
	//	as GetValidPathAttributes
	virtual void OutputTextRenderMode ();
	virtual void OutputTextAspectRatio ();
	virtual void OutputTextTracking ();
	virtual void OutputTextJustification ();
	virtual void OutputTextLineSpacing ();
	virtual void OutputTextBaselineShift ();
	virtual void OutputTextSubSuperScript ();

	// (ChrisG 18/12/00)
	// This is called to write out any additional info for gradient fills, just before 
	//	the standard fill operators (b,f,s, etc...) are used.
	virtual void WriteGradientFillInstance ();

	// Member variables.
	BOOL			ValidPen;
	CCLexFile		*ExportFile;
	TCHAR			*CreatorString;

	// This render regions caps. We keep a cached version here as it can be needed in rendering
	RRCaps Caps;

	// Flag that specifies that we need to print the path attribute tokens out again.
	BOOL	m_bValidPathAttrs;
	BOOL	m_bValidTextAttrs;
};


/********************************************************************************************

>	class EPSExportDC : public ExportDC

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Provide a device context for rendering to an EPS file.  
	SeeAlso:	ExportDC; EPSFilter

********************************************************************************************/

class EPSExportDC : public ExportDC
{
public:
	EPSExportDC(Filter*);
	BOOL Init(CCLexFile*);

	// Functions to do the actual output to the EPS file.
	virtual BOOL OutputNewLine();
	virtual BOOL OutputToken(TCHAR*);
	virtual BOOL OutputDirect(BYTE *, INT32);
};

#endif // INC_SAVEEPS
