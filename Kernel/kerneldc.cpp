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

// Device Contexts that are kernel-safe.

/*
*/

#include "camtypes.h"

#include "kerneldc.h"
#include "colourix.h"
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "epsfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]

DECLARE_SOURCE("$Revision$");

#define new CAM_DEBUG_NEW

CC_IMPLEMENT_DYNAMIC(KernelDC, CCDummyDC);

/********************************************************************************************

>	KernelDC::KernelDC(RenderType rType)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/95
	Inputs:		The type of DC being made.
	Purpose:	Initialise a kernel DC.
				This initialises the user space origin to (0,0) by default (only relevant
				for DCs that use the PostScript stream functions).
	SeeAlso:	KernelDC::SetOrigin; KernelDC::OutputUserSpaceValue; 
				KernelDC::OutputCoord

********************************************************************************************/

KernelDC::KernelDC(RenderType rType) : CCDummyDC(rType)
{
	// Initialise other fields.
	LineWidth = 0;
	Origin.x = 0;
	Origin.y = 0;

	// Default to 2dp accuracy for user space values.
	FullAccuracy = FALSE;

	// No ASCII85 conversion by default.
	RawBuf = NULL;
	A85Buf = NULL;
	RawBufSize = 0;
	RLEtheASCII85Data = FALSE;
}

/********************************************************************************************

>	KernelDC::KernelDC(CDC *pDC, RenderType rType)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/95
	Inputs:		The OIL DC pointer, and the type of DC being made.
	Purpose:	Initialise a kernel DC.
				This initialises the user space origin to (0,0) by default (only relevant
				for DCs that use the PostScript stream functions).
	SeeAlso:	KernelDC::SetOrigin; KernelDC::OutputUserSpaceValue; 
				KernelDC::OutputCoord

********************************************************************************************/

KernelDC::KernelDC(CNativeDC *pDC, RenderType rType) : CCDummyDC(pDC, rType)
{
	// Initialise other fields.
	LineWidth = 0;
	Origin.x = 0;
	Origin.y = 0;

	// Default to 2dp accuracy for user space values.
	FullAccuracy = FALSE;

	// No ASCII85 conversion by default.
	RawBuf = NULL;
	A85Buf = NULL;
	RawBufSize = 0;
	RLEtheASCII85Data = FALSE;
}

/********************************************************************************************

>	void KernelDC::SetOrigin(DocCoord &NewOrigin)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		NewOrigin - the desired origin (in spread coords)
	Purpose:	Set the user space origin to the specified position in spread coordinates.
	SeeAlso:	KernelDC::OutputUserSpaceValue; KernelDC::OutputCoord

********************************************************************************************/

void KernelDC::SetOrigin(DocCoord &NewOrigin)
{
	Origin = NewOrigin;
}

