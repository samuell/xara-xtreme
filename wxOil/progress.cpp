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

//	The progress indicator, for slow operations (kernel implementation).


#include "camtypes.h"

#include "app.h"
#include "csrstack.h"
//#include "jason.h"
#include "keypress.h"
//#include "mainfrm.h"
#include "monotime.h"
//#include "oilprog.h"
//#include "progbar.h"
#include "node.h"

#include "progress.h"


DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_MEMDUMP(Progress, CC_CLASS_MEMDUMP)

#define new CAM_DEBUG_NEW


BOOL Progress::AbortJob		= FALSE;		// TRUE if the we want to abort a job in ralph

PORTNOTE("progress", "Removed progress system")
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

// --- Thread activation - set to 1 to enable the delayed-show thread
// (NOTE: The start-delay thread is currently non-operational, so turning it on
// has no effect. See below for details)
#define DELAY_THREAD 0

// --- Debugging trace statements. Set to 1 to turn on debug output
#define DEBUG_OUTPUT 0


// --- Constants
// IMMEDIATE REDRAW
// const INT32 STARTDELAY = 3333;
const INT32 STARTDELAY = 333;		 				// Default delay before cursor appears (1/3rd sec)



/********************************************************************************************

>	BOOL BeginSlowJob(INT32 finalcount = -1, BOOL delay = TRUE, String_64 *Description = NULL)
	Purpose:	DEPRECATED - do not call this function!
	SeeAlso:	Progress::Start; Progress::Progress

********************************************************************************************/

BOOL BeginSlowJob(INT32 finalcount /* =-1 */, BOOL delay /* = TRUE */,
					String_64 *Description /* = NULL */)
{
	Progress::Start(delay, Description, finalcount);

	return(TRUE);
}



/********************************************************************************************

>	BOOL ContinueSlowJob(INT32 upto = 0) 
	Purpose:	DEPRECATED - do not call this function!
	SeeAlso:	Progress::Update

********************************************************************************************/

BOOL ContinueSlowJob(INT32 upto)
{
	return(Progress::Update(upto));
}



/********************************************************************************************

>	void EndSlowJob(void)
	Purpose:	DEPRECATED - do not call this function!
	SeeAlso:	Progress::Stop

********************************************************************************************/

void EndSlowJob(void)
{
	Progress::Stop();
}



/********************************************************************************************

>	void SmashSlowJob(void)
	Purpose:	DEPRECATED - do not call this function!
	SeeAlso:	Progress::Smash

********************************************************************************************/

void SmashSlowJob(void)
{
	Progress::Smash();
}











// -----------------------------------------------------------------------------------------
// --- The new Progress class - firstly the static threading support

#if DELAY_THREAD
// Thread control
volatile static BOOL ThreadAlive = FALSE;			// TRUE while the thread is alive - set to FALSE to kill it


/********************************************************************************************

>	static UINT32 ThreadProc(LPVOID Param)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		10/11/95

	Purpose:	This function is the progress system's delayed-show thread. It runs
				alongside the main program, sleeping for 1/3rd of a second. After this
				delay period, it calls Progress::ThreadCallback.

	Notes:		While this thread is running, ThreadAlive will be TRUE.
				Set ThreadAlive to FALSE to kill the thread.

	Scope:		static/private in winoil\progress.cpp

	SeeAlso:	Progress::ThreadCallback

********************************************************************************************/

static UINT32 ThreadProc(LPVOID Param)
{
	// If there's already an active thread, then we'll exit immediately
	if (ThreadAlive)
		return(0);

	// Flag the fact that we are alive
	ThreadAlive = TRUE;

	// While we're alive, update the progress display periodically. 
	// The main program thread will set ThreadAlive=FALSE if they want us to stop
	while (ThreadAlive)
	{
		Sleep(STARTDELAY);

		if (ThreadAlive)	// Check again as the value may have changed while we slept
			Progress::ThreadCallback();
	}

	return(0);
}
#endif



