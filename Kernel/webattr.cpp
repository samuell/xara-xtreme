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
#include "webattr.h"
//#include "hotlink.h"
//#include "mario.h"		//For _R(IDE_NOMORE_MEMORY)
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixed16.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blendatt.h"
//#include "ink.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"
#include "rechattr.h"	// Record handling classes for attributes
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resimmap.h"	//_R(IDS_TAG_WEBADDRESS)
DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(AttrWebAddress, NodeAttribute)
CC_IMPLEMENT_DYNCREATE(WebAddressAttribute, AttributeValue)

// Declare smart memory handling in Debug builds
// Give this file in memory dumps
#define new CAM_DEBUG_NEW
	
/****************************************************************************************

  AttrWebAddress member functions

  ***************************************************************************************/
	
/***********************************

  Constructors
								 
  **********************************/
	
/********************************************************************************************

>	AttrWebAddress::AttrWebAddress()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrWebAddress::AttrWebAddress()
{
	Value=WebAddressAttribute();
}

	
/********************************************************************************************

>	AttrWebAddress::AttrWebAddress (Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		See NodeAttribute for a description
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a Web Address Attribute at a specific place in the tree
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrWebAddress::AttrWebAddress(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{   
	Value=WebAddressAttribute();
} 

/********************************************************************************************

>	AttrWebAddress::AttrWebAddress(WebAddress url, BOOL fBounding=FALSE, TCHAR* pcFrame=NULL)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Specific constructor for an AttrWebAddress
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrWebAddress::AttrWebAddress(WebAddress url, BOOL fBounding, TCHAR* pcFrame)
{
	//Construct our Value member variable using the WebAddressAttribute constructor
	Value=WebAddressAttribute(url, fBounding, pcFrame);
}

/********************************************************************************************

>	AttrWebAddress::AttrWebAddress(TCHAR* pcURL, BOOL fBounding=FALSE, TCHAR* pcFrame=NULL)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Specific constructor for an AttrWebAddress
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrWebAddress::AttrWebAddress(TCHAR* pcURL, BOOL fBounding, TCHAR* pcFrame)
{
	//Construct our Value member variable using the WebAddressAttribute constructor
	Value=WebAddressAttribute(pcURL, fBounding, pcFrame);
}


/*********************************

  Overloaded operators

  *******************************/

/********************************************************************************************

>	INT32 AttrWebAddress::operator==(const NodeAttribute& naCompare)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		naCompare		The NodeAttribute against which to compare this one
	Outputs:	-
	Returns:	TRUE if naCompare is identical to this attribute
				FALSE if naCompare is not an AttrWebAddress or if the value
						stored in naCompare is different from the one stored
						in this attribute
	Purpose:	A virtual comparison operator.

				It checks that:
				a. naCompare and this attribute are the same class
				b. naCompare and this attribute hold identical Value member variables
																					  
	Errors:		-
				 
	SeeAlso:	NodeAttribute::operator==; WebAddressAttribute::IsDifferent()

********************************************************************************************/

INT32 AttrWebAddress::operator==(const NodeAttribute& naCompare)
{
	//If naCompare is not an AttrWebAddress, return FALSE
	if (!naCompare.IsKindOf(CC_RUNTIME_CLASS(AttrWebAddress)))
		return FALSE;

	//Otherwise, cast naCompare into an AttrWebAddress pointer
	AttrWebAddress* pwaAttr = (AttrWebAddress*) &naCompare;
			 
	//And check whether the member variables are the same
	BOOL fAreDifferent=Value.IsDifferent((AttributeValue*) &(pwaAttr->Value));

	//And return accordingly
	return (!fAreDifferent); 
} 

/*********************************

  Operations

  *******************************/

