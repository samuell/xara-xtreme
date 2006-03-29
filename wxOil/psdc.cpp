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

// Device context used for printing to PostScript devices.

/*
*/

#include "camtypes.h"

DECLARE_SOURCE("$Revision: 662 $");

#define new CAM_DEBUG_NEW

#include "psdc.h"

#include "errors.h"
#include "camelot.h"
#include "prntview.h"
#include "psrndrgn.h"
#include "app.h"
#include "fontman.h"

/********************************************************************************************

>	PSDCFontInfo::PSDCFontInfo()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Purpose:	Initialisation of the font cache information.
	SeeAlso:	PSDCFontInfo

********************************************************************************************/

PSDCFontInfo::PSDCFontInfo()
{
	Rotation = FIXED16(0);
}


/********************************************************************************************

>	PSPrintDC::PSPrintDC(CDC *pDC)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/95
	Inputs:		The device context of the PostScript printer.
	Purpose:	Initialise a DC for printing PostScript.

********************************************************************************************/

PSPrintDC::PSPrintDC(CDC *pDC) : KernelDC(pDC, RENDERTYPE_PRINTER_PS)
{
	// Initialise the buffer to being empty.
	Buffer.nCount = 0;
	Buffer.Data[0] = 0;

	// Work out which Escape to use - OS-dependent.
	if (IsWin32NT() || IsWin32c())
	{
		// Do not use POSTSCRIPT_DATA - it screws up under NT (the buffer pointers
		// seem to get out of sync).
		PassThruEscape = POSTSCRIPT_PASSTHROUGH; // Fails with Win16 driver
	}
	else
	{
		PassThruEscape = PASSTHROUGH;
	}

	// No view yet
	pView = NULL;

	// We haven't set up the Camelot context yet, and no-one has asked to do any GDI
	// output yet.
	GDIContextIsCurrent = TRUE;
	SafeToUseGDI = 0;

	// Once per page
	StartOfPage = TRUE;

	// No fonts yet
	FontInfo.pRenderFont = NULL;
	FontInfo.pOldFont = NULL;

	// No render region yet.
	pPSRegion = NULL;

	// Suppress the output of a postscript clipping region
	UsePSLevelClipping = FALSE;
}


/********************************************************************************************

>	PSPrintDC::~PSPrintDC()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/30/95
	Purpose:	Clean up the fonts in the DC.

********************************************************************************************/

PSPrintDC::~PSPrintDC()
{
	// Deselect the normal rendering font from the DC
	if (FontInfo.pRenderFont != NULL)
	{
		SelectObject(FontInfo.pOldFont);
		FontInfo.pOldFont = NULL;
		delete FontInfo.pRenderFont;
		FontInfo.pRenderFont = NULL;
	}
}



/********************************************************************************************

>	BOOL PSPrintDC::OutputNewLine()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/95
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Causes a new line to be started in the PostScript stream.
	SeeAlso:	PSPrintDC; PSPrintDC::OutputToken
	Errors:		Disk/file/print error => ERROR1

********************************************************************************************/

BOOL PSPrintDC::OutputNewLine()
{
	static char NewLine[] = "\n";

	// Make sure we have enough room in the buffer
	if (!MakeRoomInBuffer(cc_strlenBytes(NewLine)))
		// Error occured in buffer handling.
		return FALSE;

	// Add newline to buffer
	_tcscat(Buffer.Data, NewLine);
	Buffer.nCount += cc_strlenBytes(NewLine);

	// Update line width record.
	LineWidth = 0;

	// Success
	return TRUE;
}