/********************************************************************************************

>	static void Progress::ThreadCallback(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		10/11/95

	Purpose:	This method is called by our delay thread to show any delayed progress
				displays that we wish to display. It should not be called from elsewhere.

	Notes:		There are 2 progress displays - the hourglass and progress bar. There is no
				point in showing the bar until there is a percentage to display, so this
				function only turns on the hourglass cursor.

				NOTE also that if you change this method do anything more than this, you
				must be very careful - generally the thread can introduce instability
				that causes random access violations and stuff.

	Scope:		public, but don't let that fool you. It'd be private but for a technical hitch.

	SeeAlso:	::ThreadProc

********************************************************************************************/

void Progress::ThreadCallback(void)
{
#if DELAY_THREAD
	//-- This does not work. The SetCursor call is executed, but the cursor stubbornly
	//-- refuses to change shape - I suspect this is because the window under the cursor
	//-- belongs to the main process thread, not us...

	// Tell the thread that it can exit now
	ThreadAlive = FALSE;

	// And change the cursor shape to an hourglass
	// NOTE that I don't call BeginBusyCursor here, to avoid the cursor stack
	// doing anything - that way the busy pointer will happily go away as soon as anyone
	// tries to do anything with the Camelot cursor stack.
	HCURSOR Busy = ::LoadCursor(NULL, _R(IDC_WAIT));
	::SetCursor(Busy);
#endif
}




// -----------------------------------------------------------------------------------------
// --- The new Progress class

// --- Statics
INT32 Progress::ActiveDisplays	= 0;			// A count of the number of active progress incarnations
BOOL Progress::JobCancelled		= FALSE;		// TRUE if the job has been cancelled

BOOL Progress::JobDelayed		= FALSE;		// TRUE if the job has a delayed-start

INT32 Progress::FinalCount		= 0;			// The number we're currently counting up to
INT32 Progress::CurrentPercent	= 0;			// The number we're currently counting up to

BOOL Progress::HourglassShown	= FALSE;		// TRUE if we have already shown the hourglass

String_64 Progress::JobDescription = TEXT("");	// A description of the current job
BOOL Progress::HaveJobDescription = FALSE;		// TRUE if JobDescription is a proper (caller supplied) one

MonotonicTime Progress::StartTime;				// Time the first call to Show() was made

CProgressBar *Progress::ProgBar = NULL;			// The progress bar object (if used)


/********************************************************************************************

	Preference:		ProgressBar
	Section:		Displays
	Range:			TRUE or FALSE
	Purpose:		If TRUE, a progress bar will be displayed while Camelot is busy
					processing.
	SeeAlso:		Hourglass

********************************************************************************************/

INT32 Progress::DisplayBar = TRUE;				// Preference: Is progress bar shown?





/********************************************************************************************

>	Progress::Progress()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		10/11/95

	Purpose:	Constructs an instance of a Progress object.
				This starts the hourglass running.
				Destruct the object to stop the hourglass.

				See Progress::Start() for more details on the progress system

	Notes:		IMPORTANT!
				Only use this constructor for small, deep-down functions which you can't
				supply a useful operation description for. Whenever we're busy we must
				endeavour to show a useful description to the user.

	SeeAlso:	Progress::~Progress; Progress::Start

********************************************************************************************/

Progress::Progress()
{
	ProgressCount = 0;

	Progress::Start(TRUE, (StringBase *)NULL, -1);
}



/********************************************************************************************

>	Progress::Progress(StringBase *Description, INT32 FinalCount,BOOL Delay=TRUE, BOOL bEnable = TRUE)
>	Progress::Progress(UINT32 DescriptionID, INT32 FinalCount,BOOL Delay=TRUE, BOOL bEnable = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		10/11/95

	Inputs:		Description - A pointer to a string describing what the program is busy doing
				or DescriptionID - The string Resource ID of the string to use

				FinalCount	- -1 if you want no progress bar (percentage) indication, else
							  the number you expect to count up to (e.g. number of lines
							  in the file you're reading, number of nodes you're changing, etc)

				Delay       - TRUE if you want the hourglass to appear after a short delay
							  FALSE for it to appear immediately

	Purpose:	Constructs an instance of a Progress object.
				This starts the hourglass (and optional progress bar) running.
				Destruct the object to stop the hourglass.

				See Progress::Start() for more details on the progress system

	SeeAlso:	Progress::~Progress; Progress::Start; Progress::Update

********************************************************************************************/

