// $Id: ccobject.h 662 2006-03-14 21:31:49Z alex $
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
 *  */

#ifndef INC_CCOBJECT
#define INC_CCOBJECT

// CCObject and CCRuntimeClass are mutually dependent, so we have these forward references.
class CCObject;
class CCRuntimeClass;


/********************************************************************************************

>	class CCRuntimeClass

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/93
	Purpose:	Provide run-time class information and dynamic class creation for objects
				derived from CCObject.
				
	SeeAlso:	CCObject

********************************************************************************************/


class CCAPI CCRuntimeClass
{
public:
	// Text string og the class's name e.g. "CCObject"
	LPCTSTR m_lpszClassName;
	
	// Size of the object in bytes
	INT32 m_nObjectSize;
	
	// Schema number - this used in serialisation. (-1 for classes which do not support
	// serialisation).
	UINT32 m_wSchema; 
	
	// If the class supports dynamic creation, this is a pointer to the default
	// constructor for the class. Do not use this pointer - use CreateObject() instead.
	void (PASCAL* m_pfnConstruct)(void* p);
	
	// A pointer to the runtime class information object of the base class that this
	// class is derived from.
	CCRuntimeClass* m_pBaseClass;

	// If the class supports dynamic creation, this function will create an instance of
	// the required class.  e.g.  RUNTIME_CLASS(Node)->CreateObject()
	CCObject* CreateObject();
	BOOL ConstructObject(void* pThis);
	BOOL IsKindOf(const CCRuntimeClass* pClass) const;
	LPCTSTR			GetClassName() const {return m_lpszClassName;}
	CCRuntimeClass*	GetBaseClass1() const {return m_pBaseClass;}
	INT32				GetSize() const {return m_nObjectSize;}

	// Used to maintain the global list of CCRuntimeClass structures of all classes
	// derived from CCObject which support dynamic classing (CC_DECLARE_DYNAMIC).
	// This list is built by constructors of static class members of type
	// CC_CLASSINIT.
	static CCRuntimeClass* pFirstClass; 
	CCRuntimeClass* m_pNextClass;       
};


/********************************************************************************************

>	class SimpleCCObject

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/93
	Purpose:	Override new and delete, so that new returns NULL when no more memory is
				available, rather than throwing an exception.
				
				NB. ALL Camelot objects not derived from OIL class libraries should be
				be derived from SimpleCCObject in some way, be it directly, or via CCObject.
				This is so they don't throw exceptions if we try to create them and there is
				not enough memory.
	SeeAlso:	-

********************************************************************************************/

class CCAPI SimpleCCObject
{
public:

	// This version of new is used to dynamically create objects - it allocates
	// no memory, but calls the constructor to fill in object's data pointed to
	// by p. (See Construct and ConstructObject).
	// The operation of this dynamic creation is not fully understood at this time...
	void* operator new(size_t, void* p) { return p; }
#if defined(__WXMSW__)
	void operator delete(void* p, void* p1) {delete p;}
#elif defined(__WXMAC__)
PORTNOTE("MacPort", "Memory tracking is disabled on the Mac")
//	void operator delete(void* p, void* p1) {delete p;}
#endif
	
	// 'Standard' new/delete functions.
	void* operator new(size_t nSize);
	void operator delete(void* p);

	void* operator new(size_t nSize, LPCSTR lpszFileName, INT32 nLine);
#if defined(__WXMSW__)
	void operator delete(void* p, LPCSTR lpszFileName, INT32 nLine) {delete p;}
#elif defined(__WXMAC__)
PORTNOTE("MacPort", "Memory tracking is disabled on the Mac")
//	void operator delete(void* p, LPCSTR lpszFileName, INT32 nLine) {delete p;}
#endif
	
	// Support functions
	static void MemoryTrackAdd(void* p, LPCSTR Filename, INT32 Line);
	static void MemoryDump();
	static BOOL Init();

#ifdef _DEBUG
	static BOOL CheckMemory( BOOL Assert );

	// Low memory test support
	static void EnableLowMemoryTesting();
	static void DisableLowMemoryTesting();
	static void SetClaimLimit(UINT32 BytesToAllow);
	static BOOL GetLowMemoryState() {return FailingMemory;}
	static size_t GetLowMemoryLimit() {return BytesBeforeFail;}

protected:
	static size_t BytesBeforeFail;
	static BOOL FailingMemory;

public:
	static BOOL CheckMemoryFlag;

#endif	// _DEBUG

};