/********************************************************************************************

>	BOOL KernelDC::OutputCoord(DocCoord& Coord, EPSAccuracy Accuracy = ACCURACY_NORMAL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		Coord - the coordinate to write out to the file.
				Accuracy - optional parameter - specifies how numbers should be output:
					ACCURACY_NORMAL:  		2dp
					ACCURACY_ROUNDUP: 		0dp, rounded up
					ACCURACY_ROUNDDOWN: 	0dp, rounded down.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Write out a coordinate x,y pair to the export file, automatically 
				converting from spread coordinates to the EPS user space coordinate system,
				including adjusting the origin.
	SeeAlso:	KernelDC::OutputUserSpaceValue
	Errors:		File/disk error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputCoord(DocCoord& Coord, EPSAccuracy Accuracy)
{
	BOOL Ok = (OutputUserSpaceValue(Coord.x - Origin.x, Accuracy) &&
			   OutputUserSpaceValue(Coord.y - Origin.y, Accuracy));

	return Ok;
}


/********************************************************************************************

>	BOOL KernelDC::OutputUserSpaceValue(MILLIPOINT n, 
									    EPSAccuracy Accuracy = ACCURACY_NORMAL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		n - the value (in millipoints) to output.
				Accuracy - optional parameter - specifies how numbers should be output:
					ACCURACY_NORMAL:  		2dp, or 3dp (see KernelDC::SetFullAccuracy)
					ACCURACY_ROUNDUP: 		0dp, rounded up
					ACCURACY_ROUNDDOWN: 	0dp, rounded down.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1

	Purpose:	Output a 'user space' value to the export file.
				User space is the coordinate system used for EPS files (and more generally,
				PostScript programs).   For Illustrator-based EPS, the user space is
				expressed in units of points, and usually has the origin at (0.0).
				By default, we follow the Illustrator convention of truncating values to
				2dp (so we can generate files compatible with Illustrator etc), but
				Camelot EPS uses the full 3dp accuracy (we don't store more any	accuracy 
				than this internally).  The accuracy can be changed by calling
				KernelDC::SetFullAccuracy
				Note that this routine does not do origin translation, because
				it may be used for dimensions such as line widths, which have no earthly use
				for origins (and anyway, it's a bit tricky to do origin translation with 
				only one dimension!).

				e.g. OutputUserSpaceValue(31465) will output "3.147" to the export file.

	SeeAlso:	KernelDC::OutputCoord; KernelDC::OutputToken;
				KernelDC::SetFullAccuracy
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputUserSpaceValue(MILLIPOINT n, EPSAccuracy Accuracy)
{
	TCHAR Buf[20];
	INT32 Integer;
	INT32 Fraction;

	const TCHAR * negative=_T("-");
	const TCHAR * positive=_T("");
	const TCHAR * sign = positive;

	// Convert millipoint to EPS user space value in desired format.
	switch (Accuracy)
	{
		case ACCURACY_NORMAL:
			// Convert to points, getting integer and fractional parts
			// invert the sign if necessary
			if (n<0)
			{
				sign=negative;
				n=-n;
			}

			// as n is positive (or zero), this works in the way expected. Both
			// Integer and fraction are >=0 too.	
			Integer  = n / 1000;
			Fraction = n % 1000;

			// Output to string (accurate to 2/3dp)

			// If fraction is 0, just output integer value.
			if (Fraction == 0)
			{
				camSprintf(Buf, _T("%d"), Integer);
			}
			else if (FullAccuracy)
			{
				camSprintf(Buf, _T("%s%d.%.3d"), sign, Integer, Fraction);
			}
			else
			{
				// Normal 2dp accuracy
				Fraction=(Fraction+5)/10;
				camSprintf(Buf, _T("%s%d.%.2d"), sign, Integer, Fraction);
			}
			break;

		case ACCURACY_ROUNDDOWN:
			// Convert to points, getting integer part.
			if (n < 0)
				Integer = (n - 999)/ 1000;
			else
				Integer = (n) / 1000;

			// Output to string (accurate to 0dp)
			camSprintf(Buf, _T("%d"), Integer);
			break;

		case ACCURACY_ROUNDUP:
			// Convert to points, getting integer part.
			if (n < 0)
				Integer = (n) / 1000;
			else
				Integer = (n + 999) / 1000;

			// Output to string (accurate to 0dp)
			camSprintf(Buf, _T("%d"), Integer);
			break;
	}

	return OutputToken(Buf);
}

/********************************************************************************************

>	BOOL KernelDC::OutputColourValue(UINT32 n)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		n - the colour value (range 0-255) to write to the EPS file.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Output a colour value to the export file.  A 'colour value' is a value
				as used in the Camelot 'Colour' class, i.e. in the range 0 to 255.
				This range is converted to the range 0.0 to 1.0, and output to the file.
	SeeAlso:	KernelDC::OutputColour
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputColourValue(UINT32 n)
{
	// Convert to points, getting integer and fractional parts
	INT32 Integer  = n / 255;
	INT32 Fraction = ((n % 255) * 100) / 255;

	// Output to string
	TCHAR Buf[20];
	// Ensure we always get at least 2 decimal figures with %.2d
	camSprintf(Buf, _T("%d.%.2d"), Integer, Abs(Fraction));
	return OutputToken(Buf);
}

/********************************************************************************************

>	BOOL KernelDC::OutputColour(PColourCMYK *pCol)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		Col - the CMYK colour values to be output.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Output a colour, as expressed by the Camelot 'Colour' class.  This function
				takes the CMYK variation, is this is most appropriate to EPS files.
				Camelot colour values are converted from the 0-255 range to the 0.0-1.0
				range before being output.

				e.g.
				MonoOn
					PColourCMYK Col = { 255, 255, 128, 0 };
					pDC->OutputColour(&Col);
				MonoOff
				will give the following output:
				MonoOn
					1.0 1.0 0.5 0.0
				MonoOff
	SeeAlso:	KernelDC::OutputColourValue
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputColour(PColourCMYK *pCol)
{
	// Output each of the colour values.
	BOOL Ok = (OutputColourValue(pCol->Cyan)    &&
			   OutputColourValue(pCol->Magenta) &&
			   OutputColourValue(pCol->Yellow)  &&
			   OutputColourValue(pCol->Key));

	// Return success or failure.
	return Ok;
}

/********************************************************************************************

>	BOOL KernelDC::OutputNamedColour(DocColour *pCol, ColourContext* pContext = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/94
	Inputs:		pCol - the colour to output.
				pContext - the context to use to convert the colour before output.
						   if this is NULL the default CMYK context will be used.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Similar to OutputColour(), except it outputs the colour name and tint
				of the colour as well.  If pCol does not reference an indexed colour,
				then the name "NoName" is used.
	SeeAlso:	KernelDC::OutputColour
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputNamedColour(DocColour *pCol, ColourContext* pContext)
{
	// Get CMYK version of this colour.
	PColourCMYK CMYK;
	pCol->GetCMYKValue(pContext, &CMYK);

	// Output CMYK version
	if (!OutputColour(&CMYK))
		return FALSE;
		
	// Get the indexed colour from the DocColour.
	IndexedColour *pIndCol = pCol->FindParentIndexedColour();

	// Cope with the unexpected!
//	ENSURE(pIndCol != NULL, "Named colour has no index colour!");

	if (pIndCol == NULL)
	{
		if (pCol->IsTransparent())
		{
			// This is a 'no colour' type colour, so output a zero-length colour name,
			// as this is the only way we can handle this at the moment.
			if (!OutputString(_T("")))
				return FALSE;
		}
		else
		{
			// Otherwise make up a colour name (see epsfiltr.h).
			if (!OutputString(ImmediateColourFudgeyBodgeName))
				return FALSE;
		}
	}
	else
	{
		// Got an indexed colour - output its name
		// (Pass in TRUE to get a unique-identifier for local colours rather than "Local colour")
		String_64 *ColName = pIndCol->GetName(TRUE);
		if (!OutputString((TCHAR *) (*ColName)))
			return FALSE;
	}

	// Always tint 0
	return OutputToken(_T("0"));
}


/********************************************************************************************

>	BOOL KernelDC::OutputColourName(DocColour *pCol)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/00
	Inputs:		pCol - the colour to output.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Outputs a named colour's name as a string. If pCol does not reference an 
				indexed colour, then the name "NoName" is used. This was seperated from 
				OutputNamedColour so that the DeviceContext will not need to worry about
				which ColourModel to use (CMYK, RGB, etc...), as this will be determined 
				by the RenderRegion.
	SeeAlso:	KernelDC::OutputNamedColour
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputColourName(DocColour *pCol)
{
	// Success / Failure flag for the export of this colour's name.
	BOOL ok = TRUE;
		
	// Get the indexed colour from the DocColour.
	IndexedColour *pIndCol = pCol->FindParentIndexedColour();

	if (pIndCol == NULL)
	{
		if (pCol->IsTransparent())
		{
			// This is a 'no colour' type colour, so output a zero-length colour name,
			// as this is the only way we can handle this at the moment.
			ok = OutputString(_T(""));
		}
		else
		{
			// Otherwise make up a colour name (see epsfiltr.h).
			ok = OutputString(ImmediateColourFudgeyBodgeName);
		}
	}
	else
	{
		// Got an indexed colour - output its name
		// (Pass in TRUE to get a unique-identifier for local colours rather than "Local colour")
		String_64 *ColName = pIndCol->GetName(TRUE);
		ok = OutputString((TCHAR *) (*ColName));
	}

	return ok;
}


/********************************************************************************************

>	BOOL KernelDC::OutputString(TCHAR *pString)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/94
	Inputs:		pString - the string token to output.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Output a string in PostScript format, i.e. delimited by '(' and ')', 
				and escaping any parentheses so the string is syntactically correct.
				e.g.  "String" is output as "(String)",
				and   "Hello (there)" is output as "(Hello \(there\))"
				and so on.
	SeeAlso:	KernelDC
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputString(TCHAR *pString)
{
	TCHAR Buf[128];

	// Open string delimiter
	Buf[0] = '(';

	// Copy the string, looking for embedded delimiters.
	INT32 src = 0,
		dst = 1;
	while ( (pString[src]) && (dst < 120 ) )	//while ((pString[src] != 0) && (dst < 120))-adapted for DBCS
	{
		if( (pString[src]== _T('(')) || (pString[src]==_T(')')) || (pString[src]== _T('\\')) )
	  //if( (pString[src] == '(')    || (pString[src] == ')')      || (pString[src] == '\\') )-adapted for DBCS
		{
			// escape this delimiter
			Buf[dst++] = '\\';
		}

		// Copy this character
		Buf[dst++] = pString[src++];
	}

	ERROR3IF(dst > 120, "String too long in KernelDC::OutputString");

	// Terminate the string token
	Buf[dst++] = ')';
	Buf[dst] = 0;

	// Output it
	return OutputToken(Buf);
}

/********************************************************************************************

>	BOOL KernelDC::OutputMatrix(Matrix * Mat)
					 
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		M- pointer to the matrix to output to the EPS stream.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Output a Matrix to the EPS stream.
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputMatrix(Matrix * M)
{
	FIXED16 abcd[4];
	INT32 ef[2];
	M->GetComponents(abcd, ef);

	if(!OutputReal(abcd[0].MakeDouble()))
		return FALSE;
	if(!OutputReal(abcd[1].MakeDouble()))
		return FALSE;
	if(!OutputReal(abcd[2].MakeDouble()))
		return FALSE;
	if(!OutputReal(abcd[3].MakeDouble()))
		return FALSE;
	if(!OutputUserSpaceValue(ef[0]))
		return FALSE;
	if(!OutputUserSpaceValue(ef[1]))
		return FALSE;
	return TRUE;


}


/********************************************************************************************

>	BOOL KernelDC::OutputValue(INT32 Value)
					 
	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		Value - the number to write to the EPS stream.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Output a signed integer to the EPS stream.
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputValue(INT32 Value)
{
	TCHAR buf[30];
	camSprintf(buf, _T("%d"), Value);
	return OutputToken(buf);
}

/********************************************************************************************

>	BOOL KernelDC::OutputValue(UINT32 Value)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		Value - the number to write to the EPS stream.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Output an unsigned integer to the EPS stream.
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputValue(UINT32 Value)
{
	TCHAR buf[30];
	camSprintf(buf, _T("%u"), Value);
	return OutputToken(buf);
}


/********************************************************************************************

>	BOOL KernelDC::OutputReal(double Value)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/94
	Inputs:		Value - the number to write to the EPS stream.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Output a real (floating point) number to the EPS stream.
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputReal(double Value)
{
	TCHAR buf[100]; // 100 should be enough for a floating point number!
	// Save floating point in scientific notation (because otherwise the number output
	// could be very long, e.g. 6.2e66 or similar would be very long if we didn't use
	// the exponent syntax).
	camSprintf(buf, _T("%g"), Value);
	return OutputToken(buf);
}



/********************************************************************************************

>	BOOL KernelDC::OutputFloat(const double Value, const UINT32 DecPl)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/95
	Inputs:		Value - the number to write to the EPS stream.
				DecPl - the number of decimal places to output. (1..8)
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Output a real (floating point) number to the EPS stream.
				
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputFloat(const double Value, const UINT32 DecPl)
{
	TCHAR buf[100];
	TCHAR *pch=buf;
	INT32 len=0, dp=-1;

	switch (DecPl)
	{
		case 1: camSprintf(buf,_T("%.1f"),Value); break;
		case 2: camSprintf(buf,_T("%.2f"),Value); break;
		case 4: camSprintf(buf,_T("%.4f"),Value); break;
		case 5: camSprintf(buf,_T("%.5f"),Value); break;
		case 6: camSprintf(buf,_T("%.6f"),Value); break;
		case 7: camSprintf(buf,_T("%.7f"),Value); break;
		case 8: camSprintf(buf,_T("%.8f"),Value); break;
		default:camSprintf(buf,_T("%.3f"),Value); break;
	}

	// supress a few trailing zero's
	while (*pch != '\0')
	{
		if (*pch=='.')
			dp=len;
		len++;
		pch++;
	}

	// check no decimal place
	if (dp==-1)
		return OutputToken(buf);

	pch--;
	while ((len>0) && (*pch=='0'))
	{
		pch--;
		len--;
	}

	if ((len==0) || (*pch=='.'))
		pch++;

	pch++;
	*pch='\0';

	return OutputToken(buf);
}

/********************************************************************************************

>	BOOL KernelDC::OutputArray(INT32* Array, INT32 ArraySize)
				 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/95
	Inputs:		Array, pointer to the array data.
				ArraySize, the number of elements in the array.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Output an Array to the EPS stream.
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL KernelDC::OutputArray(INT32* Array, INT32 ArraySize)
{
	// Output the 'ArrayStart'
	if (!OutputToken(_T("[")))
		return FALSE;

	// Output the actual array elements
	for (INT32 el = 0; el < ArraySize; el++)
	{
		if(!OutputUserSpaceValue(Array[el]))
			return FALSE;
	}

	// Output the 'ArrayEnd'
	if (!OutputToken(_T("]")))
		return FALSE;

	return TRUE;
}

/********************************************************************************************

>	INT32 KernelDC::OutputRawBinary(BYTE *Data, UINT32 Length, UINT32 Alignment = 1)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		Data - pointer to the data to output.
				Length - the number of bytes to output.
				Alignment - the byte alignment to use - it will pad the end of
							the data with 0 bytes if Length is not divisible by
							this parameter.  Hence use 1 for no padding.
	Returns:	The number of bytes output to the file, in terms of the source data, not
				in terms of the number of ASCII characters used to represent them.
				-1 => error occured while writing => ERROR1
	Purpose:	Outputs a sequence of bytes as raw hex data, as used by PostScript
				operators such as readhexstring.
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

INT32 KernelDC::OutputRawBinary(BYTE *Data, UINT32 Length, UINT32 Alignment)
{
	INT32 nBytes = 0;

	// Do 30 bytes at a time so we get lines with 60 characters on each.
	if (LineWidth > 0)
	{
		if (!OutputNewLine())
			// Error encountered
			return -1;
	}

	// Work out the padding needed
	ENSURE(Alignment != 0, "Bad alignment in OutputRawBinary!");
	UINT32 Padding = Alignment - (Length % Alignment);
	if (Padding == Alignment)
		Padding = 0;

	while (Length > 0)
	{
		// Work out how much data to output
		UINT32 ChunkLength = 32;
		if (Length < ChunkLength)
			ChunkLength = Length;

		// Convert the next chunk to hex and write it out
		TCHAR HexBuf[80];
		ConvertToHex(Data, ChunkLength, HexBuf);
		if (!OutputTCHARAsChar(HexBuf, ChunkLength * 2))
		{
			// Error
			return -1;
		}
		
		nBytes += ChunkLength;

		// Adjust length and check for padding requirements
		Length -= ChunkLength;
		Data += ChunkLength;

		if ((Length == 0) && (Padding > 0))
		{
			// Put the string "00" into HexBuf
			HexBuf[0] = '0';
			HexBuf[1] = '0';
			HexBuf[2] = 0;

			// Output it however many times we need to
			while (Padding > 0)
			{
				if (!OutputTCHARAsChar(HexBuf, 2))
					// Error
					return -1;

				nBytes++;
				Padding--;
			}
		}

		if (!OutputNewLine())
			// Error encountered
			return -1;
	}
	
	// All done
	return nBytes;
}


/********************************************************************************************

>	void KernelDC::ConvertToHex(BYTE *Data, UINT32 Length, TCHAR *Buf)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		Data - pointer to the data to convert
				Length - the number of bytes to convert.
	Outputs:	Buf - the buffer to place the hex string into.
					  NB. must be big enough to hold the 0-terminator!
	Purpose:	Convert a block of bytes to a hexadecimal ASCII stroing representation of
				the data.
	SeeAlso:	KernelDC::OutputRawBinary

********************************************************************************************/

