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

#ifndef INC_NODETXTS
#define INC_NODETXTS

#include "node.h"
#include "nodepath.h"
#include "matrix.h"

class CaretNode; 
class VisibleTextNode;
class TextLine;
class TextStory;
class BaseCamelotFilter;
class ImportedStringList;
class ImportedString;
class DocColour;
class ExtendParams;
class FormatRegion;

/********************************************************************************************
>	class BaseTextClass: public BaseTextClass

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	The root of the TextObject hierarchy
********************************************************************************************/

class BaseTextClass: public NodeRenderableInk
{
	CC_DECLARE_DYNAMIC(BaseTextClass)
	
public:
	BaseTextClass(); 	
	~BaseTextClass();
	BaseTextClass(Node* ContextNode, AttachNodeDirection Direction);
	void Init();

	void CopyNodeContents(BaseTextClass* NodeCopy);
	virtual Node* SimpleCopy() { ERROR2(NULL,"BaseTextClass::SimpleCopy() - this is an abstract class!!!!"); }
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	virtual BOOL IsABaseTextClass() const		{ return TRUE; }
	virtual BOOL IsSetCandidate() const			{ return FALSE; }
	virtual BOOL IsCompound() const				{ return TRUE; }		// See also NodeCompound

	// All text objects need to be associated with the BaseTextClass current attribute group
	virtual CCRuntimeClass* GetCurrentAttribGroup(); 

	virtual BOOL AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState=TRUE,
												 BOOL DoPreTriggerEdit = TRUE);
	BOOL PreOpProcessing(ObjChangeParam* pParam);

	TextStory* FindParentStory();

	// abstraction of undo/non-undoable ops
	BOOL DoHideNode(UndoableOperation* pUndoOp);
	BOOL DoInsertNewNode(UndoableOperation* pUndoOp, Node* pContextNode, AttachNodeDirection Direction);
	BOOL DoFactorOutCommonChildAttributes(UndoableOperation* pUndoOp, BOOL Global, AttrTypeSet* pAffectedAttrTypes);
	BOOL DoLocaliseCommonAttributes(UndoableOperation* pUndoOp, BOOL CheckForDuplicates, 
									BOOL Global, AttrTypeSet* pAffectedAttrTypes);
	void GetAttachNodeAndDirectionToAttachFirstChildObject(Node** ppNode, AttachNodeDirection* pDir);
	BOOL AddNonLineLevelDescendantAttrsToSet(AttrTypeSet* pAttrTypeSet);

	void UnionNodeAndDescendantsOldAndNewBounds(DocRect* pBounds, BOOL ParentModifiedByOp=FALSE);
	BOOL ReCacheNodeAndDescendantsMetrics(FormatRegion* pFormatRegion);
	virtual BOOL ReCacheMetrics(FormatRegion* pFormatRegion);

	void FlagNodeAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp();
	void FlagNodeAndDescendantsAffectedAndParentsHaveDescendantAffected();
	void FlagPrevTextCharAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp(); // Jonathan 17/10/2000
	void ClearNodeAndDescendantsFlags();

	inline BOOL ModifiedByOp()                 { return mModifiedByOpFlag; }
	inline BOOL DescendantModifiedByOp()       { return mDescendantModifiedByOpFlag; }
	inline BOOL NodeOrDescendantModifiedByOp() { return mModifiedByOpFlag || mDescendantModifiedByOpFlag; }
	inline BOOL Affected()                     { return mAffectedFlag; }
	inline BOOL DescendantAffected()           { return mDescendantAffectedFlag; }
	inline BOOL NodeOrDescendantAffected()     { return mAffectedFlag || mDescendantAffectedFlag; }
	inline BOOL AlreadyWritten()			   { return mAlreadyWritten; }

	inline void SetAlreadyWritten(BOOL f) { mAlreadyWritten = f; }

private:
	void FlagParentsHaveDescendantModifiedByOp();
	void FlagDescendantsModifiedByOp();
	void FlagParentsHaveDescendantAffected();
	void FlagDescendantsAffected();

	inline void FlagModifiedByOp()           { mModifiedByOpFlag           = TRUE; mAffectedFlag           = TRUE; }
	inline void FlagDescendantModifiedByOp() { mDescendantModifiedByOpFlag = TRUE; mDescendantAffectedFlag = TRUE; }
	inline void FlagAffected()               { mAffectedFlag               = TRUE; }
	inline void FlagDescendantAffected()     { mDescendantAffectedFlag     = TRUE; }
	inline void ClearFlags();

