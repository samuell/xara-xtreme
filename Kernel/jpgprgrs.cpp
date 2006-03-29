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
#include "filters.h"
#include "progress.h"

#include "jinclude.h"
#include "jpeglib.h"
#include "jpgprgrs.h"



// Place any IMPLEMENT type statements here
//CC_IMPLEMENT_MEMDUMP(CCWobJob, CC_CLASS_MEMDUMP)


// We want better memory tracking
#define new CAM_DEBUG_NEW


// Functions follow


/********************************************************************************************

>	JPEGProgressMonitor::JPEGProgressMonitor(j_decompress_ptr cinfo)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Inputs:		cinfo : the structure provided by the IJG library
	Purpose:	Derived classes should call this prior to their own construction to construct
				a safe base class.
	Scope:		protected

********************************************************************************************/
JPEGProgressMonitor::JPEGProgressMonitor(j_decompress_ptr cinfo)
{
	// Set "base class" member...
	progress_monitor = Update;

	m_pcinfo			= cinfo;
	m_bContinueOp		= TRUE;
}


/********************************************************************************************

>	JPEGProgressMonitor::~JPEGProgressMonitor()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Provides bizarre pure implementation for derived class destructors
	Scope:		protected

********************************************************************************************/
JPEGProgressMonitor::~JPEGProgressMonitor()
{
}


/********************************************************************************************

>	void JPEGProgressMonitor::Update(j_common_ptr cinfo)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Inputs:		cinfo : data provided by the IJG Library
	Purpose:	Callback function for the IJG library.
				It updates the Camelot Progress bar
	Scope:		static
	Notes:		This member is static because the IJG library is written in C, so requiring
				classless pointers to functions

********************************************************************************************/
void JPEGProgressMonitor::Update(j_common_ptr cinfo)
{
	JPEGProgressMonitor* pThis = (JPEGProgressMonitor*) cinfo->progress;
	pThis->OnUpdate();
}





/********************************************************************************************

>	JPEGProgressBySelf::JPEGProgressBySelf(j_decompress_ptr cinfo, 
											const StringBase& BarString)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Inputs:		BarString: a reference to the string to be used on the progress bar during
				the time-consuming operation
	Purpose:	Constructor for a class providing a progress callback to the IJG Library
				JPEG filter.
	Notes:		A progress bar will be constructed via this constructor.
	See Also:	JPEGProgressByFilter

********************************************************************************************/
JPEGProgressBySelf::JPEGProgressBySelf(j_decompress_ptr cinfo, const StringBase& BarString)
	: JPEGProgressMonitor(cinfo)
{
	m_BarString			= BarString;
	m_uLastPercentDone	= 0;

	// Create a new progress bar. Actually not that bothered if it doesn't construct
	m_pProgressBar		= new Progress(&m_BarString, 100);
}


/********************************************************************************************

>	JPEGProgressBySelf::~JPEGProgressBySelf()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Destructor - stops the progress bar
	Notes:		Derived classes should call this subsequent to any specialized destruction
				code of their own.

********************************************************************************************/
JPEGProgressBySelf::~JPEGProgressBySelf()
{
	// If started, then stop then progress bar
	if (m_pProgressBar != NULL)
	{
		delete m_pProgressBar;
		m_pProgressBar = NULL;
	}
}


/********************************************************************************************

>	virtual void JPEGProgressBySelf::OnUpdate()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Called by JPEGProgressMonitor::Update() to provide feedback on the filter
				progress. Derived classes should provide an implementation for this.
				JPEGProgressBySelf calls the contained Progress::Update member with the
				percentage done.

********************************************************************************************/
void JPEGProgressBySelf::OnUpdate()
{
	/*
		Fraction done =

			completed_passes + (pass_counter/pass_limit)
			--------------------------------------------
						total_passes
	*/
	UINT32 PercentDone = (UINT32) (100L * completed_passes + (100L * pass_counter) / pass_limit)
							/ total_passes;

	// Update the progress count, if required
	// but don't bother if the user aborted
	if (PercentDone != m_uLastPercentDone && m_bContinueOp)
	{
		m_bContinueOp = m_pProgressBar->Update(PercentDone);
		m_uLastPercentDone = PercentDone;
	}
}





/********************************************************************************************

>	JPEGProgressByFilter::JPEGProgressByFilter(j_decompress_ptr cinfo, Filter* pFilterToUse,
												UINT32 Increment);

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Inputs:		cinfo : the structure provided by the IJG library
				pFilterToUse : a pointer to a Filter with an IncProgressBarCount(UINT32 ...) 
				member, which will be called with an update of the percentage progress made.
				ImportSize : the size of the file we are importing

	Purpose:	Constructor for a class providing a progress callback to the IJG Library
				JPEG filter.
	Notes:		No progress bar will be constructed via this constructor. The given filter
				is expected to provide one.
	See Also:	JPEGProgressBySelf

********************************************************************************************/
JPEGProgressByFilter::JPEGProgressByFilter(j_decompress_ptr cinfo, Filter* pFilterToUse,
						UINT32 ImportSize)
	: JPEGProgressMonitor(cinfo)
{
	m_pFilterForUpdate	= pFilterToUse;
	m_uLastScanLineDone	= 0;
	m_uImportSize = ImportSize;
}


/********************************************************************************************

>	virtual void JPEGProgressByFilter::OnUpdate()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Called by JPEGProgressMonitor::Update() to provide feedback on the filter
				progress. Derived classes should provide an implementation for this.
				JPEGProgressByFilter calls the contained Filter's Update member with the
				number of scanlines processed.

********************************************************************************************/
void JPEGProgressByFilter::OnUpdate()
{
	INT32 Difference = m_pcinfo->output_scanline - m_uLastScanLineDone;
	if (Difference > 0)
	{
		UINT32 PerLineIncrement = m_uImportSize / m_pcinfo->output_height;
		m_pFilterForUpdate->IncProgressBarCount(Difference * PerLineIncrement);
		m_uLastScanLineDone += Difference;
	}
}