void KernelDC::ConvertToHex(BYTE *Data, UINT32 Length, TCHAR *Buf)
{	
	UINT32 DataOfs = 0, 
		  DestOfs = 0;

	while (DataOfs < Length)
	{
		// Extract each 4-bit set and encode as hex
		INT32 Nybble = (Data[DataOfs] & 0xF0) >> 4;
		ENSURE(Nybble < 16, "Bad hex digit in KernelDC::ConvertToHex");

		TCHAR Ch;
		if (Nybble < 10)
			Ch = '0' + (TCHAR) Nybble;
		else
			Ch = 'A' + (TCHAR) (Nybble - 10);

		Buf[DestOfs++] = Ch;

		Nybble = Data[DataOfs] & 0xF;
		ENSURE(Nybble < 16, "Bad hex digit in KernelDC::ConvertToHex");

		if (Nybble < 10)
			Ch = '0' + (TCHAR) Nybble;
		else
			Ch = 'A' + (TCHAR) (Nybble - 10);

		Buf[DestOfs++] = Ch;

		// Do the next byte...
		DataOfs++;
	}

	// Terminate the string
	Buf[DestOfs] = 0;
}


// By default, enough room to convert 100 quadruples at once.
const static INT32 A85DataSize = 100;

BOOL KernelDC::StartASCII85Output(BOOL RunLengthEncode)
{
	// Sanity check
	ERROR2IF(A85Buf != NULL, FALSE, "Unfinished ASCII85 output!");

	// Remember whether or not we need to RLE the data.
 	RLEtheASCII85Data = RunLengthEncode;

	// Allocate buffers
	// (Jason here - NOTE that I add a 4 byte padding to the end of both buffers in case
	// of small overflow. Otherwise, the ConvertToASCII85 routine fills the buffer to the
	// brim and then slaps a 0 terminator on the end, corrupting the heap)
	RawBuf = (LPBYTE) CCMalloc((A85DataSize * 4) + 4);
	if (RawBuf == NULL)
		return(FALSE);
	
	A85Buf = (LPBYTE) CCMalloc((A85DataSize * 5) + 4);
	if (A85Buf == NULL)
	{ 
		CCFree(RawBuf);
		return(FALSE);
	}

	// No characters in buffer yet.
	RawBufSize = 0;

	// Ok
	return TRUE;
}