private:
	// the following flags are set by AllowOp/OnChildChange, and cleared at end of UpdateChangedNodes
	BYTE mModifiedByOpFlag           : 1;	// node modified directly by op
	BYTE mDescendantModifiedByOpFlag : 1;	// descendant of node directly modified by an op
	BYTE mAffectedFlag               : 1;	// node affect by op, wordwrap or format
	BYTE mDescendantAffectedFlag     : 1;	// descendant of node affected
	BYTE mAlreadyWritten			 : 1;	// node has already been written to the file (see TextChar and v2 file format)
};


/********************************************************************************************
>	enum BaseShiftEnum

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/95
	Purpose:	enum to represent alignment of baseline relative to the path text is on
				when imported from Corel
********************************************************************************************/

enum BaseShiftEnum { AlignBaseline, AlignAscenders, AlignDescenders, AlignCentres };


// These will eventually become user changable - but for the mo they are here
const double Text_SuperScriptSize 	=  0.5;
const double Text_SuperScriptOffset =  0.33;
const double Text_SubScriptSize     =  0.5;
const double Text_SubScriptOffset   = -0.1;


/********************************************************************************************
>	class TextStoryInfo: public CC_CLASS_MEMDUMP

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/96
	Purpose:	Hold story level information
********************************************************************************************/

class TextStoryInfo : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(TextStoryInfo);

public:
	TextStoryInfo();

	UndoableOperation* pUndoOp;
	BOOL               WordWrap;		// indicated if wrapping shoulf be undertaken
										// eg FALSE for undo/redo and stories on clipboard
	MILLIPOINT         StoryWidth;
	BOOL               WordWrapping;	// indicates story is word wrapping
	Path*              pPath;
	MILLIPOINT         PathLength;
	MILLIPOINT         PathClosed;
	double             UnitDirectionVectorX;
	double             UnitDirectionVectorY;
	MILLIPOINT         LeftPathIndent;
	MILLIPOINT         RightPathIndent;
	MILLIPOINT         DescentLine;
	BOOL               DescentLineValid;
};


/********************************************************************************************
>	class TextStory: public BaseTextClass

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	The root of the TextObject hierarchy
********************************************************************************************/

class TextStory: public BaseTextClass
{
	CC_DECLARE_DYNAMIC(TextStory)
	
public:
	static TextStory* CreateTextObject(DocCoord Anchor);
	static TextStory* CreateTextObject(Matrix TheMatrix);
	static TextStory* CreateFromChars(DocCoord Pos, char* pChars, WCHAR* pWChars, Document* pCreateDoc,
					LOGFONT* pLogFont=NULL, BOOL ControlCodes=FALSE, DocColour* pColour=NULL);
	TextStory(); 	
	~TextStory();
	TextStory(Node* ContextNode, AttachNodeDirection Direction);
	void Init();

	String_256 GetStoryAsString();

	virtual BOOL IsSetCandidate() const		{ return TRUE; }

