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
// Operation for doing an interactive scale of the selection

/*
*/


#include "camtypes.h"
#include "opscale2.h"

//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rik.h"
#include "selector.h"
//#include "clikdrag.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "selstr.h"




// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
//CC_IMPLEMENT_DYNCREATE(OpScale2Trans, OpScaleTrans)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW








/********************************************************************************************

>	OpScale2Trans();

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/11/96
	Purpose:	default constructor

********************************************************************************************/

/*OpScale2Trans::OpScale2Trans()
	: OpScaleTrans()
{
}*/




/********************************************************************************************

>	virtual  ~OpScale2Trans();

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/11/96
	Purpose:	destructor

********************************************************************************************/

/*OpScale2Trans::~OpScale2Trans()
{
}*/




/********************************************************************************************

>	OpScale2Trans( const OpScale2Trans& );

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/11/96
	Purpose:	copy constructor

********************************************************************************************/
/*
OpScale2Trans::OpScale2Trans( const OpScale2Trans& Other )
	: OpScaleTrans( Other )
{
}
*/

/********************************************************************************************

>	OpScale2Trans&  operator=( const OpScale2Trans& );

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/11/96
	Purpose:	assignment operator:  delegates to virtual Assign()

********************************************************************************************/
/*
OpScale2Trans&  OpScale2Trans::operator=( const OpScale2Trans& Other )
{
	Assign( Other );

	return  *this;
}
*/



/********************************************************************************************

>	virtual	 OpScale2Trans*  Clone ()   const;

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/11/96
	Purpose:	clone function

********************************************************************************************/
/*
OpScale2Trans*  OpScale2Trans::Clone ()   const
{

   OpScale2Trans*  Clone  =  new OpScale2Trans( *this );

   if( !Clone )
      throw  "OpScale2Trans::Clone()\n\nCouldn't get memory for a OpScale2Trans\n";

   return  Clone;

}
*/



/********************************************************************************************

>	private: virtual void  Assign( const OpScale2Trans& );

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/11/96
	Purpose:	virtual assignment

********************************************************************************************/
/*
void  OpScale2Trans::Assign( const OpScale2Trans& Other )
{

	OpScaleTrans::operator=( Other );

}
*/



/********************************************************************************************

>	static BOOL OpScale2Trans::Declare()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

/*BOOL OpScale2Trans::Declare()
{
	return  RegisterOpDescriptor( 0, _R(IDS_SCALETRANS), CC_RUNTIME_CLASS(OpScale2Trans),
							      OPTOKEN_SCALE2, OpScaleTrans::GetState              );
}*/








/********************************************************************************************

>	virtual void  DoWithParam( OpDescriptor* pDescriptor,  OpParam* pOpParam )

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com> (adapted from Rik)
	Created:	6/11/97
	Inputs:		pDescriptor - The operations descriptor
				pOpParam    - The parameters for this operation
	Purpose:	override: slightly change to allow the scale factors setup and 
				operation execution to be delegated to a separate function - IterateToConvergence()
	See also:	TransOperation::DoWithParam()

********************************************************************************************/

/*void OpScale2Trans::DoWithParam( OpDescriptor*,  OpParam* pOpParam )
{

	// Get at my data
	TransformData* TransData = (TransformData*)(pOpParam->Param1);

	// copy across all the relavent bits of data for the transform
	CentreOfTrans 	= TransData->CentreOfTrans;
	LockAspect 		= TransData->LockAspect;
	LeaveCopy 		= TransData->LeaveCopy;
	ScaleLines 		= TransData->ScaleLines;
	TransFills 		= TransData->TransFills;
	TransformRange	= TransData->pRange;		// if NULL use the selection

	// It is not possible to move between spreads in an immediate operation
	CanChangeToNewSpread = FALSE;

	// Where was this operation started from, and are we interested
	SetStartBlob(TransData->StartBlob);

	// Find the Selection and fail if it is not there
	Selection = GetApplication()->FindSelection();
	if (Selection==NULL)
	{
		FailAndExecute();
		End();
		return;
	}


	/// a call to the extra bit
	IterateToConvergence( pOpParam );


	// mark the selection cache as invalid (bounding rects etc will no longer be correct)
	Selection->Update();


	// End the operation
	End();	

}*/




/********************************************************************************************

>	protected: virtual INT32 IterateToConvergence ( OpParam* pParam );

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/97
	Inputs:		pParam -  The parameters for this operation
	Returns:	dummy
	Purpose:	Calculate the scaling by iterative improvement
					1.	set scale & matrix, accumulate matrix
					2.	do a transform
					3.	compare resultant width with intended width
					4.  call to do the whole transform

********************************************************************************************/