/********************************************************************************************

>	BOOL PSPrintDC::OutputToken(TCHAR *Str)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/95
	Inputs:		Str - the character string to write to the file.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Outputs a string token to the EPS file.
	SeeAlso:	PSPrintDC; KernelDC::OutputToken
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL PSPrintDC::OutputToken(TCHAR *Str)
{
#ifndef STANDALONE
	// Should we re-instate the Camelot context?
	if (!SafeToUseGDI && GDIContextIsCurrent)
	{
		// Yes - just save the context (so we can restore GDI's context) and put our
		// dictionary on the dict stack.
		// Must do this first or else we get an infinite loop!
		GDIContextIsCurrent = FALSE;

		// Code by Mike
		// We need to work out where we are in order to render the correct bits of postscript header.
		BOOL AtStartOfPaper = FALSE;
		BOOL AtStartOfPatch = StartOfPage;
		StartOfPage=FALSE;

		// Work out whether this is the first call to output token for this page
		if (AtStartOfPatch)
		{
			PrintView* pView = (PrintView*)(pPSRegion->GetRenderView());
			ERROR2IF(pView==NULL,FALSE,"No printview in this PS render region");
			ERROR3IF(!pView->IsKindOf(CC_RUNTIME_CLASS(PrintView)), "Not a PrintView in PSPrintDC output token");
			AtStartOfPaper = ((pView->PatchInfo.PatchNumber)==1);
		}
		
		OutputNewLine();

		// ok now check our flag. This may be the first time we've
		// been called from this PSPrintDC construction. If so we
		// need to output page header stuff.
		if (AtStartOfPaper)
		{
			// We need to get the name of the output plate here
			// this might be a bit tricky! Na easy...
			if (!pPSRegion->WritePlateName(this))
				return FALSE;
		}

		// save the graphics state
		OutputToken("save");
		OutputNewLine();

		if (!pPSRegion->WritePhotoNegative(this))
			return FALSE;

		if (AtStartOfPaper)
		{
			// We need to fill the page with black if the plate says we
			// need to negate everything.
			if (!pPSRegion->WriteRenderPaper(this))
				return FALSE;
		}

		// Write out our procset.
		if (!pPSRegion->WriteProlog(this))
			// Error 
			return FALSE;

		OutputNewLine();
		OutputToken("XaraStudio1Dict begin");
		OutputNewLine();

		// Now output the setscreen function for this plate
		if (!pPSRegion->WritePlateScreen(this))
			return FALSE;

		// try to write out the sep tables if we need them
		if (!pPSRegion->WriteSepTables(this))
			return FALSE;

		// export any clip region we might need.
		if (!OutputPSClipping())
			return FALSE;
	}	

	// Special tokens
	static char Space = ' ';
	static char NewLine[] = "\n";

	if (LineWidth > 70)
	{
		// Line is getting INT32 - wrap around
		OutputNewLine();
	}

	// Pad with a space (unless at the beginning of the line)
	if (LineWidth > 0)
	{
		// Make sure we have enough room in the buffer
		if (!MakeRoomInBuffer(1))
			// Error occured in buffer handling.
			return FALSE;

		// Add space to buffer
		_tcscat(Buffer.Data, " ");
		Buffer.nCount++;

		// Update line width record.
		LineWidth++;
	}

	// Write the token out to the file
	INT32 Len = cc_strlenBytes(Str);

	// Make sure we have enough room in the buffer
	if (!MakeRoomInBuffer(Len))
		// Error occured in buffer handling.
		return FALSE;

	// Add space to buffer
	_tcscat(Buffer.Data, Str);
	Buffer.nCount += Len;

	// Update line width record.
	LineWidth += Len;

	// Success
	return TRUE;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL PSPrintDC::OutputDirect(BYTE *Buf, INT32 nBytes)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/95
	Inputs:		Buf - the bytes to send to the stream.
				nBytes - the number of bytes to send to the stream.
	Returns:	TRUE if all the bytes were sent ok;
				FALSE if not.
	Purpose:	Send bytes directly to the PostScript stream with no alteration or padding.
				Used for sending binary/hex data to stream.
				NB. The data output must NOT affect the rendering context set up by GDI
					in any way, as this function does not preserve that context (it cannot
					because it does not change the data passed in in any way).

					Do NOT use this to send commands that will render something, as the
					CTM will NOT be set up correctly for Camelot's co-ordinate system.

	SeeAlso:	KernelDC::OutputNewLine; KernelDC::OutputToken

********************************************************************************************/

