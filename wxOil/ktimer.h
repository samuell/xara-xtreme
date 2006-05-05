// $Id: ktimer.h 869 2006-04-24 14:03:18Z alex $
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

#ifndef INC_KTIMER
#define	INC_KTIMER

class KernelTimer;

/********************************************************************************************

>	class TimerMsg: public Msg

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	25/04/2006
	Purpose:	This message is sent whenever something happens with a timer
	SeeAlso:	-

********************************************************************************************/

class TimerMsg: public Msg
{
	CC_DECLARE_DYNAMIC(TimerMsg);
public:

	enum TimerMsgType
	{
		NOTIFY			// A timer event has occurred 
	};

	TimerMsgType m_MsgType;
	KernelTimer* m_pTimer; // A pointer to the timer concerned

	TimerMsg(TimerMsgType MsgType=TimerMsg::NOTIFY, KernelTimer * pTimer = NULL)
	  : m_MsgType(MsgType), m_pTimer(pTimer)
		{ /* empty */ }
};


/*******************************************************************************************

>	class KernelTimer : public CCObject

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	25/04/2005
	Purpose:	A class exposing a timer to the kernel
	Notes:		In the OIL
	See Also:	

This class provides a kernel timer object, with an OIL-layer independent interface layer.
Currently it uses wxTimer, though any timer interface could be used.

There are two main ways of using this class:
1. As is, in which case it will broadcast timer events
2. By deriving a class from it, and overriding Notify()

If you know exactly where the notifcation should go, then in your class definition
do something like the following, which will avoid any broadcasts.

	class MyClass : public CCObject
	{
		MyClass() : m_Timer(this);
		void OnTimer();

	private:
		class MyTimer : public KernelTimer
		{
		public:
			MyTimer(MyClass * pOwner) : m_pOwner(pOwner) {}
			virtual void Notify() {m_pOwner->OnTimer();}
			MyClass * m_pOwner;
		};
		MyTimer m_Timer;
	}

********************************************************************************************/

class KernelTimer : public CCObject
{
	CC_DECLARE_DYNAMIC(KernelTimer);

private:
	CCRuntimeClass* m_BroadcastClass;

public:
	KernelTimer(CCRuntimeClass * BroadcastClass = NULL) : m_BroadcastClass (BroadcastClass), m_Timer(this) {}
	~KernelTimer() {}

	void SetBroadcastClass (CCRuntimeClass * BroadcastClass) {m_BroadcastClass = BroadcastClass;}
	CCRuntimeClass * GetBroadcastClass () const {return m_BroadcastClass;}

protected:
	virtual void Notify()
	{
		Msg * pMsg = new TimerMsg(TimerMsg::NOTIFY, this);
		if (pMsg)
			MessageHandler::Broadcast(pMsg, m_BroadcastClass);
	}

private:

	class CamTimer : public wxTimer
	{
	public:
		CamTimer(KernelTimer * pKTimer = NULL) : m_pKTimer(pKTimer) {};
		~CamTimer() {};
		KernelTimer * m_pKTimer;
		virtual void Notify() {if (m_pKTimer) { m_pKTimer->Notify();}}
	};

	CamTimer m_Timer;

public:
	INT32 GetInterval() const { return m_Timer.GetInterval(); }
	BOOL IsOneShost() const { return m_Timer.IsOneShot(); }
	BOOL IsRunning() const { return m_Timer.IsRunning(); }
	BOOL Start(INT32 milliseconds = -1, BOOL OneShot = FALSE) { return m_Timer.Start(milliseconds, OneShot); }
	void Stop() { m_Timer.Stop(); }
};

#endif