// This #define allows new to do all sorts of extra stuff
//// Declare smart memory handling in Debug builds
#ifdef _DEBUG
#if !defined(__WXMAC__)
	#define CAM_DEBUG_NEW new(THIS_FILE, __LINE__)
#else
PORTNOTE("MacPort", "Memory tracking is disabled on the Mac")
	#define CAM_DEBUG_NEW new
#endif
#else
	#define CAM_DEBUG_NEW new
#endif


/**********************************************************************************************

>	class CCObject : public SimpleCCObject

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/1993
	Base Class:	SimpleCCObject
	Purpose:	CCObject is derived from SimpleCCObject, so it provides the same new and
				delete operators.  It also provides facilities for dynamic class
				information, and dynamic construction of objects.
				
				You should derive your classes from CCObject, where practical.  To give a
				class dynamic class information, you must do the following:
				
				In your class declaration (preferably at the beginning) you should put:
				MonoOn
				CC_DECLARE_DYNAMIC(MyClass)
				MonoOff
				where MyClass is the name of your class.  Note that there is semi-colon at 
				the end of the line. Then you should put the following in your class 
				definition (.cpp file):
				MonoOn
				CC_IMPLEMENT_DYNAMIC(MyClass, BaseClass)
				MonoOff
				where BaseClass is the name of the class that MyClass is derived from.  This
				may be CCObject, or it may be something else if you have derived your class
				from another class which is in turn derived from CCObject, and so on.
				THe position of this line is not important (as long as it is not inside any
				functions), but it must be present for dynamic class information to work.
				Also note that there is no semi-colon at the end of this line.
				
				Declaring your class as dynamic in this way allows you to test its type
				when you don't know what it will be at runtime.  For example:
				
				MonoOn
				void *GetAnObject();
				
				CCObject *pObject = GetAnObject();
				
				if (pObject->IsKindOf(CC_RUNTIME_CLASS(MyClass))
					do_something;
				else if (pObject->IsKIndOf(CC_RUNTIME_CLASS(Node))
					do_something else;
				else ...
				MonoOff
				
				The special macro CC_RUNTIME_CLASS should be used when trying to identify
				an object at runtime.  It evaluates to a reference to the structure containing
				the runtime class information for the specified class - the IsKindOf() function
				then simply compares pointers to see if the objects are from the same class.
				Therefore this type-checking is quite efficient.
				
				When your class is declared as dynamic, it also means that memory dumps are
				more helpful because instead of saying "CCObject at $247247" they will say
				"MyClass at $247247", so we know exactly what kind of objects are not being
				cleaned up.
				
				Deriving a class from CCObject can be quite an 'expensive' thing to do, as it
				means that the objects have virtual function tables.  However, in debug 
				builds, the dynamic class info can be very useful.  For this reason, there
				are a number of macros which enable you to have dynamic class facilities in
				debug builds, but not in retail builds.  These macros are as follows:
				
				MonoOn
				CC_CLASS_MEMDUMP
				MonoOff
				This macro should be used in class declarations like this:
				
				MonoOn
				class MyClass : public CC_CLASS_MEMDUMP
				{
					...
				};
				MonoOff
				
				This macro will evaulate to SimpleCCObject in retail builds, and CCObject
				in debug builds.  You then need to use the CC_DECLARE_MEMDUMP and
				CC_IMPLEMENT_MEMDUMP macros in your class declaration and definition.
				These macros are similar to the CC_DECLARE_DYNAMIC and CC_IMPLEMENT_DYNAMIC
				macros described above, and the parameters are the same.  Doing all this gives
				you a class that uses the correct new operator in retail builds, but which
				also provides runtime-classing (and hence more helpful memory diagnostics) in
				debug builds.
				
				\b Dynamic Construction \b0
				
				You may also want to dynamically create objects at run-time, when the type
				of the object you want to create is unknown at compile-time.  An example of
				this is to copy an object.  There are another set of macros that provide all
				the functionality of the CC_..._DYNAMIC macros, but which also provide
				dynamic object creation.  The macros are:
				MonoOn
				CC_DECLARE_DYNCREATE
				CC_IMPLEMENT_DYNCREATE
				MonoOff
				The use and parameters of these macros are the same as for the DYNAMIC macros
				described above.
				
				To copy an object derived from CCObject you need to get the CCRuntimeClass
				object associated with the class, and then call a function of that object
				to create a new one.  For example:
				
				MonoOn
				// Some arbitrary object
				CCObject *pObject = (CCObject *) SomeObject;
				
				// Get the runtime class info on this object
				CCRuntimeClass *pCCRuntimeClass = pObject->GetRuntimeClass();
				
				// Create another object of the same type
				CCObject *pNewObject = pCCRuntimeClass->CreateObject();
				
				// It really is the correct type of object...
				ASSERT(pNewObject(IsKindOf(pCCRuntimeClass)));  // This will succeed
				MonoOff


	Errors:		-
	SeeAlso:	SimpleCCObject; CCRuntimeClass;
				CC_DECLARE_DYNAMIC; CC_IMPLEMENT_DYNAMIC;
				CC_DECLARE_DYNCREATE; CC_IMPLEMENT_DYNCREATE;
				CC_DECLARE_MEMDUMP; CC_IMPLEMENT_MEMDUMP; CC_CLASS_MEMDUMP;
				CC_RUNTIME_CLASS; GetRuntimeClass; IsKindOf; CreateObject;
				AssertValid; Dump

**********************************************************************************************/

