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

#ifndef INC_EXJPEG
#define INC_EXJPEG

#include "bitfilt.h"
#include "jpglib_namespace.h"
//#include "filtrres.h"

class CXaraFileRecord;

typedef UINT32 JPEG_QUALITY;

/********************************************************************************************

>	class JPEGExportOptions : public BitmapExportOptions

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/96
	Purpose:   	Allows JPEG specific export options to be manipulated

********************************************************************************************/
class JPEGExportOptions : public BitmapExportOptions
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNCREATE(JPEGExportOptions);
public:
	JPEGExportOptions();
	JPEGExportOptions(const FilterType FilterID, const StringBase* pFilterName);

	virtual BOOL	RetrieveDefaults();
	virtual BOOL	SetAsDefaults() const;

	// Save and load from named export set properties.
	virtual BOOL Write(CXaraFileRecord* pRec);
	virtual BOOL Read(CXaraFileRecord* pRec);

	virtual BOOL	GetSupportsImageMap() { return TRUE; }

	// Functions to edit actual options
	BOOL			DoAsProgressive() const;
	BOOL			SetMakeProgressive(BOOL bProgressive);

	JPEG_QUALITY	GetQuality() const;
	BOOL			SetQuality(const JPEG_QUALITY& Quality);

	libJPEG::J_DCT_METHOD	GetDCTMethod() const;
	BOOL			SetDCTMethod(const libJPEG::J_DCT_METHOD& DCTMethod);

	libJPEG::J_COLOR_SPACE	GetColourModel() const;
	UINT32			GetColourComponentCount() const;
	BOOL			SetColourModel(const libJPEG::J_COLOR_SPACE& ColourModel);

	static void	SetKernelBitmap (KernelBitmap* pBitmap) { pKernelBitmap = pBitmap; }
	static KernelBitmap* GetKernelBitmap ()				{ return pKernelBitmap; }

	static void SetJPEGPresentAndSelected(BOOL Value)	{ m_JPEGPresentAndSelected = Value; }
	static BOOL GetJPEGPresentAndSelected()				{ return m_JPEGPresentAndSelected;  }

	// for object copying and comparing
	virtual BOOL CopyFrom(BitmapExportOptions *pOther);

	//  Same as above, except it is used to copy information when changing file types
	virtual BOOL FileTypeChangeCopyFrom(BitmapExportOptions *pOther);
//	virtual BOOL Equal(BitmapExportOptions *pOther);

	// duplicating the static flag below - needed for the export preview
	BOOL			m_bJPEGPresentAndSelected;

	virtual UINT32 GetFilterNameStrID() { return _R(IDS_JPG_EXP_FILTERNAME);}

protected:
	
	JPEG_QUALITY			m_Quality;
	BOOL					m_DoAsProgressive;
	libJPEG::J_DCT_METHOD	m_DCTMethod;
	libJPEG::J_COLOR_SPACE	m_ColourModel;
	UINT32					m_ColourComponents;

	static KernelBitmap*	pKernelBitmap;
	static BOOL 			m_JPEGPresentAndSelected ;

};

class JPEGErrorManager;
class JPEGDataDestination;
class CCLexFile;


/**************************************************************************************

>	class JPEGExportFilter : public BaseBitmapFilter

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Provides an Accusoft-free JPEG export filter

**************************************************************************************/
class JPEGExportFilter : public BaseBitmapFilter
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNAMIC(JPEGExportFilter);

public:
	JPEGExportFilter();
	~JPEGExportFilter();

	// Overrides from Filter class
	virtual BOOL Init();

	INT32 HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
						UINT32 FileSize) {return 0;}	// dummy implementation

	virtual BOOL IsThisBppOk(UINT32 Bpp);

	virtual BOOL WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi);
	virtual BOOL WriteBitmapToFile(KernelBitmap* pKernelBitmap, BaseCamelotFilter *pFilter,
										CCLexFile *pFile, INT32 Compression);

	BOOL WriteBitmapSource(BitmapSource *Source, UINT32 Height, BaseCamelotFilter* pThisFilter, CCLexFile* pCCFile);

	BOOL DoExportBitmapWithOptions(Operation* pOp, CCLexFile* pFile, PathName* pPath,
									KernelBitmap* pBitmap, BitmapExportOptions *pOptions);

	virtual BitmapExportOptions* CreateExportOptions() const;
	virtual void PostGetExportOptions(BitmapExportOptions* pOptions);

protected:
// Overridden Functions
	virtual BOOL SetExportHint(Document* pDoc);
	
	virtual BOOL GetRenderBottomToTop();

	virtual BOOL GetExportOptions(BitmapExportOptions* pOptions);
//	virtual BOOL GetExportOptions(UINT32 *Depth, double *DPI, SelectionType *Selection,
//										Coord* pForceSize);

	virtual BOOL PrepareForOperation();
	virtual BOOL PrepareToExport(Spread *pSpread, UINT32 Depth, double DPI,
									SelectionType Selection, UINT32 Dither);
	BOOL InternalPrepareToExport();
	BOOL EndWriteToFile(void);

	virtual BOOL WriteFileHeader(void);
	virtual BOOL WriteFrame(void);
	virtual BOOL WritePostFrame(void);

	virtual void CleanUpAfterExport();

// Support Functions
	BOOL DoFilter(KernelBitmap* pKernelBitmap, CCLexFile *pFile);

	BOOL WriteRawBitmap(const ADDR& pBitmapData, 
						const BMP_SIZE& Width, const BMP_SIZE& Height,
						const BMP_DEPTH& Depth,
						DIBConvert* pConverter);

	BOOL	InitErrorHandler();
	BOOL	InitFileHandler();

	Filter*	GetFilterForUpdate() const;
	BOOL	SetFilterForUpdate(Filter* const pFilterForUpdate);

	BOOL	GetPostOperation() const			{return m_PostOperation;   }
	void	SetPostOperation(BOOL Value)		{ m_PostOperation = Value; }


protected:
	// Attributes
	// for the file we're exporting to
	BOOL							m_bOldReportErrors;
	BOOL							m_bOldThrowExceptions;
	BOOL							m_PostOperation;

	Filter*							m_pFilterForUpdate;

	// the IJG control structure
	struct libJPEG::jpeg_compress_struct m_cinfo;
	// ...and support
	JPEGErrorManager*				m_pErrorHandler;
	JPEGDataDestination*			m_pDestinationHandler;
};



#endif	// INC_EXJPEG