/*INT32  OpScale2Trans::IterateToConvergence ( OpParam* pParam )
{

	const MILLIPOINT	IntendedWidth_k       =  ( (MILLIPOINT*)( pParam->Param2 ) )[0];
	const INT32			MaxWidthInaccuracy_k  =  (INT32)37;   // 0.05 * 1/96 inches (in millipoints) = 0.05 pixels
	const BOOL			OriginalSucceedAndDiscard_k  =  OpFlags.SucceedAndDiscard;
	const UINT32			MaxIterations_k       =  8;          // must be >= 1

	Matrix				AccumulatedScaling;      // default identity
	DocRect				BoundingRect          =  Selection->GetBoundingRect();
	INT32				WidthInaccuracy;
	Matrix				tmp;
	UINT32				i                     =  MaxIterations_k;



	/// do the iterations to calculate the whole transform

	OpFlags.SucceedAndDiscard  =  TRUE;   // suppress undo   // ???

	do
	{
		if( !InitTransformImmediate2( pParam,  BoundingRect ) )
			return  Failure( OriginalSucceedAndDiscard_k );
		BuildMatrix();

		/// check if the composition will underflow
		FIXED16   abcd[4];
		( tmp *= Transform ).GetComponents( abcd, 0 );
		if( abcd[0]  ==  0 )		break;
		else						AccumulatedScaling  =  tmp;


		if( !CompleteTransformation() )
			return  Failure( OriginalSucceedAndDiscard_k );
		Selection->Update();


		BoundingRect     =  Selection->GetBoundingRect();
		WidthInaccuracy  =  BoundingRect.Width() - IntendedWidth_k;
		if( WidthInaccuracy < 0 )   WidthInaccuracy  =  -WidthInaccuracy;
	}
	/// stop iterating when: iterated enough, width is accurate enough
	while(     ( --i )
	       &&  ( WidthInaccuracy > MaxWidthInaccuracy_k )  );



	/// now the scaling is calulated, it can be performed, almost straightforwardly
	ApplyAsSingleTransform( AccumulatedScaling,  OriginalSucceedAndDiscard_k );


	return  1;

}*/




/********************************************************************************************

  > protected: virtual INT32 OpScale2Trans::ApplyAsSingleTransform( Matrix& AccumulatedScaling,
                                                                  const BOOL OriginalSucceedAndDiscard_k )

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/97
	Inputs:		AccumulatedScaling           - the whole transform
				OriginalSucceedAndDiscard_k  - original undo flag
	Returns:	dummy
	Purpose:	1.	undo all iterated transforms, undo off
				2.	do the accumulated transform, undo set to original state
	SeeAlso:	OpScaleTrans::IterateToConvergence()

********************************************************************************************/

/*INT32 OpScale2Trans::ApplyAsSingleTransform( Matrix& AccumulatedScaling,  const BOOL OriginalSucceedAndDiscard_k )
{

	/// undo all the iterations

	Transform  =  AccumulatedScaling.Inverse();		// set  TransScaleFactor = YScaleFactor  ???

	if( !CompleteTransformation() )
		return  Failure( OriginalSucceedAndDiscard_k );

	Selection->Update();
	


	/// do the overall, correct scaling

	OpFlags.SucceedAndDiscard  =  OriginalSucceedAndDiscard_k;   // reset undo on/off choice
	Transform  =  AccumulatedScaling;				// set  TransScaleFactor = YScaleFactor  ???

	if( !CompleteTransformation() )
		return  Failure( OriginalSucceedAndDiscard_k );

	Selection->Update();



	return  1;

}*/




/********************************************************************************************

  >	protected: OpScale2Trans::Failure ( const BOOL OriginalSucceedAndDiscard_k );

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/97
	Inputs:		original undo flag
	Returns:	dummy
	Purpose:	acts like a 'catch' block:  general fail recovery

********************************************************************************************/

/*INT32	 OpScale2Trans::Failure ( const BOOL OriginalSucceedAndDiscard_k )
{

	OpFlags.SucceedAndDiscard  =  OriginalSucceedAndDiscard_k;   // reset undo on/off choice
	FailAndExecute();

	return  0;

}*/


			
			
/********************************************************************************************

  >	protected: virtual BOOL OpScale2Trans::InitTransformImmediate2( OpParam* pParam,  const DocRect& BoundingRect )

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/97
	Inputs:		pParam       -  The parameters for this operation
				BoundingRect -  (of the selection)
	Returns:	TRUE - all ok,  FALSE - fixed pt over/underflow
	Purpose:	override: calculate scale factors from the relative	change in size
	SeeAlso:	OpScaleTrans::InitTransformImmediate()

********************************************************************************************/

/*BOOL OpScale2Trans::InitTransformImmediate2( OpParam* pParam,  const DocRect& BoundingRect )
{
	/// Retrieve parameters
	MILLIPOINT*	pParams   =  (MILLIPOINT*)( pParam->Param2 );
	MILLIPOINT	Width     =  pParams[0];
	MILLIPOINT	Height    =  pParams[1];


	/// Calculate relative change in size
	double		fpXscale  =  (double)Width  / BoundingRect.Width();
	double		fpYscale  =  (double)Height / BoundingRect.Height();


	/// Check if the scale will under- or over-flow the fixed-point numbers used in the
	/// transform system.
	if( pSelTool->BeyondFixedRange(fpXscale) || pSelTool->BeyondFixedRange(fpYscale) )
	{
		TRACEUSER( "Harrison", _T("Under/overflow in OpScale2Trans::InitTransformImmediate\n"));
		//pInfoBarOp->ReportEditError(_R(IDS_SEL_ERROR_TOO_SMALL_OR_LARGE));
		ToolInformError(TOOLID_SELECTOR, _R(IDS_SEL_ERROR_TOO_SMALL_OR_LARGE));
		return  FALSE;
	}


	FIXED16 fxParams[2];
	fxParams[0]  =  fpXscale;
	fxParams[1]  =  fpYscale;

	OpScaleTrans::InitTransformImmediate( &OpParam((INT32)pParam->Param1, (INT32)fxParams) );


	return  TRUE;
}*/