class CCAPI CCObject : public SimpleCCObject
{
public:

	virtual CCRuntimeClass* GetRuntimeClass() const;
	virtual ~CCObject();
	static CCRuntimeClass* GetRuntimeClassByName(LPCTSTR pClassName);

protected:
	CCObject();

private:
	CCObject(const CCObject& objectSrc);              
	void operator=(const CCObject& objectSrc);       

public:
	BOOL IsKindOf(const CCRuntimeClass* pClass) const;
	
// Overridables
//	virtual void Serialize(CArchive& ar) {};

	// Diagnostic Support
	virtual void AssertValid() const;
PORTNOTE("other","Removed MFC CDumpContext")
#ifndef EXCLUDE_FROM_XARALX
	virtual void Dump( CDumpContext &dc ) const;
#endif

public:
	static CCRuntimeClass cc_classCCObject;
	static CCRuntimeClass* GetMyClass();
};


/********************************************************************************************

>	class AutoDeleteCCObject

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/05
	Purpose:	Simplifies cleanup of temporary CCObject derived objects
	SeeAlso:	-

********************************************************************************************/

class AutoDeleteCCObject
{
public:
	AutoDeleteCCObject(CCObject* pObj) : m_pObj(pObj) {}
	~AutoDeleteCCObject()
	{
		if (m_pObj)
			delete m_pObj;
	}
	void Attach(CCObject* pObj)
	{
		m_pObj = pObj;
	}
	void Detach()
	{
		m_pObj = NULL;
	}

protected:
	CCObject* m_pObj;
};


/********************************************************************************************

>	Macro IS_SAME_CLASS(pObj1, pObj2)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		pObj1, pObj2 - Pointers to the objects to compare.
	Returns:	TRUE if both objects are EXACTLY the same class type;
				FALSE if not.
	Purpose:	This macro determines whether or not two CCObject-derived objects have the
				same runtime class identifier, i.e. they are of the same class.  This is
				a strict check, unlike IsKindOf(), which follows the inheritance chain.

				e.g.
				MonoOn
					if (IS_SAME_CLASS(pNode, pOtherNode))
					{
						...
					}
				MonoOff

	Errors:		If used on a class that does not support runtime classing, it will cause
				a compile-time error.
	SeeAlso:	CCObject; CCRuntimeClass; CreateObject; IsKindOf

********************************************************************************************/

#define IS_SAME_CLASS(pObj1, pObj2) \
(((pObj1)->GetRuntimeClass()) == ((pObj2)->GetRuntimeClass()))

/********************************************************************************************

>	Macro IS_A(pObj, ClassName)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		pObj - Pointer to the object to test (must be CCObject derived, or a
					   CCRuntime class object).
				ClassName - Name of the class to test for
	Returns:	TRUE if pObj is the same class as, or derived from, the class specified
				by ClassName.
	Purpose:	This macro is a convenient shorthand for checking to see if a CCObject-
				derived object is of a particular class.  This is a strict check, unlike 
				IsKindOf(), which follows the inheritance chain.

				e.g.
				MonoOn
					if (IS_A(pNode, NodeRenderable))
					{
						...
					}
				MonoOff

	Errors:		If used on a class that does not support runtime classing, it will cause
				a compile-time error.
	SeeAlso:	CCObject; CCRuntimeClass; CreateObject; IsKindOf

********************************************************************************************/

