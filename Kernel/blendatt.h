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
// Defines a class that is supplied as a parameter to the NodeAttribute::Blend() and 
// AttributeValue::Blend() functions
//
// Implemented in attr.cpp

#ifndef INC_BLENDATTRPARAM
#define INC_BLENDATTRPARAM

class CCAttrMap;
class NodeAttribute;
class AttributeValue;
class NodeRenderableInk;
class BlendPath;
class NodeBlendPath;
class HandleBecomeA;
class SumAllPathsPathProcessor;
class NodeBlend;

enum ColourBlendType	{  	COLOURBLEND_FADE,
							COLOURBLEND_RAINBOW,
							COLOURBLEND_ALTRAINBOW,
							COLOURBLEND_NONE
						};

enum EditState {EDIT_STEPS, EDIT_DISTANCE,};
enum EndObject {NONE, FIRST, LAST};

/********************************************************************************************

>	class BlendAttrParam : public CC_CLASS_MEMDUMP

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Purpose:	Class that encapsulates the params needed by virtual NodeAttribute::Blend()

	NOTE:  GCS (27/7/2000):  I have ammended this class to allow blends to specify whether
	blending should affect all attributes data excluding position stuff.  The type of blending
	now done is determined by the slider that is dragged within the blend tool.

	This is particularly true of fills - who contain both 'colour' data and position data.
	When profiling the attributes of the blend, we do not want to alter the position data with
	respect to the attribute profile.

********************************************************************************************/

class BlendAttrParam : public CC_CLASS_MEMDUMP
{
CC_DECLARE_MEMDUMP(BlendAttrParam);
public:
	BlendAttrParam();

	// The destructor will delete it's internal node attr and attr val ptrs (set by SetBlendedAttr()
	// and SetBlendedAttrVal() repectively)	if they are not NULL
	~BlendAttrParam();

	// This sets up the object - See attr.cpp for the implementation of this function
	BOOL Init(RenderRegion *pThisRenderRegion,
			  NodeAttribute* pThisOtherAttr, double ThisBlendRatio,
			  ColourBlendType ColBlend, CCAttrMap* pStartAttrMap, CCAttrMap* pEndAttrMap, BOOL LastStep=FALSE);

	BOOL Init(RenderRegion *pThisRenderRegion,
			  NodeAttribute* pThisOtherAttr, double ThisBlendRatio, double thisObjectRatio, BOOL objProcess,
			  ColourBlendType ColBlend, CCAttrMap* pStartAttrMap, CCAttrMap* pEndAttrMap, BOOL LastStep=FALSE);

	// Member variable access functions
	RenderRegion*		GetRenderRegion()	{ return pRenderRegion; }
	NodeAttribute*		GetOtherAttr()		{ return pOtherNodeAttr; }
	AttributeValue*		GetOtherAttrVal()	{ return pOtherAttrVal; }
	double				GetBlendRatio()		{ return BlendRatio; }
	double				GetObjectRatio()	{ return objectRatio; }
	BOOL				GetObjectProfileProcessing () { return (objectProfileProcessing); }
	ColourBlendType		GetColourBlend()	{ return ColourBlend; }

	CCAttrMap*			GetStartAttrMap()   { return pAttrMapStart; }
	CCAttrMap*			GetEndAttrMap()     { return pAttrMapEnd; }

	// When you call these two get funcs, the pointer passed back becomes your responsibility (if its not NULL).
	// This means that you will have to delete it if you don't want to cause memory leaks.
	// Also, you will get NULL for all subsequent calls to the 'Get' func, until a new value is set by
	// it's sister 'Set' func.
	//
	// I.e. GetBlendedAttr() returns it's internal node attr ptr, then sets it to NULL
	NodeAttribute*		GetBlendedAttr();
	AttributeValue*		GetBlendedAttrVal();

