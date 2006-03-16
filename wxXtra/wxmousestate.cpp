// $Id: wxmousestate.cpp 479 2006-02-15 18:30:13Z alex $
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

// This file is derived from CVSHEAD wxWidgets and is thus
// under the wxWidgets / wxWindows license.

#include "wxmousestate.h"

#ifndef HAVE_WXGETMOUSESTATE

#if defined( __WXGTK__ )

// Avoid includes as we don't know where they are...

typedef enum
{
  GDK_SHIFT_MASK    = 1 << 0,
  GDK_LOCK_MASK     = 1 << 1,
  GDK_CONTROL_MASK  = 1 << 2,
  GDK_MOD1_MASK     = 1 << 3,
  GDK_MOD2_MASK     = 1 << 4,
  GDK_MOD3_MASK     = 1 << 5,
  GDK_MOD4_MASK     = 1 << 6,
  GDK_MOD5_MASK     = 1 << 7,
  GDK_BUTTON1_MASK  = 1 << 8,
  GDK_BUTTON2_MASK  = 1 << 9,
  GDK_BUTTON3_MASK  = 1 << 10,
  GDK_BUTTON4_MASK  = 1 << 11,
  GDK_BUTTON5_MASK  = 1 << 12,
  /* The next few modifiers are used by XKB, so we skip to the end.
   * Bits 16 - 28 are currently unused, but will eventually
   * be used for "virtual modifiers". Bit 29 is used internally.
   */
  GDK_RELEASE_MASK  = 1 << 30,
  GDK_MODIFIER_MASK = GDK_RELEASE_MASK | 0x1fff
} GdkModifierType;

extern "C" { void* gdk_window_get_pointer(void*, int*, int*, GdkModifierType*); }

wxMouseState wxGetMouseState()
{
    wxMouseState ms;

    gint x;
    gint y;
    GdkModifierType mask;

    gdk_window_get_pointer(NULL, &x, &y, &mask);

    ms.SetX(x);
    ms.SetY(y);
    ms.SetLeftDown(mask & GDK_BUTTON1_MASK);
    ms.SetMiddleDown(mask & GDK_BUTTON2_MASK);
    ms.SetRightDown(mask & GDK_BUTTON3_MASK);

    ms.SetControlDown(mask & GDK_CONTROL_MASK);
    ms.SetShiftDown(mask & GDK_SHIFT_MASK);
    ms.SetAltDown(mask & GDK_MOD1_MASK);
    ms.SetMetaDown(mask & GDK_MOD2_MASK);

    return ms;
}

#elif defined(__WXMAC__) || defined(__WXCOCOA__)

#ifdef __DARWIN__

#include <CoreServices/CoreServices.h>
#include "/Developer/Headers/FlatCarbon/CarbonEvents.h"

#else

#include <Windows.h>
#include <ToolUtils.h>
#include <Scrap.h>
#include <MacTextEditor.h>

#endif

wxMouseState wxGetMouseState()
{
    wxMouseState ms;

    wxPoint pt = wxGetMousePosition();
    ms.SetX(pt.x);
    ms.SetY(pt.y);

    UInt32 buttons = GetCurrentButtonState();
    ms.SetLeftDown( (buttons & 0x01) != 0 );
    ms.SetMiddleDown( (buttons & 0x04) != 0 );
    ms.SetRightDown( (buttons & 0x02) != 0 );

    UInt32 modifiers = GetCurrentKeyModifiers();
    ms.SetControlDown(modifiers & controlKey);
    ms.SetShiftDown(modifiers & shiftKey);
    ms.SetAltDown(modifiers & optionKey);
    ms.SetMetaDown(modifiers & cmdKey);

    return ms;
}

#elif defined ( __WXMSW__ )


wxMouseState wxGetMouseState()
{
    wxMouseState ms;
    POINT pt;
    GetCursorPos( &pt );

    ms.SetX(pt.x);
    ms.SetY(pt.y);
    ms.SetLeftDown( (GetAsyncKeyState(VK_LBUTTON) & (1<<15)) != 0 );
    ms.SetMiddleDown( (GetAsyncKeyState(VK_MBUTTON) & (1<<15)) != 0 );
    ms.SetRightDown( (GetAsyncKeyState(VK_RBUTTON) & (1<<15)) != 0 );

    ms.SetControlDown( (GetAsyncKeyState(VK_CONTROL) & (1<<15)) != 0 );
    ms.SetShiftDown( (GetAsyncKeyState(VK_SHIFT) & (1<<15)) != 0 );
    ms.SetAltDown( (GetAsyncKeyState(VK_MENU) & (1<<15)) != 0 );
//    ms.SetMetaDown();

    return ms;
}

#else

#error "Platform not supported - no wxGetMouseState()"

#endif

#endif // HAVE_WXGETMOUSESTATE
