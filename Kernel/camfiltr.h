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
// This filter forms the base filter for the v2 Native and Web file filters.

#ifndef INC_CAMFILTER
#define INC_CAMFILTER

#include "filters.h"
#include "ccfile.h"		// FilePos
#include "doccoord.h"	
#include "node.h"		// AttachNodeDirection 

class CXaraFile;
class CXaraFileRecord;
class CXaraFileRecordHandler;
class Node;
class DocColour;
class KernelBitmap;
class ColourListComponent;
class BitmapListComponent;
class UnitListComponent;
class DocInfoComponent;
class ViewComponent;
class FontComponent;
class PrintComponent;
class Progress;
class WebPrefsDlgParam;
class CCPanose;
class BitmapSource;
class NodePath;
class TextStory;
class NodeGroup;

class AtomicTagList;
class InsertLevelStack;
class EssentialTagList;
class TagDescriptionList;
class AtomicTagListItem;
class EssentialTagListItem;
class TagDescriptionListItem;
class CXaraFilePathRecordRefList;
// WEBSTER - markn 31/1/97
// Replaced with general system
//class CXaraFileTextStoryGroupRefList;
class CXaraFileNodeGroupRefList;

#define EXPORT_FILETYPE_WEB		"CXW"	// The .web format
#define EXPORT_FILETYPE_MIN		"CXM"	// The minimal web format
#define EXPORT_FILETYPE_NATIVE	"CXN"	// The native format

typedef enum 
{
	INSERTMODE_ATTACHTOTREE,
	INSERTMODE_SETCURRENTATTRIBUTE
} InsertMode;


typedef enum
{
	BWL_NONE = 0,
	BWL_COMPOUND,
	BWL_ALL
} BoundsWriteLevel;


/********************************************************************************************

>	struct InsertTreeContext

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/97
	Purpose:	Structure for saving and restoring tree insertion contexts. This is used
				by things like print marks and brush stroke definitions, which embed a 
				subtree definition in the file, but want to import it somewhere other than
				into the document tree.

				This structure encapsulates the current insertion position in the tree
				so that it can be saved & restored around such out-of-doc subtrees.

	SeeAlso:	BaseCamelotFilter::GetInsertContext;
				BaseCamelotFilter::RestoreInsertContext

********************************************************************************************/

typedef struct
{
	Node *pContextNode;
	BOOL InsertAsChild;
} InsertTreeContext;



/********************************************************************************************

>	class BaseCamelotFilter : public Filter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Purpose:	This filter is for the new web file format which is the minimilistic form of
				the new version 2 native file filter.
				Derive off VectorFilter as this is what this file format defines. 

********************************************************************************************/

class BaseCamelotFilter : public VectorFilter
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(BaseCamelotFilter);