/********************************************************************************************

>	void AttrWebAddress::Render( RenderRegion* pRender)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		pRender		The region into which to render this attribute
	Outputs:	-
	Returns:	-
	Purpose:	Set this attribute as current in the render region
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrWebAddress::Render( RenderRegion* pRender)
{
	//Check the render region pointer
	if (pRender==NULL)
		ERROR2RAW("AttrWebAddress::Render - pRender is NULL");

	//Set the clickable rectangle of our WebAddressAttribute
	Value.SetClickableRectangleInRendering(GetClickableRectangle());

	//Then set our WebAddressAttribute member variable into that render region
//#pragma message( __LOCMSG__ "Removed RenderRegion usage" )
	pRender->SetWebAddress(&Value, FALSE);
}

/********************************************************************************************

>	virtual BOOL AttrWebAddress::Blend(BlendAttrParam* pBlendParam)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from MarkN
	Created:	27/3/97
	Inputs:		pBlendParam = ptr to blend attribute param object holding all info needed
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Blends one Web Address attribute with another.
				
	SeeAlso:	WebAddressAttribute::Blend

********************************************************************************************/

BOOL AttrWebAddress::Blend(BlendAttrParam* pBlendParam)
{
	//Check our parameter
	ERROR2IF(pBlendParam == NULL, FALSE, "AttrWebAddress::Blend - NULL parameter");
	
	//Try and blend our WebAddressAttribute member variable
	if (Value.Blend(pBlendParam))
	{
		// Get the blended attr val. After this call, the ptr is our reponsibility
		// so we have to delete it if it's no longer needed
		AttributeValue* pBlendedAttrVal = pBlendParam->GetBlendedAttrVal();

		if (pBlendedAttrVal != NULL)
		{
			// We have a blended attr val, so ask it to make a NodeAttribute out of itself
			// and set the pBlendParam's blended NodeAttribute ptr to it
			NodeAttribute* pBlendedAttr = pBlendedAttrVal->MakeNode();
			pBlendParam->SetBlendedAttr(pBlendedAttr);

			if (pBlendedAttr != NULL)
			{
				// We were able to make a blended NodeAttribute
				// so delete the blended attr val, and return TRUE
				delete pBlendedAttrVal;
				return TRUE;
			}
			else
			{
				// Couldn't make a blended NodeAttribute, so give the blended attr val back
				// and return FALSE
				pBlendParam->SetBlendedAttrVal(pBlendedAttrVal);
				return FALSE;
			}
		}
	}
	 	
	return FALSE;
}

/********************************************************************************************

  >	virtual BOOL AttrWebAddress::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		pFilter - ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes this AttrWebAddress out to the filter.

	SeeAlso:	AttrWebAddress::WriteString

********************************************************************************************/

BOOL AttrWebAddress::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	//Check our incoming parameter
	ERROR2IF(pFilter == NULL,FALSE,"AttrWebAddress::WritePreChildrenWeb - NULL parameter");

	//Set up some variables.
	//This pointer points to the record we are currently writing
	CamelotFileRecord* pRecord=NULL;

	// Add a description of the TAG_MOULD_BOUNDS record, for older importers that don't understand this record
	pFilter->AddTagDescription(TAG_WEBADDRESS,_R(IDS_TAG_WEBADDRESS));

	//And this lets us know whether there's been an error
	BOOL ok=TRUE;

	//Now, is the clickable area of this attribute the bounding box?
	if (Value.m_fBounding)
	{
		//Yes it is.
		//So create a "bounding box" record
		pRecord=new CamelotFileRecord(pFilter,TAG_WEBADDRESS_BOUNDINGBOX,TAG_WEBADDRESS_SIZE);

		//Check it's been created OK
		ok=(pRecord!=NULL);

		//Initialise it
		if (ok)
			pRecord->Init();

		//We also want to write out the start of the record, which is the clickable
		//rectangle of this record

		//So get the clickable rectangle
		DocRect rectBounding=GetClickableRectangle();

		//And write it out
		pRecord->WriteCoordInterleaved(rectBounding.lo);
		pRecord->WriteCoordInterleaved(rectBounding.hi);
	}
	else
	{
		//No, the clickable area of this attribute is the object shape
		//So create a normal Web Address record
		pRecord=new CamelotFileRecord(pFilter,TAG_WEBADDRESS,TAG_WEBADDRESS_SIZE);

		//And check it's been created OK
		ok=(pRecord!=NULL);

		//Initialise it
		if (ok)
			pRecord->Init();
	}

	//The rest of the data we need to write out is common to both types of record.
	//We need to write out the URL and Frame strings, in that order.

	
	//So write out the URL string
	String_256 strToWrite=Value.m_url.GetWebAddress();

	if (ok)
		ok=WriteString(pRecord, strToWrite);

	//And the Frame string

	//We do this using our toolkit function WriteString. This function
	//will write out a NULL character if the character pointer we pass it
	//is NULL
	if (ok)
		ok=WriteString(pRecord, Value.m_pcFrame);

	//And write out our record
	if (ok) 
		ok = pFilter->Write(pRecord);

	//And delete it
	delete pRecord;

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

  >	virtual BOOL AttrWebAddress::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		pFilter - ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes this AttrWebAddress out to the filter.

				Native records are written out in exactly the same way as Web
				records are. So we simply call WritePreChildrenWeb()

	SeeAlso:	AttrWebAddress::WritePreChildrenWeb()

