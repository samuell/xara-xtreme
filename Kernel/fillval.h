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

// This file holds all the AttributeValue classes to do with path
// filling attributes.

#ifndef INC_FILLVAL
#define INC_FILLVAL

#include "attrval.h"
#include "nodeattr.h"
#include "bitmap.h"
#include "biasgain.h"
#include "gconsts.h"

class MouldGeometry;
class NodeMould;
class MouldAttribute;
class PathStrokerVector;
class ColourRamp;
class TransparencyRamp;

// Fill shapes
#define FILLSHAPE_UNKNOWN -1
#define FILLSHAPE_FLAT 0
#define FILLSHAPE_LINEAR 1
#define FILLSHAPE_CIRCULAR 2
#define FILLSHAPE_ELLIPTICAL 3
#define FILLSHAPE_CONICAL 4
#define FILLSHAPE_DIAMOND 5
#define FILLSHAPE_3POINT 6
#define FILLSHAPE_4POINT 7
#define FILLSHAPE_BITMAP 8
#define FILLSHAPE_CLOUDS 9
#define FILLSHAPE_PLASMA 10

enum RepeatType
{
	RT_NoRepeatType = 0,
	RT_Simple,
	RT_Repeating,
	RT_RepeatInverted
};

// Solid = 0, Mix = 1, Bleach = 2 and Stainedglass = 3
// Special Additive = 10, Special Subtractive = 11 and Transparency Lookup = 12
// These are the new Transparency types Contrast = 13, Saturation = 16, Darken = 19,
// Lighten = 22, Brightness = 25, Luminosity = 28, Hue = 31, Bevel = 34
enum TranspType
{
	TT_NoTranspType = 0,
	TT_Mix,
	TT_StainGlass,
	TT_Bleach,
//	TT_Contrast,
//	TT_Saturation,
//	TT_Darken,
//	TT_Lighten,
//	TT_Brightness,
//	TT_Luminosity,
//	TT_Hue,
//	TT_Bevel,
	TT_SPECIAL_1 = T_SPECIAL_1,		// Link following values to TransparencyEnum
	TT_SPECIAL_2,
	TT_SPECIAL_3,
	//				The values below are not legal in Camelot
	//				data structures (they come from GDraw)
	//				|||||||||||||||||||||||||||||||||||||||
	//				vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	TT_CONTRAST,	TT_FLAT_CONTRAST,	TT_GRAD_CONTRAST,
	TT_SATURATION,	TT_FLAT_SATURATION,	TT_GRAD_SATURATION,
	TT_DARKEN,		TT_FLAT_DARKEN,		TT_GRAD_DARKEN,
	TT_LIGHTEN,		TT_FLAT_LIGHTEN,	TT_GRAD_LIGHTEN,
	TT_BRIGHTNESS,	TT_FLAT_BRIGHTNESS,	TT_GRAD_BRIGHTNESS,
	TT_LUMINOSITY,	TT_FLAT_LUMINOSITY,	TT_GRAD_LUMINOSITY,
	TT_HUE,			TT_FLAT_HUE,		TT_GRAD_HUE,
	TT_BEVEL,		TT_FLAT_BEVEL,		TT_GRAD_BEVEL,

	// Add new values here and ideally they should be duplicated in TransparencyEnum

	TT_MAX
};
 
/********************************************************************************************

>	class FillGeometryAttribute : public AttributeValue

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	The base class for a fill geometry attribute, such as flat, linear,
				radial, conical, etc.
				This provides the basic interface for fill rendering.
	SeeAlso:	FlatFillAttribute; LinearFillAttribute; RadialFillAttribute;
				ConicalFillAttribute; BitmapFillAttribute

********************************************************************************************/

class FillGeometryAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(FillGeometryAttribute)

public:
	FillGeometryAttribute();

	virtual void Restore(RenderRegion *, BOOL) = 0;
	virtual void Render(RenderRegion *, BOOL Temp = FALSE) = 0;
	virtual void SimpleCopy(AttributeValue *);
	virtual BOOL IsDifferent(AttributeValue*);

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	// Extra virtual function for fill geometry attributes.
	virtual BOOL RenderFill(RenderRegion *, Path *) { return FALSE; }

	virtual BOOL IsAspectLocked() { return FALSE; }
	virtual void SetAspectLock(BOOL) {}

	virtual BOOL IsPerspective() { return FALSE; }
	virtual void MakePerspective() {}
	virtual void RemovePerspective() {}

	virtual DocCoord* GetStartPoint() { return NULL; }
	virtual DocCoord* GetEndPoint() { return NULL; }
	virtual DocCoord* GetEndPoint2() { return NULL; }
	virtual DocCoord* GetEndPoint3() { return NULL; }
	virtual void SetStartPoint(DocCoord*) {}
	virtual void SetEndPoint(DocCoord*) {}
	virtual void SetEndPoint2(DocCoord*) {}
	virtual void SetEndPoint3(DocCoord*) {}

	virtual DocColour* GetStartColour() { return NULL; }
	virtual DocColour* GetEndColour() { return NULL; }
	virtual DocColour* GetEndColour2() { return NULL; }
	virtual DocColour* GetEndColour3() { return NULL; }
	virtual void SetStartColour(DocColour*) {}
	virtual void SetEndColour(DocColour*) {}
	virtual void SetEndColour2(DocColour*) {}
	virtual void SetEndColour3(DocColour*) {}

	virtual UINT32* GetStartTransp() { return NULL; }
	virtual UINT32* GetEndTransp() { return NULL; }
	virtual UINT32* GetEndTransp2() { return NULL; }
	virtual UINT32* GetEndTransp3() { return NULL; }
	virtual void SetStartTransp(UINT32*) {}
	virtual void SetEndTransp(UINT32*) {}
	virtual void SetEndTransp2(UINT32*) {}
	virtual void SetEndTransp3(UINT32*) {}

	virtual UINT32 GetTranspType() { return TT_NoTranspType; }
	virtual void SetTranspType(UINT32 NewType) {}

	virtual UINT32 GetDPI() { return 0; }
	virtual BOOL SetDPI(UINT32 NewDpi) { return FALSE; }

	virtual UINT32 GetFractalDPI() { return 0; }
	virtual BOOL SetFractalDPI(UINT32 NewDpi) { return FALSE; }

	virtual UINT32 GetFractalDim() { return 0; }
	virtual void SetFractalDim(UINT32 NewDim) {}

	virtual INT32	GetSeed()	{ return 0; }
	virtual FIXED16 GetGraininess();
	virtual FIXED16 GetGravity();
	virtual FIXED16 GetSquash() { return 0; }
	virtual BOOL	GetTileable() { return FALSE; }

	virtual BOOL SetSeed(INT32 NewSeed)			 { return FALSE; }
	virtual BOOL SetGraininess(FIXED16 NewGrain) { return FALSE; }
	virtual BOOL SetGravity(FIXED16 NewGrav)	 { return FALSE; }
	virtual BOOL SetSquash(FIXED16 NewSquash)	 { return FALSE; }
	virtual BOOL SetTileable(BOOL NewTile)		 { return FALSE; }

	virtual	double GetNoiseScale()  { return 0; }

	virtual BOOL SetNoiseScale(double)  { return FALSE; }
	
	virtual BOOL Randomise() { return FALSE; }
	virtual BOOL RecalcFractal() { return FALSE; }

	virtual BOOL AttachBitmap(KernelBitmap* pBitmap) { return FALSE; }
	virtual BOOL DetachBitmap() { return FALSE; }
	virtual KernelBitmap* GetBitmap() { return NULL; }
	virtual BOOL CopyBitmap(KernelBitmap* pBitmap) { return FALSE; }

	virtual KernelBitmapRef* GetBitmapRef() { return NULL; }

	virtual INT32 GetTesselation() { return RT_NoRepeatType; }
	virtual void SetTesselation(INT32) {}

	virtual BOOL Blend(BlendAttrParam* pBlendParam);
	virtual BOOL BlendFillColours(DocColour* Start, DocColour* End, DocColour* Blend,
								  double& Ratio, BlendAttrParam* pBlendParam);
	virtual BOOL BlendFillTransp(UINT32* Start, UINT32* End, UINT32* Blend,
								 double& Ratio, BlendAttrParam* pBlendParam);
	virtual BOOL BlendControlPoints(DocCoord* Start, DocCoord* End, DocCoord* Blend,
								    double& Ratio, BlendAttrParam* pBlendParam, BOOL swapOrder = FALSE);
	virtual BOOL CheckForGreyscaleBitmapBlend(KernelBitmap* pBitmap, 
				 							  DocColour* StartCol, DocColour* EndCol);

	virtual INT32 Mould(MouldAttribute*, DocCoord*, INT32, DocCoord*);

	virtual BOOL IsAColourFill() { return FALSE; }
	virtual BOOL IsATranspFill() { return FALSE; }

	virtual BOOL IsAFlatFill() { return FALSE; }
	virtual BOOL IsAGradFill() { return FALSE; }

	virtual BOOL IsALinearFill() { return FALSE; }
	virtual BOOL IsARadialFill() { return FALSE; }
	virtual BOOL IsAConicalFill() { return FALSE; }
	virtual BOOL IsABitmapFill() { return FALSE; }
	virtual BOOL IsAKindOfBitmapFill() { return FALSE; }
	virtual BOOL IsASquareFill() { return FALSE; }
	virtual BOOL IsAThreeColFill() { return FALSE; }
	virtual BOOL IsAFourColFill() { return FALSE; }
	
	virtual BOOL IsATextureFill() { return FALSE; }
	virtual BOOL IsAFractalFill() { return FALSE; }
	virtual BOOL IsANoiseFill()	{ return FALSE; }

	virtual void CacheFractalData(FillGeometryAttribute *pCachedFractal);
	virtual BOOL IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal);

	virtual BOOL SupportsFillRamps()	{ return FALSE; }
	virtual ColourRamp* GetColourRamp() { return NULL; }
	virtual TransparencyRamp* GetTranspRamp() { return NULL; }
	virtual BOOL SetColourRamp(ColourRamp* pRamp) { return FALSE; }
	virtual BOOL SetTranspRamp(TransparencyRamp* pRamp) { return FALSE; }

	virtual DocCoord GetGeometryCoord(float pos) const { return DocCoord(0,0); }
	virtual float	 GetGeometryParam(const DocCoord& c) const { return 0.0f; }

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_UNKNOWN); }