Progress::Progress(StringBase *Description, INT32 FinalCount,BOOL Delay, BOOL bEnable)
{
	if (bEnable)
	{
		ProgressCount = 0;
		ERROR3IF(Description == NULL, "Progress constructor - Illegal NULL parameter");
		Progress::Start(Delay, Description, FinalCount);
	}
}


Progress::Progress(UINT32 DescriptionID, INT32 FinalCount,BOOL Delay, BOOL bEnable)
{
	if (bEnable)
	{
		ProgressCount = 0;
		ERROR3IF(DescriptionID == NULL, "Progress constructor - Illegal NULL parameter");
		Progress::Start(Delay, DescriptionID, FinalCount);
	}
}



/********************************************************************************************

>	Progress::~Progress()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		10/11/95

	Purpose:	Destructs an instance of a Progress object.
				This stops the hourglass (and optional progress bar) which the constructor
				started going.

	SeeAlso:	Progress::Progress; Progress::Stop

********************************************************************************************/

Progress::~Progress()
{
	Progress::Stop();
}


/********************************************************************************************

>	BOOL Progress::SetDescription(const StringBase* const Description)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Date:		18/12/96

	Inputs:		Description:	See Start() for a description of this parameter

	Purpose:	Gives the user some textual description of what's going on when the hourglass
				is blocking the view.

********************************************************************************************/
BOOL Progress::SetDescription(const StringBase* const Description)
{
	Description->Left(&JobDescription, 63);
	HaveJobDescription = TRUE;

	return TRUE;
}


/********************************************************************************************

>	void Progress::Start(BOOL Delay, StringBase *Description, INT32 FinalCount)
>	void Progress::Start(BOOL Delay, UINT32 DescriptionID, INT32 FinalCount)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		10/11/95

	Inputs:		Delay		- TRUE to start the hourglass after a 1/3rd second delay
							  FALSE to show the hourglass immediately [see Notes]

				Description - NULL ("Processing... please wait") or a pointer to a string
							  describing what the program is currently busy doing
				or DescriptionID - NULL or the string Resource ID of the string to use

				FinalCount	- -1 if you want no progress bar (percentage) indication, else
							  the number you expect to count up to (e.g. number of lines
							  in the file you're reading, number of nodes you're changing, etc)

	Purpose:	[NOTE that to show a progress display, the easiest system is to construct
				a Progress object - this will automatically turn the hourglass off when the
				object is destructed. Only use this call if you need lower level control
				or it is a lot more convenient than a simple instance of the class]

				Starts a progress (hourglass and optional progress bar) indication. Call this
				when you start doing something that could take a significant amount of time
				(more than half a second delay should show an hourglass under all circumstances)

	Notes:		The delay parameter specifies whether the hourglass should be shown immediately
				or if its appearance should be put off for a short time in case you don't take
				as long as you thought you might. NOTE however, that you *must* call 
				Progress::Update at frequent intervals or the delayed hourglass may never appear!
				Note that Progress::Update is very efficient, so may be called very often (it
				only updates 6 times a second at the most).

				IMPORTANT - The description should be filled in with an appropriate string.
				We must avoid use of the default generic description whenever possible.

				Multiple progress indications may be active at any time. In this case, the
				calls to Start and Stop are "stacked" so the hourglass will only go away when
				all Start calls have been countermanded by Stops. Preferably, the outermost
				Start/Stop will provide a progress percentage, while the inner ones will
				simply use Start() with the default parameters, merely to ensure that an
				hourglass is shown while they are busy. Don't rely upon your caller to
				show the hourglass for you! The start/stop/update calls have all been designed
				to be called often without introducing noticable performance overheads.

	SeeAlso:	Progress::Start; Progress::Update; Progress::Stop; Progress::Smash;
				Progress::Progress

********************************************************************************************/

void Progress::Start(BOOL Delay, UINT32 DescriptionID, INT32 IntendedFinalCount)
{
	if (DescriptionID == NULL)
		Progress::Start(Delay, (StringBase *)NULL, IntendedFinalCount);
	else
	{
		String_256 Description(DescriptionID);

		Progress::Start(Delay, (StringBase *)&Description, IntendedFinalCount);
	}
}