********************************************************************************************/
BOOL AttrWebAddress::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

/***********************************************************************************************
> Node* AttrWebAddress::SimpleCopy() 

    Author:     Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from Will
    Created:    27/3/97
	Inputs:     - 
    Outputs:    -
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrWebAddress::SimpleCopy()
{
	AttrWebAddress* NodeCopy = new AttrWebAddress();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 



/***********************************************************************************************
> ` void AttrWebAddress::CopyNodeContents( AttrWebAddress* NodeCopy)

    Author:     Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from Simon
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrWebAddress::CopyNodeContents( AttrWebAddress* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );
	
	NodeCopy->Value=Value;
} 



/***********************************************************************************************
>   void AttrWebAddress::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrWebAddress::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrWebAddress), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrWebAddress))
		CopyNodeContents((AttrWebAddress*)pNodeCopy);
}



/***********************************************************************************************
> ` DocRect AttrWebAddress::GetClickableRectangle()

    Author:     Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
    Created:    27/3/97
	
	Inputs:     -
    Outputs:    -
    Returns:    The clickable rectangle of this Web Address attribute
		 
    Purpose:    This function calls GetImagemapClickableRectangle() on this
				node's parent.

				If the parent node is a text character, the rectangle returned
				will be the bounding box of that text character and all
				adjacent text characters with the same Web Address attribute.

				Otherwise, the rectangle returned will be the bounding box
				of the parent node.
		      
	Errors:     -
	SeeAlso:	NodeRenderableBounded::GetImagemapClickableRectangle(); 
				AbstractTextChar::GetImagemapClickableRectangle()
	
    Scope:      protected
								     
***********************************************************************************************/

DocRect AttrWebAddress::GetClickableRectangle()
{
	//Get our parent
	Node* pNode=FindParent();

	
	//If it's not an object, return a NULL value
	if (pNode==NULL)
		return DocRect(0,0,0,0);
	
	if (!pNode->IsAnObject())
		return DocRect(0,0,0,0);

	//Otherwise, return the bounding box of that object
	return (((NodeRenderableBounded*)pNode)->GetImagemapClickableRectangle());
																			  	
} 

/********************************************************************************************

>	BOOL AttrWebAddress::WriteString(CamelotFileRecord* pcfrRecord, TCHAR* pcWrite)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		pcWrite		Pointer to a string to write out to the record
	Outputs:	pcfrRecord	The record to write out to
	Returns:	-
	Purpose:	Writes pcWrite out to the file.

				If pcWrite is NULL, a NULL character will be written out instead.

	SeeAlso:	-

********************************************************************************************/

BOOL AttrWebAddress::WriteString(CamelotFileRecord* pcfrRecord, TCHAR* pcWrite)
{
	//Does pcWrite point to a string?
	if (pcWrite)
	{
		//Yes. So write out that string.
		return pcfrRecord->WriteUnicode(pcWrite);
	}
	else
	{
		//No. So write out a NULL character
		TCHAR cNull='\0';

		return pcfrRecord->WriteUnicode(&cNull);
	}
}


