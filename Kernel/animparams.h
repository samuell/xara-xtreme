// $Id: animparams.h 793 2006-04-10 09:36:42Z alex $
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

#ifndef INC_ANIMPARAMS
#define INC_ANIMPARAMS

#include "ops.h"
#include "paldefs.h"

/*******************************************************************************************************

  class FramePropertiesParam : publc OpParam

  Author:	Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
  Created:	30/04/97
  Purpose:	Allows the transfer of data from our Frame Gallery to our current layer object.
				
********************************************************************************************************/

class FramePropertiesParam: public OpParam
{

//	CC_DECLARE_DYNAMIC(FramePropertiesParam)

public:
	FramePropertiesParam();

	String_256& GetFrameName()								{ return m_FrameName;	};
	void		SetFrameName(const String_256& Name)		{ m_FrameName = Name;	};
	UINT32		GetAnimDelay () const						{ return m_FrameDelay;	};
	void		SetAnimDelay(const UINT32& Delay)			{ m_FrameDelay = Delay; };
	BOOL		GetOverlay()								{ return m_Overlay;		};
	void		SetOverlay (const BOOL& Val)				{ m_Overlay = Val;		};
	BOOL		GetSolid()									{ return m_Solid;		};
	void		SetSolid (const BOOL& Val)					{ m_Solid = Val;		};

protected:
	String_256 m_FrameName;				// Name of the selected bitmap.
	UINT32	   m_FrameDelay;			// Delay between frames for Animations.
	BOOL m_Overlay;						// Set if the previous layer is said to overlay the
										//	previous one rathar than  obscure it.
	BOOL m_Solid;						// A solid frame/layer is like a background layer in
										// that it forms the background for frames above it.
};


/*******************************************************************************************************

  class AnimPropertiesParam : publc OpParam

  Author:	Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
  Created:	30/04/97
  Purpose:	Allows the transfer of data from our Frame Gallery to a current spread object.
				
********************************************************************************************************/
class AnimPropertiesParam: public OpParam
{
	CC_DECLARE_DYNAMIC(AnimPropertiesParam)

public:
	AnimPropertiesParam();

	DWORD			GetAnimLoop()const							{ return m_Animloop;			};
	void			SetAnimLoop(const DWORD& Loop)				{ m_Animloop = Loop;			};
	DWORD			GetGlobalAnimDelay() const					{ return m_GlobalAnimDelay;		};
	void			SetGlobalanimDelay(const DWORD& Delay)		{ m_GlobalAnimDelay = Delay;	};
	DITHER			GetDither()const							{ return m_Dither;				};
	void			SetDither(const DITHER& dither)				{ m_Dither = dither;			};
	WEB_PALETTE		GetPalette() const							{ return m_Palette;				};
	void			SetPalette(const WEB_PALETTE& Palette)		{ m_Palette = Palette;			};
	DWORD			GetNumColsInPalette() const					{ return m_NumColsInPalette;	};
	void			SetNumColsInPalette(const DWORD& num)		{ m_NumColsInPalette = num;		};
	PALETTE_COLOURS	GetPaletteCols() const						{ return m_PaletteColours;		};
	void			SetPaletteCols(const PALETTE_COLOURS& Pal)	{ m_PaletteColours = Pal;		};
	BOOL 			GetUseSystemCols() const					{ return m_UseSystemColours;	};
	void			SetUseSystemCols(const BOOL &Val)			{ m_UseSystemColours = Val;		};
	BOOL 			GetIsBackGroundTransp() const				{ return m_BackGroundIsTransp;	};
	void			SetIsBackGroundTransp(const BOOL &Val)		{ m_BackGroundIsTransp = Val;	};

	AnimPropertiesParam& operator=(const AnimPropertiesParam& obj);

protected:

	DWORD			m_Animloop;	
	DWORD		 	m_GlobalAnimDelay; 		
	DITHER			m_Dither;				// Dither type
	WEB_PALETTE		m_Palette;				// Global\Local
	PALETTE_COLOURS m_PaletteColours;		// Browser\Optimised
	DWORD			m_NumColsInPalette;
	BOOL			m_UseSystemColours;
	BOOL			m_BackGroundIsTransp;

public:
	// Stores and gives access to the bounding rectangle of the export
	// If this changes then we will need to regenerate all frames
	void			SetBoundingRect(const DocRect& BoundingRect)	{ m_BoundingRect = BoundingRect; };
	DocRect			GetBoundingRect() const							{ return m_BoundingRect;		};
	
	// Stores and gives access to the capture quality of the export
	// If this changes then we will need to regenerate all frames
	void			SetAnimationQuality(const Quality& NewQuality)	{ m_Quality = NewQuality; };
	Quality			GetAnimationQuality() const						{ return m_Quality;		};

protected:
	DocRect			m_BoundingRect;
	Quality			m_Quality;
};

#endif	// INC_ANIMPARAMS