public:
	// new (test) profile bits
	CProfileBiasGain  DiagramMapper;
	void SetProfile (CProfileBiasGain& SetWith);
	CProfileBiasGain& GetProfile ();
	CProfileBiasGain* GetProfilePtr ();

public:
	virtual AttributeValue *MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale = 1.0);
			// Mould all geometry points using the given stroke moulder, and scaling 
			// transparency (if any) by the given fraction.

protected:
	virtual void TidyMouldPoints(DocCoord*, DocCoord*, BOOL*, INT32);

protected:
	KernelBitmap* GenerateFractalBitmap(INT32, double, double, double, UINT32); 
	KernelBitmap* GenerateNoiseBitmap(FIXED16 grain, INT32 seed);
};


/********************************************************************************************

>	class ColourFillAttribute : public FillGeometryAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Purpose:	The base class for a fill geometry attribute, such as flat, linear,
				radial, conical, etc.
				This provides the basic interface for fill rendering.
	SeeAlso:	FlatFillAttribute; LinearFillAttribute; RadialFillAttribute;
				ConicalFillAttribute; BitmapFillAttribute

********************************************************************************************/

class ColourFillAttribute : public FillGeometryAttribute
{
	CC_DECLARE_DYNCREATE(ColourFillAttribute)

public:
	ColourFillAttribute();

	static BOOL Init();

	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual DocColour* GetStartColour() { return &Colour; }
	virtual void SetStartColour(DocColour* NewCol);

	virtual BOOL IsAColourFill() { return TRUE; }
	virtual BOOL ContainsNamedColour();

public:
	DocColour Colour;
};



/********************************************************************************************

>	class FlatFillAttribute : public ColourFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Specifies a flat fill attribute for an object, i.e. it is filled with
				one solid colour.
	SeeAlso:	FillGeometryAttribute

********************************************************************************************/

class FlatFillAttribute : public ColourFillAttribute
{
	CC_DECLARE_DYNCREATE(FlatFillAttribute)
public:
	FlatFillAttribute();
	FlatFillAttribute(DocColour& NewColour) { SetStartColour(&NewColour); }

	virtual BOOL Blend(BlendAttrParam* pBlendParam);

	virtual NodeAttribute *MakeNode();
	virtual void SetStartColour(DocColour* NewCol);

	virtual BOOL IsAFlatFill() { return TRUE; }

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_FLAT); }

public:
	virtual AttributeValue *MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale = 1.0);
			// Mould all geometry points using the given stroke moulder, and scaling 
			// transparency (if any) by the given fraction.
};


/********************************************************************************************

>	class GradFillAttribute : public ColourFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Base class for a graduated fill attribute to be derived from.
				It includes one vector (stored as two coordinates) to represent the basic
				direction of the fill.  Some fills will need more vectors (e.g. radial
				fills) but they can just add data fields in their derived classes.
	SeeAlso:	FillGeometryAttribute

********************************************************************************************/

class GradFillAttribute : public ColourFillAttribute
{
	CC_DECLARE_DYNCREATE(GradFillAttribute)
public:
	 GradFillAttribute();
	~GradFillAttribute();

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual void SimpleCopy(AttributeValue *);

	virtual BOOL RenderFill(RenderRegion *, Path *);

	virtual DocCoord*   GetStartPoint() { return &StartPoint; }
	virtual DocCoord*   GetEndPoint()   { return &EndPoint; }
	virtual DocColour*  GetEndColour()  { return &EndColour; }

	virtual void SetStartPoint(DocCoord* Pos);
	virtual void SetEndPoint(DocCoord* Pos);
	virtual void SetEndColour(DocColour* NewCol);

	virtual BOOL Blend(BlendAttrParam* pBlendParam);

	virtual BOOL IsAGradFill() { return TRUE; }
	virtual DocCoord GetGeometryCoord(float pos) const;
	virtual float	 GetGeometryParam(const DocCoord& c) const;