	// When you call these funcs, the object you pass it becomes the property of the BlendAttrParam object.
	// 
	// If it's internal pointer is the same as the entry param, nothing happends
	// If it's internal pointer is not NULL and different to the entry param, the old one is deleted first.
	void SetBlendedAttr(NodeAttribute* pAttr);
	void SetBlendedAttrVal(AttributeValue* pAttrVal);

	// See if we are blending the last blend step
	// NB you need to call Init(..) with an accurately calcualted LastStep param
	// (see NodeBlend::BlendAttributes for an example)
	BOOL					IsLastBlendStep() { return m_bLastStep; }

private:
	RenderRegion*			pRenderRegion;

	NodeAttribute*			pOtherNodeAttr;
	AttributeValue*			pOtherAttrVal;
	double					BlendRatio;

	NodeAttribute*			pBlendedNodeAttr;
	AttributeValue*			pBlendedAttrVal;

	CCAttrMap* 				pAttrMapStart;
	CCAttrMap* 				pAttrMapEnd;

	ColourBlendType			ColourBlend;

	BOOL					m_bLastStep;

	// CGS ....
	
	double objectRatio;				// the 'matching' object ratio
	
	BOOL objectProfileProcessing;	// are we blending with respect to position data?
};

/********************************************************************************************

>	class BlendNodeParam : public CC_CLASS_MEMDUMP

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/2000
	Purpose:	Class which encapsulates data for blending between nodes
				We need all these so we don't duplicate code between this node
				and the blender (unfortunately)

********************************************************************************************/
class BlendNodeParam
{
public:
	BlendNodeParam()
	{
		m_pNodeBlend = NULL;
		m_pStartPath = NULL;
		m_pEndPath = NULL;
		m_BlendRatio = 0;
		m_AttrBlendRatio = 0;
		m_pRegion = NULL;
		m_pBlenderPath = NULL;
		m_AngleStart = 0;
		m_AngleEnd = 0;
		m_ObjectRatio = 0;
		m_InvObjectRatio = 0;
		m_ColourBlendType = COLOURBLEND_FADE;
		m_pNodeStart = NULL;
		m_pNodeEnd   = NULL;
		m_bIsOneToOne = FALSE;
		m_pHandleBecomeA = NULL;
		m_pPathProcessor = NULL;
	}

	void Init(NodeBlend* pBlender, RenderRegion * pRegion, BlendPath * pStartPath, BlendPath * pEndPath,
		double BlendRatio, double AttrBlendRatio,
		double AngleStart, double AngleEnd, double ObjectRatio,
		double InvObjectRatio, ColourBlendType ColBType,
		NodeRenderableInk * pNodeStart,
		NodeRenderableInk * pNodeEnd,
		BOOL IsOneToOne,
		NodeBlendPath * pBlenderPath = NULL,
		HandleBecomeA* pHandleBecomeA = NULL,
		SumAllPathsPathProcessor* pPathProcessor = NULL)
	{
		m_pNodeBlend		= pBlender;
		m_pStartPath		= pStartPath;
		m_pEndPath			= pEndPath;
		m_BlendRatio		= BlendRatio;
		m_pRegion			= pRegion;
		m_pBlenderPath		= pBlenderPath;
		m_AngleStart		= AngleStart;
		m_AngleEnd			= AngleEnd;
		m_ObjectRatio		= ObjectRatio;
		m_InvObjectRatio	= InvObjectRatio;
		m_ColourBlendType   = ColBType;
		m_pNodeStart		= pNodeStart;
		m_pNodeEnd			= pNodeEnd;
		m_bIsOneToOne		= IsOneToOne;
		m_AttrBlendRatio    = AttrBlendRatio;
		m_pHandleBecomeA	= pHandleBecomeA;
		m_pPathProcessor	= pPathProcessor;
	}

