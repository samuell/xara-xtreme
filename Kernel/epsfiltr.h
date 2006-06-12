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
// $Log $

#ifndef INC_EPSFILTR
#define INC_EPSFILTR

#include "ccfile.h"
#include "fixst256.h"
#include "epsstack.h"
#include "filters.h"
#include "docrect.h"
#include "listitem.h"
#include "txtattr.h"
//#include "fontbase.h"
#include "epsclist.h"

class PathName;
class Document;
class Node;
class NodePath;
class Layer;
class AttrLineWidth;
class AttrStrokeColour;
class AttrFillColour;
class OpEPSRead;
class Page;
class EPSRenderRegion;
class EPSExportDC;
class ColourListComponent;
class SelOperation;
class TransformBase;
class NodeRenderableBounded;
class NodeRegularShape;
class NodeEllipse;
class NodeRect;


// Special string constant to get around problem with immediate colours caused when
// we decided to use EPS as native file format.
//
// This is an arbitrary name chosen which assumes the user will never decide to
// name a colour with this sequence of characters (reasonably safe assumption).
#define ImmediateColourFudgeyBodgeName (_T("ciamcawahh42"))

// The factor to scale standard EPS coordinates by to convert them to Camelot 
// internal coordinates.
const INT32	EPSScaleFactor	= 1000;
const UINT32	EPSC_Invalid	= 0;

// EPSCommand enumerated type - all the commands used by Illustrator 3.0 EPS files.
// See the Illustrator EPS spec.

enum
{
	EPSC_showpage = 1,
	
	// Graphics state operators
	EPSC_A,
	EPSC_d,
	EPSC_i,
	EPSC_D,
	EPSC_j,
	EPSC_J,
	EPSC_M,
	EPSC_w,

	// Colour operators
	EPSC_g,
	EPSC_G,
	EPSC_k,
	EPSC_K,
	EPSC_x,
	EPSC_X,
	EPSC_p,
	EPSC_P,
	
	// Overprint operators
	EPSC_O,
	EPSC_R,

	// Group operators
	EPSC_u,
	EPSC_U,
	
	// Path Construction operators
	EPSC_m,
	EPSC_l,
	EPSC_L,
	EPSC_c,
	EPSC_C,
	EPSC_v,
	EPSC_V,
	EPSC_y,
	EPSC_Y,

	// Path Painting operators
	EPSC_N,
	EPSC_n,
	EPSC_F,
	EPSC_f,
	EPSC_S,
	EPSC_s,
	EPSC_B,
	EPSC_b,
	EPSC__u,  // Actually *u
	EPSC__U,  //      and *U

	// Clipping operators
	EPSC_q,
	EPSC_Q,
	EPSC_H,
	EPSC_h,
	EPSC_W,
	
	// Text operators
	EPSC_To,
	EPSC_TO,
	EPSC_Tp,
	EPSC_TP,

	// Matrix operators
	EPSC_Tm,
	EPSC_Td,
	EPSC_T_,
	EPSC_TR,

	// Text Attribute operators
	EPSC_Tr,
	EPSC_Tf,
	EPSC_Ta,
	EPSC_Tl,
	EPSC_Tt,
	EPSC_TW,
	EPSC_Tw,
	EPSC_TC,
	EPSC_Tc,
	EPSC_Ts,
	EPSC_Ti,
	EPSC_Tz,
	EPSC_TA,
	EPSC_Tq,

	// Text Body operators
	EPSC_Tx,
	EPSC_Tj,
	EPSC_TX,
	EPSC_Tk,
	EPSC_TK,
	EPSC_Tplus,
	EPSC_Tminus,

	// Special tokens
	EPSC_end,
	EPSC_ctx,
	EPSC_ctex,

	EPSC_Name,
	EPSC_ArrayStart,
	EPSC_ArrayEnd,
	EPSC_Slash,
	EPSC_Integer,
	EPSC_Double,
	EPSC_FixedPoint,
	EPSC_String, 
	EPSC_Comment,
	EPSC_EOL,
	EPSC_EOF
};

/********************************************************************************************

>	class CommandMap

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/02/94
	Purpose:	Provides a mapping from a token (i.e. an integer) to the string
				representation of that token.
				An array of these objects is used to map an EPS command string onto the
				enumerated type used for the token representation within Camelot.

********************************************************************************************/

class CommandMap
{
public:
	EPSCommand Cmd;
	TCHAR       CmdStr[20];
};

/********************************************************************************************

>	class RangeList : public List

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/94
	Purpose:	Holds a list of ranges.  This is used to keep track when importing
				a file and preserving the layer information, and the import was initiated
				via drag and drop.  When the import has finished, the bounding box of
				all the nodes on all the ranges is found, and then the all the nodes in
				all the ranges are centred on the drop point.
	SeeAlso:	OpDroppedFiles

********************************************************************************************/

