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

// Implementation of Adobe Illustrator EPS filter.

#include "camtypes.h"

#include "ai_eps.h"
#include "ai_epsrr.h"
#include <strstrea.h>
#include <stdio.h>

#include "nodepath.h"
#include "paths.h"
//#include "tim.h"
//#include "nev.h"
#include "oilfltrs.h"
#include "ccdc.h"
#include "spread.h"
#include "page.h"
#include "fillattr.h"
#include "docview.h"
#include "opbevel.h"
#include "progress.h"        
#include "ai_grad.h"        
#include "ai_bmp.h"
#include "ai_layer.h"  
#include "swffiltr.h"

//#include "filtrres.h"	// for PhotoShop EPS strings.

DECLARE_SOURCE("$Revision");

#define new CAM_DEBUG_NEW

CC_IMPLEMENT_DYNAMIC(AIEPSFilter, EPSFilter)

static EPSCommand DeferredToken = 0;

// This is the array of AI EPS command/keyword names.
CommandMap AIEPSFilter::AICommands[] =
{
	// Text handling
	EPSC_z,			"z",
	EPSC_e,			"e",
	EPSC_T,			"T",
	EPSC_t,			"t",
	
	// Sentinel
	EPSC_Invalid,	"Invalid"
};

/********************************************************************************************

>	AIEPSFilter::AIEPSFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Purpose:	Constructor for an AIEPSFilter object.  The object should be 
				initialised	before use.
	SeeAlso:	EPSFilter::Init

********************************************************************************************/

AIEPSFilter::AIEPSFilter()
{
	// Set up filter descriptions.
	FilterNameID = _R(IDT_AIEPS_FILTERNAME);
	FilterInfoID = _R(IDT_AIEPS_FILTERINFO);
	ImportMsgID  = _R(IDT_IMPORTMSG_AI);

	FilterID = FILTERID_AIEPS;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	//WEBSTER-Martin-27/01/97
#ifdef WEBSTER
	Flags.CanExport = FALSE;
#else
	Flags.CanExport = TRUE;
#endif //WEBSTER
#else
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#endif

	AdjustOrigin = FALSE;	// Illustrator gets it right...

	bDoClip = FALSE;
}

/********************************************************************************************

>	BOOL AIEPSFilter::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an AIEPSFilter object.
	Errors:		Will fail if not enough memory to initialise the EPS stack.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL AIEPSFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new AIEPSOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(FilterNameID);
	FilterInfo.Load(FilterInfoID);

	bDoClip = FALSE;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	INT32 AIEPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the header is ok and import should proceed, FALSE if not.
	Purpose:	Checks to see if the EPS comment headers specify that this is an AI
				generated EPS file, as required.

********************************************************************************************/

INT32 AIEPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)
{
	UINT32	Lines	= 0;
	char	*Buffer	= NULL;

	// !PS-Adobe line is ok - check creator line...
	CCMemTextFile HeaderFile ( reinterpret_cast<char *> ( pFileHeader ), HeaderSize );

	if( HeaderFile.IsMemFileInited () == FALSE || HeaderFile.InitLexer () == FALSE )
	{
		HeaderFile.close();
		return 0;
	}

	// Graeme (28/6/00) - Adobe have changed their file format, and so the first line can
	// now be a %PDF directive. Therefore look for this directive in the first twenty
	// lines.
	while ( ( Lines < 100 ) && !HeaderFile.eof () )
	{
		// Get the current line from the file.
		HeaderFile.GetLineToken();
		Buffer = const_cast<char *> ( HeaderFile.GetTokenBuf () );

		// Ensure that it's OK.
		ERROR2IF(Buffer == 0, 0, "Returned buffer from lex file == 0");

		// Increment the line counter.
		Lines++;

		if (_tcsncmp(Buffer, "%!PS-Adobe", 10) == 0)
		{
			// Now find the %%Creator string.
			while ((Lines < 100) && !HeaderFile.eof())
			{
				HeaderFile.GetLineToken();
				Buffer = const_cast<char *> ( HeaderFile.GetTokenBuf() );
				ERROR2IF(Buffer == 0, 0, "Returned buffer from lex file == 0");
				Lines++;

				// Return TRUE if this file was created by Illustrator, or has been exported
				// in Illustrator format.
				if (_tcsncmp(Buffer, "%%Creator: Adobe Illustrator", 28) == 0)
				{
					// We definitely want this.
		 			HeaderFile.close();
					return 9;
				}

				// Another variation on the Illustrator theme
				if (_tcsncmp(Buffer, "%%Creator: AI", 13) == 0)
				{
					// We definitely want this.
					HeaderFile.close();
					return 9;
				}

				// yet another variation (see cru_logo.eps for this one)
				if (_tcsncmp(Buffer, "%%Creator: ps2ai.ps", 19) == 0)
				{
					HeaderFile.close ();
					return 9;
				}

				// If there is a creator field, see if it mentions Illustrator
				// NOTE: this test must be the last one of the "Creator:" tests.
				if (_tcsncmp(Buffer, "%%Creator:", 10) == 0)
				{
					// Found the creator line - does it contain the word Illustrator?
					if (_tcsstr(Buffer, "Illustrator") != NULL)
					{
						HeaderFile.close();
						return 9;
					}
					else
						break;
				}

				// If we find the compression token then stop the search as we don't want to
				// start looking in the compressed data!
				if (_tcsncmp(Buffer, "%%Compression:", 14)==0)
					break;
			}

			// Remember to close the file before returning.
			HeaderFile.close();

			// Didn't find a suitable Creator line, but the EPS line was ok, so return
			// that we're interested, but not sure.
			return 5;
		}

		// If we find the compression token then stop the search as we don't want to start
		// looking in the compressed data!
		if (_tcsncmp(Buffer, "%%Compression:", 14)==0)
			break;
	}

	// Remember to close the file before returning.
	HeaderFile.close();
	
	// This file type isn't suitable.
	return 0;
}

