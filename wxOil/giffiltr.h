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

#ifndef INC_GIFFILTR
#define INC_GIFFILTR

#include "maskfilt.h"
#include "outptgif.h"
#include "bmpprefs.h"  	// SelectionType
//#include "accures.h"

class CXaraFileRecord;

#define ZERO 0

/********************************************************************************************

  >	class GIFImportOptions : public BitmapImportOptions

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Purpose:   	Provides GIF specific options for the import

********************************************************************************************/
class GIFImportOptions : public BitmapImportOptions
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(GIFImportOptions);
public:
	GIFImportOptions() : BitmapImportOptions() {m_bProvideProgress = TRUE;}

};

typedef UINT32 CENTISECONDS;
/********************************************************************************************

>	class GIFExportOptions : public MaskedFilterExportOptions

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/96
	Purpose:   	Base class for storing information about the bitmap we're exporting.

********************************************************************************************/
class GIFExportOptions : public MaskedFilterExportOptions
{
	CC_DECLARE_DYNCREATE(GIFExportOptions)
public:
	static BOOL Declare();

	GIFExportOptions();
    GIFExportOptions(const FILTER_ID FilterID, const StringBase* pFilterName);

	virtual BOOL	RetrieveDefaults();
	virtual BOOL	SetAsDefaults() const;

	// Save and load from named export set properties.
	virtual BOOL Write(CXaraFileRecord* pRec);
	virtual BOOL Read(CXaraFileRecord* pRec);

	virtual BOOL	GetSupportsImageMap() { return TRUE; }

	BOOL				GetGlobalPalette() const						{ return m_GlobalPalette; }
	void				SetGlobalPalette(UINT32 GlobalPalette)			{ m_GlobalPalette = GlobalPalette; }

	UINT32				GetLeftOffset() const							{ return m_LeftOffset; }
	void				SetLeftOffset(UINT32 LeftOffset)					{ m_LeftOffset = LeftOffset; }

	UINT32				GetTopOffset() const							{ return m_TopOffset; }
	void				SetTopOffset(UINT32 TopOffset)					{ m_TopOffset = TopOffset; }

	GIFDisposalMethod	GetAnimationRestore();
	BOOL				SetAnimationRestore(const GIFDisposalMethod& DisposalMethod);

	CENTISECONDS		GetAnimationDelay();
	BOOL				SetAnimationDelay(CENTISECONDS Delay);

	UINT32				GetAnimationLoopCount() const;
	BOOL				SetAnimationLoopCount(UINT32 LoopCount, BOOL Value);

	Coord				GetAnimationSize() const;
	BOOL				SetAnimationSize(const Coord& Size);

	BOOL				GetDelayValueIsDefault()	const				{ return DelayValueIsDefault; }
	BOOL				SetDelayValueIsDefault(BOOL Value)				{ DelayValueIsDefault = Value; return TRUE; }

	static KernelBitmap**	GetBitmapList()								{ return m_pBitmapList; }
	static void				SetBitmapList(KernelBitmap** pBitmap)		{ m_pBitmapList = pBitmap; }

	static UINT32				GetListSize()				{ return m_ListSize; }
	static void				SetListSize(UINT32 n)			{ m_ListSize = n;	 }
	static void				IncListSize(UINT32 n)			{ m_ListSize += n;	 }	
	static void				DecListSize(UINT32 n)			{ m_ListSize -= n;   } 

	// for object copying and comparing
	virtual BOOL CopyFrom(BitmapExportOptions *pOther);
//	virtual BOOL Equal(BitmapExportOptions *pOther);

	virtual BOOL FileTypeChangeCopyFrom(BitmapExportOptions *pOther);

	UINT32	DelayIndex;
	UINT32	RestoreIndex;
	