class RangeList : public List
{
	CC_DECLARE_MEMDUMP(RangeList);

public:
	BOOL AddRange(Node *pFirst, Node *pLast);
	DocRect GetBoundingRect();
	void Transform(TransformBase&);
};


/********************************************************************************************

>	class EPSClipContextItem : public ListItem

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/22/95
	Purpose:	USed to implement a stack of clipping regions for importing AI EPS with.
	SeeAlso:	EPSClipContext

********************************************************************************************/

class EPSClipContextItem : public ListItem
{
	CC_DECLARE_MEMDUMP(EPSClipContextItem)

public:
	EPSClipContextItem(INT32 TheContextLevel, Path *pNewClipPath);
	~EPSClipContextItem();

public:
	INT32 ContextLevel;
	Path *pClipPath;
	INT32 ComplexRegionLevel;
};


/********************************************************************************************

>	class EPSClipContext : public List

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/22/95
	Purpose:	Used to maintain clipping regions (masks) while importing Illustrator EPS.
				A stack of regions is maintained, similar to attribute rendering, with the 
				usual save/restore semantics.
				Adding a new clipping region results in the new clipping region being
				merged with the current clipping region.  In this way complex paths can
				be used to build up complex masks (this is the brain-damaged way AI EPS
				does it anyway).
	SeeAlso:	EPSFilter

********************************************************************************************/

class EPSClipContext : public List
{
	CC_DECLARE_MEMDUMP(EPSClipContext)

public:
	EPSClipContext();
	void SaveContext();
	BOOL RestoreContext();
	BOOL AddNewClippingPath(Path *pNewClipPath);
	BOOL StartComplexClipRegion();
	BOOL EndComplexClipRegion();
	BOOL ClipPathToRegion(Path *pPathToClip);
	Path *GetClipRegion();
	void SetClippingFlags(UINT32 NewFlags) {ClippingFlags = NewFlags;};

protected:
	INT32 ContextLevel;
	INT32 ComplexRegionLevel;
	Path *pCachedClipPath;
	BOOL CacheIsValid;
	UINT32 ClippingFlags;
};


/********************************************************************************************

< EPSFilter::PathType

	Comment:	Used by the EPS filters - it indicates what to do with the next path that
				is found, i.e.whether to use it normally, throw it away, or do something
				special with it.  The current values and their meanings are:

				PATH_NORMAL			- Store the path in the tree as a normal path.

				PATH_DISCARD		- Discard the path because it's not needed, e.g. a
									  blend object would just be loaded as the start and
									  end paths, and the 'blended' paths would be discarded.

				PATH_DISCARD_STICKY - Like PATH_DISCARD, but stays in effect until a token
									  is found which explicitly causes it to be changed
									  back to PATH_NORMAL (or whatever).  Usually after
									  a path has been processed, the path type is set back
									  to PATH_NORMAL - but this type is the exception.

				PATH_RECT			- Add the path as a rectangle object.

				PATH_ELLIPSE		- Add the path as an ellipse object.

				PATH_MANGLED		- Stores the path in the tree as a normal path but
									  sets its mangled bit.

	SeeAlso:	EPSFilter

********************************************************************************************/ 

typedef struct
{	
	// TRUE if a complex path is being constructed
	//BOOL	use an INT32 instead for clipping matters!
	INT32 ComplexPath;

	// TRUE if we should stroke the complex path being constructed.
	BOOL StrokeComplexPath;

	// TRUE if the path being constructed has not had any attributes added yet
	BOOL NoAttributes;

	// TRUE if we've found an error in the EPS
	BOOL EPSErrorEncountered;

	// TRUE if we're adding nodes to a layer created by the filter
	// FALSE if adding to a layer that was already in the document
	BOOL AddToNewLayer;

	// TRUE if the last path was handled by an 'h' or 'H' operator, in which
	// case when we find a moveto we just delete the path and start a new one,
	// instead of complaining and flagging an EPS syntax error.  We have to do this
	// because ArtWorks EPS uses 'H' or 'h' when it should use 'N' or 'n'.
	BOOL PathIsHidden;

} EPSFlagsDefn;



/********************************************************************************************

>	class EPSFilter : public VectorFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Purpose:	Encapsulates a basic Adobe Illustrator EPS import/export filter.
				This filter can be used as a base class, because most EPS formats are
				based (more or less) on AI format.  Derived filters can override whichever
				parts they want to implement other flavours of EPS.
	SeeAlso:	EPSFilter::Init; EPSFilter::DoImport; EPSFilter::DoExport; Filter;
				ArtWorksEPSFilter; Corel3Filter; Corel4Filter

********************************************************************************************/

