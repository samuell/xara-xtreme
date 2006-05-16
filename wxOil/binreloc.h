// $Id$
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */
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
/*
 * BinReloc - a library for creating relocatable executables
 * Written by: Hongli Lai <h.lai@chello.nl>
 * http://autopackage.org/
 *
 * This source code is public domain. You can relicense this code
 * under whatever license you want.
 *
 * See http://autopackage.org/docs/binreloc/ for
 * more information and how to use this.
 */

#ifndef __BINRELOC_H__
#define __BINRELOC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** These error codes can be returned by br_init(), br_init_lib(), gbr_init() or gbr_init_lib(). */
typedef enum {
	/** Cannot allocate memory. */
	BR_INIT_ERROR_NOMEM,
	/** Unable to open /proc/self/maps; see errno for details. */
	BR_INIT_ERROR_OPEN_MAPS,
	/** Unable to read from /proc/self/maps; see errno for details. */
	BR_INIT_ERROR_READ_MAPS,
	/** The file format of /proc/self/maps is invalid; kernel bug? */
	BR_INIT_ERROR_INVALID_MAPS,
	/** BinReloc is disabled (the ENABLE_BINRELOC macro is not defined). */
	BR_INIT_ERROR_DISABLED
} BrInitError;


#ifndef BINRELOC_RUNNING_DOXYGEN
/* Mangle symbol names to avoid symbol collisions with other ELF objects. */
	#define br_init             SlLp57100427862049_br_init
	#define br_init_lib         SlLp57100427862049_br_init_lib
	#define br_find_exe         SlLp57100427862049_br_find_exe
	#define br_find_exe_dir     SlLp57100427862049_br_find_exe_dir
	#define br_find_prefix      SlLp57100427862049_br_find_prefix
	#define br_find_bin_dir     SlLp57100427862049_br_find_bin_dir
	#define br_find_sbin_dir    SlLp57100427862049_br_find_sbin_dir
	#define br_find_data_dir    SlLp57100427862049_br_find_data_dir
	#define br_find_locale_dir  SlLp57100427862049_br_find_locale_dir
	#define br_find_lib_dir     SlLp57100427862049_br_find_lib_dir
	#define br_find_libexec_dir SlLp57100427862049_br_find_libexec_dir
	#define br_find_etc_dir     SlLp57100427862049_br_find_etc_dir
	#define br_strcat           SlLp57100427862049_br_strcat
	#define br_build_path       SlLp57100427862049_br_build_path
	#define br_dirname          SlLp57100427862049_br_dirname


#endif
int   br_init             (BrInitError *error);
int   br_init_lib         (BrInitError *error);

char *br_find_exe         (const char *default_exe);
char *br_find_exe_dir     (const char *default_dir);
char *br_find_prefix      (const char *default_prefix);
char *br_find_bin_dir     (const char *default_bin_dir);
char *br_find_sbin_dir    (const char *default_sbin_dir);
char *br_find_data_dir    (const char *default_data_dir);
char *br_find_locale_dir  (const char *default_locale_dir);
char *br_find_lib_dir     (const char *default_lib_dir);
char *br_find_libexec_dir (const char *default_libexec_dir);
char *br_find_etc_dir     (const char *default_etc_dir);

/* Utility functions */
char *br_strcat  (const char *str1, const char *str2);
char *br_build_path (const char *dir, const char *file);
char *br_dirname (const char *path);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BINRELOC_H__ */