/********************************************************************************************

>	BOOL AIEPSFilter::PrepareToExport(CCLexFile* pFile, Spread *pSpread)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/94
	Inputs:		pFile - The file that we are to Export to
				pSpread - The spread to export
	Returns:	TRUE if succeeded, FALSE if not (e.g. no memory for EPS stack)
	Purpose:	Prepare to import EPS data using this filter.  This sets up the filter
				to a sensible state for reading.
	Errors:		Out of memory.
	SeeAlso:	EPSFilter::DoImport; EPSFilter::CleanUpAfterImport
	Scope: 		Private

********************************************************************************************/

BOOL AIEPSFilter::PrepareToExport(CCLexFile* pFile, Spread *pSpread)
{
	// Use base class to do most of it
	if (!EPSFilter::PrepareToExport(pFile, pSpread))
		return FALSE;

	// Create a new render region to export to:

	// Don't care about clip regions when exporting - create a null region.
	DocRect NullClipRect;
	NullClipRect.MakeEmpty();

	// Don't use rendering matrix when exporting EPS as it uses fractional coordinates.
	Matrix Identity;

	// Don't use view scale; set to 1
	FIXED16 Scale(1);

	// Create the region specific to our filter.
	ExportRegion = new AIEPSRenderRegion(NullClipRect, Identity, Scale);
	if (ExportRegion == NULL)
		return FALSE;

	// Attach to the right device.
	ExportRegion->AttachDevice(DocView::GetSelected(), ExportDCPtr, pSpread);

	// All ok
	return TRUE;
};

/********************************************************************************************

>	void AIEPSFilter::LookUpToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if the token is an AI EPS token; FALSE if not.
	Purpose:	Compare the current token against the AI keywords to see if it is
				one of them.
	SeeAlso:	EPSFilter::LookUpToken; EPSFilter::DecodeToken

********************************************************************************************/

void AIEPSFilter::LookUpToken()
{
	// Not interested in comments
	if (Token == EPSC_Comment)
		return;

	// Check to see if it is a keyword - cycle through the array of keyword names and
	// compare against our token (could use a hash table?)
	INT32 i = 0;
	while (AICommands[i].Cmd != EPSC_Invalid)
	{
		if (_tcscmp(TokenBuf, AICommands[i].CmdStr) == 0)
		{
			// Found the token - set the token variable and return success
			Token = AICommands[i].Cmd;
			return;
		}
		// Try next command
		i++;
	}

	// Did not find this token - pass on to base class.
	EPSFilter::LookUpToken();
}

/********************************************************************************************

>	BOOL AIEPSFilter::ProcessToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if token understood and processed ok, FALSE if not.
	Purpose:	Processes EPS tokens that are not part of the standard Illustrator set, or
				which need to be handled differently to the standard Illustrator meanings.
				i.e. this is the function that handles all the AI EPS operators.
	Errors:		Syntax error in EPS, Out of memory.
	SeeAlso:	EPSFilter::ProcessToken

********************************************************************************************/