INT32 KernelDC::EndASCII85Output()
{
	// Are we useing RLE compression?
	if (RLEtheASCII85Data)
	{
		// Ok - end of data, so we add the RLE EOD marker to the buffer (byte value 128)
		BYTE EOD[2];
		EOD[0] = (BYTE) 128;
		if (!QueueASCII85Data(EOD, 1))
			// An error occured
			return -1;
	}

	// Flush out the buffer, padding with zeroes if necessary.
	INT32 nBytes = FlushASCII85Buffer();

	// End of ASCII85 data, so output the ASCII85 EOD marker.
	TCHAR EOD[] = _T("~>");
	OutputTCHARAsChar(EOD, 2);
	OutputNewLine();

	// Free up the buffers
	CCFree(RawBuf);
	CCFree(A85Buf);
	RawBuf = NULL;
	A85Buf = NULL;
	RawBufSize = 0;
	RLEtheASCII85Data = FALSE;

	// Tell caller how many byts we saved
	return nBytes;
}

/********************************************************************************************

>	INT32 KernelDC::OutputASCII85(BYTE *Data, UINT32 Length)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/95
	Inputs:		Data - pointer to the data to output.
				Length - the number of bytes to output.
				RunLengthEncode - if TRUE then run length encode the data before encoding it
								  to ASCII85 format (see Red Book for RLE spec).
	Returns:	The number of bytes output to the file, in terms of the source data, not
				in terms of the number of ASCII characters used to represent them.
				-1 => error occured while writing => ERROR1
	Purpose:	Outputs a sequence of bytes as ASCII85 hex data, as used by PostScript
				Level 2.  (This encoding should not be used when generating PostScript that
				must be compatible with Level 1 interpreters).
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

INT32 KernelDC::OutputASCII85(BYTE *Data, UINT32 Length)
{
	// Sanity check
	ERROR2IF(RawBuf == NULL, FALSE, "ASCII85 output not initialised correctly!");

 	// Run length encode the data first if required by the caller.
	BYTE *pRLLBuf = NULL;

	if (RLEtheASCII85Data)
	{
		// Encode this data using run length encoding.
		INT32 NewLength = Length;
		pRLLBuf = KernelDC::RunLengthEncode(Data, &NewLength);
		if (pRLLBuf == NULL)
			// Error encoding data
			return -1;

		#ifdef _DEBUG

			// Debug check - let's just decompress that data to make sure it was encoded ok!
			INT32 DecodedLength = NewLength;
			BYTE *Decoded = RunLengthDecode(pRLLBuf, &DecodedLength);

			// Check for data equality
			BOOL ValidEncoding = (DecodedLength == (INT32) Length);

			if (ValidEncoding)
				// Perform byte-wise comparison of data
				ValidEncoding = (memcmp(Data, Decoded, Length) == 0);

			// De-allocate the decoding buffer
			CCFree(Decoded);

			ERROR2IF(!ValidEncoding, -1, "Error in RunLengthEncoding integrity check!");

		#endif

		// Point ASCII85 routine to the RLL encoded data
		Data = pRLLBuf;
		Length = NewLength;
	}


	INT32 nBytes = -1;

	// Start this chunk on a new line.
	if (LineWidth > 0)
	{
		if (!OutputNewLine())
		{
			// Error encountered
			if (RLEtheASCII85Data)
				CCFree(pRLLBuf);
			return -1;
		}
	}

	// Queue up the data (which may or may not be RLL encoded).
	if (QueueASCII85Data(Data, Length))
		// Data queued successfully.
		nBytes = Length;

	// Free up the buffer used for run length encoding.
	if (RLEtheASCII85Data)
		CCFree(pRLLBuf);

	// All done
	return nBytes;
}


BOOL KernelDC::QueueASCII85Data(BYTE *Data, UINT32 Length)
{
	INT32 MaxData = A85DataSize * 4;

	// Keep filling and flushing the buffer until we are out of data to send.
	INT32 nBytes = Length;
	while (nBytes > 0)
	{
		ERROR2IF((INT32) RawBufSize > MaxData, FALSE, "ASCII85 buffer overflow!");

		if ((INT32) RawBufSize == MaxData)
		{
			// Buffer is full - encode it and write it to disk.
			if (FlushASCII85Buffer() == -1)
				return FALSE;
		}

		// Put as much data into the buffer as possible
		INT32 SpaceLeft = MaxData - RawBufSize;
		if (nBytes > SpaceLeft)
		{
			// Not enough room to put it all in, so fill up the buffer
			memcpy(RawBuf + RawBufSize, Data, SpaceLeft);
			RawBufSize += SpaceLeft;
			Data += SpaceLeft;
			nBytes -= SpaceLeft;
		}
		else
		{
			// Enough room in buffer to put all the data in, so do it.
			memcpy(RawBuf + RawBufSize, Data, nBytes);
			RawBufSize += nBytes;
			nBytes = 0;
		}
	}

	// All ok
	return TRUE;
}

INT32 KernelDC::ConvertToASCII85(BYTE *Src, UINT32 Length, BYTE *Dest)
{
	// Precompute the powers of 85 to use when encoding.
/*
	static const INT32 Power85[5] = { 1, 
									 85, 
									 85 * 85, 
									 85 * 85 * 85, 
									 85 * 85 * 85 * 85 };
*/

	// Start at the beginning of the buffer
