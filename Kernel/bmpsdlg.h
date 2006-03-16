// $Id: bmpsdlg.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_BMPSDLG
#define INC_BMPSDLG

#include "dialogop.h"
#include "bitmap.h"

PORTNOTE("gif","Removed gifutil usage")
#ifndef EXCLUDE_FROM_XARALX
#include "gifutil.h"
#endif

typedef INT32 GIFDisposalMethod;
const INT32	GDM_LEAVE = 0;

class BmpDlgParam;

/*******************************************************************************************************

  class BmpDlg : publc DialogOp

  Author:	Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
  Created:	22/11/96
  Purpose:	Provides a Bitmap Properties Dialog for Webster.
				-Provides support for the entry of GIF animation delays\Restore on a per-frame basis.
				
********************************************************************************************************/


#define OPTOKEN_BMP_DLG _T("BmpDlg")

class BmpDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE (BmpDlg)

public: 

	BmpDlg();												// constructor					
	static BOOL Init();										// Standard Setup Funcrion		
	BOOL InitDialog();
	virtual ~BmpDlg();										// Virtual destructor				
	static OpState	GetState(String_256*, OpDescriptor*);
	void Do(OpDescriptor*);															
	BOOL  Create();											// Obtains resources to open dialog	

	static const UINT32 IDD;									// Dialog box id
	static const CDlgMode Mode;								

	void UpdateInterpolationCheckbox();						// 'interpolate when zoomed in' checkbox handler
	virtual MsgResult Message(Msg* Messgse);				// Message Handler		
	void DoWithParam(OpDescriptor*, OpParam* Param);
	static	void InvokeBmpDlg(BmpDlgParam* pBmpDlgParam);
	BmpDlgParam* CommitDialogValue();						//Called after OK is pressed, sets control values.	

	static	BmpDlgParam*  pParam;							// This will allow us to store our value for AnimDelay.

	void SetIsManyDisplayed(BOOL Value)		{ m_IsManyDisplayed = Value; }
	BOOL GetIsManyDisplayed() const			{ return m_IsManyDisplayed;  }

	void SetDelayChanged(BOOL value)		{ m_HasDelayChanged = value; }
	BOOL GetDelayChanged() const			{ return m_HasDelayChanged;  }
	
	void SetListDropped(BOOL value)			{ m_ListDropped = value; }
	BOOL GetListDropped() const				{ return m_ListDropped;  }

	void SetInterpolationChanged(BOOL value){ m_bInterpolationChanged = value; }
	BOOL GetInterpolationChanged() const	{ return m_bInterpolationChanged;  }

protected:
	BOOL m_HasDelayChanged;
	BOOL m_IsManyDisplayed;
	BOOL m_ListDropped;
	BOOL m_bInterpolationChanged;
	
};

/***************************************************************************************************************

	class BmpDlgParam : public OpParam

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	28/11/96
	Purpose:	This will allow data (for delay\Restore between frames in animation)
				to be passed to and from the Bitmap Properties Dialog. 
	
*****************************************************************************************************************/


class BmpDlgParam : public OpParam
{