#define IS_A(pObj, ClassName) \
(((pObj)->GetRuntimeClass()) == (CC_RUNTIME_CLASS(ClassName)))

/********************************************************************************************

>	Macro IS_KIND_OF(ClassName)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		ClassName - Name of the class to test for
	Returns:	TRUE if pObj is the same class as, or derived from, the class specified
				by ClassName.
	Purpose:	This macro is a convenient shorthand for the IsKindOf() function.

				e.g.
				MonoOn
					if (pNode->IS_KIND_OF(NodeRenderable))
					{
						...
					}
				MonoOff

	Errors:		If used on a class that does not support runtime classing, it will cause
				a compile-time error.
	SeeAlso:	CCObject; CCRuntimeClass; CreateObject; IsKindOf

********************************************************************************************/

#define IS_KIND_OF(ClassName) IsKindOf(CC_RUNTIME_CLASS(ClassName))

/********************************************************************************************

>	Macro CC_RUNTIME_CLASS(class_name)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		Name of the class
	Outputs:	-
	Returns:	Pointer to a CCRuntimeClass object
	Purpose:	This macro returns a pointer to the CCRuntimeClass object associated with
				the named class.  This structure can be used for determining if an object
				is of a particular type, or for dynamically creating a new object of this
				type.  To use such features, the class must have been declared as
				supporting them with CC_DECLARE_DYNAMIC and so on.
	Errors:		If used on a class that does not support runtime classing, it will cause
				a compile-time error.
	SeeAlso:	CCObject; CCRuntimeClass; CreateObject; IsKindOf

********************************************************************************************/

#if NEW_RUNTIME_INFO
// DLL version has to call (static) function whose name  gets fixed up
#define	CC_RUNTIME_CLASS(class_name) ( class_name::GetMyClass() )
#else
// normal definition
#define CC_RUNTIME_CLASS(class_name) (&class_name::cc_class##class_name)
#endif

/********************************************************************************************

>	Macro CC_DECLARE_DYNAMIC(class_name)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		Name of the class being declared as dynamic.
	Outputs:	-
	Returns:	-
	Purpose:	Adds declarations to a class which has been derived from CCObject which
				enable it to use runtime classing facilities.

				NB. Do not have a trailing semi-colon after this macro.

	Errors:		-
	SeeAlso:	CC_IMPLEMENT_DYNAMIC; CC_RUNTIME_CLASS; CCObject; CCRuntimeClass

********************************************************************************************/

#define CC_DECLARE_DYNAMIC(class_name) \
public: \
	static CCRuntimeClass cc_class##class_name; \
	static CCRuntimeClass* GetMyClass(); \
	virtual CCRuntimeClass* GetRuntimeClass() const;

/********************************************************************************************

>	Macro CC_DECLARE_DYNCREATE(class_name)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Adds declarations to a class which has been derived from CCObject which
				enable it to use runtime classing and dynamic object construction facilities.

				NB. Do not have a trailing semi-colon after this macro.

	Errors:		-
	SeeAlso:	CC_IMPLEMENT_DYNCREATE; CC_RUNTIME_CLASS; CCObject; CCRuntimeClass

********************************************************************************************/

#define CC_DECLARE_DYNCREATE(class_name) \
	CC_DECLARE_DYNAMIC(class_name) \
	static void PASCAL Construct(void* p);


/********************************************************************************************

>	class CC_CLASSINIT

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Purpose:	This type is important because it is used to build up the runtime class 
				information used by CCObject and CCRuntimeClass.
				What happens is that any class declared as dynamic has a static instance of
				an object of this type.  This static object is declared by the 
				CC_IMPLEMENT_{DYNAMIC,DYNCREATE} macro.  Declaring this object as static
				means that its constructor is called at program startup (very early on in 
				the initialisation.  The constructor is the important part - the object is
				declared passing in a pointer to the runtime class object (CCRuntimeClass)
				associated with the class in question.  The CC_CLASSINIT then adds this 
				object to the list of classes maintained and used by the CCRuntimeClass
				class.

	SeeAlso:	CCObject; CCRuntimeClass

********************************************************************************************/

class CCAPI CC_CLASSINIT
{
public:
	// Constructor
	CC_CLASSINIT(CCRuntimeClass* pNewClass);
};



// Implementation of CC_IMPLEMENT_{DYNAMIC,DYNCERATE}