BOOL AIEPSFilter::ProcessToken()
{
	// Decode the command, and execute it...
	switch (Token)
	{
		//
		// Various random text stuff - ignore it for now.
		//

		case EPSC_z:
			if (!Stack.Discard(5))
				goto EPSError;
			break;

		case EPSC_e:
			if (!Stack.DiscardArray())
				goto EPSError;
			break;

		case EPSC_t:
			if (!Stack.Discard())
				goto EPSError;
			break;

		case EPSC_T:
			break;

		// These are the new implementations of the appropriate illustrator tokens.
		// In order to work properly and handle complex paths properly, the 
		// stroking and filling commands have to do something different if they're
		// in the middle of some kind of complex path or other

		/*
		/N					% - N -
		{
		_pola 0 eq 
			{
			_doClip 1 eq {clip /_doClip 0 ddef} if 
			newpath
			} 
			{
			/CRender {N} ddef
			}ifelse
		} def

		*/
		case EPSC_N:
		case EPSC_n:
			if (EPSFlags.ComplexPath == 0)
			{
				if (bDoClip)
				{
					// cannot continue as there's no path
					if (pInkPath == NULL)
						goto EPSError;

					if (!ClipRegion.AddNewClippingPath(pInkPath))
						// Error!
						return FALSE;

					bDoClip = FALSE;
					pInkPath->ClearPath();
					delete pPath;
					pPath = NULL;
					pInkPath = NULL;
				}
				else
				{
					// Graeme (14/4/00) - Catch NULL pointers.
					if ( pInkPath != NULL )
					{
						// Deleting pInkPath throws access violations, which basically break
						// the import proceedure. Do not add a delete pInkPath line here!
						pInkPath->ClearPath ();
						pInkPath = NULL;
					}

					// Delete isn't worried about NULL pointers.
					delete pPath;
					pPath = NULL;
				}
			}
			else
			{
				// We have a deferred render situation. Simply remember the token
				DeferredToken = Token;
			}
			break;
		
		/*
		/W					% - W -
		{
		/_doClip 1 ddef
		} def
		*/

		case EPSC_W:
			bDoClip = TRUE;
			break;

		/*u					% - *u -
		{
		_pola 1 add /_pola exch ddef
		} def
		*/
		case EPSC__u:
			EPSFlags.ComplexPath++;
			break;

		/*U					% - *U -
		{
		_pola 1 sub /_pola exch ddef 
		_pola 0 eq {CRender} if
		} def
		*/
		case EPSC__U:
			EPSFlags.ComplexPath--;
			if (EPSFlags.ComplexPath <0)
				return FALSE;
			if (EPSFlags.ComplexPath == 0)
			{
				EPSCommand oldToken = Token;
				Token = DeferredToken;
				if (!ProcessToken())
					return FALSE;
				Token = oldToken;
			}
			break;

		
			// Now come the actualy path rendering primitives
			// These basically amount to either stroking or
			// filling the path, or both. However, if the
			// ComplexPath flag is not zero, they will do nothing
			// and merely record their token as the deferred token
			// Allowing the *U operator to render them at the end
			// of the complex group.
			// Clipping is equally complicated - if bDoClip is set
			// when ComplexPath is zero, it adds the current path
			// to the clipping region (i.e. clips to the clipping
			// region).
			// if bDoClip is zero (FALSE) then it goes ahead and
			// renders the path as it normally would
		/*
		/N					% - N -
		{
		_pola 0 eq 
			{
			_doClip 1 eq {clip /_doClip 0 ddef} if 
			newpath
			} 
			{
			/CRender {N} ddef
			}ifelse
		} def
		/n					% - n -
		{N} def
		/F					% - F -
		{
		_pola 0 eq 
			{
			_doClip 1 eq 
				{
				gsave _pf grestore clip newpath /_lp /none ddef _fc 
				/_doClip 0 ddef
				}
				{
				_pf
				}ifelse
			} 
			{
			/CRender {F} ddef
			}ifelse
		} def
		/f					% - f -
		{
		closepath
		F
		} def
		/S					% - S -
		{
		_pola 0 eq 
			{
			_doClip 1 eq 
				{
				gsave _ps grestore clip newpath /_lp /none ddef _sc 
				/_doClip 0 ddef
				}
				{
				_ps
				}ifelse
			} 
			{
			/CRender {S} ddef
			}ifelse
		} def
		/s					% - s -
		{
		closepath
		S
		} def
		/B					% - B -
		{
		_pola 0 eq 
			{
			_doClip 1 eq 	% F clears _doClip
			gsave F grestore 
				{
				gsave S grestore clip newpath /_lp /none ddef _sc
				/_doClip 0 ddef
				} 
				{
				S
				}ifelse
			}
			{
			/CRender {B} ddef
			}ifelse
		} def
		/b					% - b -
		{
		closepath
		B
		} def
		*/
		case EPSC_S:
		case EPSC_b:
		case EPSC_B:
		case EPSC_f:
		case EPSC_F:
		case EPSC_s:
			if (EPSFlags.ComplexPath > 0)
			{
				DeferredToken = Token;
				break;
			}

			if (bDoClip)
			{
				// cannot continue as there's no path
				if (pInkPath == NULL)
					goto EPSError;

				if (!ClipRegion.AddNewClippingPath(pInkPath))
					// Error!
					return FALSE;

				bDoClip = FALSE;
				pInkPath->ClearPath();
				delete pPath;
				pPath = NULL;
				pInkPath = NULL;
				break;
			}
			// Since now all we want is for it to do what the default
			// filter will do, pass it on.
			return EPSFilter::ProcessToken();
			break;

		case EPSC_H:
		case EPSC_h:
			// This operator does nothing in Illustrator. Let's follow suit:
			break;

		
		default:
			// Token not understood - pass on to base class
			return EPSFilter::ProcessToken();
	}


	// No errors encountered while parsing this token and its operands.
	return TRUE;
	
	
	// Error handlers:
EPSError:
	HandleEPSError();
	return FALSE;

#if 0	
NoMemory:
	HandleNoMemory();
	return FALSE;
#endif
}