	// alternative way of initialising
	// takes all values from the blend node param, but just takes 2 blend paths
	void Init(BlendNodeParam *pParam, BlendPath * pStartPath, BlendPath * pEndPath,
		BOOL bInverse = FALSE)
	{
		m_pNodeBlend		= pParam->GetNodeBlend ();
		m_pStartPath		= pStartPath;
		m_pEndPath			= pEndPath;

		m_pRegion			= pParam->GetRenderRegion();
		m_pBlenderPath		= pParam->GetNodeBlendPath();
		m_AngleStart		= pParam->GetAngleEnd();
		m_AngleEnd			= pParam->GetAngleStart();
		m_ObjectRatio		= pParam->GetObjectRatio();
		m_InvObjectRatio	= pParam->GetInvertedAttributeRatio();
		m_ColourBlendType   = pParam->GetColourBlendType();
		m_pNodeStart		= pParam->GetNodeStart();
		m_pNodeEnd			= pParam->GetNodeEnd();
		m_bIsOneToOne		= pParam->GetOneToOne();
		m_pHandleBecomeA	= pParam->GetHandleBecomeA ();
		m_pPathProcessor	= pParam->GetPathProcessor ();
		
		if (!bInverse)
		{
			m_AttrBlendRatio    = pParam->GetAttrBlendRatio();
			m_BlendRatio		= pParam->GetBlendRatio();
		}
		else
		{
			m_AttrBlendRatio    = 1.0 - pParam->GetAttrBlendRatio();
			m_BlendRatio		= 1.0 - pParam->GetBlendRatio();
		}

	}

	BlendPath * GetStartBlendPath()		{ return m_pStartPath; }
	BlendPath * GetEndBlendPath()		{ return m_pEndPath; }
	double GetBlendRatio()				{ return m_BlendRatio; }
	double GetAttrBlendRatio()			{ return m_AttrBlendRatio; }
	RenderRegion * GetRenderRegion()    { return m_pRegion; }
	NodeBlendPath* GetNodeBlendPath()	{ return m_pBlenderPath; }
	double GetAngleStart()				{ return m_AngleStart; }
	double GetAngleEnd()				{ return m_AngleEnd; }
	double GetObjectRatio()				{ return m_ObjectRatio; }
	double GetInvertedAttributeRatio()  { return m_InvObjectRatio; }
	ColourBlendType GetColourBlendType() { return m_ColourBlendType; }
	BOOL GetOneToOne()					{ return m_bIsOneToOne; }

	void SetBlendRatio(double Ratio)	{ m_BlendRatio = Ratio; }

	NodeRenderableInk * GetNodeStart()  { return m_pNodeStart; }
	NodeRenderableInk * GetNodeEnd()	{ return m_pNodeEnd; }

	HandleBecomeA* GetHandleBecomeA () { return (m_pHandleBecomeA); }

	// CGS:  support for convert to editable shapes ....

	SumAllPathsPathProcessor* GetPathProcessor () { return (m_pPathProcessor); }
	void SetPathProcessor (SumAllPathsPathProcessor* proc) { m_pPathProcessor = proc; }

	NodeBlend* GetNodeBlend () { return (m_pNodeBlend); }

private:
	NodeBlend* m_pNodeBlend;		// CGS:  ptr to the creator NodeBlend.  I need this so
									// that I may delete things correctly ....
	
	BlendPath * m_pStartPath;
	BlendPath * m_pEndPath;
	NodeBlendPath * m_pBlenderPath; // the path for the blender
	RenderRegion * m_pRegion;
	double m_BlendRatio;
	double m_AttrBlendRatio;
	double m_AngleStart;
	double m_AngleEnd;
	double m_ObjectRatio;
	double m_InvObjectRatio;
	ColourBlendType m_ColourBlendType;
	NodeRenderableInk * m_pNodeStart;
	NodeRenderableInk * m_pNodeEnd;
	BOOL m_bIsOneToOne;
	
	// CGS:  allows us to detect whether we are rendering, or whether we are
	// converting to editable shapes
	HandleBecomeA* m_pHandleBecomeA;
	
	// CGS:  by storing a ptr to the path processor we can convert to editable shapes
	SumAllPathsPathProcessor* m_pPathProcessor;
} ;

#endif  // INC_BLENDATTRPARAM