	virtual UINT32 GetFilterNameStrID() { return _R(IDN_FILTERNAME_GIF);}


protected:
	BOOL				m_GlobalPalette;	// Animation has a global rather than a local palette
	UINT32				m_LeftOffset;		// left offset from overall bounds of animation
	UINT32				m_TopOffset;		// top offset from overall bounds of animation
	CENTISECONDS		m_AnimDelay;		// Delay between frames for Animations
	GIFDisposalMethod	m_AnimDispose;		// Disposal method for frames in Animation
	UINT32				m_AnimLoop;			// Number of times to loop Animation
	Coord				m_AnimSize;			// Size for Animation frames
	BOOL						DelayValueIsDefault;
	static KernelBitmap**		m_pBitmapList;
	static UINT32					m_ListSize;

	// Persistant buffer
	static CENTISECONDS			g_AnimationDelay;		// Milliseconds between frames.
	static GIFDisposalMethod	g_AnimationRestoreType;
	static UINT32					g_AnimationLoopCount;	// Times animation should repeat.
};

class KernelBitmap;
class BaseCamelotFilter;

/********************************************************************************************

>	class TI_GIFFilter : public MaskedFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Purpose:	Encapsulates a Windows GIF export and import filter.
				Derives off the masked filter which provides funcitonality for doing masked
				bitmap export and import.

********************************************************************************************/

class TI_GIFFilter : public MaskedFilter
{
	CC_DECLARE_DYNAMIC(TI_GIFFilter);
	
public:
	TI_GIFFilter();
	virtual BOOL Init();

	virtual INT32 HowCompatible( PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,
							   UINT32 FileSize );

	//Graham 5/11/97
	virtual BOOL IsDefaultDocRequired(const TCHAR* pcszPathName);

	virtual INT32 GetFilterType() const { return TI_GIF; }

	// Import related functions
	virtual BOOL ReadFromFile(OILBitmap* pOilBitmap);

	// This is the function for native/web files to use
    virtual BOOL ReadFromFile(OILBitmap* pOilBitmap, BaseCamelotFilter* pFilter,
							  CCLexFile* pFile, BOOL IsCompressed);

	// Function for web/native filters to use to write out the bitmap data to file
	virtual BOOL WriteBitmapToFile(KernelBitmap* pKernelBitmap, BaseCamelotFilter* pFilter,
								   CCLexFile* pFile, INT32 Compression);

	// Function to write out a list of bitmaps to file
	virtual BOOL SaveExportBitmapsToFile(CCLexFile* pFile, PathName* pPath, BitmapExportParam* pParam,
										 BOOL DontShowFileName = FALSE);

	void AlterPaletteContents( LPLOGPALETTE pPalette );

	// Check if this Bitmap filter can cope with saving at this Bpp/Colour depth
	virtual BOOL IsThisBppOk(UINT32 Bpp);

	// Public way of finding out how compatable the GIF filter thought the file was
	// We might want to bring in the Accusoft filter to help out.
	INT32 GetGifCompatibility();

	// Virtual overrides
	virtual UINT32 GetExportMsgID();
	virtual BOOL ShouldReuseExistingBitmaps(); 
	virtual void PostGetExportOptions(BitmapExportOptions* pOptions);

protected:

	// Import related functions
	virtual BitmapImportOptions* CreateImportOptions() const;

	// Export related functions	
	virtual BitmapExportOptions* CreateExportOptions() const;

	virtual OutputDIB* GetOutputDIB ( void );

	virtual UINT32 GetHintType(void);
	virtual BOOL GetExportOptions( BitmapExportOptions* pOptions );
	virtual void CleanUpAfterExport();
	virtual BOOL EndWriteToFile( );

	virtual BOOL WriteFileHeader(void);
	virtual BOOL WritePreFrame(void);
	virtual BOOL WritePreSecondPass(void);
	virtual BOOL WritePostOptimisedPalette(void);
	virtual BOOL WriteFrame(void);
	virtual BOOL WritePostFrame(void);
	virtual BOOL WriteFileEnd(void);