public:
	// Construction and initialisation
	BaseCamelotFilter();
	~BaseCamelotFilter();
	virtual BOOL Init();
	virtual void Deinit();

	// Function used by the gallery thumb nail code to check the file is the native/web format
	// and then get to the preview bitmap record and leave the file pointer sitting at the bitmap image
	// WEBSTER-Martin-added extra parameter for non-Accusoft previews
	static BOOL SkipToPreviewBitmap(CCLexFile * pFile, BOOL *IsNewNativeFile, UINT32 * pFilterId = NULL);

	// Function used by the gallery index generation code to check the file is the native/web format
	// and then get to the index information record and load the information into the string
	static BOOL SkipToIndexInformation(CCLexFile * pFile, String_256 *pString, BOOL *IsNewNativeFile);


	// HowCompatible() returns value in range 0 to 10:
	//   0 => "Definitely Not"
	//  10 => "Yes Sir, that's my file format"
	virtual INT32 HowCompatible( PathName& Filename, ADDR  HeaderStart, UINT32 HeaderSize,
							   UINT32 FileSize );

	virtual INT32 HowCompatibleIsFileType(char* pFileType) = 0;

	// Filters can override these functions, if required:
	// Public  - Exporting
	virtual BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);

	// Public  - Importing
	virtual BOOL DoImport(SelOperation*, CCLexFile*, Document*, 
						  BOOL AutoChosen = FALSE, ImportPosition *Pos = NULL,
						  KernelBitmap** ppImportedBitmap = NULL,
						  DocCoord* pPosTranslate = NULL, String_256* = NULL);

	virtual BOOL IsWebFilter() = 0;				// Is it the proper web format?
	virtual BOOL IsCompactNativeFilter() = 0;	// Is it the compact native format?

	virtual BOOL IsImporting();				// Importing into existing doc?  Use this func to find out.
	virtual BOOL IsImportingAtPosition();	// Importing into existing doc and at a specific point

	virtual BOOL IsDefaultDocRequired(const TCHAR* pcszPathName);

	virtual INT32 GetMaxPathLookUp();

	static BOOL WritePathsInRelativeFormat() { return WriteRelativePaths; }

	// Record writing functions
	INT32	StartRecord(UINT32 Tag,INT32 Size);
	INT32	StartStreamedRecord(UINT32 Tag,INT32 Size);
	BOOL	EndRecord();
	BOOL	EndStreamedRecord();

	// These are used during writing????
	UINT32	GetRecordTag();
	INT32	GetRecordNum();

	void		SetCoordOrigin(const DocCoord& Origin);
	DocCoord	GetCoordOrigin();

	BOOL	Write(BYTE b);
	BOOL	Write(UINT32 n);
	BOOL	Write(INT32 n);
	BOOL	Write(const DocCoord& Coord);
	BOOL	Write(TCHAR* pStr);					// Calls WriteUnicode() (Use WriteASCII() for ASCII writing)
	BOOL	Write(BYTE* pBuf,UINT32 BufSize);
	BOOL	Write(FLOAT f);

	BOOL	WriteWCHAR(WCHAR w);

	BOOL	WriteASCII(TCHAR* pStr);
	BOOL	WriteUnicode(TCHAR* pStr);
	BOOL	WriteZeroSizedRecord(UINT32 Tag);

	UINT32	Write(CXaraFileRecord* pRecord);
	UINT32	WriteDefinitionRecord(CXaraFileRecord* pRecord);

	BOOL	WriteCCPanose(const CCPanose & Panose);
	INT32	WriteFontDefinition(WORD FontHandle, BOOL IsBold, BOOL IsItalic);

	BOOL	WriteBitmapSource(const BitmapSource& Source, UINT32 Height);
	
	INT32	WriteRecord(DocColour* pDocColour);
	INT32	WriteRecord(KernelBitmap* pBitmap);

	ColourListComponent*	GetColourDocComponent()	{ return pColComponent; }
	BitmapListComponent*	GetBitmapDocComponent()	{ return pBmpComponent; }
	UnitListComponent*		GetUnitDocComponent()	{ return pUnitsComponent; }
	DocInfoComponent*		GetDocInfoComponent()	{ return pInfoComponent; }
	ViewComponent*			GetViewDocComponent()	{ return pViewComponent; }
	PrintComponent*			GetPrintComponent()		{ return pPrintComponent; }
	FontComponent*			GetFontComponent()		{ return pFontComponent; }

	FilePos GetFilePos();
	UINT32	GetNumBytesWritten();

	virtual Document* GetDocument() { return TheDocument; }
	virtual Spread* GetSpread() { return pTheSpread; }

	virtual void GotError(UINT32 errorID);
	virtual void GotError( UINT32 errorID , const TCHAR* errorString);

	// Read-related functions
	virtual void EndOfFile() { EndOfFileFlag = TRUE; }
	virtual void SetTotalNumBytesToRead(UINT32 s);

	virtual BOOL InvalidateRegion(Node* pNode);

	// Allows say a streamed record handler, to get at the size of the record
	// currently being processed, so that it can know what value it has been allocated
	// to update the progress bar by.
	virtual UINT32 GetCurrentRecordSize();

	// Tree building functions (these should probably be protected...)
	virtual BOOL	InsertNode(Node* pNode);
	virtual BOOL	IncInsertLevel();
	virtual BOOL	DecInsertLevel();
	virtual Node*	GetInsertContextNode();				// See below if you want to save & restore the context
	virtual void	SetInsertContextNode(Node* pNode);
	virtual void	InsertNextNodeAsChild();
	virtual void	InsertNextNodeAsSibling();

	// Saving & restoring tree-building contexts
	virtual InsertTreeContext *GetInsertContext(void);
	virtual void RestoreInsertContext(InsertTreeContext *pOldState);

	// Progress bar functions
	virtual BOOL IncProgressBarCount(UINT32 n);
	virtual UINT32 GetProgressBarCount()	{ return ProgressBarCount; }
	virtual BOOL SetTotalProgressBarCount(UINT32 n);