#ifdef _DEBUG
/***********************************************************************************************

>	void AttrWebAddress::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrWebAddress::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif


void AttrWebAddress::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	(*Str) += _T("URL=");
	(*Str) += Value.m_url.GetWebAddress();
	(*Str) += _T("\r\n");

	(*Str) += _T("Frame=");
	(*Str) += Value.m_pcFrame;
	(*Str) += _T("\r\n");
	
	if (Value.m_fBounding)
		(*Str) += _T("Clickable area is RECTANGLE\r\n");
	else
		(*Str) += _T("Clickable area is OBJECT SHAPE\r\n");
	
#endif
}

/****************************************************************************************

  AttrWebAddress member functions

  ***************************************************************************************/
	
/***********************************

  Constructors and initialiser
								 
  **********************************/


/********************************************************************************************

>	WebAddressAttribute::WebAddressAttribute()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

WebAddressAttribute::WebAddressAttribute()
{
	//Set the URL to a default
	m_url=WebAddress();

	//Put "_self" into the Frame member variable
	String_256 strSelf(_R(IDS_WEBADDRESS_SHORT_SELF));

	m_pcFrame=NULL;

	CopyString(&m_pcFrame, strSelf);

	//Set the bounding flag to FALSE
	m_fBounding=FALSE;
}

/********************************************************************************************

>	WebAddressAttribute::WebAddressAttribute(WebAddress url, BOOL fBounding=FALSE, TCHAR* pcFrame=NULL)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		New values for member variables
	Outputs:	-
	Returns:	-
	Purpose:	Specific constructor for a Web Address attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

WebAddressAttribute::WebAddressAttribute(WebAddress url, BOOL fBounding, TCHAR* pcFrame)
{
	//First set our member variable string pointer to NULL, 
	//so that CopyString doesn't try to delete it
	m_pcFrame=NULL;
	
	//Then copy everything into our member variables
	m_url=url;
	CopyString(&m_pcFrame, pcFrame);
	m_fBounding=fBounding;

	//And set the clickable in rendering rectangle to a zero value
	//(See header file for a description of this variable)
	m_rectClickableInRendering=DocRect(0,0,0,0);
}

/********************************************************************************************

>	WebAddressAttribute::WebAddressAttribute(WebAddress url, BOOL fBounding=FALSE, TCHAR* pcFrame=NULL)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		New values for member variables
	Outputs:	-
	Returns:	-
	Purpose:	Specific constructor for a Web Address attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

WebAddressAttribute::WebAddressAttribute(TCHAR* pcURL, BOOL fBounding, TCHAR* pcFrame)
{
	//Simply put pcURL into a string
	String_256 strURL=pcURL;

	//Put that into a Web Address
	WebAddress urlURL(strURL);

	m_pcFrame=NULL;

	//And make our attribute as normal
	*this=WebAddressAttribute(urlURL, fBounding, pcFrame);
}


/********************************************************************************************

>	BOOL WebAddressAttribute::Init()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Registers the default WebAddressAttribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL WebAddressAttribute::Init()
{
	WebAddressAttribute *pAttr = new WebAddressAttribute();
	
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;

	ENSURE(ID == ATTR_WEBADDRESS, "Incorrect ID for Web Address attribute!");
	return TRUE;
}


/******************************************************

  Copy constructor, destructor and assignment operator

  *****************************************************/
/********************************************************************************************

>	WebAddressAttribute::WebAddressAttribute(const WebAddressAttribute& waaCopy)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		WebAddressAttribute to copy
	Outputs:	-
	Returns:	*this
	Purpose:	Copy constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

WebAddressAttribute::WebAddressAttribute(const WebAddressAttribute& waaCopy)
{
	//Set our member variables to NULL
	m_pcFrame=NULL;

	m_rectClickableInRendering=DocRect(0,0,0,0);

	//Simply use the assigment operator
	*this=waaCopy;
						   
}

/********************************************************************************************

>	WebAddressAttribute::~WebAddressAttribute()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor
	Errors:		-						   
	SeeAlso:	-

********************************************************************************************/

WebAddressAttribute::~WebAddressAttribute()
{
	if (m_pcFrame)
		free(m_pcFrame);
}

