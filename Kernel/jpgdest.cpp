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


#include "camtypes.h"
#include "ccfile.h"

#include "jinclude.h"
#include "jpeglib.h"
#include "jpgermgr.h"

#include "errors.h"
//#include "tim.h"			// for _R(IDE_FILE_WRITE_ERROR)

#include "jpgdest.h"


// Place any IMPLEMENT type statements here
CC_IMPLEMENT_MEMDUMP(JPEGDataDestination, CC_CLASS_MEMDUMP)


// We want better memory tracking
#define new CAM_DEBUG_NEW


// Functions follow



/********************************************************************************************

>	JPEGDataDestination::JPEGDataDestination(j_compress_ptr cinfo, CCFile* pOutputFile)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		pOutputFile : a pointer to a CCFile providing the destination for the JPEG
				output
	Purpose:	Default constructor

********************************************************************************************/
JPEGDataDestination::JPEGDataDestination(CCFile* pOutputFile)
{
	ERROR3IF(pOutputFile == NULL || !pOutputFile->IS_KIND_OF(CCFile), "pOutputFile isn't");


	m_DestMgr.pThis	= this;

	m_DestMgr.init_destination		= InitDestination;
	m_DestMgr.empty_output_buffer	= EmptyOutputBuffer;
	m_DestMgr.term_destination		= TerminateDestination;

	m_pOutputFile	= pOutputFile;
	m_pBuffer		= NULL;
}


/********************************************************************************************

>	BOOL JPEGDataDestination::Init(j_compress_ptr cinfo)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		cinfo : a pointer to the IJG compression structure
	Purpose:	Initializes the given compression structure with this JPEGDataDestination

********************************************************************************************/
BOOL JPEGDataDestination::Init(j_compress_ptr cinfo)
{
	ERROR3IF(cinfo == NULL, "cinfo NULL");

	cinfo->dest = &m_DestMgr;

	return TRUE;
}


/********************************************************************************************

>	void JPEGDataDestination::InitDestination(j_compress_ptr cinfo)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		cinfo : a pointer to the IJG compression control structure
	Purpose:	Callback for IJG destination file handler
				Initialize destination --- called by jpeg_start_compress
				before any data is actually written.

********************************************************************************************/
void JPEGDataDestination::InitDestination(j_compress_ptr cinfo)
{
	JPEGDataDestination* pThis = GetThis(cinfo);

	/* Allocate the output buffer --- it will be released when done with image */
	pThis->m_pBuffer = (JOCTET *)(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  OUTPUT_BUF_SIZE * SIZEOF(JOCTET));

	pThis->m_DestMgr.next_output_byte = pThis->m_pBuffer;
	pThis->m_DestMgr.free_in_buffer = OUTPUT_BUF_SIZE;
}


/********************************************************************************************

>	boolean	JPEGDataDestination::EmptyOutputBuffer(j_compress_ptr cinfo)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		cinfo : a pointer to the IJG compression control structure
	Purpose:	Callback for IJG destination file handler
				Empty the output buffer --- called whenever buffer fills up.
	Notes:		Says IJG docs,
				"In typical applications, this should write the entire output buffer
				(ignoring the current state of next_output_byte & free_in_buffer),
				reset the pointer & count to the start of the buffer, and return TRUE
				indicating that the buffer has been dumped."
	Scope:		static

********************************************************************************************/
boolean	JPEGDataDestination::EmptyOutputBuffer(j_compress_ptr cinfo)
{
	JPEGDataDestination* pThis = GetThis(cinfo);
	CCFile* pFile = pThis->GetFile();

	// Write out the full buffer
	pFile->write(pThis->m_pBuffer, OUTPUT_BUF_SIZE);

	// Check written OK
	if (pFile->bad() || pFile->fail())
	{
		JPEGErrorManager* pError = (JPEGErrorManager*)cinfo->err;
		pError->ThrowError(_R(IDE_FILE_WRITE_ERROR));
	}

	pThis->m_DestMgr.next_output_byte	= pThis->m_pBuffer;
	pThis->m_DestMgr.free_in_buffer	= OUTPUT_BUF_SIZE;

  return TRUE;
}


/********************************************************************************************

>	void JPEGDataDestination::TerminateDestination(j_compress_ptr cinfo)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		cinfo : a pointer to the IJG compression control structure
	Purpose:	Terminate destination --- called by jpeg_finish_compress
				after all data has been written.  Usually needs to flush buffer.
	Notes:		*not* called by jpeg_abort or jpeg_destroy; surrounding
				application must deal with any cleanup that should happen even
				for error exit.

********************************************************************************************/
void JPEGDataDestination::TerminateDestination(j_compress_ptr cinfo)
{
	JPEGDataDestination* pThis = GetThis(cinfo);
	CCFile* pFile = pThis->GetFile();

	size_t DataCount = OUTPUT_BUF_SIZE - pThis->m_DestMgr.free_in_buffer;

	// Write any data remaining in the buffer
	if (DataCount > 0)
	{
		// Write out the full buffer
		pFile->write(pThis->m_pBuffer, DataCount);
	}
	pFile->flush();

	// Make sure we wrote the output file OK
	if (pFile->bad() || pFile->fail())
	{
		JPEGErrorManager* pError = (JPEGErrorManager*)cinfo->err;
		pError->ThrowError(_R(IDE_FILE_WRITE_ERROR));
	}
}



/********************************************************************************************

>	JPEGDataDestination* JPEGDataDestination::GetThis(j_compress_ptr cinfo)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		cinfo : a pointer to the IJG compression control structure
	Purpose:	Helper function to return the this pointer buried in the cinfo structure

********************************************************************************************/
JPEGDataDestination* JPEGDataDestination::GetThis(j_compress_ptr cinfo)
{
	JPEGDataDestination* pThis = ((IJGDestMgr*)(cinfo->dest))->pThis;
	ERROR3IF(!pThis->IS_KIND_OF(JPEGDataDestination), "GetThis - pThis isn't");

	return pThis;
}


/********************************************************************************************

>	CCFile* JPEGDataDestination::GetFile() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Helper function to return the CCFile associated with this JPEGDataDestination

********************************************************************************************/
CCFile* JPEGDataDestination::GetFile() const
{
	return m_pOutputFile;
}
