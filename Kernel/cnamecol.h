// $Id: cnamecol.h 662 2006-03-14 21:31:49Z alex $
// $Header:
// CNamedColourReplacer class header
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

#ifndef INC_NAMCOLREP
#define INC_NAMCOLREP

class CCObject;
class ColourFillAttribute;
class DocColour;
class TranspFillAttribute;

const INT32 MAX_TRANSP_VALUE = 255;

/********************************************************************************************

>	class CNamedColourReplacer : public CCObject

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/2000
	Purpose:	This is a somewhat specialised class whose purpose is to temporarily replace the
				Named colours in a ColourFillAttribute.  Its stores the colours that are replaced
				and can be called upon to put them back at a later time.
				It was designed for use with the Brush attribute, where we wish to be able to 
				replace named colours in the brush objects, and then change them back.  If you
				can think of any other uses for it then feel free.

********************************************************************************************/

class CNamedColourReplacer : public CCObject
{
	CC_DECLARE_DYNCREATE(CNamedColourReplacer)

public:
	CNamedColourReplacer();
	CNamedColourReplacer(ColourFillAttribute*, DocColour*); 
		// if you use this constructor you can skip initialisation

	~CNamedColourReplacer();

public:
	virtual BOOL Initialise(ColourFillAttribute*, DocColour*);
	
	virtual BOOL IsInitialised();

	virtual BOOL ReplaceColours(); // replaces the named colours in the attribute
	virtual BOOL RestoreColours(); // restores the replaced colours

public: // Access fns. in case you change your mind after initialisation
	ColourFillAttribute* GetColourFill();
	BOOL                 SetColourFill(ColourFillAttribute*);

	DocColour*			 GetReplacementColour();
	BOOL				 SetReplacementColour(DocColour*);

protected:  //data
	ColourFillAttribute* m_pColourFill;   
	DocColour*			 m_pReplaceColour;
	DocColour			 m_StartColour;
	DocColour			 m_EndColour1;
	DocColour			 m_EndColour2;
	DocColour			 m_EndColour3;
};

/********************************************************************************************

>	class CHSVChanger : public CNamedColourReplacer

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/2000
	Purpose:	Similar to its parent class but adds the following options:
				- alter HSV values in existing colours, with no replacement colour
				- replace named colours (as parent class) and make HSV changes to all
				colours whether named or not.

	Notes:      This works by taking a multiplier value and calculating a fixed value that
				will be added or subtracted from each colour value.  Values are in the range
				0 - 255, however Hue values can 'wrap - around' due to the colour wheel effect.
********************************************************************************************/

class CHSVChanger : public CNamedColourReplacer
{
	CC_DECLARE_DYNCREATE(CHSVChanger)

public:
	CHSVChanger();
	CHSVChanger(ColourFillAttribute*, DocColour*); 
		// if you use this constructor you can skip initialisation, though you will still need
		// to set the multipliers

	~CHSVChanger();

public:
	virtual BOOL ReplaceColours(); // a safe way of accessing the base class function
	virtual BOOL RestoreColours(); // restores the replaced colours
		
			BOOL ChangeHSVValues(); // does the HSV changing	
			BOOL ShouldChangeHSV();  // indicates whether or not colours need changing

			virtual BOOL IsInitialised();

public:	
	void   ChangeColour(DocColour* pCol);  // changes the values of just this colour

public: // access fns.
	double GetHueMultiplier();
	BOOL   SetHueMultiplier(double Value);

	double GetSaturationMultiplier();
	BOOL   SetSaturationMultiplier(double Value);

protected:  // helper fns.
	void	ApplyIncrements(DocColour* pCol);

protected: //data
	double m_HueMultiplier;
	double m_SaturationMultiplier;
	double m_ValueMultiplier;

	INT32   m_HueIncrement;   // these can only be set internally
	INT32   m_SatIncrement;

protected: // storing the original values, this is a bit grim but given that we sometimes 
		   // have to clip values that go over 255 we cannot rely on the mults and incrs to undo the change
	DocColour m_OrigStartCol;
	DocColour m_OrigEndCol1;
	DocColour m_OrigEndCol2;
	DocColour m_OrigEndCol3;

};

/********************************************************************************************

>	class CTransparencyReplacer : public CObject

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/2000
	Purpose:	Similar to CNamedColourReplacer, this class takes a transparency attribute, changes
				its values, and then changes them back.

********************************************************************************************/

class CTransparencyReplacer : public CCObject
{
	CC_DECLARE_DYNAMIC(CTransparencyReplacer)

public:
	CTransparencyReplacer();
	~CTransparencyReplacer() {};

public: // access
	BOOL SetReplacementValue(INT32 Value);
	INT32 GetReplacementValue();

	BOOL SetAdjustmentValue(double Value);
	double GetAdjustmentValue();
	BOOL SetInvertedAdjuster(double Value);
	double GetInvertedAdjuster();

	void SetTransparencyAttr(TranspFillAttribute* pAttr);
	BOOL ReplaceTransparency();
	BOOL RestoreTransparency();

	BOOL IsInitialised();
protected: // data
	UINT32 m_OriginalTransp;  // store the original value so we can replace it
	INT32 m_ReplaceTransp;   // the replacement value, can be negative, indicating do nothing

	double m_TranspAdjust;  // adjusts the final value
	double m_InvertAdjust; // we now take a second adjuster which gets inverted

	TranspFillAttribute* m_pTranspFill; // the transparency we're working on 


};


#endif