	ColourRamp *GetColourRamp() { return m_pColourRamp; }

public:
	// All fills have Start and End Colours and Three control points.
	DocColour EndColour;
	DocCoord  StartPoint;
	DocCoord  EndPoint;

public:
	// new colour ramp bits
	virtual BOOL SupportsFillRamps() { return TRUE; }
	virtual BOOL SetColourRamp(ColourRamp* pRamp);
	
	ColourRamp* MakeNewColourRamp();
	BOOL SameColourRampAs(ColourRamp *pOtherRamp);
	void DeleteColourRamp();

protected:
	ColourRamp *m_pColourRamp;
};

/********************************************************************************************

>	class LinearFillAttribute : public GradFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Specifies a linear graduated fill for an object, including the start and
				end points.
	SeeAlso:	GradFillAttribute

********************************************************************************************/

class LinearFillAttribute : public GradFillAttribute
{
	CC_DECLARE_DYNCREATE(LinearFillAttribute)
public:
	LinearFillAttribute();
	virtual NodeAttribute *MakeNode();
//	virtual BOOL RenderFill(RenderRegion *, Path *);

	virtual DocCoord* GetEndPoint2() { return &EndPoint2; }
	virtual DocCoord* GetEndPoint3() { return &EndPoint3; }
	virtual void SetEndPoint2(DocCoord* Pos);
	virtual void SetEndPoint3(DocCoord* Pos);

	virtual BOOL IsPerspective() { return IsPersp; }
	virtual void MakePerspective();
	virtual void RemovePerspective();

	virtual BOOL IsALinearFill() { return TRUE; }
	virtual INT32 GetGeometryShape() { return(FILLSHAPE_LINEAR); }

public:
	virtual AttributeValue *MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale = 1.0);
			// Mould all geometry points using the given stroke moulder, and scaling 
			// transparency (if any) by the given fraction.

public:
	DocCoord EndPoint2;
	DocCoord EndPoint3;

	BOOL IsPersp;
};

/********************************************************************************************

>	class RadialFillAttribute : public GradFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Specifies a radial fill attribute for an object, including the two vectors
				that define the ellipse.
	SeeAlso:	GradFillAttribute

********************************************************************************************/

class RadialFillAttribute : public GradFillAttribute
{
	CC_DECLARE_DYNCREATE(RadialFillAttribute)
public:
	RadialFillAttribute();
	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();
//	virtual BOOL RenderFill(RenderRegion *, Path *);

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual BOOL IsAspectLocked() { return Circular; }
	virtual void SetAspectLock(BOOL Locked) { Circular = Locked; }

	// Radial fills can be locked, so they are always circular
	// These functions control its state
	void MakeCircular();
	void MakeElliptical();

	BOOL IsCircular() { return Circular; }
	BOOL IsElliptical() { return !Circular; }

	virtual DocCoord* GetEndPoint2() { return &EndPoint2; }
	virtual DocCoord* GetEndPoint3() { return &EndPoint3; }
	virtual void SetEndPoint2(DocCoord* Pos);
	virtual void SetEndPoint3(DocCoord* Pos);
	
	virtual BOOL IsPerspective() { return IsPersp; }
	virtual void MakePerspective();
	virtual void RemovePerspective();

	virtual BOOL IsARadialFill() { return TRUE; }
	virtual INT32 GetGeometryShape() { return(Circular ? FILLSHAPE_CIRCULAR : FILLSHAPE_ELLIPTICAL); }

public:
	virtual AttributeValue *MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale = 1.0);
			// Mould all geometry points using the given stroke moulder, and scaling 
			// transparency (if any) by the given fraction.

public:
	DocCoord EndPoint2;
	DocCoord EndPoint3;
	BOOL IsPersp;

private:
	BOOL Circular;
};

/********************************************************************************************

>	class ConicalFillAttribute : public GradFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Specifies a conical fill attribute for an object, including the vector that
				defines the centre of the cone, and the angle of the fill.
	SeeAlso:	GradFillAttribute

********************************************************************************************/

class ConicalFillAttribute : public GradFillAttribute
{
	CC_DECLARE_DYNCREATE(ConicalFillAttribute)
public:
	ConicalFillAttribute();
	virtual NodeAttribute *MakeNode();
//	virtual BOOL RenderFill(RenderRegion *, Path *);
	virtual DocCoord GetGeometryCoord(float pos) const;
	virtual float	 GetGeometryParam(const DocCoord& c) const;

	virtual BOOL IsAConicalFill() { return TRUE; }
	virtual INT32 GetGeometryShape() { return(FILLSHAPE_CONICAL); }
};


/********************************************************************************************

>	class SquareFillAttribute : public GradFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Specifies a Square graduated fill for an object, including the start and
				end points.
	SeeAlso:	GradFillAttribute

********************************************************************************************/

class SquareFillAttribute : public GradFillAttribute
{
	CC_DECLARE_DYNCREATE(SquareFillAttribute)
public:
	SquareFillAttribute();
	virtual NodeAttribute *MakeNode();
//	virtual BOOL RenderFill(RenderRegion *, Path *);

	virtual DocCoord* GetEndPoint2() { return &EndPoint2; }
	virtual DocCoord* GetEndPoint3() { return &EndPoint3; }
	virtual void SetEndPoint2(DocCoord* Pos);
	virtual void SetEndPoint3(DocCoord* Pos);

	virtual BOOL IsPerspective() { return IsPersp; }
	virtual void MakePerspective();
	virtual void RemovePerspective();

	virtual BOOL IsASquareFill() { return TRUE; }
	virtual INT32 GetGeometryShape() { return(FILLSHAPE_DIAMOND); }

public:
	DocCoord EndPoint2;
	DocCoord EndPoint3;

	BOOL IsPersp;
};

/********************************************************************************************

>	class ThreeColFillAttribute : public GradFillAttribute

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/96
	Purpose:	Specifies a Square graduated fill for an object, including the start and
				end points.
	SeeAlso:	GradFillAttribute

********************************************************************************************/

class ThreeColFillAttribute : public GradFillAttribute
{
	CC_DECLARE_DYNCREATE(ThreeColFillAttribute)
public:
	ThreeColFillAttribute();

	virtual INT32 operator==(const FillGeometryAttribute& Attrib);
	
	virtual NodeAttribute *MakeNode();

	virtual DocCoord* GetEndPoint2() { return &EndPoint2; }
	virtual void SetEndPoint2(DocCoord* Pos);
	virtual DocCoord* GetEndPoint3() { return &EndPoint3; }
	virtual void SetEndPoint3(DocCoord* Pos);

	virtual DocColour* GetEndColour2() { return &EndColour2; }
	virtual void SetEndColour2(DocColour* NewCol);

	virtual BOOL IsPerspective() { return IsPersp; }
	virtual void MakePerspective();
	virtual void RemovePerspective();

	virtual BOOL IsAThreeColFill() { return TRUE; }
	virtual BOOL SupportsFillRamps() { return FALSE; }
	virtual INT32 GetGeometryShape() { return(FILLSHAPE_3POINT); }

public:
	DocCoord EndPoint2;
	DocCoord EndPoint3;

	BOOL IsPersp;