void Progress::Start(BOOL Delay, StringBase *Description, INT32 IntendedFinalCount)
{
#if DEBUG_OUTPUT
TRACE( _T("Progress::Start ActiveDisplays = %d IntendedFinalCount = %d\n"),ActiveDisplays,IntendedFinalCount);
#endif

	// Ensure the ActiveDisplays value is valid
	if (ActiveDisplays < 0)
		ActiveDisplays = 0;

	ActiveDisplays++;
	if (ActiveDisplays <= 1)
	{
		// --- We're starting a brand new progress indication - set everything up
		JobDelayed		= Delay;
		JobCancelled	= FALSE;
		CurrentPercent	= 0;
		FinalCount		= IntendedFinalCount;
		HourglassShown	= FALSE;

		// Remember when we started and when we last updated the screen displays
		StartTime.Sample();

		if (Description != NULL)
		{
			SetDescription(Description);
		}
		else
		{
			JobDescription.MakeMsg(_R(IDS_BUSYMSG));
			HaveJobDescription = FALSE;
		}

		// Inform the Node subsystem that we're putting up an hourglass
		Node::StartHourglass();

		// Update the status line text
		GetApplication()->UpdateStatusBarText((String_256 *)&JobDescription, FALSE);

#if DELAY_THREAD
		// Attempt to start a thread to show the hourglass after a delay
		if (JobDelayed)
			::AfxBeginThread(ThreadProc, NULL);
#endif
	}
	else
	{
		// --- We're starting a progress indication while another one is already active.
		// If they've got a better idea about what the final count should be, use their estimate

		// If an "inner" job wants the hourglass to _not_ be delayed, we override the
		// current JobDelayed state to make sure it definitely shows an hourglass
		if (!Delay)
			JobDelayed = FALSE;

		// Removed by Jason, 22/1/96. This causes ensures to go off and bad progress
		// displays when you have a case like the following:
		//		Start (-1)
		//			Start(100)
		//			End
		//			Start(200)
		//			End
		//		End
		// ...In the 2nd progress bar, the FinalCount remains at 100, so Update error3's
		// This occurs in saving/exporting and other multi-part busy processes
		// A proper fix for this problem requires proper nesting of hourglasses,
		// which is not feasible at this stage (a *lot* of conversion work on
		// existing code would be necessary).
		// This still won't work if progress bars are nested, but should work
		// reasonably well otherwise.
		//		if (FinalCount <= 0)
		//			FinalCount = IntendedFinalCount;
		// The replacement code for the above now occurs below, after we get the JobDescription
		

		// Similarly, if we have only a generic job desc and they've supplied one, then we'll
		// use the one that they supplied.
		if (!HaveJobDescription && Description != NULL)
		{
			SetDescription(Description);
		}

		// Check if we're starting a new progress-bar job, in which case it overrides
		// any previous settings for FinalCount and the description string
		if (IntendedFinalCount > 0)
		{
			FinalCount = IntendedFinalCount;

			// And reset the current percentage and job description fields
			CurrentPercent	= 0;
			if (Description != NULL)
			{
				SetDescription(Description);
			}

			// And reset/redraw the progress bar display to make sure it starts from 0 again
			// and shows the new job description (if any).
			if (ProgBar != NULL)
				ProgBar->SetPercent(0, TRUE, (HaveJobDescription) ? &JobDescription : NULL);
		}
	}

	// And finally, update to show the current state. This will show the progress displays
	// if appropriate at this time, etc.
	Progress::Update(0);
}



