// $Id$
// ai_grad.h: interface for the AIGradientProcessor class.
//
//////////////////////////////////////////////////////////////////////
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

#if !defined(AFX_AI_GRAD_H__CFD00AD1_0547_11D4_836F_006008484641__INCLUDED_)
#define AFX_AI_GRAD_H__CFD00AD1_0547_11D4_836F_006008484641__INCLUDED_

class AI5EPSFilter;
class ColourRamp;


/********************************************************************************************

>	class AIHighlight

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/00
	Purpose:	Helps with the import of Adobe Illustrator 8.0 files.
				Holds the definition of a highlight to be used in subsequent fills.
				(not interpreted at present)

********************************************************************************************/
class AIHighlight
{
public:
	AIHighlight( const INT32 nAngle, const double& dLength, const DocCoord& hilight ) :
		mnAngle(nAngle), mdLength(dLength), mHilight(hilight)
	{}

	INT32		mnAngle;
	double		mdLength;
	DocCoord	mHilight;
};


/********************************************************************************************

>	class AI5Gradient : public ListItem

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/94
	Purpose:	Used to hold the definition of an Illustrator 5 gradient fill, while we
				are importing an AI5 file.
	SeeAlso:	AI5EPSFilter

********************************************************************************************/

class AI5Gradient : public ListItem
{
public:
	String_64 		Name;
	BOOL			IsRadial;
	DocColour		StartColour;
	DocColour		EndColour;
	ColourRamp*		mpCurrentRamp;
	AIHighlight*	mpHighlight;
	INT32			midPoint;

public:
	AI5Gradient();
	virtual ~AI5Gradient();
};


/********************************************************************************************

>	class AIGradientProcessor  

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/00
	Purpose:	Helps with the import of Adobe Illustrator 8.0 files.
				(at the time of writing only the version 7.0 documentation was available
				so some omissions are inevitable).
				It creates a list gradient fills and allows the filter to extract them.

********************************************************************************************/
class AIGradientProcessor  
{
public:
	AIGradientProcessor();
	virtual ~AIGradientProcessor();

	BOOL BeginGradient();
	BOOL DecodeBd( AI5EPSFilter& filter );
	BOOL DecodeBS( AI5EPSFilter& filter );
	BOOL DecodeBh( AI5EPSFilter& filter );
	BOOL EndGradient( AI5EPSFilter& filter );

	AI5Gradient* FindGradient( const StringBase& name );
private:

	BOOL Approx (const double & d1, const double & d2);

	// The list of imported grad fills
	List			mGradFills;

	// Used to indicsate if we found any gradient fills too complex to handle properly.
	BOOL			mbGradFillTooComplex;

	// The grad fill we are currently reading in (if any)
	AI5Gradient*	mpCurrentFill;

	// (ChrisG 3/4/2001) The midpoint for the fill (used to set the bias). Since Xara X
	//	profiling is for the entire fill, not individual points, this will; just be the
	//	first valid value read in.
	INT32			mMidPoint;
	INT32			mLastRampPoint;
};

#endif // !defined(AFX_AI_GRAD_H__CFD00AD1_0547_11D4_836F_006008484641__INCLUDED_)