#if XAR_TREE_DIALOG
	static List* GetRecordHandlers() { return &RecordHandlerList; }
#endif

	virtual WebPrefsDlgParam* GetCachedExportOptions() { return pPrefs; }

	// Error and warning handling
	BOOL UnrecognisedTag(UINT32 Tag);

	BOOL MergeSameNamedLayers();

private:
	void UpdateLastSafeToRenderNode(Node* pNode);

	Node* pLastUnsafeToRenderNode;

	// Do we want to export this template node?
	BOOL IsThisASelectedTemplate ( Node *pNode );

protected:
	// Ways of warning the user about problems when loading a document
	// These warn about tags and data which are unknown
	void AppendIgnoredDataWarning(UINT32 Tag);
	void UnkownEssentialDataError(UINT32 Tag);

public:
	// This allows the user to be warned about things like bitmap definitions not be loadable 
	// Needs to be public as items like the colour and bitmap components need access
	void AppendWarning(UINT32 WarningId);
	//void AppendWarning(String_256 *pString);

public:
	virtual BOOL WriteNodes(Node* pNode);
	virtual BOOL WriteNodeAndSubNodes (Node* pNode);

private:
	String_256 IgnoredDataString;
	String_256 WarningsString;

public:
	virtual BOOL ReadPostChildren(Node* pNode);
	virtual BOOL ReadFileUntil(INT32 tag);

protected:

	virtual BOOL StartProgressBar(String_64* pMessage);
	virtual void EndProgressBar();

	virtual void ResetImportExportVars();
	virtual BOOL PrepareImportExportVars();

	virtual CXaraFile* CreateCXaraFile();
	virtual BaseCamelotFilter* CreateNULLFilter(Document* pDocument) = 0;
	
	// Exporting
	virtual BOOL PrepareToExport(CCLexFile* pFile);
	virtual void CleanUpAfterExport(BOOL Success);

	virtual BOOL FindDocComponents();
	virtual BOOL StartExportDocComponents(BOOL BeforeCompression);
	virtual BOOL EndExportDocComponents(BOOL Success);

	virtual BOOL BeginDocumentExport();
	virtual BOOL EndDocumentExport();

	virtual Node* GetExportNode();
	
	virtual BOOL WriteDocument(Operation * pExportOp);
	virtual BOOL WritePreChildren(Node* pNode);
	virtual BOOL CanWriteChildren(Node* pNode);
	virtual BOOL WritePostChildren(Node* pNode);
	virtual BOOL WriteBeginChildRecords(Node* pNode);
	virtual BOOL WriteEndChildRecords(Node* pNode);
	virtual BOOL WriteCurrentAttributes();

	// Functions to write out nodes that are either selected, or necessary for the
	//	selected nodes to be exported correctly.
	virtual BOOL WriteSelectedNodes (Operation * pExportOp, Node * pNode, SelRange * pSelection);
	virtual BOOL WriteSelectedLayerAndNodes (Operation * pExportOp, Node * pNode, SelRange * pSelection);

	virtual BOOL WriteHeader();

	// Function to get export options, if any
	virtual BOOL GetExportOptions( WebPrefsDlgParam *pPrefs );
	virtual OpDescriptor* GetDialogueOp ( void );

	// The data we get from the export options dialog box
	WebPrefsDlgParam *pPrefs;

	// Function to export a preview bitmap
	virtual BOOL ExportPreviewBitmap(CCLexFile* pFile, Operation* pOp,
									 PathName* pPath, Document* pDoc);

	// Function to go and correct the file header once we have exported everything
	virtual BOOL CorrectFileHeader(CCLexFile* pFile);

	// Calcs an estimate of the number of bytes that will be written to the file
	virtual UINT32 CalcExportSize(Document* pDocument);

	// Importing
	static BOOL CreateRecordHandlers();
	static void DestroyRecordHandlers();
	BOOL InitRecordHandlers();
	BOOL DeinitRecordHandlers();

	virtual BOOL PrepareToImport(CCLexFile* pFile);
	virtual void CleanUpAfterImport(BOOL Successful);
	virtual BOOL PrepareTreeBuilding();
	virtual void CleanUpTreeBuilding();

	virtual BOOL EnsureFrameLayerIntegrity(Spread* pSpread);
	virtual BOOL EnsureLayerIntegrity(Spread* pSpread);

	virtual BOOL StartImportDocComponents();
	virtual BOOL EndImportDocComponents(BOOL Successful);

	virtual BOOL ReadFile();

