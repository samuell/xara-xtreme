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
// The Archive class used to save the Camelot native file format

/*
*/

#include "camtypes.h"
#include "archive.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "intelmac.h"

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_MEMDUMP(CCArchive, CC_CLASS_MEMDUMP)
#define new CAM_DEBUG_NEW




/********************************************************************************************

>	CCArchive::CCArchive(CCLexFile* pNewFile, UINT32 Mode)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		pNewFile - A CCDiskFile that is ready to be saved into or loaded out of
				Mode - The mode of the Archive. Can be one of CCArchive::Load or
				CCArchive::Save.
	Purpose:	Constructs a new CCArchive object. The Archive class assumes the following
				things about the size of objects.


********************************************************************************************/

CCArchive::CCArchive(CCLexFile* pNewFile, UINT32 Mode)
  :	pFile(pNewFile),
	IsLoadArchive(FALSE),
	IsArchiveOK(TRUE)
{
	// Find out what sort of Archive we are going to be
	if (Mode & CCArchive::Load)
	{
		// We are loading
		IsLoadArchive = TRUE;
	}
	else if (Mode & CCArchive::Save)
	{
		// We are saving
		IsLoadArchive = FALSE;
	}
	else
	{
		// Something is wrong. Mode should be Save or Load
		IsArchiveOK = FALSE;
	}
}


/********************************************************************************************

>	CCArchive::~CCArchive()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Purpose:	Destructor

********************************************************************************************/

CCArchive::~CCArchive()
{
	// Empty.
}



/********************************************************************************************

>	void CCArchive::Close()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Purpose:	Flushes and closes the CCArchive. After this has been done, you can not write
				any more data to it.

********************************************************************************************/

void CCArchive::Close()
{
	// Close the file associated with the Archive. This should flush the file as well...
	pFile->close();
}




/********************************************************************************************

>	CCArchive& CCArchive::operator>>(BYTE& Var)
	CCArchive& CCArchive::operator>>(WORD& Var)
	CCArchive& CCArchive::operator>>(INT32& Var)
	CCArchive& CCArchive::operator>>(DWORD& Var)
	CCArchive& CCArchive::operator>>(float& Var)
	CCArchive& CCArchive::operator>>(double& Var)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		Various - The item to be loaded from the archive
	Outputs:	Various - The data from the file will be in the variable supplied
	Returns:	A reference to a CCArchive to allow chaining of loading commands
	Purpose:	Loads a primitive item out of the archive into the variable. This function
				will throw an ArchiveExeption if there is an error
	SeeAlso:	CCArchive::operator<<

********************************************************************************************/

CCArchive& CCArchive::operator>>(BYTE& Var)
{
	// Read an INT32 out of the file
	pFile->read(&Var, sizeof(BYTE));
	return *this;
}

// The INT32 verion of the Loading operator
CCArchive& CCArchive::operator>>(INT32& Var)
{
	ERROR3IF(sizeof(INT32)!=4, "INT32 is no longer 4 bytes!\n");

	// Get a Word out the Archive
	INT32 LoadMe;
	pFile->read(&LoadMe, sizeof(INT32));

	// Fix the word up from Intel format
	INT_FROM_INTEL(LoadMe, Var);
	return *this;
}

// The WORD verion of the Loading operator
CCArchive& CCArchive::operator>>(WORD& Var)
{
	ERROR3IF(sizeof(WORD)!=2, "WORD is no longer 2 bytes!\n");

	// Get a Word out the Archive
	WORD LoadMe;
	pFile->read(&LoadMe, sizeof(WORD));

	// Fix the word up from Intel format
	WORD_FROM_INTEL(LoadMe, Var);
	return *this;
}

// The INT32 verion of the Loading operator
CCArchive& CCArchive::operator>>(INT32& Var)
{
	ERROR3IF(sizeof(INT32)!=4, "INT32 is no longer 4 bytes!\n");

	// Get a INT32 out the Archive
	INT32 LoadMe;
	pFile->read(&LoadMe, sizeof(INT32));

	// Fix the INT32 up from Intel format
	LONG_FROM_INTEL(LoadMe, Var);
	return *this;
}

// The DWORD verion of the Loading operator
CCArchive& CCArchive::operator>>(DWORD& Var)
{
	ERROR3IF(sizeof(DWORD)!=4, "DWORD is no longer 4 bytes!\n");

	// Get a DWORD out the Archive
	DWORD LoadMe;
	pFile->read(&LoadMe, sizeof(DWORD));

	// Fix the DWORD up from Intel format
	DWORD_FROM_INTEL(LoadMe, Var);
	return *this;
}

// The float verion of the Loading operator
CCArchive& CCArchive::operator>>(float& Var)
{
	ERROR3IF(sizeof(float)!=4, "float is no longer 4 bytes!\n");

	// Get a float out the Archive
	float LoadMe;
	pFile->read(&LoadMe, sizeof(float));

	// Fix the float up from Intel format
	FLOAT_FROM_INTEL(LoadMe, Var);
	return *this;
}

// The double verion of the Loading operator
CCArchive& CCArchive::operator>>(double& Var)
{
	ERROR3IF(sizeof(double)!=8, "double is no longer 8 bytes!\n");

	// Get a float out the Archive
	double LoadMe;
	pFile->read(&LoadMe, sizeof(double));

	// Fix the float up from Intel format
	DOUBLE_FROM_INTEL(LoadMe, Var);
	return *this;
}