class EPSFilter : public VectorFilter
{
	CC_DECLARE_DYNAMIC(EPSFilter);
	
public:
	EPSFilter();
	virtual BOOL Init();
	static BOOL InitPrefs();
	INT32 HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize);
	BOOL IsDefaultDocRequired(const TCHAR* pcszPathName);
	BOOL DoImport(SelOperation *Op, CCLexFile*, Document *DestDoc, 
				  BOOL, ImportPosition *Pos = NULL,
				  KernelBitmap** ppImportedBitmap = NULL,
				  DocCoord* pPosTranslate = NULL, String_256* URL=NULL);
	BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);

	// Functions to deal with the binary header version of the file
	BOOL ExportBinaryHeader(CCLexFile*);
	BOOL CorrectBinaryHeader(CCLexFile*);

	// Functions to allow the filter a chance to write stuff into the sections
	virtual BOOL WriteEPSProlog();
	virtual BOOL WriteEPSSetup();
	virtual BOOL WriteEPSComments();
	virtual BOOL WriteScript();

	// Must it export the print componets?
	virtual BOOL NeedsPrintComponents ( void );

	TCHAR *GetEPSCommand(EPSCommand Cmd);

	EPSExportDC *GetExportDC() { return ExportDCPtr; }
	EPSRenderRegion* GetExportRegion() { return ExportRegion; }

	virtual INT32 ImportBinary(ADDR pData, INT32 Length);

	virtual BOOL AddNewNode(Node *pNewNode);


	typedef enum
	{
		PATH_NORMAL,
		PATH_DISCARD,
		PATH_DISCARD_STICKY,
		PATH_RECT,
		PATH_ELLIPSE,
		PATH_MANGLED
	} PathType;

	// Can be called by e.g. Doc components if they find a problem in their EPS comments.
	void HandleEPSError();

	INT32 DecodeHexString(ADDR pData, INT32 Length, INT32 nStart = 0);

	// Allow people to change the import origin
	BOOL ResetImportOrigin();
	BOOL ResetImportOrigin(DocCoord NewOrigin);

	// Options for the export of XS EPS
	static INT32 XSEPSExportPSType;
	static INT32 XSEPSExportDPI;
	static BOOL XSEPSExportTextAsCurves;