public:
	// Function to find the file type for this filter
	virtual char*	GetExportFileType() = 0;
	virtual void	SetImportFileType(char* pFileType);
	virtual	BOOL	IsOpeningMinimalWebFormat();

private:
	char ImportFileType[4];

protected:
	// Resource IDs of the strings that describe this filter.
	UINT32 FilterNameID;
	UINT32 FilterInfoID;

	// The string to display when actually exporting rather than rendering.
	// 0 if we're not an export filter.
	// ExportMsgID is used in ExportRender when we are rendering the bitmap
	UINT32 ExportingMsgID;

	// Message to show when a bad file is found
	UINT32 BadFileMsgID;

	// Somewhere to keep the info about where various file sections
	// Start and end
	FilePos	FileStart;
	FilePos	FileEnd;

	// The start of the file header record
	FilePos	FileHeaderRecord;

	// Our note of the uncompressed file size
	UINT32 FileSize;

	// Items to handle the file compression
	INT32 CompressionType;	// type of compression in use (0 at present)
	BOOL CompressionOn;		// on/off flag

	virtual BOOL SetFileCompressionState(BOOL NewState); 
	virtual BOOL GetFileCompressionState(); 

	double FileVersionNumber;
	double BuildVersionNumber;

	// Read vars
	BOOL EndOfFileFlag;
	BOOL EscapePressed;

	// Tree building vars
	Node*				pInsertContextNode;
	BOOL				InsertNextAsChild;
	UINT32				InsertLevel;
	InsertLevelStack*	pInsertLevelStack;

	// ptrs to vital objects
	CXaraFile*				pCXaraFile;

	ColourListComponent*	pColComponent;
	BitmapListComponent*	pBmpComponent;
	UnitListComponent*		pUnitsComponent;
	DocInfoComponent*		pInfoComponent;
	ViewComponent*			pViewComponent;
	PrintComponent*			pPrintComponent;
	FontComponent*			pFontComponent;

	Spread*					pTheSpread;			// The spread we are importing into /exporting from

	// List of record handlers used to import files
	static List RecordHandlerList;
	static BOOL RecordHandlersCreated;

	struct
	{
		SelOperation*	pOp;	// The operation that caused the import
		ImportPosition*	pPos;	// Where the file was dropped for drag'n'drop, or NULL if drag'n'drop was not used.
	} ImportInfo;

public:
	// Returns the SelOperation that initiated this import, needed by
	// TemplateAttrRecordHandler::HandleRecord for attaching undoably
	// to NodeSetSentinel.
	SelOperation* GetImportSelOp() const	{ return ImportInfo.pOp; }

	// Tag management functions
	void AddAtomicTag(AtomicTagListItem* pItem);
	void AddEssentialTag(EssentialTagListItem* pItem);

	BOOL IsTagInAtomicList(UINT32 Tag);
	BOOL IsTagInEssentialList(UINT32 Tag);

	TagDescriptionListItem* GetTagDescription(UINT32 Tag);

	// WEBSTER - markn 11/2/97
	// System for writing tag descriptions to the file (introduced for the TAG_MOULD_BOUNDS record)
	virtual void AddTagDescription(UINT32 Tag,UINT32 ID);
	BOOL SetDocumentNudgeSize(UINT32 newVal);
	virtual void WriteTagDescriptionRecord();
	BOOL WriteNudgeSizeRecord();
	BOOL WriteBitmapSmoothingRecord();
	BOOL WriteDuplicationOffsetRecord();
	BOOL WriteRemainingAtomicTagDefinitions();
	UINT32 WriteXPEBitmapPlaceHolder(String_256& strBitmapName);

	// Don't use this version of AddTagDescription() unless you know what you're doing
	virtual BOOL AddTagDescription(TagDescriptionListItem* pItem);