/********************************************************************************************

>	WebAddressAttribute& operator= (const WebAddressAttribute& waaOther)					

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		waaOther	Attribute to copy
	Outputs:	-
	Returns:	-
	Purpose:	Assignment operator
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

WebAddressAttribute& WebAddressAttribute::operator= (const WebAddressAttribute& waaOther)					
{
	//Copy everything into our member variables
	m_url=waaOther.m_url;
	CopyString(&m_pcFrame, waaOther.m_pcFrame);
	m_rectClickableInRendering=waaOther.m_rectClickableInRendering;
	m_fBounding=waaOther.m_fBounding;

	return *this;
}

/**********************************

  Operations

  **********************************/

/********************************************************************************************

>	void WebAddressAttribute::Render(RenderRegion *pRegion, BOOL Temp)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/97
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets this Web Address attribute as current within the render
				region.

				Note that nothing is actually seen on screen as a result
				of rendering the Web Address Attribute.

********************************************************************************************/

void WebAddressAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "Removed RenderRegion usage" )
	pRegion->SetWebAddress(this, Temp);
}

/********************************************************************************************

>	void WebAddressAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/97
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Restores this Web Address attribute into the region

********************************************************************************************/

void WebAddressAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "Removed RenderRegion usage" )
	pRegion->RestoreWebAddress(this, Temp);
}

/********************************************************************************************

>	void WebAddressAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		pValue - pointer to the value to copy
	Purpose:	Assignment operator
	SeeAlso:	WebAddressAttribute::operator=

********************************************************************************************/

void WebAddressAttribute::SimpleCopy(AttributeValue *pValue)
{
	//Check parameter
	if (pValue==NULL)
	{
		ERROR2RAW("WebAddressAttribute::SimpleCopy - NULL parameter");
		return;
	}



	//If pValue is a Web Address
	if (pValue->IsKindOf(CC_RUNTIME_CLASS(WebAddressAttribute)))
	{
		//Copy it into this attribute
		*this=*((WebAddressAttribute*) pValue);
	}
	else
		//Make this attribute blank
		*this=WebAddressAttribute();
}

/********************************************************************************************

>	NodeAttribute* WebAddressAttribute::MakeNode()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from Tim
	Created:	27/3/97
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrWebAddress node from this line width attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *WebAddressAttribute::MakeNode()
{
	// Create new attribute node
	AttrWebAddress *pAttr = new AttrWebAddress();

	// Copy attribute value into the new node
	pAttr->Value=*this;

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	BOOL WebAddressAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Purpose:	Determines whether the two attributes are different

				If the two attributes are of different types, FALSE is returned.

	Errors:		-
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL WebAddressAttribute::IsDifferent(AttributeValue *pAttr)
{
	//Check parameter
	ERROR2IF(pAttr==NULL, FALSE, "WebAddressAttribute::IsDifferent - NULL parameter");

	//If pAttr doesn't point to a WebAddressAttribute, return FALSE
	if (!pAttr->IsKindOf(CC_RUNTIME_CLASS(WebAddressAttribute)))
		return FALSE;

	//Cast pAttr into a WebAddressAttribute*
	WebAddressAttribute* pwaaThis=(WebAddressAttribute*) pAttr;

	//And now check whether each of the member strings are different
	BOOL fSameURL=(m_url==pwaaThis->m_url);
	BOOL fSameFrame=AreStringsEqual(m_pcFrame, pwaaThis->m_pcFrame);
	BOOL fSameArea=(m_fBounding==pwaaThis->m_fBounding);
		
	//If any of them were different, return TRUE
	return (!fSameURL || !fSameFrame || !fSameArea);
}

/********************************************************************************************

>	BOOL WebAddressAttribute::Blend(BlendAttrParam *pBlendParam)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from Markn
	Created:	27/3/97
	Inputs:		pBlendParam = ptr to param holding all data needed for the attr val to blend
	Outputs:	if TRUE  returned, pBlendParam->GetBlendedAttrVal() will get ptr to the blended attr val
				if FALSE returned, pBlendParam->GetBlendedAttrVal() will return NULL
	Returns:	TRUE if successful, FALSE otherwaie
	Purpose:	Blends this attr val with the attr val held in pBlendParam
	SeeAlso:	-

********************************************************************************************/

