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

/*


*/


#include "camtypes.h"

#include <fstream.h>
#include <strstrea.h>
#include <ctype.h>

#include "node.h"
#include "nodepath.h"
#include "docrect.h"
#include "lineattr.h"        
#include "doccolor.h"
#include "ensure.h"

#include "readeps.h"

DECLARE_SOURCE("$Revision: 662 $");

// Scale factor of ArtWorks EPS coords -> Camelot DocCoords
const INT32 ScaleFactor = 1000;

#if 0
/********************************************************************************************

>	void ReadEPSData(char *FileName, Node *Parent, DocRect& PageRect, BOOL AddAttrs)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Inputs:		FileName - name of EPS file to read.
						Parent - The node to add the EPS data under.
						PageRect - The coordinates of the page to put the data onto.
						AddAttrs - if TRUE, random colour and line width attributes will be added to
						the lines.
	Outputs:	-
	Returns:	-
	Purpose:	Read an ArtWorks EPS file and convert it to paths for use by Camelot by
						constructing path objects in the document tree.  The ArtWorks EPS files
						must be filtered so they only contain moveto, lineto, and curveto
						commands. This can be done with egrep as follows:
				
						egrep " c$| l$| m$" SrcEPS > DestEPS
				
	Errors:		-
	SeeAlso:	Document

********************************************************************************************/

void ReadEPSData(char *FileName, Node *Parent, DocRect& PageRect, BOOL AddAttrs)
{
	// Get the directory of the .EXE file.
	char Appname[_MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(), Appname, _MAX_PATH-1);
	char Drive[_MAX_DRIVE];
	char Dir  [_MAX_DIR  ];
	char Fname[_MAX_FNAME];
	char Ext  [_MAX_EXT  ];
	_tsplitpath(Appname, Drive, Dir, Fname, Ext);
	 
	// Use this location to construct the full pathname for EPS file
	char FullFileName[_MAX_PATH];
	_tcscpy(FullFileName, Drive);
	_tcscat(FullFileName, Dir);
	_tcscatl(FullFileName, FileName);
	
	// Array to hold coords that are read in from the file.
	DocCoord Coords[4];
	DocCoord StartPoint;
	DocCoord LastPoint;
	INT32 	 SegmentCount = 0;			// counts how long the line is
	
	// Pointer to path node
	NodePath *MyPath = NULL;

	// Path tag
	INT32 *Current;
	
	// Open the data file...	
	ifstream in(FullFileName, ios::in | ios::nocreate);
	
	if (in.is_open())
	{
		char buf[256];
		while (!in.eof())
		{
			// Read in the next line
			in.getline(buf, 255);
			
			// Prepare to parse the line
			unsigned i = 0;
			istrstream line(buf);
			
			BOOL CmdExecuted = FALSE;
			
			while ((!CmdExecuted) && (i <= 3))
			{
				// Read in the next field
				line.eatwhite();

				// Is it a coordinate or command?
				if (!isdigit(line.peek()))
				{
					// Didn't find an numeric - must be a command
					char Command;
					line >> Command;
					if (Command == 'm')
					{
						// MoveTo - clean up current path and start a new one
						if (SegmentCount < 2  &&  MyPath != NULL)
						{
							MyPath->CascadeDelete();
							MyPath = NULL;
						}
						
						if (MyPath != NULL)
						{
							MyPath->UpdateInkBoundingRect();
							
							if ( LastPoint.x == StartPoint.x  &&  LastPoint.y == StartPoint.y )
								MyPath->InkPath.SetFilled( TRUE );
							else
								MyPath->InkPath.SetFilled( FALSE );
                            
							//	Testing node attributes here 
							//  ****************************
							if (AddAttrs)
							{
		 						DocColour Random((INT32)(rand()%255),
		    					   				 (INT32)(rand()%255),
			   									 (INT32)(rand()%255));
	    
								AttrFillColour* Attr1 = new AttrFillColour(MyPath, LASTCHILD);
		    					Attr1->FillColour = DocColour(Random);
	
								AttrLineColour* Attr2 = new AttrLineColour(MyPath, LASTCHILD);
		    					Attr2->LineColour = DocColour(Random);
	    												
		    					AttrLineWidth* Attr3 =  new AttrLineWidth(MyPath, LASTCHILD); 
		    					Attr3->LineWidth = (MILLIPOINT)(750*((rand()%3) + 1)); 
							}
						}

						MyPath = new NodePath(Parent, LASTCHILD); 
						if (!MyPath->SetUpPath())
						{
							MyPath->CascadeDelete();
							ENSURE(FALSE, "ReadEPSData: Run out of memory");
							return;
						}
							
						Current = MyPath->InkPath.FindStartOfPath();
						Current = MyPath->InkPath.InsertMoveTo(Coords[0], Current);
						StartPoint = Coords[0];
						LastPoint = StartPoint;
						SegmentCount = 1;
					}
					else if (Command == 'l')
					{
						// LineTo
						Current = MyPath->InkPath.InsertLineTo(Coords[0], Current);
						LastPoint = Coords[0];
						SegmentCount++;
					}
					else if (Command == 'c')
					{
						// CurveTo
						Current = MyPath->InkPath.InsertCurveTo(Coords[0], Coords[1],
																Coords[2], Current);
						LastPoint = Coords[0];
						SegmentCount++;
					}
					else if (IsUserName("Tim"))
					{
						TRACE( _T("ReadEPSData: unknown command in path data file: '%c'\n"),
							  Command);
					}
					
					CmdExecuted = TRUE;
				}
				else
				{
					// Coordinate - get the x and y coords, and scale them up
					float NewCoord;

					line >> NewCoord;
					Coords[i].x = (INT32) (NewCoord * ScaleFactor);
					Coords[i].x += PageRect.lo.x;
					
					line >> NewCoord;
					Coords[i].y = (INT32) (NewCoord * ScaleFactor);
					Coords[i].y += PageRect.lo.y;

					i++;
				}
			}
			if ((!CmdExecuted) && IsUserName("Tim"))
			{
				TRACE( _T("Document constructor: Too many coords in path data file.\n"));
				break;
			}
		}
		// Set the bounding rectangle for the last path created.
		if (SegmentCount < 2)
		{
			MyPath->CascadeDelete();
			MyPath = NULL;
		}
						
		if (MyPath != NULL)
		{
			MyPath->UpdateInkBoundingRect(); 
			
			if ( LastPoint.x == StartPoint.x  &&  LastPoint.y == StartPoint.y )
				MyPath->InkPath.SetFilled( TRUE );
			else
				MyPath->InkPath.SetFilled( FALSE );
			

			//	Testing node attributes here 
			//  ****************************
			if (AddAttrs)
			{
				DocColour Random((INT32)(rand()%255),
								 (INT32)(rand()%255),
								 (INT32)(rand()%255));
	    
				AttrFillColour* Attr1 = new AttrFillColour(MyPath, LASTCHILD);
				Attr1->FillColour = DocColour(Random);
	
				AttrLineColour* Attr2 = new AttrLineColour(MyPath, LASTCHILD);
	    												
				AttrLineWidth* Attr3 =  new AttrLineWidth(MyPath, LASTCHILD); 
				Attr3->LineWidth = (MILLIPOINT)(750*((rand()%3) + 1)); 
			}
		}
	}
	else if (IsUserName("Tim"))
		TRACE( _T("Could not open EPS file '%s'\n"), FullFileName);
}

#endif