BOOL PSPrintDC::OutputDirect(BYTE *Buf, INT32 nBytes)
{
	// This could be anything, so we flush the buffer first, and then write directly
	// to stream.
	if (!FlushPSBuffer())
		// Error occured in buffer handling
		return FALSE;

	// Copy data to our buffer - do it in stages if necessary
	while (nBytes > 0)
	{
		// Work out how much we can send this time around.
		INT32 nBytesToSend = min(nBytes, MAX_PSBUF);

		// Put data into our buffer
		memcpy(Buffer.Data, Buf, nBytesToSend);

		// Update buffer count and send data
		Buffer.nCount = (short) nBytesToSend;
		if (Escape(PassThruEscape, (INT32) Buffer.nCount, (LPCSTR) &Buffer, NULL) <= 0)
			// Error!
			return FALSE;

		// Update variables to reflect sending this number of bytes
		nBytes -= nBytesToSend;
		Buf += nBytesToSend;
	}

	// Clear out buffer
	Buffer.nCount = 0;
	Buffer.Data[0] = 0;

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL PSPrintDC::FlushPSBuffer()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/24/95
	Returns:	TRUE if data flushed ok;
				FALSE if not (e.g. printer/driver error)
	Purpose:	Flushes the buffer of pending PostScript data to the PostScript device.
	SeeAlso:	PSPrintDC::MakeRoomInBuffer

********************************************************************************************/

BOOL PSPrintDC::FlushPSBuffer()
{
	if (Buffer.nCount == 0)
		// Nothing to flush
		return TRUE;

	// Get system to flush any of its data first.
	Escape(FLUSHOUTPUT, NULL, NULL, NULL);

	// Send data to printer
	if (Escape(PassThruEscape, (INT32) Buffer.nCount, (LPCSTR) &Buffer, NULL) <= 0)
		// Error!
		return FALSE;

	// Clear out buffer
	Buffer.nCount = 0;
	Buffer.Data[0] = 0;

	// Ok if we get here
	return TRUE;
}


/********************************************************************************************

>	BOOL PSPrintDC::MakeRoomInBuffer(INT32 nBytesRequired)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/24/95
	Inputs:		nBytesRequired - how many bytes of free space are needed in the buffer.
	Returns:	TRUE if enough space was found;
				FALSE if not.
	Purpose:	Make sure there is enough room in the PostScript buffer to add the
				specified number of bytes to the stream.  If not, the buffer is flushed
				to the device.
	Errors:		Printer/driver error => ERROR1
				Buffer cannot accomodate requested size even when empty => ERROR2
	SeeAlso:	PSPrintDC::FlushPSBuffer

********************************************************************************************/

BOOL PSPrintDC::MakeRoomInBuffer(INT32 nBytesRequired)
{
	if (nBytesRequired >= MAX_PSBUF)
	{
		// Can't ever do that many bytes!
		ERROR2(FALSE, "Too many bytes asked for in PSPrintDC::MakeRoomInBuffer()");
	}

	// Do we need to flush?
	if (nBytesRequired > (MAX_PSBUF - Buffer.nCount))
		// Yes
		return FlushPSBuffer();

	// All ok if we get here
	return TRUE;
}


/********************************************************************************************

>	BOOL PSPrintDC::FlushDC()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/25/95
	Returns:	TRUE if ok;
				FALSE if not (e.g. output problem)
	Purpose:	Tidy up the PostScript DC and reinstate the GDI context if necessary.
				Should be called when printing is finished.  (Won't cause problems if
				called more often - just more output).
	SeeAlso:	PSPrintDC::FlushPSBuffer()

********************************************************************************************/

BOOL PSPrintDC::FlushDC()
{
	// Restore OS context - just pretend we want to do some OS output
	//if (!StartOSOutput())
		// Error
	//	return FALSE;

	// Clear out the buffer.
	if (!FlushPSBuffer())
		// Error
		return FALSE;

	// Go back to using Camelot context
	// (NB this is deferred - it will only be reinstated if we do Camelot output)
	//return EndOSOutput();
	return TRUE;
}



/********************************************************************************************

>	BOOL PSPrintDC::OutputUserSpaceValue(MILLIPOINT n, 
									     EPSAccuracy Accuracy = ACCURACY_NORMAL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/95
	Inputs:		n - the value (in millipoints) to output.
				Accuracy - optional parameter - ignored for GDI PostScript output as GDI
						   sets up a coordinate system where 1 user space unit = 1 device unit.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1

	Purpose:	Output a 'user space' value to the device context.
				User space is the coordinate system used for PostScript files.
				GDI sets up the CTM so that 1 user space unit = 1 device unit, so all we
				do is use the rendering matrix to convert to Windows co-ordinates and output
				this directly at 0dp accuracy (because a fractional user space value would
				equate to a fractional device pixel, which has no effect on output).

	SeeAlso:	KernelDC::OutputCoord; KernelDC::OutputToken;
				KernelDC::SetFullAccuracy
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL PSPrintDC::OutputUserSpaceValue(MILLIPOINT n, EPSAccuracy Accuracy)
{
	// We need a view to scale the value.
	ERROR2IF(pView == NULL, FALSE, "No view attached to PostScript DC!");

	// Use pixel size to scale to device units.
	n /= PixelSize;

	// Output as device units
	return OutputValue(n);
}


/********************************************************************************************

>	BOOL PSPrintDC::OutputCoord(DocCoord& Coord, EPSAccuracy Accuracy = ACCURACY_NORMAL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		Coord - the coordinate to write out to the file.
				Accuracy - optional parameter - ignored for GDI PostScript output as GDI
						   sets up a coordinate system where 1 user space unit = 1 device unit.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Write out a coordinate x,y pair to the export file, automatically 
				converting from spread coordinates to the user space coordinate system.
				User space is the coordinate system used for PostScript files.
				GDI sets up the CTM so that 1 user space unit = 1 device unit, so all we
				do is use the rendering matrix to convert to Windows co-ordinates and output
				this directly at 0dp accuracy (because a fractional user space value would
				equate to a fractional device pixel, which has no effect on output).

	SeeAlso:	PSPrintDC::OutputUserSpaceValue
	Errors:		File/disk error => ERROR1

********************************************************************************************/

BOOL PSPrintDC::OutputCoord(DocCoord& Coord, EPSAccuracy Accuracy)
{
	// We need a view to scale the coords.
	ERROR2IF(pView == NULL, FALSE, "No view attached to PostScript DC!");

	// Use the rendering matrix of our parent render region to transform
	// the co-ordinate to device units before proceeding.
	OilCoord NewCoord(Coord.x, Coord.y);
	RenderMatrix.transform(&NewCoord);

	// Convert to Windows device units.
	WinCoord PSCoord = NewCoord.ToWin(pView);

	// Output to PostScript stream.
	BOOL Ok = (OutputValue(PSCoord.x) && OutputValue(PSCoord.y));

	return Ok;
}


/********************************************************************************************

>	void PSPrintDC::SetDCTransforms(Matrix NewRenderMatrix, View *pNewView)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/95
	Inputs:		The matrix and view to transform user space values with.
	Purpose:	Inform the DC how to transform user space values when performing output
				via OutputCoord. and OutputUserSpaceValue().
	SeeAlso:	PSPrintDC::OutputCoord; PSPrintDC::OutputUserSpaceValue

********************************************************************************************/

void PSPrintDC::SetDCTransforms(Matrix NewRenderMatrix, View *pNewView)
{
	// Remember these values
	RenderMatrix = NewRenderMatrix;
	pView = pNewView;

	// Get the pixel size from the view
	FIXED16 fxPixelWidth, fxPixelHeight;
	pView->GetScaledPixelSize(&fxPixelWidth, &fxPixelHeight);
	PixelSize = (MILLIPOINT) fxPixelWidth.MakeLong();
}


/********************************************************************************************

>	WinCoord PSPrintDC::TransformCoord(const DocCoord &Coord)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/95
	Inputs:		Coord - the co-ordinate to transform.
	Returns:	The transformed co-ordinate.
	Purpose:	Transform a DocCoord to a WinCoord, using this DC's transformations, as set
				up by SetDCTransforms().
	SeeAlso:	PSPrintDC::SetDCTransforms

********************************************************************************************/

WinCoord PSPrintDC::TransformCoord(const DocCoord &Coord)
{
	// Use the rendering matrix of our parent render region to transform
	// the co-ordinate to device units before proceeding.
	OilCoord NewCoord(Coord.x, Coord.y);
	RenderMatrix.transform(&NewCoord);

	// Convert to Windows device units.
	WinCoord PSCoord = NewCoord.ToWin(pView);

	// Return to caller
	return PSCoord;
}


/********************************************************************************************

>	BOOL PSPrintDC::StartOSOutput()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/30/95
	Returns:	TRUE if ok;
				FALSE if not (e.g. file/printer/driver error).
	Purpose:	Restore the PostScript DC to the context expected by the host OS (GDI), so 
				we can use OS (GDI) functions.
				You must call EndOSOutput() before calling any Camelot rendering functions.
				There may be nested calls to this function.
	Errors:		File/printer error
	SeeAlso:	PSPrintDC::EndOSOutput

********************************************************************************************/

BOOL PSPrintDC::StartOSOutput()
{
	if ((SafeToUseGDI == 0) && !GDIContextIsCurrent)
	{
		// We need to restore the GDI context - this involves removing our dictionary
		// from the dictionary stack, and calling restore.
		if (!OutputToken("end restore") || !OutputNewLine())
			// Error
			return FALSE;
		
		// Flush our buffer so Camelot PS syncs correctly with GDI PS.
		if (!FlushPSBuffer())
			// Error
			return FALSE;

		GDIContextIsCurrent = TRUE;
	}

	// Increment GDI count
	SafeToUseGDI++;

	// Ok to use GDI now
	return TRUE;
}

/********************************************************************************************

>	BOOL PSPrintDC::EndOSOutput()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/30/95
	Returns:	TRUE if ok;
				FALSE if not (e.g. call has no corresponding call to StartOSOutput()).
	Purpose:	Inform the DC that we have finished doing GDI output, and so we should
				prepare the DC for Camelot PS commands (this is deferred until we actually
				do some output on the stream).
	SeeAlso:	PSPrintDC::StartOSOutput

********************************************************************************************/

BOOL PSPrintDC::EndOSOutput()
{
	// Just decrement counter - we do the other stuff in OutputToken().
	SafeToUseGDI--;
	ERROR2IF(SafeToUseGDI < 0, FALSE, "Unbalanced call to Start/EndOSOutput() functions!");

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL PSPrintDC::SelectNewFont(WORD Typeface, BOOL Bold, BOOL Italic, 
								  MILLIPOINT Width, MILLIPOINT Height)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/28/95
	Inputs:		Details of the font to select.
				NB. Width is not the actual width; it expresses the aspect ratio required.
					So, e.g. if Width == Height/2 then the aspect ratio is 50%, and so on.
	Returns:	TRUE if the font was selected ok;
				FALSE if not.
	Purpose:	Selects the specified font into the DC.  Performs clever stuff to cache
				the font so that subsequent requests for the same font don't cause anything
				to happen.

********************************************************************************************/

BOOL PSPrintDC::SelectNewFont(WORD Typeface, BOOL Bold, BOOL Italic, 
							  MILLIPOINT Width, MILLIPOINT Height, ANGLE Rotation)
{
	// Check to see if it is cached
	if ((FontInfo.pRenderFont != NULL) &&
		(FontInfo.Typeface == Typeface) &&
		(FontInfo.Bold == Bold) &&
		(FontInfo.Italic == Italic) &&
		(FontInfo.Width == Width) &&
		(FontInfo.Height == Height) &&
		(FontInfo.Rotation == Rotation))
	{
		// It is the same as the currently selected/cached font
		TRACEUSER( "Tim", _T("Using the cached font\n"));
		return TRUE;
	}

	TRACEUSER( "Tim", _T("Font cache invalid - generating font to use\n"));

	// Get ready for GDI operations
	if (!StartOSOutput())
		return FALSE;

	// Not the cached font - ok, deselect and delete the cached font.
	if (FontInfo.pOldFont != NULL)
	{
		SelectObject(FontInfo.pOldFont);
		FontInfo.pOldFont = NULL;
	}

	if (FontInfo.pRenderFont != NULL)
	{
		delete FontInfo.pRenderFont;
		FontInfo.pRenderFont = NULL;
	}

	// Create the font and select it into the DC
	TRY
	{
		FontInfo.pRenderFont = new CFont;
	}
	CATCH (CMemoryException, e)
	{
		return FALSE;
	}
	END_CATCH

	// Work out the font weight - bold or normal?
	INT32 FontWeight = Bold ? FW_BOLD : FW_NORMAL;

	// Get the typeface name of the font

	ENUMLOGFONT *pEnumLogFont = FONTMANAGER->GetEnumLogFont(Typeface);
	if (pEnumLogFont == NULL)
		// Error
		return FALSE;

	// Work out how big the font is, in logical pixels
	FIXED16 fxPixWidth, fxPixHeight;
	pView->GetScaledPixelSize(&fxPixWidth, &fxPixHeight);
	INT32 FontWidth = (INT32) (Width / fxPixWidth.MakeDouble());
	INT32 FontHeight = (INT32) (Height / fxPixHeight.MakeDouble());

	if (FontHeight == FontWidth)
	{
		// Aspect ratio is 100% - use width of 0, and the GDI font engine gives us 100%
		// ratio automatically
		FontWidth = 0;
	}
	else
	{
		// Aspect ratio is not 100% - we need to do some clever stuff to work out
		// how wide we want the font to be.  Because fonts are generally much taller
		// than they are wide, we can't just pass in FontWidth directly - we must
		// transform it to the correct width.

		// BODGETEXT: ideally, we should use a NEWTEXTMETRIC structure here, to work out
		//			  the correct width, but this involves calling EnumLogFontFamily() or
		//			  whatever, which is too slow.  As the text code caches the LOGFONT
		//			  for each font, it could also cache the NEWTEXTMETRIC structure,
		//			  which this routine could then use.
		//			  Having said all that, this code seems to work!

		// Create the font - the first time is to find out how wide the characters are,
		// so we can scale the width correctly. (So we give a width of 0 to get the normal
		// width for the given height)
		if (FontInfo.pRenderFont->CreateFont(-100, 0, 0, 0, FontWeight, Italic, 
										 	 FALSE, FALSE, DEFAULT_CHARSET, 
										 	 OUT_TT_PRECIS, CLIP_TT_ALWAYS,
						   				 	 PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
						   				 	 pEnumLogFont->elfLogFont.lfFaceName) == 0)
		{
			// Can't create font - do it as curves
			delete FontInfo.pRenderFont;
			FontInfo.pRenderFont = NULL;
			EndOSOutput();
			return FALSE;
		}

		if (FALSE)//TRUE)
		{
			FontInfo.pOldFont = SelectObject(FontInfo.pRenderFont);

			// Get the outlines to see how big the font should be...
			INT32 Widths[30];

			// First for lower case
			GetOutputCharWidth('a', 'z', Widths);
			INT32 AveCharWidth = 0;
			for (INT32 i = 0; i < 26; i++)
				AveCharWidth += Widths[i];

			// Now for upper case
			GetOutputCharWidth('A', 'Z', Widths);
			for (i = 0; i < 26; i++)
				AveCharWidth += Widths[i];

			// Ok, get average width
			AveCharWidth /= 52;

			// Scale the width by the font's aspect ratio: 

			// ActualWidth = (RealWidth / Height) * Width
			FontWidth = MulDiv(AveCharWidth, FontWidth, 100);

			// Select old font back into DC
			SelectObject(FontInfo.pOldFont);
		}
		else
		{
			// Get the metrics
			TEXTMETRIC Metrics;

			if (TRUE) //RFlags.Metafile)
			{
				// Metafile - we need to use the screen DC to do this
				CDC *pDesktopDC = CWnd::GetDesktopWindow()->GetDC();

				// Get the metrics
				FontInfo.pOldFont = pDesktopDC->SelectObject(FontInfo.pRenderFont);
				pDesktopDC->GetTextMetrics(&Metrics);

				// Select old font back into screen DC
				pDesktopDC->SelectObject(FontInfo.pOldFont);
				CWnd::GetDesktopWindow()->ReleaseDC(pDesktopDC);
			}
			else
			{
				FontInfo.pOldFont = SelectObject(FontInfo.pRenderFont);

				GetTextMetrics(&Metrics);

				// Select old font back into DC
				SelectObject(FontInfo.pOldFont);
			}

			// Scale the width by the font's aspect ratio: 

			// ActualWidth = (RealWidth / Height) * Width
			FontWidth = MulDiv(Metrics.tmAveCharWidth, FontWidth, 100);
		}

		// Delete the font
		delete FontInfo.pRenderFont;

		// Create the 'proper' font and select it into the DC
		TRY
		{
			FontInfo.pRenderFont = new CFont;
		}
		CATCH (CMemoryException, e)
		{
			return FALSE;
		}
		END_CATCH
	}

	// Work out the rotation of the font, in tenths of degrees.
	INT32 lfRotation;
	if (Rotation == FIXED16(0))
		lfRotation = 0;
	else
		lfRotation = (INT32) ( (Rotation.MakeDouble() * 360.0 * 10.0) / (2 * PI) );

	// Create the font with the correct width and rotation
	if (FontInfo.pRenderFont->CreateFont(-FontHeight, FontWidth, lfRotation, 0, 
										 FontWeight, Italic, 
									 	 FALSE, FALSE, DEFAULT_CHARSET, 
									 	 OUT_TT_PRECIS, CLIP_TT_ALWAYS,
					   				 	 PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
					   				 	 pEnumLogFont->elfLogFont.lfFaceName) == 0)
	{
		// Can't create font (device may not support rotation).
		delete FontInfo.pRenderFont;
		FontInfo.pRenderFont = NULL;
		EndOSOutput();
		return FALSE;
	}

	// Select the real font into the DC.
	FontInfo.pOldFont = SelectObject(FontInfo.pRenderFont);

	// Validate the cache
	FontInfo.Typeface = Typeface;
	FontInfo.Bold 	  = Bold;
	FontInfo.Italic   = Italic;
	FontInfo.Width    = Width;
	FontInfo.Height   = Height;
	FontInfo.Rotation = Rotation;

	// End of GDI operations
	if (!EndOSOutput())
		return FALSE;

	// Font created and selected ok
	return TRUE;
}


/********************************************************************************************

>	BOOL PSPrintDC::SetClipping(BOOL state)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/09/96
	Returns:	The old state of the clipping flag
	Purpose:	Set the use of a clip region.

********************************************************************************************/

BOOL PSPrintDC::SetClipping(BOOL newstate)
{
	BOOL oldstate = UsePSLevelClipping;
	UsePSLevelClipping = newstate;
	return oldstate;
}


/********************************************************************************************

>	BOOL PSPrintDC::OutputPSClipping()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/09/96
	Returns:	TRUE if a clip region has been exported
	Purpose:	Set the use of a clip region.

********************************************************************************************/

BOOL PSPrintDC::OutputPSClipping()
{
	if (UsePSLevelClipping)
	{
		// DocRect theRect =  pPSRegion->CurrentClipRect;

		// BODGE - Currently need to deflate this clip region
		// by the bleed size. This only works because we know we are not
		// stripping but rendering complete pages. Hence this clip rectangle represents
		// the page with bleed applied in the document area. We need to output a clip
		// rectangle which will clip all objects beyond the bleed. This is quite a complex process.

		PrintView* pView = (PrintView*)(pPSRegion->GetRenderView());
		ERROR2IF(pView==NULL,FALSE,"No printview in this PS render region");
		ERROR3IF(!pView->IsKindOf(CC_RUNTIME_CLASS(PrintView)), "Not a PrintView in PSPrintDC output token");

		// Get the current patch clip rectangle
		DocRect theRect = pView->PatchInfo.GetClipRect(FALSE,FALSE);

		// read the regions render matrix.
		Matrix RMatrix = pPSRegion->GetMatrix();
		Matrix IMatrix = RMatrix.Inverse();
		RMatrix.transform(&theRect.lo);
		RMatrix.transform(&theRect.hi);
		PrintView::CorrectRotatedRectangle((Rect*)&theRect);
		pView->PatchInfo.InflateRectBy(&theRect,TRUE,FALSE);
		IMatrix.transform(&theRect.lo);
		IMatrix.transform(&theRect.hi);
		PrintView::CorrectRotatedRectangle((Rect*)&theRect);

		// And finally write the clip region out
		if (!pPSRegion->WriteClipRegion(this, theRect))
			return FALSE;
	}
	return TRUE;
}