protected:
	//
	// Import related functions
	//
	virtual BOOL PrepareToImport();
	virtual void CleanUpAfterImport(BOOL Successful);

    // Input manipulation/parsing routines
	void DecodeToken();
	virtual void LookUpToken();
	virtual BOOL ProcessToken();

	BOOL GetToken();
	BOOL GetLineToken();
	BOOL HandleToken();
	BOOL ProcessEndOfPath ( void );
	BOOL ProcessUnfilledPath ( void );
	BOOL ProcessGroup();
	virtual BOOL ProcessComment(BOOL BypassDocComponents = FALSE);
	BOOL ProcessRectangle(NodeRect* pRect, NodeRegularShape* pQuickShape);
	BOOL ProcessEllipse(NodeEllipse* pEllipse, NodeRegularShape* pQuickShape);
	virtual BOOL ProcessFilterComment();
	void HandleNoMemory();
	virtual BOOL AddAttributes(NodeRenderableBounded *pNode, BOOL Stroked, BOOL Filled);

	virtual INT32 EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize);

	// Layer insertion functions.
	BOOL   UseLayer	( String_256	&LayerName,
					  BOOL			GuideLayer	= FALSE );
	Layer* AddLayer ( String_256	&LayerName,
					  BOOL			GuideLayer );

	BOOL StartGroup();
	BOOL EndGroup(BOOL Masked = FALSE);
	virtual BOOL ValidateGroup(Node *Group);
	virtual BOOL MaskedGroupEnding();
	virtual void ProcessTextMatrix(Matrix* pMatrix);
	BOOL AttachAllNodes();
	BOOL InsertNewNode(Node *pNewNode);

	virtual BOOL Import_gsave();
	virtual BOOL Import_grestore();

	// Export related functions
	virtual BOOL PrepareToExport(CCLexFile*, Spread *pSpread);
	virtual void CleanUpAfterExport();
	virtual EPSExportDC* CreateExportDC();
	void RemoveMessage(CCLexFile* pFile);

	virtual BOOL GetExportOptions( );

	// EPS comment related functions
	virtual BOOL ProcessEPSComment();
	virtual BOOL ProcessEPSTrailer();
	virtual BOOL ProcessBoundingBox();

	void ProcessEPSProlog();
	void ProcessEPSSetup();

	// Nice way of extracting and constructing colours for EPS files
	void GetEPSColour(DocColour *DestCol, 
					  PColourCMYK *pCMYK, 
					  TintType Tint, FIXEDPOINT TintVal, 
					  String_64 *ColName);
	void GetEPSColourRGB (DocColour *DestCol, 
						INT32 red, INT32 green, INT32 blue,
						TintType Tint, FIXEDPOINT TintVal, 
						String_64 *ColName);

	// Used to get unique layer names when a clash occurs
	void MakeLayerNameUnique(String_256& LayerName, Spread *pSpread);

	// Text pocessing
	BOOL ProcessText();
	BOOL GetStoryMatrix(Matrix* pMatrix);
	BOOL SetStrokeColourNone();
	BOOL SetFillColourNone();
	BOOL MakeNewTextLine(Node * pParent);

	// Somewhere to keep the info about where various file sections
	// Start and end
	FilePos	EPSStart;
	FilePos	EPSEnd;
	FilePos	PreviewStart;
	FilePos	PreviewEnd;

	// Resource IDs of the strings that describe this filter.
	UINT32 FilterNameID;
	UINT32 FilterInfoID;

	// The array of keyword names
	static CommandMap Commands[];

	// A list of all comments.
	EPSCommentList EPSComments;

	// The file we are reading
	CCLexFile *EPSFile;
	
	// Buffer to hold the current token
	const TCHAR *TokenBuf;
	
	// How many characters we had read the last time we updated the progress display.
	INT32 LastProgressUpdate;
	
	// Current token
	EPSCommand Token;
	
	// Token values
	union
	{
		INT32		Long;
		FIXEDPOINT	FixedPoint;
		double		Double;
		TCHAR   	*String;
		TCHAR       *Comment;
	} TokenData;

	// The EPS stack - quite handy!
	EPSStack Stack;

	// The pasteboard rectangle of the spread we're importing onto - used to see if
	// image is a rampantly incorrect position (it is adjusted if it is).
	DocRect SpreadRect;
		
	// The path node being created (if any)
	NodePath *pPath;
	
	// The layer that objects are placed on
	Layer *pLayer;
	
	// The page that objects appear on
	Page *pPage;
	
	// The Node to which new objects are attached (may be a Layer or a Group node)
	Node *pNode;

	// the flags
	EPSFlagsDefn EPSFlags;

	struct
	{
		// The operation that caused the import
		SelOperation *pOp;

		// Where the file was dropped for drag'n'drop, or NULL if drag'n'drop was not used.
		ImportPosition *Pos;

		// Where we should add the imported file
		Spread *pSpread;

	} ImportInfo;

	// What to do with the path when we have finished.
	PathType ThePathType;

	// Used to export EPS files.
	EPSRenderRegion *ExportRegion;
	EPSExportDC		*ExportDCPtr;		// Graeme (23-0-00) - renamed to avoid namespace
										// conflicts. (It's original name was ExportDC.)

	// Pointer to the document's colour component (only when importing).
	ColourListComponent *pColours;

	// Bounding box for simple shapes
	DocCoord ShapeBBox[4];

	// Holds all nodes imported when importing with layers.
	static RangeList ImportRange;

	// Used to remember the range as we build it
	Node *pFirstNodeInRange,
		 *pLastNodeInRange;

	// Indicates whether or not we should try to reposition the objects if they fall
	// off the spread boundaries (does not apply to drag'n'drop positioning checks).
	// (Defaults to TRUE in base class).
	BOOL AdjustOrigin;

	// Indicates whether this EPS format supports layers at all
	BOOL SupportsLayers;

	// Indicates whether we are using layers when importing
	BOOL UseLayers;

	// Used to check for balanced groups and masked groups.
	INT32 GroupNesting;
	INT32 MaskedGroupNesting;

	// Used to validate paths
	BOOL InvalidPathFound;

	// Used to indicate where to store path data
	Path *pInkPath;

	// Used for clipping regions
	EPSClipContext ClipRegion;

	INT32 TextComment[3];

	// gsave/grestore stack
	List GraphicStateStack;

	// Extra font flags block processing
	struct _FontFlags {
		BOOL Bold;
		BOOL Italic;
	} FontFlags;

	// Describes types of fonts loaded from native files (eg truetype)
	FontClass ClassOfFont;
	
	// A new tag token describing the mould threshold
	INT32 NewMouldThreshold;

	//	Are we in a group or not. For clipping region thingy.
	BOOL	EPS_Group;

	// The end of the EPS record in the file.
	FilePos	EndOfEPS;
};	

#endif