	virtual BOOL WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi);
	
	// this is used for the actual writing of the file
	static BOOL WriteDataToFile( BOOL End, UINT32 Bpp, UINT32 Compression);
	static BOOL WriteToFile ( CCLexFile* File, LPBITMAPINFO Info, LPBYTE Bits,
							  String_64* ProgressString = NULL);
	// This is the form used for direct saving bitmaps into the native/web file format
	static BOOL WriteToFile ( CCLexFile* File, LPBITMAPINFO Info, LPBYTE Bits,
							  BOOL Interlace, INT32 Transparent,
							  BaseCamelotFilter* pFilter = NULL);

	// The class we use for actually outputting the GIF data and converting from 32 to n bpps
	static OutputGIF DestGIF;

	// This is so we can remember what we thought of the GIF file.
	INT32 GIFHowCompatible;

	static INT32 FilterType;		// Type of filter in use (TI_GIF .. TI_GIF_TRANSINTER)

	BOOL m_DoingAnimation;		// flag for wether or not we are generating a GIF animation
	UINT32 m_AnimationFrame;		// the frame we are on in the animation

	// The string to display when exporting the second stage.
	UINT32 Export2ndStageMsgID;

	// When we load a GIF, if there is any x,y offset, then we use 
	UINT32 m_LeftOffset;
	UINT32 m_TopOffset;

	// Give the filter a chance to add in an offset that was contained in the bitmap definition
	virtual BOOL AddOffsetFromBitmap(DocCoord * pOffset);

	// Give the filter a chance to change the layer's flags given the newly loaded bitmap
	// Only applies if loading multiple bitmaps onto new layers.
	virtual BOOL SetFlagsFromBitmap(Layer * pLayer, KernelBitmap * pBitmap, UINT32 nBitmapToRead);

	// Give the filter a chance to change the animation properties given the newly loaded bitmaps
	// Only applies if loading multiple bitmaps onto new layers.
	virtual BOOL SetAnimationPropertiesFromLoaded(Spread * pSpread);

	// Normal action is to centre the object around the drop position. We do not want this
	// and so oeverride this
	virtual BOOL GetDragAndDropTranslation(ImportPosition* pPos, DocRect BoundsRect, Coord* Offset);

	GIFDisposalMethod m_PreviousMethod;
};

/********************************************************************************************

>	class AnimatedGIFImage : public ListItem

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/97
	Purpose:	Stores details on the GIF image which is part of an export animation sequence.
	
********************************************************************************************/

class AnimatedGIFImage : public ListItem
{
	CC_DECLARE_DYNCREATE( AnimatedGIFImage )

public:
	AnimatedGIFImage();
	~AnimatedGIFImage();

	AnimatedGIFImage(const INT32 TransColour, const GIFDisposalMethod FrameRestore,
					 const CENTISECONDS FrameDelay,
					 const UINT32 LeftOffset, const UINT32 TopOffset,
					 const LPBITMAPINFO pInfo, const LPBYTE pBytes,
					 const LPBITMAPINFO pDiffInfo = NULL, const LPBYTE pDiffBytes = NULL);

	LPBITMAPINFO		GetOutputInfo()	const	{ return m_pInfo; }
	LPBYTE				GetOutputBits()	const	{ return m_pBytes; }

	void SetFrameRestore(const GIFDisposalMethod FrameRestore) { m_FrameRestore = FrameRestore; }
	
	INT32				GetTransColour() const	{ return m_TransColour; }
	GIFDisposalMethod	GetFrameRestore() const	{ return m_FrameRestore; }
	CENTISECONDS		GetFrameDelay()	const	{ return m_FrameDelay; }
	UINT32				GetLeftOffset()	const	{ return m_LeftOffset; }
	UINT32				GetTopOffset()	const	{ return m_TopOffset; }

protected:
	// Useful information about this frame in the image
	INT32				m_TransColour;
	GIFDisposalMethod	m_FrameRestore;
	CENTISECONDS		m_FrameDelay;

	UINT32				m_LeftOffset;
	UINT32				m_TopOffset;

	// This is a note of the bitmap image that needs outputting
	// It is a reference and so does not need cleaning up
	LPBITMAPINFO		m_pInfo;
	LPBYTE				m_pBytes;
	// This is the difference bitmap that may have been allocated
	// This does need cleaing up
	LPBITMAPINFO		m_pDiffInfo;
	LPBYTE				m_pDiffBytes;
};


#endif // INC_GIFFILTR