private:
	// Tag management variables
	AtomicTagList*		pAtomicTagList;
	EssentialTagList*	pEssentialTagList;
	TagDescriptionList*	pTagDescriptionList;

public:
	// List management - used for import & export

	// Path record references
	virtual void		AddPathRecordRefToList(NodePath* pNodePath, UINT32 RecordNumber);
	virtual UINT32		FindPathRecordRefRecordNumber(NodePath* pNodePath);
	virtual NodePath*	FindPathRecordRefPath(UINT32 RecordNumber);
	virtual BOOL		FindSimilarPath(NodePath* pPath,UINT32* pOtherPathRecNum,Matrix* pTransform);
	virtual double		GetSimilarPathTolerance(NodePath* pPath) = 0;

	// Text story & group references
	// WEBSTER - markn 31/1/97
	// Replaced with general system
//	virtual void		AddTextStoryGroupRefToList(TextStory* pStory,NodeGroup* pGroup);
//	virtual NodeGroup*	FindTextStoryGroupRefGroup(TextStory* pStory);
//	virtual TextStory*	FindTextStoryGroupRefTextStory(NodeGroup* pGroup);

  	// WEBSTER - markn 29/1/97
	// Part of the general form of the system used to convert text to outlines in v1.5
	// Node & group references
	virtual void		AddNodeGroupRefToList(Node* pNode,NodeGroup* pGroup);
	virtual NodeGroup*	FindGroupForThisNode(Node* pNode);
	virtual BOOL		WriteNodeAsOutlines(Node *pNode);

private:
	// List management vars

	// Path record reference list
	CXaraFilePathRecordRefList*	pPathRecordRefList;

	// WEBSTER - markn 31/1/97
	// Replaced with general system
	// Text story & group reference list
	//CXaraFileTextStoryGroupRefList*	pTextStoryGroupRefList;

  	// WEBSTER - markn 29/1/97
	// Part of the general form of the system used to convert text to outlines in v1.5
	// Node & group reference list
	CXaraFileNodeGroupRefList*	pNodeGroupRefList;

public:
	BOOL SetLastRecordHandler(CXaraFileRecordHandler* pHandler,UINT32 Tag);

public:
	// Function calls used for building the document structure when loading
	Document*	GetLastDocumentInserted()			{ return pLastDocument; }
	Chapter*	GetLastChapterInserted()			{ return pLastChapter; }
	Spread*		GetLastSpreadInserted()				{ return pLastSpread; }
	Layer*		GetLastLayerInserted()				{ return pLastLayer; }
	NodePath*	GetLastNodePathInserted()			{ return pLastNodePath; }
	NodeAttribute*	GetLastCurrentAttrInserted()	{ return pLastCurrentAttr; }

	void	SetLastDocumentInserted(Document* pNode)			{ pLastDocument = pNode; }
	void	SetLastChapterInserted(Chapter* pNode)				{ pLastChapter = pNode; }
	void	SetLastSpreadInserted(Spread* pNode)				{ pLastSpread = pNode; }
	void	SetLastLayerInserted(Layer* pNode)					{ pLastLayer = pNode; }
	void	SetLastNodePathInserted(NodePath* pNode)			{ pLastNodePath = pNode; }
	void	SetLastCurrentAttrInserted(NodeAttribute* pNode)	{ pLastCurrentAttr = pNode; }

	UINT32	GetDocumentInsertedCount()		{ return DocumentCount; }
	UINT32	GetChapterInsertedCount()		{ return ChapterCount; }
	UINT32	GetSpreadInsertedCount()		{ return SpreadCount; }
	UINT32	GetLayerInsertedCount()			{ return LayerCount; }
	UINT32	GetSetSentinelInsertedCount()	{ return SetSentinelCount; }

	void	IncDocumentInsertedCount()		{ DocumentCount++; }
	void	IncChapterInsertedCount()		{ ChapterCount++; }
	void	IncSpreadInsertedCount()		{ SpreadCount++; }
	void	IncLayerInsertedCount()			{ LayerCount++; }
	void	IncSetSentinelInsertedCount()	{ SetSentinelCount++; }

	//Graham 22/5/97	Function to export HTML to the clipboard
