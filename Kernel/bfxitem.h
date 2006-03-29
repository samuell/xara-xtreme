// $Id$
// PlugInItem.h : header file
//
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

#ifndef INC_BFXPLUGINITEM
#define INC_BFXPLUGINITEM

#include "plugin.h"
#include "bfxmngr.h"
//#include "bfxrc.h"

class Progress;
class Document;
class OpDescriptor;

/**************************************************************************************

>	class BfxPlugIn : public PlugInItem

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Purpose:	Provides storage for details on a BFX plug-in. 

**************************************************************************************/

class BfxPlugIn : public PlugInItem
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNAMIC(BfxPlugIn);

public:
	// Construction and destruction
	BfxPlugIn();
	~BfxPlugIn();

	// return a string describing a unique identifier for the plug-ins this handler is interested in
	virtual String_32 GetUniqueID() const; 
	// return a unique type for this kind of plug-in
	virtual TypeOfPlugIn GetTypeOfPlugIn() const;

	// Special BFX get dialog id function
	// Baseclass version is pure vitual to force defined classes to use it
	//virtual UINT32 GetDialogID()					= 0;
	virtual OpDescriptor * GetOpDescriptor()	= 0;

protected:

public:
	// Operations available
	// Find out about a plug-in
//	virtual BOOL About();

	// Applying the plug-in to a bitmap
	virtual BOOL Apply(KernelBitmap *pInputBitmap, Document* pDocument,
					   KernelBitmap **ppOutputBitmap = NULL);
//	virtual BOOL ApplyLastFilter(KernelBitmap *pInputBitmap, Document* pDocument,
//								 KernelBitmap **ppOutputBitmap = NULL);

	// Checking if a bitmap's colour depth is suitable
	virtual BOOL IsBitmapModeOk(INT32 ColourDepth, BOOL GreyScale);

protected:
	// Useful helper functions

	// If ApplyLast is True		- To call it to do its work on a bitmap
	// or if ApplyLast is True	- To call it to repeat its work on a bitmap
	virtual BOOL ApplyBitmapEffect(KernelBitmap *pInputBitmap, Document* pDocument, KernelBitmap **ppOutputBitmap = NULL);
//	virtual BOOL Apply(WinBitmap *pInputDIB, WinBitmap * pOutputDIB);
	//virtual BOOL ApplyPlugIn(BOOL ApplyLastFilter);
};

/**************************************************************************************

>	class BfxBrightnessContrast : public BfxPlugIn

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/97
	Purpose:	Provides storage for details on a BfxBrightnessContrast plug-in. 

**************************************************************************************/

class BfxBrightnessContrast : public BfxPlugIn
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNAMIC(BfxBrightnessContrast);

public:
	// Construction and destruction
	BfxBrightnessContrast();

//	virtual UINT32 GetDialogID() { return _R(IDD_BFXPLUGINDLG_PG3); }
	virtual OpDescriptor * GetOpDescriptor();
};

/**************************************************************************************

>	class BfxSpecialEffects : public BfxPlugIn

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/97
	Purpose:	Provides storage for details on a BfxSpecialEffects plug-in. 

**************************************************************************************/

class BfxSpecialEffects : public BfxPlugIn
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNAMIC(BfxSpecialEffects);

public:
	// Construction and destruction
	BfxSpecialEffects();

//	virtual UINT32 GetDialogID() { return _R(IDD_BFXPLUGINDLG_PG5); }
	virtual OpDescriptor * GetOpDescriptor();
};

/**************************************************************************************

>	class BfxResize : public BfxPlugIn

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/97
	Purpose:	Provides storage for details on a BfxResize plug-in. 

**************************************************************************************/

class BfxResize : public BfxPlugIn
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNAMIC(BfxResize);

public:
	// Construction and destruction
	BfxResize();

//	virtual UINT32 GetDialogID() { return _R(IDD_BFXPLUGINDLG_PG3); }
	virtual OpDescriptor * GetOpDescriptor();
};

/**************************************************************************************

>	class BfxFlipAndRotate : public BfxPlugIn

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/97
	Purpose:	Provides storage for details on a BfxFlipAndRotate plug-in. 

**************************************************************************************/

class BfxFlipAndRotate : public BfxPlugIn
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNAMIC(BfxFlipAndRotate);

public:
	// Construction and destruction
	BfxFlipAndRotate();

//	virtual UINT32 GetDialogID() { return _R(IDD_BFXPLUGINDLG_PG3); }
	virtual OpDescriptor * GetOpDescriptor();
};

/**************************************************************************************

>	class BfxColourDepth : public BfxPlugIn

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/97
	Purpose:	Provides storage for details on a BfxColourDepth plug-in. 

**************************************************************************************/

class BfxColourDepth : public BfxPlugIn
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNAMIC(BfxColourDepth);

public:
	// Construction and destruction
	BfxColourDepth();

//	virtual UINT32 GetDialogID() { return _R(IDD_BFXPLUGINDLG_PG3); }
	virtual OpDescriptor * GetOpDescriptor();
};


#endif // INC_BFXPLUGINITEM