#define _CC_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew) \
	/* Provide textual name of class */ \
	static TCHAR BASED_CODE _lpsz##class_name[] = wxT( #class_name ); \
	/* Declare runtime class information for this class */ \
	CCRuntimeClass class_name::cc_class##class_name = { \
		_lpsz##class_name, sizeof(class_name), wSchema, pfnNew, \
			CC_RUNTIME_CLASS(base_class_name), NULL }; \
	/* Declare the CC_CLASSINIT structure so that the class info is linked in */ \
	static CC_CLASSINIT _init_##class_name(&class_name::cc_class##class_name); \
	/* Implement GetRuntimeClass() */ \
	CCRuntimeClass* class_name::GetRuntimeClass() const \
		{ return &class_name::cc_class##class_name; } \
	/* Implement GetMyClass() (a static function) */ \
	CCRuntimeClass* class_name::GetMyClass() \
		{ return &class_name::cc_class##class_name; } \
// end of _CC_IMPLEMENT_RUNTIMECLASS


/********************************************************************************************

>	CC_IMPLEMENT_DYNAMIC(class_name, base_class_name)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		class_name - Name of the class being declared as dynamic.
				base_class_name - Immediate base class of class 'class_name'.
	Outputs:	-
	Returns:	-
	Purpose:	This macro provides the definitions required to make a class dynamic, i.e.
				make it possible to determine its type at runtime.  Note that this macro
				should be used in the .cpp file, and will only work if the class
				declaration (.h file) also uses the CC_DECLARE_DYNAMIC macro.
				
				The base class name is used so that the correct result is returned for
				derived classes, e.g.
				MonoOn
				
				// pObject is really a NodeRenderableInk object
				CCObject *pObject = SomeObject;
				
				// This succeeds because NodeRenderableInk is also a Node because
				// NodeRenderableInk is (ultimately) derived from Node
				ASSERT(pObject->IsKindOf(CC_RUNTIME_CLASS(Node)));
				
				MonoOff
				
				NB. Do not have a trailing semi-colon after this macro.
				
	Errors:		-
	SeeAlso:	CC_DECLARE_DYNAMIC; CCObject; CCRuntimeClass; 
				CC_RUNTIME_CLASS; GetRuntimeClass; IsKindOf

********************************************************************************************/

#define CC_IMPLEMENT_DYNAMIC(class_name, base_class_name) \
	_CC_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, NULL)


/********************************************************************************************

>	CC_IMPLEMENT_DYNCREATE(class_name, base_class_name)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		class_name - Name of the class being declared as dynamic.
				base_class_name - Immediate base class of class 'class_name'.
	Outputs:	-
	Returns:	-
	Purpose:	This macro provides the definitions required to make a class dynamic, i.e.
				make it possible to determine its type at runtime, and to make it possible
				to create an object dynamically without knowing its type at runtime (see
				CCObject for more details).
				Note that this macro should be used in the .cpp file, and will only work if
				the class declaration (.h file) also uses the CC_DECLARE_DYNCREATE macro.

				The base class name is used so that the correct result is returned for
				derived classes, e.g.
				MonoOn
				
				// pObject is really a NodeRenderableInk object
				CCObject *pObject = SomeObject;
				
				// This succeeds because NodeRenderableInk is also a Node because
				// NodeRenderableInk is (ultimately) derived from Node
				ASSERT(pObject->IsKindOf(CC_RUNTIME_CLASS(Node)));
				
				MonoOff
				
				NB. Do not have a trailing semi-colon after this macro.

	Errors:		-
	SeeAlso:	CC_DECLARE_DYNCREATE; CCObject; CCRuntimeClass; 
				CC_RUNTIME_CLASS; GetRuntimeClass; IsKindOf; CreateObject

********************************************************************************************/

#define CC_IMPLEMENT_DYNCREATE(class_name, base_class_name) \
	/* Implement the Construct() function */ \
	void PASCAL class_name::Construct(void* p) \
		{ new(p) class_name; } \
	_CC_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, \
		class_name::Construct)
// end of CC_IMPLEMENT_DYNCREATE


/********************************************************************************************

<	CC_CLASS_MEMDUMP
<	Runtime Classing

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Comment:	This is a macro that is used when runtime class information is desirable in 
				debug builds, but not in retail builds.  Use as follows in your class 
				declaration:
				
				MonoOn
				class MyClass : public CC_CLASS_MEMDUMP
				{
					...
				};
				MonoOff
				
				This macro evaluates to CCObject in debug builds, i.e. it provides
				dynamic class information, and in retail builds it evaluates to
				SimpleCCObject, i.e. it overrides new and delete, but you don't get the
				dynamic class information, or the overhead of a virtual function table.
				
				It should be used in conjunction with CC_DECLARE_MEMDUMP and
				CC_IMPLEMENT_MEMDUMP.
				
				Using this facility makes memory diagnostics much more detailed, and
				so it is easier to tell which objects are not being cleaned up.
				
	SeeAlso:	CC_DECLARE_MEMDUMP; CC_IMPLEMENT_MEMDUMP; CCObject; SimpleCCObject

********************************************************************************************/

