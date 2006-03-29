// $Id$
// mtrand.cpp : Defines the entry point for the console application.
//
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
#include "camtypes.h"
#include "mtrand.h"

UINT32 MTRand::lastSeed = 1;

/********************************************************************************************

>	UINT32 MTRand::GetNextRandomNumberScaled(UINT32 MaxValue, UINT32 MinValue)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/11/2005
	Inputs:     -
	Returns:	the next number in an already seeded random number sequence, scaled to between
				the two specified values
	Purpose:	as above, note that you MUST have already seeded the sequence
	SeeAlso:

********************************************************************************************/

UINT32 MTRand::GetNextRandomNumberScaled(UINT32 MaxValue, UINT32 MinValue)
{
	UINT32 Random =  this->operator ()();
	double ScaleFactor = ((double)(MaxValue - MinValue)) / ((double)MTRAND_MAX);
	double Res = (double)Random * ScaleFactor;
	return (UINT32)Res + MinValue;
}


/********************************************************************************************

>	UINT32 MTRand::GetNextRandomNumberScaled(UINT32 MaxValue, UINT32 MinValue, UINT32 Median)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/11/2005
	Inputs:     MaxValue - the top of our random range
				MinValue - the bottom of our random range
				Median - duh
	Returns:	the next number in an already seeded random number sequence, scaled to between
				the two specified values
	Purpose:	Overridden version which allows you to have a non-linear range.  By specifying the
				median all random values > MTRAND / 2 will be scaled from Median - MaxValue and
				all random values <= MTRAND / 2 will be scaled from MinValue - Median.
	SeeAlso:

********************************************************************************************/

UINT32 MTRand::GetNextRandomNumberScaled(UINT32 MaxValue, UINT32 MinValue, UINT32 Median)
{
	UINT32 Random = rand();

	UINT32 Retval = 0;
	if (Random < MTRAND_MED)
	{
		double ScaleFactor = ((double)(Median - MinValue)) / (double)MTRAND_MED;
		double Res = ScaleFactor * Random;
		Retval = (UINT32)Res + MinValue;
	}
	if (Random >= MTRAND_MED)
	{
		double ScaleFactor = ((double)(MaxValue - Median)) / (double)MTRAND_MED;
		double Res = ScaleFactor * (Random - MTRAND_MED);
		Retval = (UINT32)Res + Median;
	}
	return Retval;
}