//	INT32 SrcOfs = 0;
	INT32 DstOfs = 0;

	while (Length > 0)
	{
		// Encode four bytes as five characters.
		UINT32 n=0;
		
		if (Length >= 4)
		{
			// Normal 4 bytes of data
			n = Src[0];
			n = (n << 8) | Src[1];
			n = (n << 8) | Src[2];
			n = (n << 8) | Src[3];
		}
		else
		{
			// Pad with zeros...
			switch (Length)
			{
				case 3:
					n = Src[0];
					n = (n << 8) | Src[1];
					n = (n << 8) | Src[2];
					n <<= 8;
					break;

				case 2:
					n = Src[0];
					n = (n << 8) | Src[1];
					n <<= 16;
					break;

				case 1:
					n = Src[0];
					n <<= 24;
					break;

			}
		}

		if ((n == 0) && (Length >= 4))
		{
			// Special case - use "z" instead of "!!!!!"
			Dest[DstOfs++] = 'z';
		}
		else
		{
		#if 1
		    register UINT32 q = n/7225;
		    register unsigned r = (unsigned)n-7225*(unsigned)q;
		    register unsigned t;
		    Dest[DstOfs+3] = (t = r/85) + '!';
		    Dest[DstOfs+4] = r - 85*t + '!';
		    Dest[DstOfs+0] = (t = q/7225) + '!';
		    r = (unsigned)q - 7225*t;
		    Dest[DstOfs+1] = (t = r/85) + '!';
		    Dest[DstOfs+2] = r - 85*t + '!';

			if (Length >= 4)
				DstOfs += 5;
			else
				// Special case for last 5-tuple - see Red Book, bottom of p129 
				DstOfs += (Length + 1);
		#else
/*
			for (INT32 i = 4; i >= 0; i--)
			{
				// Reduce the number to a 5 digit base-85 number.
				UINT32 m = n / Power85[i];
				n -= m * Power85[i];
				Dest[DstOfs++] = (TCHAR) (m + 33);
			}
*/
		#endif
		}

		// Move on to the next four bytes
		Src += 4;
		if (Length < 4)
			Length = 0;
		else
			Length -= 4;
	}

	// Finished encoding - terminate the string (not strictly necessary but what the hell).
	// (Jason here... What do you mean "What the hell?" - you've just written one byte off the
	// end of the buffer, and corrupted the heap! I've now added 4 bytes padding
	// on our buffers (in StartASCII85Output) to make sure this is safe)
	Dest[DstOfs] = 0;

	// Return how many bytes this ASCII85 data takes up
	return DstOfs;
}