	virtual Node* SimpleCopy();
	void CopyNodeContents(TextStory* NodeCopy);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);
	INT32 BaseComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput);
	INT32 BaseComplexHide(UndoableOperation *pOp);
	virtual BOOL HidingNode();

	virtual BOOL CanBecomeA(BecomeA* pBecomeA);
	virtual BOOL DoBecomeA(BecomeA* pBecomeA); 

	virtual String Describe(BOOL Plural, BOOL Verbose);  		   
	virtual UINT32 GetNodeSize() const;
	virtual void GetDebugDetails(StringBase* Str);

	virtual BOOL OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu);

	virtual DocRect GetBlobBoundingRect(); 
	virtual void RenderObjectBlobs(RenderRegion* pRender);
	virtual void RenderTinyBlobs(RenderRegion* pRender);
	DocCoord GetBlobPosAndSize(INT32* pSize=NULL);

	// Clever bits to allow the TextStory to reformat itself when an operation effects its children

	virtual BOOL AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState = TRUE,
												 BOOL DoPreTriggerEdit = TRUE);
	virtual BOOL AllowOp_AccountForCompound(ObjChangeParam* pParam,
											BOOL SetOpPermissionState=TRUE,
											BOOL DoPreTriggerEdit = TRUE);
	virtual ChangeCode OnChildChange(ObjChangeParam* pParam);

	BOOL MoveCaretLeftAChar(); 
	BOOL MoveCaretRightAChar(); 
	BOOL MoveCaretLeftAWord(); 
	BOOL MoveCaretRightAWord(); 
	BOOL MoveCaretToStartOfLine(); 
	BOOL MoveCaretToEndOfLine();
	BOOL MoveCaretToCharacter(VisibleTextNode* pChar, AttachNodeDirection Dir);
	BOOL AttachCaretAttributes();
	VisibleTextNode* GetPrevWordChar(VisibleTextNode* pStartChar);
	VisibleTextNode* GetNextWordChar(VisibleTextNode* pStartChar);

	VisibleTextNode* GetSelectionEnd(BOOL* pDirection = NULL);
	BOOL GetCharacterAtPoint(BOOL InBounds, DocCoord Point, VisibleTextNode** pHitChar, BOOL* ToLeft);
	
	static TextStory* GetFocusStory();
	static void SetFocusStory(TextStory* pNewStory);
	CaretNode* GetCaret();

	NodePath*        GetTextPath() const;
	TextLine*        FindFirstLine() const;
	TextLine*        FindLastLine() const;
	VisibleTextNode* FindFirstVTN() const;
	VisibleTextNode* FindLastVTN() const;

	virtual void Transform(TransformBase& transform);

	DocRect GetUTStoryBounds();
	BOOL CreateUntransformedPath(TextStoryInfo* pPathInfo);
	void MatrixFitToPath();
	void MatrixRemoveFromPath();

	virtual BOOL FormatAndChildren(UndoableOperation* pUndoOp=NULL,
									BOOL UseNodeFlags=FALSE, BOOL WordWrap=TRUE);
	BOOL         UnWrapStory(UndoableOperation* pUndoOp);

	BOOL DeleteSelectedText(UndoableOperation* pUndoOp);
	BOOL DeleteSelectedTextLines(UndoableOperation* pUndoOp);
	BOOL DeleteSelectedTextCharacters(UndoableOperation* pUndoOp);
	
	void PreExportRender(RenderRegion* pRegion);
 	BOOL ExportRender(RenderRegion* pRegion);
	BOOL PostImport();
	BOOL PostDuplicate(UndoableOperation* pOp);
	BOOL ImportFix_MoveScaleToAttrs();
	BOOL CorelStyleBaselineShift(BaseShiftEnum BaseShift);

	// Version 2 file format functions
	// Export
	BOOL OKToExport();
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL CanWriteChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL CanWriteChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WriteEndChildRecordsNative(BaseCamelotFilter* pFilter);
	// Import
	void ResetStringList();										// Deletes the imported string list
	void AddImportedString(ImportedString* pImportedString);	// Adds the string to the list
	BOOL EnsureStoryHasCaret();									// Used at post import time.

	// debug validation code for story - not very oops but all in one place
	void Validate(BOOL EnsureOpFlagsReset=TRUE);
	void CheckLeaf(Node* pLeafNode);
	void CheckSubtree(Node* pNode, AttrTypeSet* pParentAttrSet=NULL, BOOL* pDescendantSelected=NULL);
	TCHAR* GetNodeInfo(Node* pNode);

	void ResetEOLs(BOOL Status);
	BOOL WillStoryWrapOnPath();

protected:
	BOOL Copy(Range& RangeToCopy, Node** pOutput);
	void SetStoryMarkers(Range& RangeToCopy);
	static BOOL DeleteChildAttribute(NodeRenderableInk* pParent, CCRuntimeClass* ReqdAttrib);	// Helper for CreateFromChars

	BOOL ExpandImportedStrings();	// Expands the imported strings into a series of TextChars
	
	// (ChrisG 8/11/00 Added so that Text with gradient fills can be identified.
	BOOL IsGradientFilled ();

