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

// The progress indicator, for slow jobs (kernel interface).

#ifndef	PROGRESS__INC
#define	PROGRESS__INC

#include "list.h"

// **** DEPRECATED INTERFACE **** 
// Use the calls provided by the ProgressState class (kernel\progstat.h) in future. DO NOT USE these ones!
BOOL BeginSlowJob(INT32 final = -1, BOOL delay = TRUE, String_64 *Description = NULL);
BOOL ContinueSlowJob(INT32 upto = 0);
void EndSlowJob(void);
void SmashSlowJob(void);
// **** -------------------- ****

/********************************************************************************************

>	class Progress : public CC_CLASS_MEMDUMP

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/95

	Purpose:	Encapsulates progress displays in a tidy class layout.
				As well as providing a tidier static interface which emulates the old
				'SlowJob' system, a local instance of this class can be created in your
				busy function, which will _automatically_ turn off progress indications
				when it is destructed on exit from your function.

	Notes:		You shouldn't call the Progress class directly. Use ProgressState instead

	SeeAlso:	class ProgressState; Progress::Start; Progress::Progress
				
********************************************************************************************/ 

class String_64;
class MonotonicTime;


class Progress : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(Progress)

public:			// Initialisation/Deinitialisation
	static BOOL Init(void);
		// Returns FALSE if initialisation failed

	static BOOL Deinit(void);
		// Returns FALSE if deinitialisation failed


public:			// External interface
	Progress();
		// Do not use this constructor, except in special circumstances

	Progress(StringBase *Description, INT32 FinalCount = -1,BOOL Delay = TRUE, BOOL bEnable = TRUE);
	Progress(UINT32 DescriptionID, INT32 FinalCount = -1,BOOL Delay = TRUE, BOOL bEnable = TRUE);
	~Progress();
		// An instance of this class will automatically start a progresss
		// indication going, and stop it again on destruction

public:
	static void Start(BOOL Delay = FALSE, StringBase *Description = NULL, INT32 FinalCount = -1);
	static void Start(BOOL Delay, UINT32 DescriptionID, INT32 FinalCount = -1);
		// Start a progress indication going

	static BOOL Update(INT32 CurrentCount = 0,BOOL FreqEscChecks = FALSE);
		// Update the progress indicator.
		// Called to change the displayed percentage and implement show-with-delay.

	static void Stop(void);
		// Stop a progress indication going

	static void Smash(BOOL ForceSmash = FALSE);
		// Stop all current progress indications

	static void SetFinalCount(INT32 n);
		// Allows you to set the final count after construction.  Use before you call Update().
		// Only updates the count if this is the only progress object around.

	// Functions for maintaining a count var for this instance of a Progress object
	// These are provided for convenience
	INT32 GetCount(INT32 n=0);		// Get the current count, and add 'n' to it, while you're at it
	void ResetCount(INT32 n=0); 		// Reset the count to 'n'

	static BOOL SetDescription(const StringBase* const Description);

	static void RalphAbortJob(){AbortJob = TRUE;};
	static void ClearRalphAbortJob(){AbortJob = FALSE;};
	static BOOL IsRalphAbort(){return AbortJob;};

public:
	static void ThreadCallback(void);
		// INTERNAL USE ONLY. Called back by a thread for delayed-show of the hourglass


protected:		// Internal state variables
	static INT32 ActiveDisplays;				// A count of the number of active progress incarnations
	static BOOL JobCancelled;				// TRUE if the job has been cancelled

	static BOOL JobDelayed;					// TRUE if the hourglass startup is delayed

	static INT32 FinalCount;					// The number we're currently counting up to
	static INT32 CurrentPercent;				// The last percentage that we displayed

	static BOOL HourglassShown;				// TRUE if we have already shown the hourglass

	static String_64 JobDescription;		// A description of the current job
	static BOOL HaveJobDescription;			// TRUE if we have a proper caller-supplied job description

	static MonotonicTime StartTime;			// Time the first call to Show() was made

//	static CProgressBar *ProgBar;			// The progress bar object (if used)

	static BOOL AbortJob;					// used to abort jobs in ralph - fake an escape condition
protected:		// Preferences
	static INT32 DisplayBar;					// Preference: Is progress bar shown?

	// The current count for this instance of a progress object
	INT32 ProgressCount;
};


#endif	// PROGRESS__INC