INT32 KernelDC::ConvertFromASCII85(BYTE *Src, UINT32 Length, BYTE *Dest)
{
	// Precompute the powers of 85 to use when encoding.
	static const INT32 Power85[5] = { 1, 
									 85, 
									 85 * 85, 
									 85 * 85 * 85, 
									 85 * 85 * 85 * 85 };

	// Start at the beginning of the buffer
//	INT32 SrcOfs = 0;
	INT32 DstOfs = 0;
	BOOL FoundEOD = FALSE;

	while ((Length > 0) && (!FoundEOD))
	{
		// Decode five characters into four bytes.
		UINT32 n = 0;
		UINT32 TupleLength = (Length >= 5) ? 5 : Length;

		// Look for the EOD marker
		for (UINT32 i = 0; i < 5; i++)
		{
			if ((Src[i] == '~') && ((i + 1) < Length) && (Src[i + 1] == '>'))
			{
				// Found the EOD marker in the data
				FoundEOD = TRUE;

				// Shorten the tuple
				TupleLength = i;
			}
		}

		if (Src[0] == 'z')
		{
			// Skip the 'z' character
			TupleLength = 1;
		}
		else
		{			
			// Decode base-85 numbers.
			for (UINT32 i = 0; i < 5; i++)
			{
				if (i < TupleLength)
					n += ((Src[i] - '!') * Power85[4 - i]);
				else if (i == TupleLength)
					// Round up fractional part
					n += 0x80 << ((4 - i) * 8);

				// 'z' is not allowed within a tuple
				ERROR2IF(Src[i] == 'z', -1, "Illegal 'z' in ASCII85 stream");
			}
		}

		// Find out how many bytes we just decoded.
		UINT32 nBytesDecoded = TupleLength - 1;

		// Copy the decoded data into the buffer.
		UINT32 Shift = 24;
		while (nBytesDecoded > 0)
		{
			Dest[DstOfs++] = (BYTE) ((n >> Shift) & 0xFF);
			nBytesDecoded--;
			Shift -= 8;
		}

		// Move on to the next tuple.
		Src += TupleLength;
		Length -= TupleLength;
	}

	// Check the lengths match
	ERROR2IF(Length != 0, -1, "Incorrect length while decoding ASCII85 data");
	//ERROR2IF(!FoundEOD,   -1, "No EOD found while decoding ASCII85 data");

	// Finished encoding - terminate the string (not strictly necessary but what the hell).
	Dest[DstOfs] = 0;

	// Return how many bytes this decoded ASCII85 data takes up
	return DstOfs;
}

INT32 KernelDC::FlushASCII85Buffer()
{
	INT32 ConvertedLength = ConvertToASCII85(RawBuf, RawBufSize, A85Buf);

	#ifdef _DEBUG

		// Let's just check that it was encoded corrrectly.
		// (NOTE: I allocate 4 bytes more than necessary because ConvertFromASCII85 may
		// write an extra terminator byte past the end of the buffer memory)
		BYTE *Dest = (BYTE *) CCMalloc(RawBufSize + 4);
		if (Dest == NULL)
			// Error - no more memory
			return -1;

		INT32 nBytes = KernelDC::ConvertFromASCII85(A85Buf, ConvertedLength, Dest);
		ERROR2IF(nBytes < 0, -1, "Error in decoding ASCII85 data");

		// Length check
		if (nBytes != (INT32) RawBufSize)
			ERROR3("Length of decoded ASCII85 data does not match original! Output may be corrupt");

		// Binary comparison
		if (memcmp(RawBuf, Dest, RawBufSize) != 0)
			ERROR3("Decoded ASCII85 data does not match original! Output may be corrupt");

		// Clean up.
		CCFree(Dest);

	#endif

	RawBufSize = 0;
	if (!OutputDirect(A85Buf, ConvertedLength))
	{
		// Error
		return -1;
	}

	// All ok
	return ConvertedLength;
}


