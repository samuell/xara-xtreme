// $Id: xh_treebook.h 1089 2006-05-16 17:57:54Z alex $
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

/////////////////////////////////////////////////////////////////////////////
// Name:        xh_treebk.h
// Purpose:     XML resource handler for wxTreebook
// Author:      Evgeniy Tarassov
// Created:     2005/09/28
// Copyright:   (c) 2005 TT-Solutions <vadim@tt-solutions.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WXXTRA_XH_TREEBK_H_
#define _WXXTRA_XH_TREEBK_H_

#include "treebook.h"

#if wxXTRA_TREEBOOK

#include <wx/xrc/xmlres.h>
#include <wx/dynarray.h>

WX_DEFINE_USER_EXPORTED_ARRAY_SIZE_T(size_t, wxArrayTbkPageIndexes,
                                     class WXDLLIMPEXP_XRC);

// ---------------------------------------------------------------------
// wxTreebookXmlHandler class
// ---------------------------------------------------------------------
// Resource xml structure have to be almost the "same" as for wxNotebook
// except the additional (size_t)depth parameter for treebookpage nodes
// which indicates the depth of the page in the tree.
// There is only one logical constraint on this parameter :
// it cannot be greater than the previous page depth plus one
class WXDLLIMPEXP_XRC wxTreebookXmlHandler : public wxXmlResourceHandler
{
public:
    wxTreebookXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);

private:
    wxTreebook *m_tbk;
    wxArrayTbkPageIndexes m_treeContext;
    bool m_isInside;

    DECLARE_DYNAMIC_CLASS(wxTreebookXmlHandler)
};


// Example:
// -------
// Label
// \--First
// |  \--Second
// \--Third
//
//<resource>
//  ...
//  <object class="wxTreebook">
//    <object class="treebookpage">
//      <object class="wxWindow" />
//      <label>My first page</label>
//      <depth>0</depth>
//    </object>
//    <object class="treebookpage">
//      <object class="wxWindow" />
//      <label>First</label>
//      <depth>1</depth>
//    </object>
//    <object class="treebookpage">
//      <object class="wxWindow" />
//      <label>Second</label>
//      <depth>2</depth>
//    </object>
//    <object class="treebookpage">
//      <object class="wxWindow" />
//      <label>Third</label>
//      <depth>1</depth>
//    </object>
//  </object>
//  ...
//</resource>

#endif // wxXTRA_TREEBOOK

#endif // _WX_XH_TREEBK_H_