/********************************************************************************************

>	CCArchive& CCArchive::operator<<(BYTE Var)
	CCArchive& CCArchive::operator<<(WORD Var)
	CCArchive& CCArchive::operator<<(INT32 Var)
	CCArchive& CCArchive::operator<<(DWORD Var)
	CCArchive& CCArchive::operator<<(float Var)
	CCArchive& CCArchive::operator<<(double Var)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		Various - The item to be Saved to the archive
	Returns:	A reference to a CCArchive to allow chaining of saving commands
	Purpose:	saves a primitive item out of the archive from the variable. This function
				will throw an ArchiveExeption if there is an error
	SeeAlso:	CCArchive::operator>>

********************************************************************************************/

CCArchive& CCArchive::operator<<(BYTE Var)
{
	pFile->write(&Var, sizeof(BYTE));
	return *this;
}


// The INT32 verion of the Saving operator
CCArchive& CCArchive::operator<<(INT32 Var)
{
	ERROR3IF(sizeof(INT32)!=4, "INT32 is no longer 4 bytes!\n");

	// Get the Word into Intel form
	INT32 SaveMe;
	INT_TO_INTEL(Var, SaveMe);

	// Save out the Fixed up word
	pFile->write(&SaveMe, sizeof(INT32));
	return *this;
}


// The WORD verion of the Saving operator
CCArchive& CCArchive::operator<<(WORD Var)
{
	ERROR3IF(sizeof(WORD)!=2, "WORD is no longer 2 bytes!\n");

	// Get the Word into Intel form
	WORD SaveMe;
	WORD_TO_INTEL(Var, SaveMe);

	// Save out the Fixed up word
	pFile->write(&SaveMe, sizeof(WORD));
	return *this;
}

// The INT32 verion of the Saving operator
CCArchive& CCArchive::operator<<(INT32 Var)
{
	ERROR3IF(sizeof(INT32)!=4, "INT32 is no longer 4 bytes!\n");

	// Get the INT32 into Intel form
	INT32 SaveMe;
	LONG_TO_INTEL(Var, SaveMe);

	// Save out the Fixed up word
	pFile->write(&SaveMe, sizeof(INT32));
	return *this;
}


// The DWORD verion of the Saving operator
CCArchive& CCArchive::operator<<(DWORD Var)
{
	ERROR3IF(sizeof(DWORD)!=4, "DWORD is no longer 4 bytes!\n");

	// Get the DWORD into Intel form
	DWORD SaveMe;
	DWORD_TO_INTEL(Var, SaveMe);

	// Save out the Fixed up word
	pFile->write(&SaveMe, sizeof(DWORD));
	return *this;
}

// The float verion of the Saving operator
CCArchive& CCArchive::operator<<(float Var)
{
	ERROR3IF(sizeof(float)!=4, "float is no longer 4 bytes!\n");

	// Get the float into Intel form
	float SaveMe;
	FLOAT_TO_INTEL(Var, SaveMe);

	// Save out the Fixed up word
	pFile->write(&SaveMe, sizeof(float));
	return *this;
}

// The double verion of the Saving operator
CCArchive& CCArchive::operator<<(double Var)
{
	ERROR3IF(sizeof(double)!=8, "double is no longer 8 bytes!\n");

	// Get the double into Intel form
	double SaveMe;
	DOUBLE_TO_INTEL(Var, SaveMe);

	// Save out the Fixed up word
	pFile->write(&SaveMe, sizeof(double));
	return *this;
}




/********************************************************************************************

>	BOOL CCArchive::IsReady()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Returns:	TRUE if the Archive is Ready, FALSE if not
	Purpose:	Tests to see if the Archive is ready to save or load data items. This tests
				that all the files are OK and everything is set up as it should be.
				You should call this function before starting to use the Loading and Saving
				operators of this class, though it does only need to be called once at the
				begining. Individual objects should NOT need to call this function before
				saving/loading their components as it should have been done before all the
				Serialise functions are called.

********************************************************************************************/

BOOL CCArchive::IsReady()
{
	// Make sure that we have a pointer to a file
	if (pFile==NULL)
		return FALSE;

	// make sure that it is open
	if (!pFile->isOpen())
		return FALSE;

	// all seemed to be ok.
	return TRUE;
}



/********************************************************************************************

>	BOOL CCArchive::IsLoading() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Returns:	TRUE if this archive is Loading data.
	Purpose:	Determines if this Archive is a loading or saving archive and responds
				appropriatly

********************************************************************************************/

BOOL CCArchive::IsLoading() const
{
	return IsLoadArchive;
}


/********************************************************************************************

>	BOOL CCArchive::IsSaving() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Returns:	TRUE if this archive is Saving data.
	Purpose:	Determines if this Archive is a loading or saving archive and responds
				appropriatly

********************************************************************************************/

BOOL CCArchive::IsSaving() const
{
	return !IsLoadArchive;
}




/********************************************************************************************

>	CCArchive::CCArchive(const CCArchive& Src)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		Src - Reference to an Archive
	Purpose:	Copy Constuctor. Since the coping of Archives is not permitted, this just
				fails with an ERROR3.
	Errors:		Always fails with an ERROR3. You should never call this function

********************************************************************************************/

CCArchive::CCArchive(const CCArchive& Src)
{
	ERROR3("CCArchive classes can not be copied\n");
}



/********************************************************************************************

>	void CCArchive::operator=(const CCArchive& Src)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		Src - An Archive
	Purpose:	Stops Archives from being copied as this is not permitted. Always fails
				with an Error3. DO NOT CALL THIS FUNCTION.
	Errors:		Always fails with an ERROR3. You are not allowed to copy Archives

********************************************************************************************/

void CCArchive::operator=(const CCArchive& Src)
{
	ERROR3("CCArchive classes can not be assigned to each other\n");
}