BYTE *KernelDC::RunLengthEncode(BYTE *Data, INT32 *pLength)
{
 	// Run length encode the data first if required by the caller.
	BYTE *pRLLBuf = NULL;

	// Work out how much space we will need (i.e. work out the worst case overhead for
	// run length encoding).
	// This is based on no repetition at all, i.e. no opportunity for compression.
	// This results in a one byte overhead per 128 bytes.

	INT32 Length = *pLength;
	INT32 Overhead = (Length / 128) + 10;  // Add 10 for good measure!  No GPFs here...

	// Get a buffer to convert the data in. Allocate 4 byes extra just to be safe
	INT32 RLLSize = Length + Overhead;
	pRLLBuf = (LPBYTE) CCMalloc(RLLSize + 4);
	if (pRLLBuf == NULL)
		// Error;
		return NULL;

	// Ok - we've got the buffer, so encode the data.
	// We don't bother to RLL unless we get a gain, i.e. 3 or more repeating characters.
	// (2 repeating bytes gives the same space usage, and can interrupt the stream so
	// actually uses more space).
	INT32 SeqLength = 0;
	INT32 DestOfs = 0;
	INT32 SrcOfs = 0;
	INT32 Ofs = 0;
	INT32 Len = Length;

	while (Len > 0)
	{
		// Check for limit on sequence length.
		ERROR2IF(SeqLength > 128, NULL, "Sequence too long in RLE encoding!");

		if (SeqLength == 128)
		{
			// Got a full sequence - copy it to the destination buffer.
			ERROR2IF(DestOfs + SeqLength >= RLLSize, NULL, "RLE buffer over-run!");
			pRLLBuf[DestOfs++] = SeqLength - 1;
			memcpy(pRLLBuf + DestOfs, Data + SrcOfs, SeqLength);
			DestOfs += SeqLength;
			SrcOfs += SeqLength;
			SeqLength = 0;
		}

		// Look for repeating characters
		if ((Len > 2) && (Data[Ofs] == Data[Ofs + 1]) && (Data[Ofs] == Data[Ofs + 2]))
		{
			// Ooh - found 3 repeating characters - see if there are any more.

			// First flush out any previous bytes.
			if (SeqLength > 0)
			{
				ERROR2IF(DestOfs + SeqLength >= RLLSize, NULL, "RLE buffer over-run!");
				pRLLBuf[DestOfs++] = SeqLength - 1;
				memcpy(pRLLBuf + DestOfs, Data + SrcOfs, SeqLength);
				DestOfs += SeqLength;
			}

			// Sequence starts at this character.
			SrcOfs = Ofs;
			SeqLength = 0;

			while ((Len > 0) && (SeqLength < 128) && (Data[Ofs] == Data[SrcOfs]))
			{
				// Try next character
				Len--;
				Ofs++;
				SeqLength++;
			}

			// End of sequence of repeating characters - place it into buffer.
			ERROR2IF(DestOfs + 2 > RLLSize, NULL, "RLE buffer over-run!");
			pRLLBuf[DestOfs++] = 257 - SeqLength;
			pRLLBuf[DestOfs++] = Data[SrcOfs];

			// Move to next character
			//Ofs++;
			SeqLength = 0;
			SrcOfs = Ofs;
		}
		else
		{
			// Add this character to the current sequence.
			Len--;
			Ofs++;
			SeqLength++;
		}
	}

	// Flush remaining sequence, if there is one.
	if (SeqLength > 0)
	{
		ERROR2IF(DestOfs + SeqLength >= RLLSize, NULL, "RLE buffer over-run!");
		pRLLBuf[DestOfs++] = SeqLength - 1;
		memcpy(pRLLBuf + DestOfs, Data + SrcOfs, SeqLength);
		DestOfs += SeqLength;
	}

	// Return the pointer to the RLL encoded data, and tell caller how long the data is.
	*pLength = DestOfs;
	return pRLLBuf;
}

BYTE *KernelDC::RunLengthDecode(BYTE *Data, INT32 *pLength)
{
	// Ok, scan the data to see how large it will expand to.
	INT32 i = 0;
	INT32 Length = *pLength;
	INT32 Size = 0;
	BOOL FoundEOD = FALSE;

	while (i < Length)
	{
		if (Data[i] <= 127)
		{
			// Straight copying of data
			Size += (Data[i] + 1);

			// Skip past the data
			i += (Data[i] + 2);
		}
		else if (Data[i] > 128)
		{
			// Repeated data
			Size += (257 - Data[i]);

			// Skip past repeat count and byte to repeat.
			i += 2;
		}
		else
		{
			// 128 == End of stream
			FoundEOD = TRUE;
			break;
		}
	}

	// Got the size of data stream - allocate a buffer.
	if (Size == 0)
	{
		// Something not right here - cope with it gracefully
		*pLength = 0;
		return NULL;
	}

	// Allocate a buffer. Allocate 4 bytes extra just in case of slight buffer overruns
	BYTE *pRLLBuf = (BYTE *) CCMalloc(Size + 1 + 4);
	if (pRLLBuf == NULL)
	{
		// Error - no memory
		*pLength = 0;
		return NULL;
	}

	// Ok, actually decode it now.
	INT32 j = 0;
	i = 0;
	FoundEOD = FALSE;

	while (i < Length)
	{
		if (Data[i] <= 127)
		{
			// Straight copying of data
			memcpy(pRLLBuf + j, Data + i + 1, Data[i] + 1);

			// Skip past the data
			j += (Data[i] + 1);
			i += (Data[i] + 2);
		}
		else if (Data[i] > 128)
		{
			// Repeated data
			memset(pRLLBuf + j, Data[i + 1], 257 - Data[i]);

			// Skip past repeat count and byte to repeat.
			j += (257 - Data[i]);
			i += 2;
		}
		else
		{
			// 128 == End of stream
			FoundEOD = TRUE;
			break;
		}
	}

	// Sanity checks
	if (j!=Size)
	{
		*pLength=0;
		ERROR2(NULL, "Incorrect decoding of Run length data");
	}

	// Ok, we've decoded it.
	*pLength = j;
	return pRLLBuf;
}


/********************************************************************************************

>	void KernelDC::SetFullAccuracy(BOOL Full)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		Full:  TRUE => Use 3dp when saving user space values
					  FALSE => Use 2dp when saving user space values
	Purpose:	Change the accuracy used when saving out user space values to the EPS file.
				This defaults to 2dp, but can be enabled to full accuracy (3dp) by 
				passing in TRUE.
				We still do 2dp because that is what Illustrator and most of the other
				mediocre programs use and we don't want to generate files that might
				upset them.  Camelot EPS uses the full 3dp accuracy though.

********************************************************************************************/

void KernelDC::SetFullAccuracy(BOOL Full)
{
	FullAccuracy = Full;
}