/********************************************************************************************

>	BOOL Progress::Update(INT32 CurrentCount = 0,BOOL FreqEscChecks = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		10/11/95

	Inputs:		CurrentCount - 0 if you are not maintaining a percentage display, or if you
				simply don't want to update the percentage on this particular call.
				Otherwise, a value between 0 and the FinalCount value passed to Start().
				This value is used to calculate a percentage value to display.

				FreqEscChecks = if TRUE, the escape key is checked once, guaranteed
								if FALSE, the escape key will only be checked if the percentage displayed increases

	Returns:	TRUE if everything is OK
				FALSE if the user has aborted the job by pressing 'escape'

	Purpose:	Updates progress indications. This serves 3 purposes:
				a) To show the progress indications if a delayed-start was specified in
				   your call to Start(). This is the default action.
				b) To check for the user pressing escape to abort the job
				c) To change the displayed percentage on the progress bar and animated cursor

	Notes:		This function is very efficient so it is perfectly safe to call it often.

				In fact, if it is not called often,
				a) Delayed start won't work, and you may not get an hourglass at all!
				b) The user won't be able to abort part way through a job

				WHEN IN DOUBT, CALL THIS ROUTINE. IT IS BETTER TO CALL IT TOO OFTEN
				THAN TO NOT CALL IT OFTEN ENOUGH.

				It is safe to call this function at ANY time, even if no progress indication
				is active, so if your function can take more than 1/3rd of a second to
				execute, it really should call Update occasionally just to ensure smooth
				operation.

				You need not check the return value on every call. Once the user has pressed
				escape, this function will always return FALSE until someone takes note.
				Thus, routines which cannot cope with abortion can possibly leave abortion
				to their callers.

				The percentage is only updated if it has become greater than the currently
				displayed percentage value.

				The FreqEscChecks parameter (markn) - this param will control the frequency
				with which the escape key is checked when this function is called.
				If it is TRUE, it is checked once, guaranteed.
				If it is FALSE, it is only checked if the percentage displayed increases.

				You should only call with FreqEscChecks = FALSE if you can detect a performance hit due
				to too many escape key checks.
				
				NOTE: If you call with FreqEscChecks = FALSE, and you never display a percentage, the user
				will not be able to abort the op part way through.



	SeeAlso:	Progress::Start; Progress::Update; Progress::Stop; Progress::Smash

********************************************************************************************/

BOOL Progress::Update(INT32 CurrentCount,BOOL FreqEscChecks)
{
#if DEBUG_OUTPUT
TRACE( _T("Progress::Update ActiveDisplays = %d CurrentCount = %d FinalCount=%d\n"),ActiveDisplays,CurrentCount, FinalCount);
#endif

	// --- If no active progress display is on, then return immediately
	if (ActiveDisplays <= 0)
	{
		ActiveDisplays = 0;
		return(TRUE);
	}

	// --- If the job has been cancelled, then return FALSE to indicate user intervention
	if (JobCancelled)
		return(FALSE);

	// If FreqEscChecks is TRUE, ensure we check the escape key, no matter what percentage is displayed	
	if (FreqEscChecks)
	{
		// Has the user cancelled the job via the Escape key?
		if (KeyPress::IsEscapePressed())
		{
			JobCancelled = TRUE;
			Beep();
			return(FALSE);
		}
	}

	// --- If the startup delay (if any) has not expired, we do nothing just yet
	if (JobDelayed && !StartTime.Elapsed(STARTDELAY))
		return(TRUE);

	// --- Check that the parameter passed in was vaguely sensible
	ERROR3IF(CurrentCount < 0, "Progress::Update called with out of range parameter");

	// --- If we haven't yet shown the hourglass cursor, do it now
	if (!HourglassShown)
	{
#if DELAY_THREAD
		ThreadAlive = FALSE;		// Stop the delay thread (if any) because it's not needed now
#endif
		BeginBusyCursor();
		HourglassShown = TRUE;

	}

	// --- Calculate the new percentage to display. This is limited to lie between
	// the last displayed percentage and 99 inclusive.
	INT32 NewPercent = CurrentPercent;
	if (FinalCount > 0)
	{
		NewPercent = (100 * CurrentCount) / FinalCount;

		if (NewPercent > 99)
			NewPercent = 99;
		
		if (NewPercent < CurrentPercent)
			NewPercent = CurrentPercent;
	}

	// --- Now, update the percentage display if it has changed, or if it is a while since
	// our last update. NOTE: Updates occur periodically for (a) checking the escape key state
	// occasionally, so we can be called often without a huge performance hit, and (b) to 
	// update frames of our animated hourglass pointer.
	if (NewPercent > CurrentPercent)
	{
		// If FreqEscChecks is FALSE, we only check the escape key when the percentage increases
		if (!FreqEscChecks)
		{
			// Has the user cancelled the job via the Escape key?
			if (KeyPress::IsEscapePressed())
			{
				JobCancelled = TRUE;
				Beep();
				return(FALSE);
			}
		}

		// (Create if necessary) and update the progress bar, if it is needed
		if (DisplayBar && FinalCount > 0)
		{
			if (ProgBar == NULL)
			{
				ProgBar = new CProgressBar;

				if (ProgBar != NULL)
				{
					if (!ProgBar->Create((CFrameWnd *) (AfxGetApp()->m_pMainWnd), &JobDescription))
					{
						delete ProgBar;
						ProgBar = NULL;
					}
					else
						ProgBar->ShowWindow(SW_SHOW);
				}

			}
			else
				ProgBar->SetPercent(NewPercent);
		}

		// Remember the last displayed percentage
		CurrentPercent = NewPercent;
	}

	return(TRUE);
}