	DocColour EndColour2;
};

/********************************************************************************************

>	class FourColFillAttribute : public ThreeColFillAttribute

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/96
	Purpose:	Specifies a Four colour fill for an object
	SeeAlso:	GradFillAttribute

********************************************************************************************/

class FourColFillAttribute : public ThreeColFillAttribute
{
	CC_DECLARE_DYNCREATE(FourColFillAttribute)
public:
	FourColFillAttribute();

	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual NodeAttribute *MakeNode();

	virtual DocColour* GetEndColour3() { return &EndColour3; }
	virtual void SetEndColour3(DocColour* NewCol);

	virtual BOOL IsAFourColFill() { return TRUE; }
	virtual INT32 GetGeometryShape() { return(FILLSHAPE_4POINT); }

public:
	DocColour EndColour3;
};





/********************************************************************************************

>	class BitmapFillAttribute : public GradFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/94
	Purpose:	Specifies a bitmap fill attribute for an object.
	SeeAlso:	FillGeometryAttribute

********************************************************************************************/

class BitmapFillAttribute : public GradFillAttribute
{
	CC_DECLARE_DYNCREATE(BitmapFillAttribute)
public:
	BitmapFillAttribute();
	~BitmapFillAttribute();

	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);

	virtual NodeAttribute *MakeNode();
	virtual BOOL RenderFill(RenderRegion *, Path *);
	virtual void SimpleCopy(AttributeValue *);

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	KernelBitmapRef BitmapRef;

	virtual KernelBitmapRef* GetBitmapRef();

	virtual BOOL AttachBitmap(KernelBitmap* pBitmap);
	virtual BOOL DetachBitmap();
	virtual KernelBitmap* GetBitmap();
	virtual BOOL CopyBitmap(KernelBitmap*);

	virtual DocCoord* GetStartPoint() { return &StartPoint; }
	virtual DocCoord* GetEndPoint() { return &EndPoint; }
	virtual DocCoord* GetEndPoint2() { return &EndPoint2; }
	virtual DocCoord* GetEndPoint3() { return &EndPoint3; }
	virtual void SetStartPoint(DocCoord* Pos);
	virtual void SetEndPoint(DocCoord* Pos);
	virtual void SetEndPoint2(DocCoord* Pos);
	virtual void SetEndPoint3(DocCoord* Pos);

	virtual void SetStartColour(DocColour*);
	virtual void SetEndColour(DocColour*);

	virtual DocColour* GetStartColour();
	virtual DocColour* GetEndColour();

	virtual INT32 GetTesselation() { return Tesselation; }
	virtual void SetTesselation(INT32 NewTess);
	
	virtual BOOL SetDPI(UINT32 NewDpi);
	virtual UINT32 GetDPI();

	virtual BOOL IsPerspective() { return IsPersp; }
	virtual void MakePerspective();
	virtual void RemovePerspective();

	virtual BOOL IsAKindOfBitmapFill() { return TRUE; }
	virtual BOOL IsABitmapFill() { return TRUE; }

	// DMc
	// whether this bitmap can be blur rendered or just blitted
	BOOL CanBlurRenderBitmap() { return m_CanBlurRender; }
	void SetCanBlurRenderBitmap(BOOL b) { m_CanBlurRender = b; }

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_BITMAP); }

public:
	DocCoord EndPoint2;
	DocCoord EndPoint3;

	INT32 Tesselation;

	BOOL IsPersp;

	BOOL m_CanBlurRender;

	static BOOL m_doBitmapSmoothing;		// this will always be true - unless we are rendering a preview bitmap
};


/********************************************************************************************

>	class NoiseFillAttribute : public BitmapFillAttribute

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97
	Purpose:	Specifies a noise fill attribute for an object.
	SeeAlso:	FillGeometryAttribute

********************************************************************************************/

class NoiseFillAttribute : public BitmapFillAttribute
{
	CC_DECLARE_DYNCREATE(NoiseFillAttribute)

public:
	 NoiseFillAttribute();
	~NoiseFillAttribute();

	virtual NodeAttribute *MakeNode();
	virtual void SimpleCopy(AttributeValue *);

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual BOOL AttachBitmap(KernelBitmap* pBitmap);
	virtual BOOL DetachBitmap();
	virtual BOOL CopyBitmap(KernelBitmap*);

	virtual BOOL RecalcFractal();

	virtual UINT32		GetFractalDim()  { return dim; }
	virtual BOOL		GetTileable()	 { return tileable; }
	virtual UINT32		GetFractalDPI()  { return dpi; }
	virtual DocColour*  GetStartColour() { return &Colour; }
	virtual DocColour*  GetEndColour()	 { return &EndColour; }

	virtual INT32		GetSeed()		 { return seed; }
	virtual FIXED16		GetGraininess()  { return grain; }

	virtual void SetTesselation(INT32 NewTess);
	virtual void SetFractalDim(UINT32 NewDim);
	virtual BOOL SetTileable(BOOL NewTile);
	virtual BOOL SetFractalDPI(UINT32 NewDpi);

	virtual void SetStartColour(DocColour*);
	virtual void SetEndColour(DocColour*);

	virtual BOOL SetSeed(INT32 NewSeed);
	virtual BOOL SetGraininess(FIXED16 NewGrain);

	virtual BOOL IsAKindOfBitmapFill()	{ return TRUE; }
	virtual BOOL IsABitmapFill()		{ return FALSE; }
	virtual BOOL IsATextureFill()		{ return TRUE; }
	virtual BOOL IsANoiseFill()			{ return TRUE; }

	virtual void CacheFractalData(FillGeometryAttribute *pCachedFractal);
	virtual BOOL IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal);

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_PLASMA); }

private:
	INT32 	seed;
	UINT32	dpi;
	BOOL	tileable;
	UINT32	dim;

private:
	FIXED16	grain;
};


/********************************************************************************************

>	class FractalFillAttribute : public BitmapFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/94
	Purpose:	Specifies a Fractal fill attribute for an object.
	SeeAlso:	FillGeometryAttribute

********************************************************************************************/

class FractalFillAttribute : public BitmapFillAttribute
{
	CC_DECLARE_DYNCREATE(FractalFillAttribute)
public:
	 FractalFillAttribute();
	~FractalFillAttribute();

	virtual NodeAttribute *MakeNode();
	virtual void SimpleCopy(AttributeValue *);

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual BOOL AttachBitmap(KernelBitmap* pBitmap);
	virtual BOOL DetachBitmap();
	virtual BOOL CopyBitmap(KernelBitmap*);

	virtual BOOL Randomise();
	virtual BOOL RecalcFractal();

	virtual UINT32		GetFractalDim()  { return Dim; }
	virtual BOOL		GetTileable()	 { return Tileable; }
	virtual UINT32		GetFractalDPI()  { return Dpi; }

	virtual DocColour*	GetStartColour() { return &Colour; }
	virtual DocColour*	GetEndColour()	 { return &EndColour; }

	virtual INT32		GetSeed()		 { return Seed; }
	virtual FIXED16		GetGraininess()  { return Graininess; }
	virtual FIXED16		GetGravity()	 { return Gravity; }
	virtual FIXED16		GetSquash()		 { return Squash; }