BOOL WebAddressAttribute::Blend(BlendAttrParam *pBlendParam)
{
	// Check entry param
	ERROR3IF(pBlendParam == NULL,"NULL entry param");
	if (pBlendParam == NULL) return FALSE;

	WebAddressAttribute* pOtherWebAttr = (WebAddressAttribute*) pBlendParam->GetOtherAttrVal();
	
	// Check that the other line attr val is not NULL, and is a WebAddressAttribute
	ERROR3IF(pOtherWebAttr == NULL,"NULL other attr val");
	ERROR3IF(!IS_A(pOtherWebAttr,WebAddressAttribute),"other attr val not a Web Address attr");
	if (pOtherWebAttr == NULL || !IS_A(pOtherWebAttr,WebAddressAttribute)) return FALSE;

	// If both attributes do not have a URL then we must return FALSE. 
	// Bug fix - Ranbirr 17/03/98
	if(!this->HasURL() && !pOtherWebAttr->HasURL())
		return FALSE;

	// Get a new WebAddressAttribute to hold the blended version, (return FALSE if this fails)
	WebAddressAttribute* pBlendedWebAttr = new WebAddressAttribute;
	if (pBlendedWebAttr == NULL) return FALSE;

	//If the blend ratio is less than 0.5, we use this WebAddressAttribute.
	//Otherwise, we use the other WebAddressAttribute
	double dRatio=pBlendParam->GetBlendRatio();

	if (dRatio<0.5)
		*pBlendedWebAttr=*this;
	else
		*pBlendedWebAttr=*pOtherWebAttr;

	// Store the ptr to the new blended line width attr val
	pBlendParam->SetBlendedAttrVal(pBlendedWebAttr);

	return TRUE;
}

/**********************************

  Toolkit functions

  **********************************/


/********************************************************************************************

void WebAddressAttribute::CopyString(TCHAR** ppcCopyTo, const TCHAR* pcCopyFrom)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		ppcCopyTo	Pointer to a string pointer to copy the string to
				pcCopyFrom	String pointer to copy from
	Outputs:	ppcCopyTo
	Returns:	-
	Purpose:	IF pcCopyFrom is NULL
					then the string pointer that ppcCopyTo is set to NULL

				ELSE IF pcCopyFrom points to a string
					then that string is copied into the string pointer that
					pcCopyTo pointer to

				This function will handle pcCopyFrom or *ppcCopyTo being NULL,
				but ppcCopyTo must be valid. It's obvious if you think about it.

	SeeAlso:	-

********************************************************************************************/

void WebAddressAttribute::CopyString(TCHAR** ppcCopyTo, const TCHAR* pcCopyFrom)
{
	//We need to check that ppcCopyTo is valid
	if (ppcCopyTo==NULL)
	{
		ERROR2RAW("WebAddress::CopyString passed NULL parameter");
	 	return;
	}

	//Now, if *ppcCopyTo points to a string, we delete it now
	if (*ppcCopyTo)
	{
		free (*ppcCopyTo);
		*ppcCopyTo=NULL;	
	}
		
	//Does pcCopyFrom point to a string?
	if (pcCopyFrom)
	{
		//Yes (we assume it does if it's not NULL)
		//So copy that string into *ppcCopyTo
		(*ppcCopyTo) = camStrdup( pcCopyFrom );
	}

	//If pcCopyFrom is NULL, then ppcCopyTo is NULL, which is correct
	//So return
}

/********************************************************************************************

BOOL WebAddressAttribute::AreStringsEqual(TCHAR* pcFirst, TCHAR* pcSecond)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		pcFirst, pcSecond - the two strings to compare
	Outputs:	ppcCopyTo
	Returns:	TRUE IF pcFirst, pcSecond are both NULL
					 OR if they point to identical strings
				FALSE otherwise
	Purpose:	To compare two string pointers
b				
				Note that this function handles its input parameters being NULL
	SeeAlso:	-

********************************************************************************************/

BOOL WebAddressAttribute::AreStringsEqual(TCHAR* pcFirst, TCHAR* pcSecond)
{
	//If either of our incoming string pointers is NULL
	if (pcFirst==NULL || pcSecond==NULL)
		//Then return TRUE if they are both NULL and FALSE otherwise
		return (pcFirst==pcSecond);

	//Otherwise, return the result of comparing the two strings
	return (camStrcmp(pcFirst, pcSecond)==0);
}





										  
