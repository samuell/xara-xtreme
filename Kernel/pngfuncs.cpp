// $Id: pngfuncs.cpp 662 2006-03-14 21:31:49Z alex $
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
// Replacements for some libpng functions (pointers to these functions
// are passed to libpng)

#include "camtypes.h"
#include "png.h"
#include "pngprgrs.h"
#include "pngutil.h"
#include "fixmem.h"

/******************************************************************* 

>       void camelot_png_error(png_structp png_ptr,
										png_const_charp message)

        Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
        Created:	29/8/2000
        Inputs:		png_ptr, error message
        Outputs:	Error message
        Returns:	void
        Purpose:	Pointer to this function given to libpng to be
					called in case of error.
*******************************************************************/

void camelot_png_error(png_structp png_ptr, png_const_charp message)
{
	TRACEUSER( "Gavin", _T("PNG: libpng error: %s\n"), message);
	
	// Use the error handler in the PNG util function which assumes TRY CATCH block in
	// operation and CCFile present, in which case it will use the CCFile GotError function
	//	PNGUtil::DefaultErrorHandler(MessageID, message);
	PNGUtil::DefaultErrorHandler();
}

/******************************************************************* 

>       void camelot_png_warning(png_structp png_ptr,
								png_const_charp message)

        Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
        Created:	29/8/2000
        Inputs:		png_ptr, warning message
        Outputs:	Nothing (except a trace message)
        Returns:	void
        Purpose:	Pointer to this function given to libpng to be
					called in case of warning.
*******************************************************************/

void camelot_png_warning(png_structp png_ptr, png_const_charp message)
{
	if (!png_ptr)
		return;
	
	TRACEUSER( "Gavin", _T("PNG: libpng warning: %s\n"), message);
}

/******************************************************************* 

>       void camelot_png_read_data(png_structp png_ptr, 
								png_bytep data, png_size_t length)

        Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
        Created:	29/8/2000
        Inputs:		png_ptr, amount to be read
        Outputs:	'data' set to data from file
        Returns:	void
        Purpose:	Pointer to this function given to libpng to be
					called for reading data.
*******************************************************************/

void camelot_png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	iostream *fp = (iostream *)png_get_io_ptr(png_ptr);
	png_uint_32 check = png_uint_32(length);
	
	if (fp)
		fp->read((char *) data, check );
	else
		png_error(png_ptr, "Read Error");
	
	if (check != length)
	{
		png_error(png_ptr, "Read Error");
	}
}

/******************************************************************* 

>       void camelot_png_write_data(png_structp png_ptr, 
								png_bytep data, png_size_t length)

        Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
        Created:	29/8/2000
        Inputs:		png_ptr, amount to be written, data
        Outputs:	Data in file
        Returns:	void
        Purpose:	Pointer to this function given to libpng to be
					called for writting data.
*******************************************************************/

void camelot_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	iostream *fp = (iostream *)png_get_io_ptr(png_ptr);
	png_uint_32 check = png_uint_32(length);

	if (fp)
		fp->write((const char*)data, check);
	else
		png_error(png_ptr, "Write Error");

	if (check != length)
	{
		png_error(png_ptr, "Write Error");
	}
}

/******************************************************************* 

>       void camelot_png_flush_data(png_structp png_ptr)

        Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
        Created:	29/8/2000
        Inputs:		png_ptr
        Outputs:	Output stream flushed
        Returns:	void
        Purpose:	Pointer to this function given to libpng to be
					called for flushing output stream on write.
*******************************************************************/

void camelot_png_flush_data(png_structp png_ptr)
{
	iostream *fp = (iostream *)png_get_io_ptr(png_ptr);

	if (fp)
		fp->flush();
}

/******************************************************************* 

>       void camelot_png_malloc(png_structp png_ptr, png_size_t size)

        Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
        Created:	29/8/2000
        Inputs:		png_ptr, size of memory
        Outputs:	Memory block
        Returns:	void * to memory
        Purpose:	Pointer to this function given to libpng to be
					called for allocing memory.
*******************************************************************/

void *camelot_png_malloc(png_structp png_ptr, png_size_t size)
{
	return (CCMalloc(size));
}

/******************************************************************* 

>       void camelot_png_free(png_structp png_ptr, png_voidp data)

        Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
        Created:	29/8/2000
        Inputs:		png_ptr, &data
        Returns:	void
        Purpose:	Pointer to this function given to libpng to be
					called for freeing data.
*******************************************************************/

void camelot_png_free(png_structp png_ptr, png_voidp data)
{
	CCFree(data);
}

/******************************************************************* 

>       void camelot_png_write_row_callback
			(png_structp png_ptr, png_uint_32 row_number, INT32 pass)

        Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
        Created:	29/8/2000
        Inputs:		png_ptr, row, pass
        Outputs:	Status bar update
        Returns:	void
        Purpose:	Pointer to this function given to libpng to be
					called when a row has been read.

					IMPORTANT NOTE: This function is not used, I have
					just included it for completness (the write code does
					its own status bar updates).
*******************************************************************/

void camelot_png_write_row_callback
			(png_structp png_ptr, png_uint_32 row_number, INT32 pass)
{
}