	virtual void SetTesselation(INT32 NewTess);
	virtual void SetFractalDim(UINT32 NewDim);
	virtual BOOL SetTileable(BOOL NewTile);
	virtual BOOL SetFractalDPI(UINT32 NewDpi);

	virtual void SetStartColour(DocColour*);
	virtual void SetEndColour(DocColour*);

	virtual BOOL SetSeed(INT32 NewSeed);
	virtual BOOL SetGraininess(FIXED16 NewGrain);
	virtual BOOL SetGravity(FIXED16 NewGrav);
	virtual BOOL SetSquash(FIXED16 NewSquash);
	
	virtual BOOL IsAKindOfBitmapFill()	{ return TRUE; }
	virtual BOOL IsABitmapFill()		{ return FALSE; }
	virtual BOOL IsATextureFill()		{ return TRUE; }
	virtual BOOL IsAFractalFill()		{ return TRUE; }


	virtual void CacheFractalData(FillGeometryAttribute *pCachedFractal);
	virtual BOOL IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal);

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_CLOUDS); }

public:
	INT32 	Seed;
	FIXED16	Graininess;			//0..about 32
	FIXED16 Gravity;			//0..about 255
	FIXED16	Squash;
	INT32	Dpi;
	BOOL	Tileable;

protected:
	INT32 	Dim;
};




/********************************************************************************************

>	class TranspFillAttribute : public FillGeometryAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Purpose:	The base class for a fill geometry attribute, such as flat, linear,
				radial, conical, etc.
				This provides the basic interface for fill rendering.
	SeeAlso:	FlatFillAttribute; LinearFillAttribute; RadialFillAttribute;
				ConicalFillAttribute; BitmapFillAttribute

********************************************************************************************/

class TranspFillAttribute : public FillGeometryAttribute
{
	CC_DECLARE_DYNCREATE(TranspFillAttribute)

public:
	TranspFillAttribute();

	static BOOL Init();

	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual UINT32* GetStartTransp() { return &Transp; }
	virtual void SetStartTransp(UINT32* NewTransp);

	UINT32 GetTranspType() { return TranspType; }
	void SetTranspType(UINT32 NewType);

	virtual BOOL IsATranspFill() { return TRUE; }
	virtual ColourFillAttribute *MakeSimilarNonTranspFillGeometry(double TransparencyScale) {return NULL;}

public:
	UINT32 TranspType;
	UINT32 Transp;
};

/********************************************************************************************

>	class FlatTranspFillAttribute : public TranspFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	Specifies a flat fill attribute for an object, i.e. it is filled with
				one solid colour.
	SeeAlso:	FillGeometryAttribute

********************************************************************************************/

class FlatTranspFillAttribute : public TranspFillAttribute
{
	CC_DECLARE_DYNCREATE(FlatTranspFillAttribute)
public:
	FlatTranspFillAttribute();
	FlatTranspFillAttribute(UINT32 NewTransp) { SetStartTransp(&NewTransp); }

	virtual BOOL Blend(BlendAttrParam* pBlendParam);

	virtual NodeAttribute *MakeNode();

	virtual BOOL IsAFlatFill() { return TRUE; }

	virtual ColourFillAttribute *MakeSimilarNonTranspFillGeometry(double TransparencyScale);

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_FLAT); }
};

/********************************************************************************************

>	class GradTranspFillAttribute : public TranspFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	Base class for a graduated fill attribute to be derived from.
				It includes one vector (stored as two coordinates) to represent the basic
				direction of the fill.  Some fills will need more vectors (e.g. radial
				fills) but they can just add data fields in their derived classes.
	SeeAlso:	FillGeometryAttribute

********************************************************************************************/

class GradTranspFillAttribute : public TranspFillAttribute
{
	CC_DECLARE_DYNCREATE(GradTranspFillAttribute)
public:
	 GradTranspFillAttribute();
	~GradTranspFillAttribute();

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual void SimpleCopy(AttributeValue *);

	virtual DocCoord* GetStartPoint() { return &StartPoint; }
	virtual DocCoord* GetEndPoint()   { return &EndPoint; }
	virtual UINT32*	  GetEndTransp()  { return &EndTransp; }
	
	virtual void SetStartPoint(DocCoord* Pos);
	virtual void SetEndPoint(DocCoord* Pos);
	virtual void SetEndTransp(UINT32* NewTransp);

	virtual BOOL Blend(BlendAttrParam* pBlendParam);
	virtual BOOL IsAGradFill() { return TRUE; }
	virtual DocCoord GetGeometryCoord(float pos) const;
	virtual float	 GetGeometryParam(const DocCoord& c) const;

	virtual ColourFillAttribute *MakeSimilarNonTranspFillGeometry(double TransparencyScale);

public:
	DocCoord StartPoint;
	DocCoord EndPoint;
	UINT32     EndTransp;

public:
	// extra transparency ramp stuff
	virtual BOOL SupportsFillRamps() { return TRUE; }
	virtual BOOL SetTranspRamp(TransparencyRamp *pRamp);
	virtual TransparencyRamp* GetTranspRamp() { return m_pTranspRamp; }

	TransparencyRamp* MakeNewTranspRamp();
	BOOL SameTransparencyRampAs(TransparencyRamp *pOtherRamp);
	void DeleteTranspRamp();

protected:
	TransparencyRamp *m_pTranspRamp;
};




/********************************************************************************************

>	class LinearTranspFillAttribute : public GradTranspFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	Specifies a linear graduated fill for an object, including the start and
				end points.
	SeeAlso:	GradFillAttribute

********************************************************************************************/

class LinearTranspFillAttribute : public GradTranspFillAttribute
{
	CC_DECLARE_DYNCREATE(LinearTranspFillAttribute)
public:
	LinearTranspFillAttribute();
	virtual NodeAttribute *MakeNode();

	virtual DocCoord* GetEndPoint2() { return &EndPoint2; }
	virtual void SetEndPoint2(DocCoord* Pos);
	virtual DocCoord* GetEndPoint3() { return &EndPoint3; }
	virtual void SetEndPoint3(DocCoord* Pos);

	virtual BOOL IsPerspective() { return IsPersp; }
	virtual void MakePerspective();
	virtual void RemovePerspective();

	virtual BOOL IsALinearFill() { return TRUE; }

	virtual ColourFillAttribute *MakeSimilarNonTranspFillGeometry(double TransparencyScale);

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_LINEAR); }

public:
	virtual AttributeValue *MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale = 1.0);
			// Mould all geometry points using the given stroke moulder, and scaling 
			// transparency (if any) by the given fraction.

public:
	DocCoord EndPoint2;
	DocCoord EndPoint3;

	BOOL IsPersp;
};

/********************************************************************************************

>	class RadialTranspFillAttribute : public GradTranspFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	Specifies a radial fill attribute for an object, including the two vectors
				that define the ellipse.
	SeeAlso:	GradFillAttribute

********************************************************************************************/

class RadialTranspFillAttribute : public GradTranspFillAttribute
{
	CC_DECLARE_DYNCREATE(RadialTranspFillAttribute)
public:
	RadialTranspFillAttribute();
	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual BOOL IsAspectLocked() { return Circular; }
	virtual void SetAspectLock(BOOL Locked) { Circular = Locked; }