#ifdef _DEBUG
#define CC_CLASS_MEMDUMP CCObject
#else
#define CC_CLASS_MEMDUMP SimpleCCObject
#endif

/********************************************************************************************

>	Macro CC_DECLARE_MEMDUMP(class_name)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		class_name - Name of the class being declared as dynamic.
	Outputs:	-
	Returns:	-
	Purpose:	Used when runtime class information is desirable in debug builds, but
				not in retail builds.  Use as follows in your class declaration:
				
				MonoOn
				class MyClass : public CCObject
				{
					CC_DECLARE_MEMDUMP(MyClass)
					...
				};
				MonoOff
				
				This macro evaluates to CC_DECLARE_DYNAMIC in debug builds, i.e. it provides
				dynamic class information, and in retail builds it evaluates to nothing,
				i.e. the runtime classing disappears.
				
				It should be used in conjunction with CC_IMPLEMENT_MEMDUMP (and 
				CC_CLASS_MEMDUMP if your class is normally derived from SimpleCCObject).
				
				Using this facility makes memory diagnostics much more detailed, and
				so it is easier to tell which objects are not being cleaned up.

				NB. Do not have a trailing semi-colon after this macro.

	Errors:		-
	SeeAlso:	CC_CLASS_MEMDUMP; CC_IMPLEMENT_MEMDUMP; CCObject; SimpleCCObject

********************************************************************************************/

#ifdef _DEBUG
#define CC_DECLARE_MEMDUMP(class_name) CC_DECLARE_DYNAMIC(class_name)
#else
#define CC_DECLARE_MEMDUMP(class_name)
#endif

/********************************************************************************************

>	Macro CC_IMPLEMENT_MEMDUMP(class_name, base_class_name)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		class_name - Name of the class being declared as dynamic.
				base_class_name - Immediate base class of class 'class_name'.
	Outputs:	-
	Returns:	-
	Purpose:	Used when runtime class information is desirable in debug builds, but
				not in retail builds.  Use as follows in your class definition (.cpp file):
				
				MonoOn
				CC_IMPLEMENT_MEMDUMP(MyClass, MyBaseClass)
				MonoOff
				
				This macro evaluates to CC_IMPLEMENT_DYNAMIC in debug builds, i.e. it 
				provides dynamic class information, and in retail builds it evaluates to 
				nothing, i.e. the runtime classing disappears.
				
				It should be used in conjunction with CC_DECLARE_MEMDUMP (and 
				CC_CLASS_MEMDUMP if your class is normally derived from SimpleCCObject).
				
				Using this facility makes memory diagnostics much more detailed, and
				so it is easier to tell which objects are not being cleaned up.

				NB. Do not have a trailing semi-colon after this macro.

	Errors:		-
	SeeAlso:	CC_CLASS_MEMDUMP; CC_DECLARE_MEMDUMP; CCObject; SimpleCCObject

********************************************************************************************/

#ifdef _DEBUG
#define CC_IMPLEMENT_MEMDUMP(class_name, base_class_name) \
CC_IMPLEMENT_DYNAMIC(class_name, base_class_name)
#else
#define CC_IMPLEMENT_MEMDUMP(class_name, base_class_name)
#endif



extern "C" void CCAPI CCAssertValidObject(const CCObject* pOb, LPCSTR FileName, INT32 Line);


/********************************************************************************************

>	CC_ASSERT_VALID(CCObject *pObject)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function is used to check the validity of an object - it checks for
				things like NULL pointers, bad segment selectors, incorrect vtable
				pointers etc - see CCAssertValidObject for more details.
	Errors:		-
	SeeAlso:	CCAssertValidObject

********************************************************************************************/

#ifdef _DEBUG
#define CC_ASSERT_VALID(pOb)  (::CCAssertValidObject(pOb, THIS_FILE, __LINE__))
#else
#define CC_ASSERT_VALID(pOb)  ((void)0)
#endif

#endif  // INC_CCOBJECT