/********************************************************************************************

>	BOOL KernelDC::OutputNewLine()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Returns:	FALSE (see Purpose)
	Purpose:	Causes a new line to be started in the EPS/PS output.   This is used
				to give a pleasant appearance to the file - most EPS/PS commands (as
				opposed to operands) are followed by a new line.

				NB. This base class version will always raise an ERROR2 and return FALSE
					because this function must be over-ridden in order to use the output
					functions.

	SeeAlso:	KernelDC; KernelDC::OutputToken
	Errors:		Always => ERROR2

********************************************************************************************/

BOOL KernelDC::OutputNewLine()
{
	ERROR2(FALSE, "OutputNewLine() called for base class KernelDC");
}

/********************************************************************************************

>	BOOL KernelDC::OutputToken(TCHAR *Str)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		Str - the character string to write to the stream.
	Returns:	FALSE (see Purpose)
	Purpose:	Outputs a string token to the EPS/PS stream.
				This is the central routine through which the other high-level routines
				eventually come. The other routines convert their output to a string,
				which they then pass on to this routine.
				A record is kept of the current line width - if it is over 70 characters
				wide before the token is output, then a new line is output to keep
				the lines in the EPS file reasonably short.
				For this reason, it is important not to output strings that contain
				newline characters, because this routine will not notice, and hence
				the LineWidth count will be wrong.
				This routine also ensures that tokens are separated by either a space
				or a newline, so it is not necessary to output spaces directly to keep the 
				tokens separate - it happens automatically.

				NB. This base class version will always raise an ERROR2 and return FALSE
					because this function must be over-ridden in order to use the output
					functions.

	SeeAlso:	KernelDC; KernelDC::OutputNewLine; KernelDC::OutputDirect
	Errors:		Always => ERROR2

********************************************************************************************/

BOOL KernelDC::OutputToken(TCHAR *Str)
{
	ERROR2(FALSE, "OutputToken() called for base class KernelDC");
}


/********************************************************************************************

>	BOOL KernelDC::OutputDirect(BYTE *Buf, INT32 nBytes)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/23/95
	Inputs:		Buf - the bytes to send to the stream.
				nBytes - the number of bytes to send to the stream.
	Returns:	FALSE (see Purpose)
	Purpose:	Send bytes directly to the PostScript stream with no alteration or padding.
				Used for sending binary/hex data to stream.

				NB. This base class version will always raise an ERROR2 and return FALSE
					because this function must be over-ridden in order to use the output
					functions.

	SeeAlso:	KernelDC::OutputNewLine; KernelDC::OutputToken
	Errors:		Always => ERROR2

********************************************************************************************/

BOOL KernelDC::OutputDirect(BYTE *, INT32)
{
	ERROR2(FALSE, "OutputDirect() called for base class KernelDC");
}



/********************************************************************************************

>	BOOL KernelDC::OutputTCHARAsChar(TCHAR *Buf, INT32 nBytes)

	Author:		Alex Bligh
	Created:	13/06/2006
	Inputs:		Buf - the bytes to send to the stream.
				nBytes - the number of bytes to send to the stream (i.e the number of
				TCHARs to read)
	Returns:	TRUE if all the bytes were sent ok;
				FALSE if not.
	Purpose:	Send bytes directly to the PostScript stream with no alteration or padding.
				Used for sending binary/hex data to stream. We send the BYTE equal to
				the TCHAR with no conversion
	SeeAlso:	KernelDC::OutputNewLine; KernelDC::OutputToken

********************************************************************************************/

BOOL KernelDC::OutputTCHARAsChar(TCHAR *Buf, INT32 nBytes)
{
	if (sizeof(TCHAR) == sizeof(char))
		return OutputDirect((BYTE *)Buf, nBytes);

	BYTE * pByte=new BYTE[nBytes];
	if (!pByte)
		return FALSE;

	INT32 i;
	for (i=0; i<nBytes; i++)
		pByte[i]=Buf[i]; // 1:1 copy

	BOOL ok=OutputDirect(pByte, nBytes);

	delete (pByte);
	return ok;
}

//
// ExportDC class
//


/********************************************************************************************

>	ExportDC::ExportDC(Filter *Parent)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Creates a DC of the correct type (RENDERTYPE_FILE).
	SeeAlso:	EPSExportDC::EPSExportDC

********************************************************************************************/

ExportDC::ExportDC(Filter *Parent) : KernelDC(RENDERTYPE_FILE)
{
	// Sanity check
	ENSURE(Parent != NULL, "NULL parent filter in ExportDC creation!");

	// Install parent
	ParentFilter = Parent;
	ExportFile = NULL;
}

/********************************************************************************************

>	BOOL ExportDC::Init(CCLexFile* pFile)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		pFile - the file to connect this DC to.
	Returns:	TRUE if DC connected to file successfully, FALSE if an error occured.
	Purpose:	Initialise an export DC by connecting it to a named file.
	Errors:		Unable to open the file.
	SeeAlso:	ExportDC

********************************************************************************************/

BOOL ExportDC::Init(CCLexFile* pFile)
{
	// This file is already open when we get it
	ExportFile = pFile;

	// Make sure that the file is open
	ERROR2IF(!ExportFile->isOpen(), FALSE, "File was not open in ExportDC::Init()");

	// File opened ok.
	return TRUE;
}

/********************************************************************************************

>	ExportDC::~ExportDC()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Deinitialise an Export device context.  If the file connected to this DC
				is still open, the file is closed.
	SeeAlso:	ExportDC

********************************************************************************************/

ExportDC::~ExportDC()
{
	// This is no longer needed as the file is closed by the caller
	/*
	if (ExportFile.isOpen())
	{
		if (IsUserName("Tim"))
			TRACE( _T("File still open in CCExportDC dtor"));

		ExportFile.close();
	}
	*/
}


/********************************************************************************************

>	Filter *ExportDC::GetParentFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/08/94
	Returns:	Pointer to the parent filter object.
	Purpose:	Get a pointer to the filter object associated with this export device
				context.
	SeeAlso:	Filter

********************************************************************************************/

Filter *ExportDC::GetParentFilter()
{
	return ParentFilter;
}