public:
	virtual void		ExportHTMLTag(PathName* ppthToUse);

	virtual void		SetInsertMode(InsertMode newInsertMode, CCRuntimeClass* pAttrGroup = NULL);
	virtual InsertMode	GetInsertMode() {return m_InsertMode;}

protected:
	virtual BOOL	AttachNode(Node* pNewNode, Node* pContextNode, AttachNodeDirection Direction);

private:
	// Private vars used for building the document structure when loading
	Document*	pLastDocument;
	Chapter*	pLastChapter;
	Spread*		pLastSpread;
	Layer*		pLastLayer;
	NodePath*	pLastNodePath;
	NodeAttribute* pLastCurrentAttr;

	UINT32		DocumentCount;
	UINT32		ChapterCount;
	UINT32		SpreadCount;
	UINT32		LayerCount;
	UINT32		SetSentinelCount;

	Layer*		pImportLayer;
	BOOL		DeleteNode(Node* pNode);

	UINT32		PreCompFlags;

	InsertMode	m_InsertMode;
	CCRuntimeClass* m_pCurrentAttrGroup;

protected:
	// Progress bar variables
	UINT32		ProgressBarCount;			// Current progress bar count
	Progress*	pProgress;					// ptr to the progress bar object
	String_64	ProgressBarMessage;			// Progress bar message
	UINT32		TotalProgressBarCount;		// Total progress bar count

private:
	DocCoord	CoordOrigin;				// The origin of all spread coords

	Layer *		m_pFirstImportedLayer;		// The first imported layer
	Layer *		m_pActiveLayerBeforeImport;	// The active layer before import started

public:
	// WEBSTER - markn 15/2/97
	// Bug fix for selection type save option
	virtual SelectionType	GetSelType()							{ return SelType; }
	virtual void			SetSelType(SelectionType ThisSelType)	{ SelType = ThisSelType; }

	// Access functions to the first layer to be imported in the document 
	virtual Layer * GetFirstImportedLayer() { return m_pFirstImportedLayer; }
	virtual void SetFirstImportedLayer(Layer * pLayer) { m_pFirstImportedLayer = pLayer; }

private:
	// WEBSTER - markn 15/2/97
	// Bug fix for selection type save option
	SelectionType	SelType;
	
public:
	// String to hold the default path for the optional web export path
	static String_256 DefaultExportPath;
	// whether the user has requested to export a web file when native saving
	static BOOL ExportWebFile;

protected:
	// whether the user has requested a preview bitmap
	static BOOL PreviewBitmapExport;
	// How much compression the user has requested when saving bitmaps
	static INT32 BitmapCompression;
	// compression preference flag
	static BOOL CompressNative;
	// save xpe bitmaps or just xpe info flag
	static BOOL SaveXPEBitmaps;
	// whether the user has requested a convert text to outlines
	static BOOL ConvertTextToOutlines;
	// what type of preview bitmap filter we should be using
	static INT32 PreviewBitmapFilterType;
	// whether invisible layers should be removed or not
	static BOOL RemoveInvisibleLayers;
	// whether unused colours should be removed or not
	static BOOL RemoveUnusedColours;
	// whether to export an HTML tag to the clipboard or not
	static BOOL HTMLToClipboard;
	// whether we export a web file in the minimal format or not
	static BOOL MinimalWebFormat;
	// Save paths in a relative format?
	static BOOL WriteRelativePaths;
	// List of fonts that never need converting to outlines when exporting to web format
	static String_256 DontConvertTheseFontsToOutlines;

	// Switches the path similarity checks on or off when saving native files.
	static BOOL NativeCheckSimilarPaths;
	// The tolerence that is used when checking for similar paths in web files. It is measured in millipoints.
	static INT32 WebSimilarPathTolerence;

	// WEBSTER - markn 28/1/97
	// whether the user has requested a convert blends to outlines
	static BOOL ConvertBlendsToOutlines;
	
	// Webster - Neville 29/7/97
	// Whether non-frame layers in an imported file are combined onto the active layer or the
	// first non-frame layer in the import
	static BOOL ImportNonFramesOntoActivelayer;

