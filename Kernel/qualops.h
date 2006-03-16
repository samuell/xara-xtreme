// $Id: qualops.h 662 2006-03-14 21:31:49Z alex $
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

// header for Quality operations

#ifndef INC_QUALOPS
#define	INC_QUALOPS

#include "ops.h"
#include "opdesc.h"



#define OPTOKEN_QUALITYSLIDER _T("QualitySlider")


// these are the constants used for setting the quality level. These are intimitely
// linked to  quality.cpp, of which OpQuality is a friend.

#define	QUALITY_LEVEL_ANTI		110
#define	QUALITY_LEVEL_NORMAL	100
#define	QUALITY_LEVEL_SIMPLE	50
#define	QUALITY_LEVEL_OUTLINE	10


class QualitySliderDescriptor : public OpDescriptor
{
	CC_DECLARE_DYNAMIC(QualitySliderDescriptor)

public:
	QualitySliderDescriptor(
						 	UINT32 toolID,                    // Tool (Module) Identifier
						 	UINT32 txID,                      // String Resource ID
						 	CCRuntimeClass* Op,				// pointer to the Op's runtime class object
						 	TCHAR* tok,						// pointer to the token string
						 	pfnGetState gs,					// pointer to the GetState function
						 	UINT32 helpId = 0,				// help identifier 
						 	UINT32 bubbleID = 0,				// string resource for bubble help
						 	UINT32 resourceID = 0,			// resource ID
						 	UINT32 controlID = 0,				// control ID within resource
						 	BOOL ReceiveMessages = TRUE, 		
						 	BOOL Smart = FALSE, 
						 	BOOL Clean = FALSE 
						   );  
	 

	// Virtual fuunctions called from the default Message handler
	virtual void OnControlCreate(OpDescControlCreateMsg* CreateMsg);
	virtual void OnSliderSet(OpDescControlMsg* SelChangedMsg);
	virtual BOOL OnDocViewMsg(DocView* pDocView, DocViewMsg::DocViewState State);
	
	// Function to help handle the view quality changing
	void OnQualityChanged(DocView* pView);

	// GetState function for the OpDescriptor
	static OpState GetState(String_256* Description, OpDescriptor*);

	// Function to update all the slider controls if the view quality changes
	static void Update();

public:
	INT32 CurrentSliderPos;			// The current position of the slider
};


/********************************************************************************************

>	class OpQuality : public Operation

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/94
	Purpose:	Operation to set quality level on a DocView
	Notes:		Will be on a dialog somewhere eventually

********************************************************************************************/				

class OpQuality : public Operation
{
	CC_DECLARE_DYNCREATE( OpQuality )

public:

	OpQuality();

	void Do(OpDescriptor*);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);	

private:

	static INT32 ConvertToValue( OpDescriptor* );

};


#endif