	// Radial fills can be locked, so they are always circular
	// These functions control its state
	void MakeCircular();
	void MakeElliptical();

	BOOL IsCircular() { return Circular; }
	BOOL IsElliptical() { return !Circular; }

	virtual DocCoord* GetEndPoint2() { return &EndPoint2; }
	virtual void SetEndPoint2(DocCoord* Pos);
	virtual DocCoord* GetEndPoint3() { return &EndPoint3; }
	virtual void SetEndPoint3(DocCoord* Pos);

	virtual BOOL IsPerspective() { return IsPersp; }
	virtual void MakePerspective();
	virtual void RemovePerspective();

	virtual BOOL IsARadialFill() { return TRUE; }

	virtual ColourFillAttribute *MakeSimilarNonTranspFillGeometry(double TransparencyScale);

	virtual INT32 GetGeometryShape() { return(Circular ? FILLSHAPE_CIRCULAR : FILLSHAPE_ELLIPTICAL); }

public:
	virtual AttributeValue *MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale = 1.0);
			// Mould all geometry points using the given stroke moulder, and scaling 
			// transparency (if any) by the given fraction.

public:
	DocCoord EndPoint2;
	DocCoord EndPoint3;
	BOOL IsPersp;

private:
	BOOL Circular;
};

/********************************************************************************************

>	class ConicalTranspFillAttribute : public GradTranspFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	Specifies a conical fill attribute for an object, including the vector that
				defines the centre of the cone, and the angle of the fill.
	SeeAlso:	GradFillAttribute

********************************************************************************************/

class ConicalTranspFillAttribute : public GradTranspFillAttribute
{
	CC_DECLARE_DYNCREATE(ConicalTranspFillAttribute)
public:
	ConicalTranspFillAttribute();
	virtual NodeAttribute *MakeNode();

	virtual BOOL IsAConicalFill() { return TRUE; }
	virtual DocCoord GetGeometryCoord(float pos) const;
	virtual float	 GetGeometryParam(const DocCoord& c) const;

	virtual ColourFillAttribute *MakeSimilarNonTranspFillGeometry(double TransparencyScale);

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_CONICAL); }
};


/********************************************************************************************

>	class SquareTranspFillAttribute : public GradTranspFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	Specifies a Square graduated fill for an object, including the start and
				end points.
	SeeAlso:	GradFillAttribute

********************************************************************************************/

class SquareTranspFillAttribute : public GradTranspFillAttribute
{
	CC_DECLARE_DYNCREATE(SquareTranspFillAttribute)
public:
	SquareTranspFillAttribute();
	virtual NodeAttribute *MakeNode();

	virtual DocCoord* GetEndPoint2() { return &EndPoint2; }
	virtual void SetEndPoint2(DocCoord* Pos);
	virtual DocCoord* GetEndPoint3() { return &EndPoint3; }
	virtual void SetEndPoint3(DocCoord* Pos);

	virtual BOOL IsPerspective() { return IsPersp; }
	virtual void MakePerspective();
	virtual void RemovePerspective();

	virtual BOOL IsASquareFill() { return TRUE; }

	virtual ColourFillAttribute *MakeSimilarNonTranspFillGeometry(double TransparencyScale);

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_DIAMOND); }

public:
	DocCoord EndPoint2;
	DocCoord EndPoint3;

	BOOL IsPersp;
};



/********************************************************************************************

>	class BitmapTranspFillAttribute : public GradTranspFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/94
	Purpose:	Specifies a bitmap fill attribute for an object.
	SeeAlso:	FillGeometryAttribute

********************************************************************************************/

class BitmapTranspFillAttribute : public GradTranspFillAttribute
{
	CC_DECLARE_DYNCREATE(BitmapTranspFillAttribute)
public:
	BitmapTranspFillAttribute();
	~BitmapTranspFillAttribute();

	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);

	virtual NodeAttribute *MakeNode();
	virtual void SimpleCopy(AttributeValue *);

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	KernelBitmapRef BitmapRef;

	virtual KernelBitmapRef* GetBitmapRef() { return &BitmapRef; }

	virtual BOOL AttachBitmap(KernelBitmap* pBitmap);
	virtual BOOL DetachBitmap();
	virtual KernelBitmap* GetBitmap();
	virtual BOOL CopyBitmap(KernelBitmap*);

	virtual DocCoord* GetStartPoint() { return &StartPoint; }
	virtual DocCoord* GetEndPoint() { return &EndPoint; }
	virtual DocCoord* GetEndPoint2() { return &EndPoint2; }
	virtual DocCoord* GetEndPoint3() { return &EndPoint3; }
	virtual void SetStartPoint(DocCoord* Pos);
	virtual void SetEndPoint(DocCoord* Pos);
	virtual void SetEndPoint2(DocCoord* Pos);
	virtual void SetEndPoint3(DocCoord* Pos);

	virtual INT32 GetTesselation() { return Tesselation; }
	virtual void SetTesselation(INT32 NewTess);
	
	virtual UINT32 GetDPI();
	virtual BOOL SetDPI(UINT32 NewDpi);

	virtual UINT32* GetStartTransp();
	virtual void SetStartTransp(UINT32* NewTransp);
	virtual UINT32* GetEndTransp();
	virtual void SetEndTransp(UINT32* NewTransp);

	virtual BOOL IsPerspective() { return IsPersp; }
	virtual void MakePerspective();
	virtual void RemovePerspective();

	virtual BOOL IsAKindOfBitmapFill() { return TRUE; }
	virtual BOOL IsABitmapFill() { return TRUE; }

	virtual ColourFillAttribute *MakeSimilarNonTranspFillGeometry(double TransparencyScale);

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_BITMAP); }

public:
	DocCoord EndPoint2;
	DocCoord EndPoint3;

	INT32 Tesselation;

	BOOL IsPersp;

	static BOOL m_doBitmapSmoothing;		// this will always be true - unless we are rendering a preview bitmap
};


/********************************************************************************************

>	class NoiseTranspFillAttribute : public BitmapTranspFillAttribute

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Purpose:	Specifies a noise fill attribute for an object.
	SeeAlso:	FillGeometryAttribute

********************************************************************************************/

class NoiseTranspFillAttribute : public BitmapTranspFillAttribute
{
	CC_DECLARE_DYNCREATE(NoiseTranspFillAttribute)

public:
	 NoiseTranspFillAttribute();
	~NoiseTranspFillAttribute();

	virtual NodeAttribute *MakeNode();
	virtual void SimpleCopy(AttributeValue *);

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual BOOL AttachBitmap(KernelBitmap* pBitmap);
	virtual BOOL DetachBitmap();
	virtual BOOL CopyBitmap(KernelBitmap*);

	virtual BOOL RecalcFractal();

	virtual UINT32	GetFractalDim()  { return dim; }
	virtual BOOL	GetTileable()	 { return tileable; }
	virtual UINT32	GetFractalDPI()	 { return dpi; }
	virtual INT32	GetSeed()		 { return seed; }
	virtual FIXED16 GetGraininess()	 { return grain; }

