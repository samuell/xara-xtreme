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
// Definition of the units class

#ifndef INC_UNITS
#define INC_UNITS

#include "strings.h"
#include "list.h"
#include "unittype.h"
#include "pump.h"
#include "ccfile.h"

/*---------------------------------------------------------------------------------

	The defines below were calculated using the following facts :

		1 mile = 1.60934 kilometres
		1 mile = 1760 yards

	They define the number of millipoints in each of the default unit types supported by Camelot.

	(MarkN 8/3/94 & 28/6/94)
*/

#define MM_MP_VAL 	2834.652715			// Num millipoints in a millimetre
#define CM_MP_VAL	(MM_MP_VAL*10)		// Num millipoints in a centimetre
#define  M_MP_VAL	(CM_MP_VAL*100)		// Num millipoints in a metre
#define KM_MP_VAL	( M_MP_VAL*1000)	// Num millipoints in a kilometre

#define MP_MP_VAL	1.0					// Num millipoints in a millipoint (well, it's consistant)
#define PT_MP_VAL	(MP_MP_VAL*1000)	// Num millipoints in a computer point
#define PI_MP_VAL  	(PT_MP_VAL*12)		// Num millipoints in a pica
#define IN_MP_VAL	(PI_MP_VAL*6)		// Num millipoints in an inch
#define FT_MP_VAL	(IN_MP_VAL*12)		// Num millipoints in a foot
#define YD_MP_VAL	(FT_MP_VAL*3)		// Num millipoints in a yard
#define MI_MP_VAL	(YD_MP_VAL*1760)	// Num millipoints in a mile
#define PX_MP_VAL 	750.0				// Num millipoints in a pixel (1/96 inch)

// Unit flags type definition
struct UnitFlagsType
{
	BOOL 	Prefix 		: 1;	// TRUE if unit ident string should prefix value 
								// Example prefix unit : "$4.30"
								// Example suffix unit : "23.1km"
	BOOL	DefaultUnit : 1;	// TRUE if this unit is one of the default units in Camelot
								// Default units cannot be edited or deleted
};

typedef enum
{
	UNITREASON_OK,
	UNITREASON_HASDESCENDENTS,
	UNITREASON_BEINGUSED
} UnitReason;

/********************************************************************************************

>	class Unit: public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Purpose:	Encapsulates units within Camelot.
				All units (i.e. default and user-defined ones) are an instance of this class.
				Each document has its own list of units controlled by an instance of DocUnitList
				within the Document class.
				Therefore you should get hold of the DocUnitList within the current doc when you 
				want to add/delete/edit/find a unit.
	SeeAlso:	DocUnitList,Document

********************************************************************************************/

class Unit : public ListItem
{
	CC_DECLARE_DYNAMIC(Unit)

public:
	Unit();
	~Unit();

	static BOOL 	Init();
	static BOOL 	Deinit();

	BOOL		IsDefault();
	BOOL		SetDefaultState(BOOL State);
	BOOL		IsPrefix();
	BOOL		SetPrefixState(BOOL State);

	UnitType	GetUnitType();
	BOOL		SetUnitType(UnitType NewUnitType);

	double		GetMillipoints();
	BOOL		SetMillipoints(double NewMillipoints);

	UnitType	GetBaseUnitType();
	BOOL		SetBaseUnitType(UnitType NewBaseUnitType);
	double		GetBaseNumerator();
	BOOL		SetBaseNumerator(double NewBaseNumerator);
	double		GetBaseDenominator();
	BOOL		SetBaseDenominator(double NewBaseDenominator);

	String_32	GetToken();
	BOOL		SetToken(const String_32& NewToken);
	String_32	GetSpecifier();
	BOOL		SetSpecifier(const String_32& NewSpecifier);

private:

	// Each unit has a unit type.  This allows standard units to be accessed
	// easily via the constants defined by enum UnitType
	UnitType		ThisUnitType;

	// Millipoints is the num millipoints in 1 of these units
	// I have used millipoints here because all object dimensions in Camelot
	// are defined in millipoints
	// E.g. "1 in" = 72000.0 millipoints
	double 			Millipoints;

	// The flags of this unit
	UnitFlagsType	Flags;

	// BaseUnitType is the unit which this is based on
	// BaseNumerator and BaseDenominator define how many BaseUnitTypes make
	// one of these units
	// E.g. one furlong = 220 yards
	//   so BaseUnitType 	== YARDS
	//		BaseNumerator  	== 220
	//		BaseDenominator == 1
	UnitType		BaseUnitType;
	double			BaseNumerator;
	double			BaseDenominator;