	CC_DECLARE_DYNAMIC(BmpDlgParam)

public:
    BmpDlgParam	()		{
							m_Many = FALSE;
							m_AnimDelay = 10; 
							m_bInterpolation = TRUE;
							m_SameColors = TRUE;
							m_SameDimensions = TRUE;
							m_MemoryUsed = "";
							m_BitmapWidth = "";
							m_BitmapHeight = "";
							m_BitmapCols = "";
							m_BitmapName = "";
							m_BitmapFormat = "";
							m_AreDelayValuesSame = TRUE;
							m_HasDelayValueChanged = FALSE;
							m_AreInterpolationValuesSame = TRUE;
							m_HasInterpolationValueChanged = FALSE;
							m_IsJPEG = FALSE;
							m_SameFormat = TRUE;
							m_SameRestoreTypes = TRUE;
							m_RestoreType = GDM_LEAVE;
							m_ManySelected = FALSE;
							m_BitmapTranspType = "";
							m_SameTranspType = TRUE;
						};

void SetSameColors (BOOL Value)					{ m_SameColors = Value; }
BOOL GetSameColors() const						{ return m_SameColors;  }

void SetSameDimensions (BOOL Value)				{ m_SameDimensions = Value;  }
BOOL GetSameDimensions () const					{ return m_SameDimensions;   }

BOOL GetAreDelayValuesSame() const				{ return m_AreDelayValuesSame;  } 
void SetAreDelayValuesSame(BOOL Value)			{ m_AreDelayValuesSame = Value; }

BOOL GetHasDelayValueChanged ()	const			{ return m_HasDelayValueChanged;   }
void SetHasDelayValueChanged (BOOL Value)		{ m_HasDelayValueChanged = Value;  }

BOOL GetAreInterpolationValuesSame() const		{ return m_AreInterpolationValuesSame;  } 
void SetAreInterpolationValuesSame(BOOL Value)	{ m_AreInterpolationValuesSame = Value; }

BOOL GetHasInterpolationValueChanged ()	const	{ return m_HasInterpolationValueChanged;   }
void SetHasInterpolationValueChanged(BOOL Value){ m_HasInterpolationValueChanged = Value;  }

void SetMany (BOOL Value)						{ m_Many = Value; }			
BOOL GetMany () const							{ return m_Many;  }

void SetIsJPEG(BOOL Value)						{ m_IsJPEG = Value;}
BOOL GetIsJPEG() const							{ return m_IsJPEG; }

BOOL GetSameFormat() const						{ return m_SameFormat;  }
void SetSameFormat(BOOL Value)					{ m_SameFormat = Value; }

void SetSameRestoreType(BOOL Value)				{ m_SameRestoreTypes = Value; }
BOOL GetSameRestoreType()						{ return m_SameRestoreTypes ; }	

void		SetInterpolation(BOOL Value)		{ m_bInterpolation = Value; }
BOOL		GetInterpolation()					{ return m_bInterpolation ; }

UINT32		GetAnimDelay () const				{ return m_AnimDelay; }
void		SetAnimDelay(UINT32 Delay)			{ m_AnimDelay = Delay; }

String_256& GetBitmapName() 					{ return m_BitmapName; }
void		SetBitmapName (String_256& Name)	{ m_BitmapName = Name; }

void SetSameTranspType(BOOL Value)				{ m_SameTranspType = Value; }
BOOL GetSameTranspType() const					{ return m_SameTranspType;  }

String_256	GetBitmapTranspType() 					{ return m_BitmapTranspType; }
void		SetBitmapTranspType(String_256 Name)	{ m_BitmapTranspType = Name; }

String_256	GetBitmapFormat() 					{ return m_BitmapFormat; }
void		SetBitmapFormat (String Format)		{ m_BitmapFormat = Format; }

String_256	GetBitmapCols() 					{ return m_BitmapCols; }
void		SetBitmapCols(String_256 Cols)		{ m_BitmapCols = Cols; }

String_256& GetBitmapWidth()					{ return m_BitmapWidth;  }
void		SetBitmapWidth(String_256& Width)	{ m_BitmapWidth = Width; }

String_256& GetBitmapHeight ()						{ return m_BitmapHeight;  }
void		SetBitmapHeight(String_256& Height)		{m_BitmapHeight = Height; }

String_256& GetMemoryUsed()							{ return m_MemoryUsed; }
void		SetMemoryUSed(String_256& Memory)		{ m_MemoryUsed = Memory; }

BOOL		GetManySelected()	const				{ return m_ManySelected; }
void		SetManySelected(BOOL value)				{ m_ManySelected = value; }

GIFDisposalMethod	GetRestoreType() const							{ return m_RestoreType; }
void				SetRestoreType(GIFDisposalMethod Type)			{ m_RestoreType = Type; }

 KernelBitmap**		GetBitmapList()	const							{ return m_pList; }
 void				SetBitmapList(KernelBitmap** pBitmap)			{ m_pList = pBitmap; }
 
 UINT32				GetListSize() const			{ return m_ListSize; }
 void				SetListSize(UINT32 n)			{ m_ListSize = n;	 }
 void				IncListSize(UINT32 n)			{ m_ListSize += n;	 }	
 void				DecListSize(UINT32 n)			{ m_ListSize -= n;   } 

protected:

	KernelBitmap**	m_pList;				// A list of Selected KernelBitmaps.
	UINT32			m_ListSize;				// The size of the Kernel Bitmap list.
	
	BOOL	   m_bInterpolation;			// 'interpolate when zoomed in' tickbox
	UINT32	   m_AnimDelay;					// Delay between frames for Animations.
	String_256 m_BitmapName;				// Name of the selected bitmap.	
	String_256 m_BitmapFormat;				// Bitmap Format.
	String_256 m_BitmapCols;				// Colours in the bitmap.
	String_256 m_BitmapWidth;				// Dimensions(Width) of the selected bitmap.
	String_256 m_BitmapHeight;				// Dimensions(Height) of the selected bitmap.
	String_256 m_MemoryUsed;				// Size of the selected bitmap.
	String_256 m_BitmapTranspType;			// Transparency type
	GIFDisposalMethod m_RestoreType;
	
	BOOL m_Many;
	BOOL m_SameColors;
	BOOL m_SameDimensions;
	BOOL m_AreDelayValuesSame;
	BOOL m_HasDelayValueChanged;
	BOOL m_AreInterpolationValuesSame;
	BOOL m_HasInterpolationValueChanged;
	BOOL m_IsJPEG;
	BOOL m_SameFormat;
	BOOL m_SameRestoreTypes;
	BOOL m_SameTranspType;
	BOOL m_ManySelected;

};

#endif