	virtual void SetTesselation(INT32 NewTess);
	virtual void SetFractalDim(UINT32 NewDim);
	virtual BOOL SetTileable(BOOL NewTile);
	virtual BOOL SetFractalDPI(UINT32 NewDpi);

	virtual BOOL SetSeed(INT32 NewSeed);
	virtual BOOL SetGraininess(FIXED16 NewGrain);

	virtual BOOL IsAKindOfBitmapFill()	{ return TRUE; }
	virtual BOOL IsABitmapFill()		{ return FALSE; }

	virtual void CacheFractalData(FillGeometryAttribute *pCachedFractal);
	virtual BOOL IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal);

	virtual ColourFillAttribute *MakeSimilarNonTranspFillGeometry(double TransparencyScale);

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_PLASMA); }

private:
	UINT32	dpi;
	UINT32	dim;
	BOOL	tileable;
	INT32	seed;

private:
	FIXED16	grain;
};


/********************************************************************************************

>	class FractalTranspFillAttribute : public BitmapTranspFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/94
	Purpose:	Specifies a Fractal fill attribute for an object.
	SeeAlso:	FillGeometryAttribute

********************************************************************************************/

class FractalTranspFillAttribute : public BitmapTranspFillAttribute
{
	CC_DECLARE_DYNCREATE(FractalTranspFillAttribute)
public:
	 FractalTranspFillAttribute();
	~FractalTranspFillAttribute();

	virtual NodeAttribute *MakeNode();
	virtual void SimpleCopy(AttributeValue *);

	virtual FillGeometryAttribute& operator=(FillGeometryAttribute& Attrib);
	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual BOOL AttachBitmap(KernelBitmap* pBitmap);
	virtual BOOL DetachBitmap();
	virtual BOOL CopyBitmap(KernelBitmap*);

	virtual BOOL Randomise();
	virtual BOOL RecalcFractal();

	virtual UINT32	GetFractalDim() { return Dim; }
	virtual BOOL	GetTileable()	{ return Tileable; }
	virtual UINT32	GetFractalDPI()	{ return Dpi; }

	virtual INT32	GetSeed()		{ return Seed; }
	virtual FIXED16 GetGraininess() { return Graininess; }
	virtual FIXED16 GetGravity()	{ return Gravity; }
	virtual FIXED16 GetSquash()		{ return Squash; }

	virtual void SetTesselation(INT32 NewTess);
	virtual void SetFractalDim(UINT32 NewDim);
	virtual BOOL SetTileable(BOOL NewTile);
	virtual BOOL SetFractalDPI(UINT32 NewDpi);

	virtual BOOL SetSeed(INT32 NewSeed);
	virtual BOOL SetGraininess(FIXED16 NewGrain);
	virtual BOOL SetGravity(FIXED16 NewGrav);
	virtual BOOL SetSquash(FIXED16 NewSquash);

	virtual BOOL IsAKindOfBitmapFill()	{ return TRUE; }
	virtual BOOL IsABitmapFill()		{ return FALSE; }
	virtual BOOL IsAFractalFill()		{ return TRUE; }

	virtual void CacheFractalData(FillGeometryAttribute *pCachedFractal);
	virtual BOOL IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal);

	virtual ColourFillAttribute *MakeSimilarNonTranspFillGeometry(double TransparencyScale);

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_CLOUDS); }

public:
	INT32 	Seed;
	FIXED16	Graininess;			//0..about 32
	FIXED16	Gravity;			//0..about 255
	FIXED16	Squash;

	INT32	Dpi;
	BOOL	Tileable;

protected:
	INT32 	Dim;
};


/********************************************************************************************

>	class ThreeColTranspFillAttribute : public GradTranspFillAttribute

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/8/96
	Purpose:	Specifies a Three colour graduated transparency for an object
				including the start and end points.
	SeeAlso:	GradFillAttribute

********************************************************************************************/

class ThreeColTranspFillAttribute : public GradTranspFillAttribute
{
	CC_DECLARE_DYNCREATE(ThreeColTranspFillAttribute)
public:
	ThreeColTranspFillAttribute();

	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual NodeAttribute *MakeNode();

	virtual DocCoord* GetEndPoint2() { return &EndPoint2; }
	virtual void SetEndPoint2(DocCoord* Pos);
	virtual DocCoord* GetEndPoint3() { return &EndPoint3; }
	virtual void SetEndPoint3(DocCoord* Pos);

	virtual UINT32* GetEndTransp2() { return &EndTransp2; }
	virtual void SetEndTransp2(UINT32* NewTransp);

	virtual BOOL IsPerspective() { return IsPersp; }
	virtual void MakePerspective();
	virtual void RemovePerspective();

	virtual BOOL IsAThreeColFill() { return TRUE; }

	virtual ColourFillAttribute *MakeSimilarNonTranspFillGeometry(double TransparencyScale);
	virtual BOOL SupportsFillRamps() { return FALSE; }

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_3POINT); }

public:
	DocCoord EndPoint2;
	DocCoord EndPoint3;

	BOOL IsPersp;

	UINT32 EndTransp2;
};


/********************************************************************************************

>	class FourColTranspFillAttribute : public ThreeColTranspFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	Specifies a FourCol graduated fill for an object, including the start and
				end points.
	SeeAlso:	GradFillAttribute

********************************************************************************************/

class FourColTranspFillAttribute : public ThreeColTranspFillAttribute
{
	CC_DECLARE_DYNCREATE(FourColTranspFillAttribute)
public:
	FourColTranspFillAttribute();

	virtual INT32 operator==(const FillGeometryAttribute& Attrib);

	virtual NodeAttribute *MakeNode();

	virtual UINT32* GetEndTransp3() { return &EndTransp3; }
	virtual void SetEndTransp3(UINT32* NewTransp);

	virtual BOOL IsAFourColFill() { return TRUE; }

	virtual ColourFillAttribute *MakeSimilarNonTranspFillGeometry(double TransparencyScale);

	virtual INT32 GetGeometryShape() { return(FILLSHAPE_4POINT); }

public:
	UINT32 EndTransp3;
};



/********************************************************************************************

>	class FillMappingAttribute : public AttributeValue

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Specifies how a fill graduation behaves, e.g. linear, sinusoidal and
				so on.
	SeeAlso:	FillFunctionLinearAttribute; FillFunctionSinAttribute

********************************************************************************************/

class FillMappingAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(FillMappingAttribute)
public:
	FillMappingAttribute();

	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);
	static BOOL Init();

	virtual BOOL IsDifferent(AttributeValue*);
	virtual FillMappingAttribute& operator=(FillMappingAttribute& Attrib);
	virtual INT32 operator==(const FillMappingAttribute& Attrib);

	INT32 Repeat;
};

/********************************************************************************************

>	class FillMappingLinearAttribute : public FillMappingAttribute

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Specifies a linear fill mapping for a filled object.  This is used for
				normal graduated fills.
	SeeAlso:	FillMappingAttribute

********************************************************************************************/

class FillMappingLinearAttribute : public FillMappingAttribute
{
	CC_DECLARE_DYNCREATE(FillMappingLinearAttribute)
public:
	FillMappingLinearAttribute() : FillMappingAttribute() {}