	// Set of strings used to identify the unit
	// "Token" is the string used in drop-down lists of units to identify it
	// "Specifier" is the string used in ed fields to specify a particular unit
	// E.g. Token == "Furlong" , Specifier == "fl"
	//		Token == "Dollars" , Specifier == "$"
	String_32		Token;
	String_32		Specifier;
};


/********************************************************************************************

>	class DocUnitList: public List

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Purpose:	Encapsulates units within Camelot.
				All units (i.e. default and user-defined ones) are an instance of this class.
				Each document has its own list of units controlled by an instance of DocUnitList
				within the Document class.
				Therefore you should get hold of the DocUnitList within the current doc when you 
				want to add/delete/edit/find a unit.
	SeeAlso:	DocUnitList,Document

********************************************************************************************/

class DocUnitList : public List
{
	CC_DECLARE_DYNAMIC(DocUnitList)

public:
	DocUnitList();
	~DocUnitList();

	static BOOL 	Init();
	static BOOL 	Deinit();

	static DocUnitList*	GetCurrentDocUnitList();

	// Useful routines for finding things given bits of info
	Unit*	FindUnit(UnitType ThisUnit);
	Unit*	FindUnit(INT32 Index);
	UnitType FindUnitType(const StringBase& Str);
	UnitType FindUnitTypeFromToken(const StringBase& Str);
	INT32		FindUnitIndex(UnitType ThisUnit);

	Unit*	FindUserUnit(INT32 Index);
	Unit*	FindFirstUserUnit();
	Unit*	FindNextUserUnit(Unit* pThisUnit);

	void 	UnitHasChanged(UnitType ChangedUnitType);
	BOOL 	MakeNewUnit(Unit** ppUnit,BOOL Initialise = TRUE);
	BOOL 	MakeDefaultUnits();
	INT32		GetNumUnits();
	UnitReason 	DeleteUnit(UnitType ThisUnitType);

	BOOL 	HasDescendents(UnitType ThisUnitType);
	BOOL	IsDescendent(Unit* pThisUnit,UnitType NewBaseUnitType);
	void 	RecalcUnit(Unit* pThisUnit);
	BOOL 	RecalcUnit(Unit* pThisUnit,UnitType ChangedUnitType);
	BOOL 	SetBaseUnitType(Unit* pThisUnit,UnitType NewBaseUnitType);


	// These are the default units used for displaying scaled details,
	// page details and font details. Getting and setting functions.
	UnitType	GetScaleUnits();
	void		SetScaleUnits(UnitType NewUnits);
	UnitType	GetPageUnits();
	void		SetPageUnits(UnitType NewUnits);
	UnitType	GetFontUnits();
	void		SetFontUnits(UnitType NewUnits);

	// From the UnitType get the token and the specifier
	String_32	GetToken(UnitType);
	String_32	GetSpecifier(UnitType);

private:
	BOOL MakeFactoryDefaultUnits();

	void OutputTrace(char* err,const TCHAR* TokenBuf);
	BOOL ReadLong(CCLexFile& file,void* pLong,char* err);
	BOOL ReadDouble(CCLexFile& file,double* pDouble,char* err);
	BOOL ReadString(CCLexFile& file,StringBase* pStr,char* err);

	BOOL ReadUnitsFromFile(CCLexFile& file);
	BOOL ReadUnitsFromDisk();
	BOOL ReadUnitsFromRes();

	static BOOL CheckUnitTypesValid();

	INT32 	NumUnits;
	INT32 	NextUnitNum;
	INT32 	NextUnitType;

	BOOL	DefaultsMade;	// TRUE if MakeDefaultUnits() has been called

	UnitType	ScaleUnits;
	UnitType	PageUnits;
	UnitType	FontUnits;
	UnitType	LastEditedUnit;
};

/********************************************************************************************

>	class UnitMsg: public Msg

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/94
	Purpose:	This message is sent whenever something happens to a unit
	SeeAlso:	-

********************************************************************************************/

class UnitMsg: public Msg
{
	CC_DECLARE_DYNAMIC(UnitMsg);
public:

	enum UnitMsgType
	{
		BEFOREDELETE,	// The unit is about to be deleted from the document
		AFTERDELETE,	// The unit has just been deleted from the document
		NEW,			// A new unit has just been created
		CHANGED			// An existing unit has changed definition
	};

	UnitMsgType 	MsgType; 		// The message type
	DocUnitList*	pDocUnitList;	// The doc unit list this unit belongs to
	UnitType		ThisUnitType;	// The unit this message refers to

	UnitMsg(DocUnitList* pDocList, UnitType UType, UnitMsgType TypeMsg): 
			MsgType(TypeMsg), pDocUnitList(pDocList), ThisUnitType(UType) { } 
};


#endif // INC_UNITS