/********************************************************************************************

>	void Progress::Stop(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		10/11/95

	Purpose:	Stops a progress indication started with a call to Start()
				The hourglass and progress bar etc will only be removed once all calls
				to Start() have been countermanded by calls to Stop().

	Notes:		Be careful to call Stop() exactly the same number of times as Start() through
				each possible execution path!

				It is recommended that you do not call Start() and Stop() directly, but 
				instead construct a local instance of the Progress class in your function.
				This will automatically start and stop the hourglass for you, and also
				ensures that Start() and Stop() calls are correctly nested/balanced.

	SeeAlso:	Progress::Start; Progress::Update; Progress::Stop; Progress::Smash

********************************************************************************************/

void Progress::Stop(void)
{
#if DEBUG_OUTPUT
TRACE( _T("Progress::Stop ActiveDisplays = %d \n"),ActiveDisplays);
#endif

	// Decrement the usage count. The progress display remains until the count returns to zero
	ActiveDisplays--;
	if (ActiveDisplays > 0)
		return;

	if (ProgBar != NULL && CurrentPercent < 97)
	{
		// We are showing a progress bar, but have not shown "completion" of the job (99%)
		// Briefly jump to 99% on the progress bar so the user can't see how bad our estimate
		// of when we'd finish really was.

		ProgBar->SetPercent(99);

		MonotonicTime Timer;
		while (!Timer.Elapsed(150))
			/* wait */ ;
	}

	// Make sure everything is reset properly
	Progress::Smash(TRUE);
}



/********************************************************************************************

>	void Progress::Smash(BOOL ForceSmash = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		10/11/95

	Inputs:		ForceSmash - Pass in FALSE for this parameter - for internal use only
				[Forces the hourglass to be smashed even if we believe it is not actually
				running at present. Used by Progress::Stop to kill its displays when its
				usage count is decremented to zero. Calling with TRUE is a lot less efficient
				than with FALSE when no jobs are currently active]

	Purpose:	Smashes the hourglass and other progress displays. This kills all active
				progress indications immediately, as if Progress::Stop() had been called enough
				times to cancel all active displays.

	Notes:		Returns very quickly if no jobs are running, so can be called often
				without significant loss of efficiency.

				This is called on each idle event to ensure that spurious hourglasses
				are removed.

	SeeAlso:	Progress::Start; Progress::Update; Progress::Stop; Progress::Smash

********************************************************************************************/

void Progress::Smash(BOOL ForceSmash)
{
	if (ActiveDisplays > 0 || ForceSmash)
	{
#if DELAY_THREAD
		ThreadAlive = FALSE;		// Stop the delay thread (if any)
#endif

		// --- Tell the node subsystem that we're turning off the hourglass
		Node::StopHourglass();

		// --- Blank the status line text
		String_256 Blank("");
		GetApplication()->UpdateStatusBarText(&Blank);

		// --- Remove any active progress bar
		if (ProgBar != NULL)
		{
			delete ProgBar;
			ProgBar = NULL;

			// Make sure it disappears immediately
			((CFrameWnd *) (AfxGetApp()->m_pMainWnd))->RecalcLayout();
		}

		// --- And reset all variables to suitable defaults
		ActiveDisplays	= 0;
		JobCancelled	= FALSE;

		JobDelayed		= TRUE;
		FinalCount		= 0;
		CurrentPercent	= 0;
		HourglassShown	= FALSE;

		JobDescription.MakeMsg(_R(IDS_BUSYMSG));
	}

	// --- And ensure no busy cursors are left active
	SmashBusyCursor();
}



/********************************************************************************************

>	static BOOL Progress::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		10/11/95

	Returns:	TRUE for success, FALSE for failure

	Purpose:	Initialise the progress display system. Call once on startup
				(see main3.cpp)

	SeeAlso:	Progress::DeInit

********************************************************************************************/