public:
	const Matrix* GetpStoryMatrix() const	{ return &StoryMatrix; }
	Matrix* GetpStoryMatrix()				{ return &StoryMatrix; }
	Matrix  GetStoryMatrix() const			{ return  StoryMatrix; }
	void    SetStoryMatrix(Matrix& matrix)	{ StoryMatrix=matrix;  }

	DocRect GetRedrawRect() const			{ return RedrawRect; }
	void    SetRedrawRect(DocRect rect)		{ RedrawRect=rect; }
	void    UpdateRedrawRect(DocRect rect)	{ RedrawRect=RedrawRect.Union(rect); }

	MILLIPOINT GetStoryWidth() const		{ return StoryWidth; }
	void SetStoryWidth(MILLIPOINT NewValue)	{ StoryWidth      = NewValue; }

	MILLIPOINT GetImportFormatWidth() const		{ return ImportFormatWidth; }
	BaseShiftEnum GetImportBaseShift() const	{ return ImportBaseShift; }
	void SetImportFormatWidth(MILLIPOINT width)	{ ImportFormatWidth=width;  }
	void SetImportBaseShift(BaseShiftEnum shift) { ImportBaseShift=shift;  }

	BOOL IsTextOnPathReversed() const   { return TextOnPathReversed; }
	BOOL IsTextOnPathTangential() const { return TextOnPathTangential; }
	BOOL IsPrintingAsShapes() const     { return PrintAsShapes; }
	BOOL IsWordWrapping() const         { return WordWrapping; }
	void ReverseTextOnPath()	             { TextOnPathReversed   = !TextOnPathReversed; }
	void ToggleTextOnPathTangentialness()    { TextOnPathTangential = !TextOnPathTangential; }
	void SetPrintingAsShapes(BOOL NewValues) { PrintAsShapes        =  NewValues; }
	void SetWordWrapping(    BOOL NewValues) { WordWrapping         =  NewValues; }

	FIXED16 GetCharsScale() const		{ return CharsScale; }
	FIXED16 GetCharsAspect() const		{ return CharsAspect; }
	ANGLE   GetCharsRotation() const	{ return CharsRotation; }
	ANGLE   GetCharsShear() const		{ return CharsShear; }
	void SetCharsScale(FIXED16 scale)     { CharsScale    = scale; }
	void SetCharsAspect(FIXED16 aspect)   { CharsAspect   = aspect; }
	void SetCharsRotation(ANGLE rotation) { CharsRotation = rotation; }
	void SetCharsShear(ANGLE shear)       { CharsShear    = shear; }

	// Jonathan 24/10/2000
	BOOL IsAutoKerning()				{ return AutoKern; }
	void SetAutoKerning(BOOL NewValue)	{ AutoKern = NewValue; }

	// Functions for the indents along a path
	MILLIPOINT GetLeftIndent()  const	{return mLeftIndent;} 
	MILLIPOINT GetRightIndent() const	{return mRightIndent;} 
	DocCoord GetLeftIndentPos() const;
	DocCoord GetRightIndentPos() const;
	static DocCoord GetPathBlobPos(MILLIPOINT IndentLength, NodePath* pPath);
	void SetLeftIndent(MILLIPOINT NewIndent)		{mLeftIndent = NewIndent;}
	void SetRightIndent(MILLIPOINT NewIndent)		{mRightIndent = NewIndent;}

// Karim MacDonald 1/12/1999
public:
	// Extend functions.
	// Note that if a sub-class returns TRUE from IsTypeExtendible, it must
	// also provide complete implementations of ValidateExtend and Extend,
	// which should usually check/extend its children.
	virtual BOOL IsTypeExtendible() const { return TRUE; }
	virtual DocRect ValidateExtend(const ExtendParams& ExtParams);
	virtual void Extend(const ExtendParams& ExtParams);

	// Find the extend-centre of this text-story, for use with Extension.
	DocCoord FindExtendCentre();

	// Determine the justification of this text story, for use with Extension.
	Justification FindJustification();

private:
	static TextStory* pFocusStory;	// Pointer to the TextStory with the input focus

	Matrix     StoryMatrix;			// transform matrix for whole text story
	DocRect    RedrawRect;
	CaretNode* CachedCaret;			// A pointer to the caret
	MILLIPOINT mLeftIndent;			// left indent along path (from LHS)
	MILLIPOINT mRightIndent;		// right indent along path (from RHS)
	MILLIPOINT StoryWidth;			// width of story WHEN NOT ON A PATH (0 => text at point)

	BOOL TextOnPathReversed   : 1;	// text is reversed on the path
	BOOL TextOnPathTangential : 1;	// text on path tangential to path (as opposed to horizontal)
	BOOL PrintAsShapes        : 1;	// print the story as shapes
	BOOL WordWrapping         : 1;	// the story is word wrapping
	BOOL BeingCopied          : 1;	// Are we currently in a complex copy?

	MILLIPOINT    ImportFormatWidth;// format width           for 1 off format on import (for CDR)
	BaseShiftEnum ImportBaseShift;	// type of baseline shift for 1 off change on import (for CDR)

	FIXED16 CharsScale;				// transforms applied to chars before fitted to a path
	FIXED16 CharsAspect;
	ANGLE   CharsRotation;
	ANGLE   CharsShear;

	ImportedStringList*	pImportedStringList;	// Used when importing strings in the v2 file format

	// Jonathan Payne 24/10/2000
	bool	AutoKern;				// Story has autokerning
};
#endif