public:
	// Control over whether we compress native files or not. 
	static BOOL SetNativeCompression(BOOL NewState);
	static BOOL GetNativeCompression();
	static BOOL ShouldSaveXPEBitmaps();

	// Functions to turn Compression on or off on the underlying CCFile
	// Reading
	virtual BOOL SetCompression(BOOL NewState);
	// Writing
	virtual BOOL StartCompression();
	virtual BOOL StopCompression();

	virtual BOOL  SetPreCompression(UINT32 Flags);
	virtual UINT32 GetPreCompression()				{ return PreCompFlags; }

	// This allows people access to the CCFile which is being used
	// Note: only people who have a genuine reason need to access this - e.g. bitmap savers
	virtual CCLexFile* GetCCFile();

	// Find out how much compression is required for bitmaps
	virtual INT32 GetBitmapCompression(); 
	virtual INT32 SetBitmapCompression(INT32 NewBmpComp);

	// Find out whether the user has requested a preview bitmap
	virtual BOOL GetPreviewBitmapExport(); 
	virtual BOOL SetPreviewBitmapExport(BOOL NewExportPreviewBmp);

	// Find out whether the user has requested convert text to outlines
	virtual BOOL GetConvertTextToOutlines(); 
	virtual BOOL SetConvertTextToOutlines(BOOL NewConvertTextToOutlines);
	
	// WEBSTER - markn 28/1/97
	// Find out whether the user has requested convert blends to outlines
	virtual BOOL GetConvertBlendsToOutlines(); 
	virtual BOOL SetConvertBlendsToOutlines(BOOL NewConvertBlendsToOutlines);
	
	// Find out whether the user has requested remove invisible layers
	virtual BOOL GetRemoveInvisibleLayers(); 
	virtual BOOL SetRemoveInvisibleLayers(BOOL NewRemoveInvisibleLayers);

	// Find out whether the user has requested to remove unused colours
	virtual BOOL GetRemoveUnusedColours(); 
	virtual BOOL SetRemoveUnusedColours(BOOL NewRemoveUnusedColours);

	// Find out whether the user has requested to remove unused colours
	virtual BOOL GetMinimalWebFormat(); 
	virtual BOOL SetMinimalWebFormat(BOOL NewMinimalWebFormat);

	// Find out whether to save XPE bitmaps
	virtual BOOL GetSaveXPEBitmaps(); 
	virtual BOOL SetSaveXPEBitmaps(BOOL NewSaveXPEBitmaps);

	// Find out whether the user wants an HTML tag put on the clipboard
	virtual BOOL ShouldExportHTMLTag(); 
	virtual BOOL SetHTMLToClipboard(BOOL NewHTMLToClipboard);


	// Returns the string that lists the fonts you don't need to convert to text
	virtual String_256 GetDontConvertTheseFontsToOutlinesString();
	
	// Returns whether the path similarity checks are on or off when saving native files.
	virtual BOOL GetNativeCheckSimilarPaths();
	// Returns the tolerence that is used when checking for similar paths in web files.
	// It is measured in millipoints.
	virtual MILLIPOINT GetWebSimilarPathTolerence();

	// This function allows filters derived from the native format filter to 
	// output regular shapes as the specific forms like the Web filter does
	virtual BOOL WriteSpecificRegularShapes() { return(FALSE); }

	// This function allows derived filters to cause objects to output
	// bounding box records
	virtual BoundsWriteLevel GetBoundsWriteLevel() { return(BWL_NONE); }

	// Karim 08/02/2001	Required for saving/loading of shadows.
private:
	BOOL PreExportShadows(Node* pRoot);
	BOOL PostExportShadows(Node* pRoot);
	BOOL PostImportShadows();
};

#endif  // INC_CAMFILTER