BOOL Progress::Init(void)
{
	Camelot.DeclareSection(TEXT("Displays"), 8);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("ProgressBar"), &DisplayBar);

	return(TRUE);		// We successfully initialised
}



/********************************************************************************************

>	static BOOL Progress::DeInit(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		10/11/95

	Returns:	TRUE for success, FALSE for failure

	Purpose:	De-initialise the progress display system. Call once on shutdown
				(see main.cpp)

	SeeAlso:	Progress::Init

********************************************************************************************/

BOOL Progress::Deinit(void)
{
	return(TRUE);
}

/********************************************************************************************

>	static void Progress::SetFinalCount(INT32 n);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Date:		18/1/96
	Inputs:		n = the final count value
	Returns:	-
	Purpose:	Allows you to set this value after construction of a Progress object.
				You should call this value before you make any calls to Update().

				If there are already other Progress objects created, this call does nothing.

	SeeAlso:	Progress::Progress()

********************************************************************************************/

void Progress::SetFinalCount(INT32 n)
{
//	if (ActiveDisplays <= 1)
//	{
		// --- We're starting a brand new progress indication - so set it up
		FinalCount = n;
//	}
}

/********************************************************************************************

>	INT32 Progress::GetCount(INT32 n=0)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Date:		18/1/96
	Inputs:		n = the increment value
	Returns:	Current count+n
	Purpose:	Gets the current progress count, and add 'n' to it.
				'n' is added before the current count is returned

				This function doubles up as an incrementing function for the progress count.

				If the final count var (as supplied to the constructor or SetFinalCount()) is
				less that 1, the counter is not incremented AND 0 is always returned.

	SeeAlso:	Progress::SetFinalCount(), Progress::ResetCount()

********************************************************************************************/

INT32 Progress::GetCount(INT32 n)
{
	if (FinalCount > 0)
	{
		ProgressCount += n;
		return ProgressCount;
	}
	else
		return 0;
}

/********************************************************************************************

>	void Progress::ResetCount(INT32 n=0)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Date:		18/1/96
	Inputs:		-
	Returns:	-
	Purpose:	Resets the progress count to n

	SeeAlso:	Progress::Progress(), Progress::GetCount()

********************************************************************************************/

void Progress::ResetCount(INT32 n)
{
	ProgressCount = n;
}

#else

// Stubs for use in Ralph, who has no progress system...
BOOL BeginSlowJob(INT32 finalcount, BOOL delay, String_64 *Description)
{
	// Ralph
	return TRUE;
}
BOOL ContinueSlowJob(INT32 upto)
{
	// test for abort
	return Progress::Update(0,0);
}
void EndSlowJob(void)
{
	// Ralph
}
void SmashSlowJob(void)
{
	// Ralph
	return;
}
void Progress::ThreadCallback(void)
{
	// Ralph
}
Progress::Progress()
{
	// Ralph
}
Progress::Progress(StringBase *Description, INT32 FinalCount,BOOL Delay, BOOL bEnable)
{
	// Ralph
}
Progress::Progress(UINT32 DescriptionID, INT32 FinalCount,BOOL Delay, BOOL bEnable)
{
	// Ralph
}
Progress::~Progress()
{
	// Ralph
}
void Progress::Start(BOOL Delay, UINT32 DescriptionID, INT32 IntendedFinalCount)
{
	// Ralph
}
void Progress::Start(BOOL Delay, StringBase *Description, INT32 IntendedFinalCount)
{
	// Ralph
}
BOOL Progress::Update(INT32 CurrentCount,BOOL FreqEscChecks)
{
	// test for a fake escape condition
	return !AbortJob;
}
void Progress::Stop(void)
{
	// Ralph
}
void Progress::Smash(BOOL ForceSmash)
{
	// Ralph
}
BOOL Progress::Init(void)
{
	// Ralph
	return TRUE;
}
BOOL Progress::Deinit(void)
{
	// Ralph
	return TRUE;
}
void Progress::SetFinalCount(INT32 n)
{
	// Ralph
}
INT32 Progress::GetCount(INT32 n)
{
	// Ralph
	return 0;
}
void Progress::ResetCount(INT32 n)
{
	// Ralph
}

#endif
