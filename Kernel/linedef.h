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

// The line definition class header.  An abstract base class from which to
// derive the definitions of things like strokes, brushes.


#ifndef INC_LINEDEF
#define INC_LINEDEF


/********************************************************************************************

>	class LineDefinition : public CCObject

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/97

	Purpose:	Stores a vector Line definition
				A static list of these items is held in the LineComponent.

	SeeAlso:	LineComponent

********************************************************************************************/

class LineDefinition : public CCObject
{
CC_DECLARE_DYNAMIC(LineDefinition);

public:
	LineDefinition();
	LineDefinition(Node *pLineTree);

	~LineDefinition();

	void SetLineName(StringBase *pName);
	
	/* StrokeDefinition functions - not sure yet if we want to keep them
	in the base class
	void SetLineRepeating(BOOL Repeating);
	void SetNumLineRepeats(INT32 NumRepeats);
	*/
	void SetOverrideFill(BOOL Override)		{ m_bOverridesFill  = Override; };
	void SetOverrideTrans(BOOL Override)	{ m_bOverridesTrans = Override; };

	static void ConvertIndexedColours(Spread* pSpread);

public:
	virtual BOOL IsDifferent(LineDefinition *pOther) = 0;
			// Returns TRUE if these Line definitions are not the same

	inline Node *GetLineTree(void)		{ return(m_pTree); };
			// Get at the Line clipart tree

	inline String_32 *GetLineName(void)	{ return(&m_Name); };
	
	/*
	inline BOOL IsRepeating(void) const		{ return(Repeating); };
			// Determine if this Line should repeat or stretch onto the destination path

	inline INT32 NumRepeats(void) const		{ return(Repeats); };
			// Determine if this Line should repeat or stretch onto the destination path
	*/
	inline BOOL OverrideFill(void) const	{ return(m_bOverridesFill); };
	inline BOOL OverrideTrans(void) const	{ return(m_bOverridesTrans); };
			// Determine if this Line overrides the fill/transparency geometry with its own ones
	
	BOOL NeedsTransparency(void) const;
			// Determine if this Line needs transparency to render correctly

	inline void  SetIOStore(UINT32 NewValue)		{ IOStore = NewValue; };
	inline UINT32 ReadIOStore(void) const		{ return(IOStore); };

protected:
	Node *m_pTree;				// A clipart subtree which defines the Line

	String_32 m_Name;				// The name of this Line (as shown in Line gallery)
	
	/*
	INT32 Repeats;				// Number of times a brush should repeat along the Line, or zero for optimal
	BOOL Repeating;				// TRUE if the Line should repeat along the path, FALSE if it should stretch
	*/
	BOOL m_bOverridesFill;			// TRUE if the Line uses its own fills rather than the current fill colour
	BOOL m_bOverridesTrans;		// TRUE if the Line uses its own transparencies rather than the current one

	BOOL m_bNeedsTrans;			// TRUE if the Line needs transparency in order to render

	UINT32 IOStore;				// Used during export to indicate whether this definition has been saved
								// Used during import to store the handle associated with this Line
								// in the file, so we can re-map handles into our array on import.
};


#endif