/********************************************************************************************

>	char *AIEPSFilter::GetEPSCommand(EPSCommand Cmd)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Inputs:		Cmd - the EPS token, e.g. EPSC_aoa
	Returns:	Pointer to the string representation of the token, e.g. "aoa"
	Purpose:	Given an EPS token, return the string representation of it; mainly for
				debugging purposes.

********************************************************************************************/

char *AIEPSFilter::GetEPSCommand(EPSCommand Cmd)
{
	INT32 i = 0;
	while (AICommands[i].Cmd != EPSC_Invalid)
	{
		if (AICommands[i].Cmd == Cmd)
			return AICommands[i].CmdStr;

		// Try next command
		i++;
	}

	// Couldn't find it - default to base class method
	return EPSFilter::GetEPSCommand(Cmd);
}

/********************************************************************************************

>	virtual BOOL AIEPSFilter::NeedsPrintComponents ()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/00
	Returns:	FALSE - AI files don't need print components.
	Purpose:	Informs the print components code that no data should be written.

********************************************************************************************/

BOOL AIEPSFilter::NeedsPrintComponents ()
{
	// We don't want print components!
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL AIEPSFilter::WriteNodes ( RenderRegion	*pRegion,
										   ExportDC		*pDC,
										   BOOL			VisibleLayersOnly,
										   BOOL			CheckSelected,
										   BOOL			ShowProgress )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com> (from Neville's original code)
	Created:	12/4/00
	Inputs:		pRegion 				- the render region to export to.
				pDc						- device context to use, can be NULL.
				VisibleLayersOnly		- use visible layers or not
				ShowProgress			- TRUE then start up a progress bar or FALSE assume
										  the caller has done it.
	Returns:	TRUE if the export process completed successfully, FALSE if an error occured.
	Purpose:	Exports the nodes from the tree. Unlike the standard export, the layer is
				dispatched first, followed by its children in their usual order.
	SeeAlso:	Filter::ExportRender, Filter::ExportRenderNodes

********************************************************************************************/

BOOL AIEPSFilter::WriteNodes ( RenderRegion	*pRegion,
							   ExportDC		*pDC,
							   BOOL			VisibleLayersOnly,
							   BOOL			CheckSelected,
							   BOOL			ShowProgress )
{
	BOOL				success		= FALSE;

#ifdef DO_EXPORT

	// Find the first node that we should export from this spread
	AIEPSRenderRegion	*pAIEPSRR	= static_cast<AIEPSRenderRegion*> ( pRegion );
	Spread				*pSpread	= pRegion->GetRenderSpread ();
	Layer				*pLayer		= static_cast<Layer*> ( pSpread->FindFirstChild
															( CC_RUNTIME_CLASS ( Layer ) ) );
	Node				*pBGNode	= NULL;

	// Set the background colour.
	pRegion->SetBackgroundColour ( FlashFilter::GetPageColour ( pSpread, &pBGNode ) );

	// Export the file, but catch any file errors.
	TRY
	{
		// (ChrisG 5/4/2001) Find the first non-guide layer.
		while (pLayer && pLayer->IsGuide ())
		{
			pLayer = pLayer->FindNextLayer ();
		}

		// Export the layer and all the nodes if one is found.
		if (pLayer)
		{
			// Export the layer.
			pAIEPSRR->StartLayer ( pLayer );

			// Invoke the base class WriteNodes function.
			success = Filter::WriteNodes ( pRegion, pDC, VisibleLayersOnly, CheckSelected,
										ShowProgress );
		}
	}	// TRY

	CATCH ( CFileException, e )
	{
		// Didn't work - report failure to caller.
		if ( pDC )
			pDC->ExportFile->SetThrowExceptions ( FALSE );
		pRegion->StopRender ();
		if ( ShowProgress )
			EndSlowJob ();
		success = FALSE;
	}
	END_CATCH
#endif

	return success;
}

/********************************************************************************************

>	virtual void AIEPSFilter::ProcessTextMatrix(Matrix* pMatrix)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/95
	Returns:	-
	Purpose:	Having read a text object position matrix from the file, we may (dependent
				on the filter type), need to process it in some way, ie add in the spread
				origin.

********************************************************************************************/

void AIEPSFilter::ProcessTextMatrix(Matrix* pMatrix)
{
	// We need to find the current import spread
	// and add in the spread origin to this matrix.
	DocCoord pos,result;
	pMatrix->GetTranslation(pos);
	if (ImportInfo.pSpread->PagesCoordToSpreadCoord(&result,pos))
		pMatrix->SetTranslation(result);
}

/********************************************************************************************

>	virtual void AIEPSFilter::SetLineSpacing(INT32 Type, INT32 EMLSpace, MILLIPOINT MLSpace, Double DLSpace)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/4/2001
	Inputs:		see Filter::SetLineSpacing ()
	Returns:	BOOL - whether the call to EPSFilter::SetLineSpacing succeeded.
	Purpose:	This is simply here to allow Illustrator import to catch attempts to set the
				line spacing to zero.

********************************************************************************************/
BOOL AIEPSFilter::SetTextLineSpacing (INT32 Type, INT32 EMLSpace, MILLIPOINT MLSpace, double DLSpace)
{
	BOOL isZero = FALSE;
	BOOL success = FALSE;

	// is the line spacing going to be set to zero
	switch (Type)
	{
	case 0:	// Em based
		if (EMLSpace)
			isZero = FALSE;
		else
			isZero = TRUE;
		break;

	case 1:	// Millipoint based
		if (MLSpace) 
			isZero = FALSE;
		else
			isZero = TRUE;
		break;

	case 2:	// Proportional
		if (DLSpace)
			isZero = FALSE;
		else
			isZero = TRUE;
	}

	//	for some reason, Illustrator assumes that a line spacing of 0 actually 
	//	means 100% and confuses more capable apps by overriding this with 
	//	final-form (i.e. printing only) attributes.
	if (isZero)
	{
		success = EPSFilter::SetTextLineSpacing(2,0,0,1.0);
	}
	else
	{
		success = EPSFilter::SetTextLineSpacing(Type, EMLSpace, MLSpace, DLSpace);
	}

	return success;
}



CC_IMPLEMENT_DYNAMIC(PhotoShopEPSFilter, EPSFilter)


/********************************************************************************************

>	PhotShopEPSFilter::PhotoShopEPSFilter()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/00
	Purpose:	Constructor for an PhotoShopEPSFilter object.  The object should be 
				initialised	before use.
	SeeAlso:	EPSFilter::Init

********************************************************************************************/

PhotoShopEPSFilter::PhotoShopEPSFilter() : EPSFilter ()
{
	// Set up filter descriptions.
	FilterNameID = _R(IDT_PHOTOSHOPEPS_FILTERNAME);
	FilterInfoID = _R(IDT_IMPORTMSG_PHOTOSHOPEPS);
	ImportMsgID  = _R(IDT_IMPORTMSG_PHOTOSHOPEPS);

	FilterID = FILTERID_PHOTOSHOPEPS;

	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
}

/********************************************************************************************

>	BOOL PhotoShopEPSFilter::Init()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/00
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an PhotoShopEPSFilter object.
	Errors:		will fail if not enough memory.
	SeeAlso:	EPSFilter::Init

********************************************************************************************/

BOOL PhotoShopEPSFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new PhotoShopEPSOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(FilterNameID);
	FilterInfo.Load(FilterInfoID);

	// All ok
	return TRUE;
}

/********************************************************************************************

>	INT32 PhotoShopEPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/00
	Returns:	The compatibility in the range (0-10)
				0	- It's not a photoshop file, so we ain't interested.
				10	- It is, so let's stop it from importing.
	Purpose:	Checks to see if the EPS comment headers specify that this is an Photoshop
				generated EPS file, as required.

********************************************************************************************/

INT32 PhotoShopEPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)
{
	UINT32	Lines	= 0;
	char	*Buffer	= NULL;

	// !PS-Adobe line is ok - check creator line...
	CCMemTextFile HeaderFile ( reinterpret_cast<char *> ( pFileHeader ), HeaderSize );

	if( HeaderFile.IsMemFileInited () == FALSE || HeaderFile.InitLexer () == FALSE )
	{
		HeaderFile.close();
		return 0;
	}

	// Graeme (28/6/00) - Adobe have changed their file format, and so the first line can
	// now be a %PDF directive. Therefore look for this directive in the first twenty
	// lines.
	while ( ( Lines < 100 ) && !HeaderFile.eof () )
	{
		// Get the current line from the file.
		HeaderFile.GetLineToken();
		Buffer = const_cast<char *> ( HeaderFile.GetTokenBuf () );

		// Ensure that it's OK.
		ERROR2IF(Buffer == 0, 0, "Returned buffer from lex file == 0");

		// Increment the line counter.
		Lines++;

		if (_tcsncmp(Buffer, "%!PS-Adobe", 10) == 0)
		{
			// Now find the %%Creator string.
			while ((Lines < 100) && !HeaderFile.eof())
			{
				HeaderFile.GetLineToken();
				Buffer = const_cast<char *> ( HeaderFile.GetTokenBuf() );
				ERROR2IF(Buffer == 0, 0, "Returned buffer from lex file == 0");
				Lines++;

				// Return TRUE if this file was created by Illustrator, or has been exported
				// in Illustrator format.
				if (_tcsncmp(Buffer, "%%Creator: Adobe Photoshop", 26) == 0)
				{
					// We definitely want this.
		 			HeaderFile.close();
					return 10;
				}

				// If we find the compression token then stop the search as we don't want to
				// start looking in the compressed data!
				if (_tcsncmp(Buffer, "%%Compression:", 14)==0)
					break;
			}

			// Remember to close the file before returning.
			HeaderFile.close();

			// The photoshop EPS filter is only there to stop Xara X from importing Photoshop 
			//	EPS files - so it's not suitable.
			return 0;
		}

		// If we find the compression token then stop the search as we don't want to start
		// looking in the compressed data!
		if (_tcsncmp(Buffer, "%%Compression:", 14)==0)
			break;
	}

	// Remember to close the file before returning.
	HeaderFile.close();
	
	// This file type isn't suitable.
	return 0;
}


