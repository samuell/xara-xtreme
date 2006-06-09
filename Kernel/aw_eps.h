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

// Declaration of ArtWorks EPS filter.

#ifndef INC_AW_EPS
#define INC_AW_EPS

//#include "epsfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "saveeps.h"
//#include "epsstack.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixstr64.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "colmodel.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodemold.h"

// A structure to hold details of the last grad fill end colour read.
struct GradEndCol {
	String_64	ColName;
	PColourCMYK	Col;
	TintType	Tint;
	FIXEDPOINT	TintVal;
};

/********************************************************************************************

>	class ArtWorksEPSFilter : public EPSFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Purpose:	A filter that imports EPS files in ArtWorks EPS format.
	SeeAlso:	EPSFilter

********************************************************************************************/

class ArtWorksEPSFilter : public EPSFilter
{
	CC_DECLARE_DYNAMIC(ArtWorksEPSFilter);
	
public:
	ArtWorksEPSFilter();
	BOOL Init();
	BOOL PrepareToExport(CCLexFile*, Spread *pSpread);
	char* GetEPSCommand(EPSCommand Cmd);
	virtual BOOL NeedsPrintComponents ( void );

	virtual BitmapFilterSupport GetBitmapSupportLevel();

protected:
	virtual void LookUpToken();
	virtual BOOL ProcessToken();
	INT32 EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize);

	// The array of keyword names
	static CommandMap ArtWorksCommands[];

	// Blend and blender processing functions
	BOOL ProcessBlend();
	BOOL ProcessBlender();
	BOOL StartBlend();
	BOOL StartBlender();
	BOOL EndBlend();
	BOOL EndBlender();

	// Mould object processing functions
	BOOL ProcessEnvelope();
	BOOL ProcessPerspective();
	BOOL StartMould(MouldSpace mSpace);
	BOOL EndMould();
	BOOL ProcessMould();
	BOOL ProcessMouldShape();
	BOOL ProcessMangledObjs();
	BOOL ConvertArtMould(NodeMould*);
	BOOL ConvertMouldStyles(Node* pNode);
	void DeleteAllMangled(Node* pNode);
	BOOL DeleteAllNoneMangled(Node* pNode);
	BOOL PromoteMangled(Node* pNode);
	BOOL HandleMouldedFill();

private:
	// Mould related locals
	DocRect		MouldRect;			// used for importing mould shapes
	INT32		LastFillType;		// holds the last read grad fill type (lin or cir)
	GradEndCol	LastStartColour;
	GradEndCol	LastEndColour;
};

// All the commands used by ArtWorks EPS files.
enum
{
	EPSC_aoa = EPSC_EOF + 1,
	EPSC_aafs,
	
	// Path related procedures
	EPSC_ar,
	EPSC_arr,
	EPSC_ae,
	EPSC_apl,
	EPSC_apc,
	EPSC_aof,
	
	// Text related procedures
	EPSC_asto,
	EPSC_aeto,
	EPSC_aco,
	EPSC_atc,
	EPSC_atph,
	EPSC_atof,
	
	// Blend related procedures
	EPSC_asbd,
	EPSC_aebd,
	EPSC_asbr,
	EPSC_aebr,
	
	// Mould related procedures
	EPSC_asev,
	EPSC_aeev,
	EPSC_aspr,
	EPSC_aepr,
	EPSC_amm,
	EPSC_aml,
	EPSC_amc,
	EPSC_amcp,
	EPSC_amep,
	
	// Group related procedures
	EPSC_anu,
	
	// Linear/radial fills
	EPSC_az,
	EPSC_ax,
	EPSC_axm,
	
	// Overprint related procedures
	EPSC_axop,
	
	// Others(!)
	EPSC_awr,
	EPSC_asc,
	EPSC_aec,
	EPSC_aca,
	EPSC_asah,
	EPSC_aeah,
	EPSC_asat,
	EPSC_aeat,
	
	// Procedures that define a text object
	EPSC_atp,
	EPSC_atf,
	EPSC_atxy,
	EPSC_atrk,
	EPSC_akrn,
	
	// Layer procedure
	EPSC_alyr,
	
	// Sprite procedure
	EPSC_ass,
	EPSC_aes	
};

/********************************************************************************************

>	class ArtWorksEPSRenderRegion : public EPSRenderRegion

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Purpose:	A render region for exporting ArtWorks EPS format files.
	SeeAlso:	EPSRenderRegion

********************************************************************************************/

class ArtWorksEPSRenderRegion : public EPSRenderRegion
{
	CC_DECLARE_DYNAMIC(ArtWorksEPSRenderRegion)
	
public:
	ArtWorksEPSRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale);
	virtual BOOL StartRender();

	virtual void OutputFillColour();
	virtual void OutputStrokeColour();

protected:
	// This version handles grad fills.
	void GetValidPathAttributes();
	virtual BOOL WriteEPSBoundingBox ( void );
	virtual BOOL WriteEPSTrailerComments ( void );
};

#endif  // INC_AW_EPS