	virtual NodeAttribute *MakeNode();
};

/********************************************************************************************

>	class FillMappingSinAttribute : public FillMappingAttribute

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Specifies a sinusoidal fill mapping for a filled object.  i.e. the graduation
				sort of wibbles from one colour to another in a sort of whirly sinusoidal
				fashion.
	SeeAlso:	FillMappingAttribute

********************************************************************************************/

class FillMappingSinAttribute : public FillMappingAttribute
{
	CC_DECLARE_DYNCREATE(FillMappingSinAttribute)
public:
	FillMappingSinAttribute() : FillMappingAttribute() {}

	virtual NodeAttribute *MakeNode();
};


/********************************************************************************************

>	class FillEffectAttribute : public AttributeValue

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Specifies the fill 'effect', i.e. how it passes from one colour to another.
				e.g. fade, rainbow, alternate rainbow
	SeeAlso:	AttributeValue

********************************************************************************************/

class FillEffectAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(FillEffectAttribute)
public:
	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);
	static BOOL Init();

	virtual BOOL IsDifferent(AttributeValue*);
	virtual FillEffectAttribute& operator=(FillEffectAttribute& Attrib);
	virtual INT32 operator==(const FillEffectAttribute& Attrib);
};


/********************************************************************************************

>	class FillEffectFadeAttribute : public FillEffectAttribute

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Specifies a 'fade' graduation for fills, i.e. a simple fade in RGB space.
	SeeAlso:	FillEffectAttribute.

********************************************************************************************/

class FillEffectFadeAttribute : public FillEffectAttribute
{
	CC_DECLARE_DYNCREATE(FillEffectFadeAttribute)
public:
	virtual NodeAttribute *MakeNode();
};


/********************************************************************************************

>	class FillEffectRainbowAttribute : public FillEffectAttribute

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Specifies a 'rainbow' graduation for fills, i.e. a fade through the HSV
				cone.
	SeeAlso:	FillEffectAttribute

********************************************************************************************/

class FillEffectRainbowAttribute : public FillEffectAttribute
{
	CC_DECLARE_DYNCREATE(FillEffectRainbowAttribute)
public:
	virtual NodeAttribute *MakeNode();
};


/********************************************************************************************

>	class FillEffectAltRainbowAttribute : public FillEffectAttribute

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Specifies an 'alternate rainbow' graduation for fills, i.e. the same as
				a FillEffectRainbowAttribute, but it goes around the HSV cone in the
				opposite direction.
	SeeAlso:	FillEffectAttribute

********************************************************************************************/

class FillEffectAltRainbowAttribute : public FillEffectAttribute
{
	CC_DECLARE_DYNCREATE(FillEffectAltRainbowAttribute)
public:
	virtual NodeAttribute *MakeNode();
};

/********************************************************************************************

>	class FillTranpsMappingAttribute : public AttributeValue

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	Specifies how a fill graduation behaves, e.g. linear, sinusoidal and
				so on.
	SeeAlso:	FillFunctionLinearAttribute; FillFunctionSinAttribute

********************************************************************************************/

class TranspFillMappingAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(TranspFillMappingAttribute)
public:
	TranspFillMappingAttribute();

	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);
	static BOOL Init();

	virtual FillMappingAttribute *MakeSimilarNonTranspFillMapping(void);

	virtual BOOL IsDifferent(AttributeValue*);
	virtual TranspFillMappingAttribute& operator=(TranspFillMappingAttribute& Attrib);
	virtual INT32 operator==(const TranspFillMappingAttribute& Attrib);

	INT32 Repeat;
};

/********************************************************************************************

>	class TranspFillMappingLinearAttribute : public TranspFillMappingAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	Specifies a linear fill mapping for a filled object.  This is used for
				normal graduated fills.
	SeeAlso:	TranspFillMappingAttribute

********************************************************************************************/

class TranspFillMappingLinearAttribute : public TranspFillMappingAttribute
{
	CC_DECLARE_DYNCREATE(TranspFillMappingLinearAttribute)
public:
	TranspFillMappingLinearAttribute() : TranspFillMappingAttribute() {}

	virtual FillMappingAttribute *MakeSimilarNonTranspFillMapping(void);

	virtual NodeAttribute *MakeNode();
};

/********************************************************************************************

>	class TranspFillMappingSinAttribute : public TranspFillMappingAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	Specifies a sinusoidal fill mapping for a filled object.  i.e. the graduation
				sort of wibbles from one colour to another in a sort of whirly sinusoidal
				fashion.
	SeeAlso:	TranspFillMappingAttribute

********************************************************************************************/

class TranspFillMappingSinAttribute : public TranspFillMappingAttribute
{
	CC_DECLARE_DYNCREATE(TranspFillMappingSinAttribute)
public:
	TranspFillMappingSinAttribute() : TranspFillMappingAttribute() {}

	virtual FillMappingAttribute *MakeSimilarNonTranspFillMapping(void);

	virtual NodeAttribute *MakeNode();
};

/********************************************************************************************

>	class StrokeColourAttribute : public ColourFillAttribute

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/06/94
	Purpose:	Represent a colour for a stroke, as opposed to a fill.
				Implementation is very similar (identical!) to the FillColourAttribute
				class.
	SeeAlso:	AttributeValue

********************************************************************************************/

class StrokeColourAttribute : public ColourFillAttribute
{
	CC_DECLARE_DYNCREATE(StrokeColourAttribute)
public:
	StrokeColourAttribute() {}
	StrokeColourAttribute(DocColour& NewColour);
	virtual NodeAttribute *MakeNode();
	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);

	static BOOL Init();
};

/********************************************************************************************

>	class StrokeTranspAttribute : public TranspFillAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Purpose:	Represent the transparency of a stroke, as opposed to a fill.
				Implementation is very similar (identical!) to the FillTranspAttribute
				class.
	SeeAlso:	AttributeValue

********************************************************************************************/

class StrokeTranspAttribute : public TranspFillAttribute
{
	CC_DECLARE_DYNCREATE(StrokeTranspAttribute)
public:
	StrokeTranspAttribute() {}
	StrokeTranspAttribute(UINT32 NewTransp);
	virtual NodeAttribute *MakeNode();
	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);

	BOOL Blend(BlendAttrParam*);

	static BOOL Init();
};

/********************************************************************************************

>	class MouldAttribute : public AttributeValue

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Specifies wether the attributes are moulded or not.
	SeeAlso:	AttributeValue

********************************************************************************************/

class MouldAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(MouldAttribute)
public:
	MouldAttribute();
	MouldAttribute(NodeMould* pMould, MouldGeometry* pMouldShape);

	virtual NodeAttribute *MakeNode();
	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);

	static BOOL Init();

	MouldGeometry* GetMouldShape() { return pMouldShape; }
	void SetMouldShape(MouldGeometry*);

	BOOL IsPerspective() { return IsPerspectiveMould; }	

	NodeMould* GetParentMould() { return pParentMould; }
	void SetParentMould(NodeMould*);

protected:
	MouldGeometry* pMouldShape;
	BOOL IsPerspectiveMould : 1;

	NodeMould* pParentMould;
};

#endif  // INC_FILLVAL