/********************************************************************************************

>	INT32 PhotoShopEPSFilter::PrepareToImport()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/00
	Returns:	FALSE - PhotoShop EPS files cannot be imported.
	Purpose:	Displays an error, if a photoshop EPS file is imported.

********************************************************************************************/

BOOL PhotoShopEPSFilter::PrepareToImport()
{
	// Display error message.
	String_256 WarnMsg;

	WarnMsg.MakeMsg(_R(IDT_IMPORTMSG_PHOTOSHOPEPS), "");
	Error::SetError(0, WarnMsg, 0);

	return FALSE;
}


/********************************************************************************************

>	void PhotoShopEPSFilter::CleanUpAfterImport(BOOL Successful)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/00
	Inputs:		Successful: TRUE => the import succeeded
						   FALSE => something went wrong with the import, so abandon all
									changes made.
	Purpose:	Cleans up after the PhotoShop "failed to import" message has been displayed
	SeeAlso:	PhotoShopEPSFilter::PrepareToImport; EPSFilter::PrepareToImport
	Scope: 		Protected

********************************************************************************************/

void PhotoShopEPSFilter::CleanUpAfterImport(BOOL Successful)
{
	// Inform all the document components that we have finished importing
	TheDocument->EPSEndImport(this, FALSE);
	TheDocument->PostImport();
}
