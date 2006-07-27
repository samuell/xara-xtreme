/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE
============================XARAHEADERSTART================================
           Xara X/Xtreme, a vector drawing and manipulation program.  
                    Copyright (C) 2005 Xara Group Ltd.

This program is free software; you can redistribute it and/or modify it under 
the terms of the GNU General Public License as published by the Free Software 
Foundation; either version 2 of the License, or (at your option) any later 
version.

This program is distributed in the hope that it will be useful, but WITHOUT 
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
this program; if not, write to the Free Software Foundation, Inc., 51 
Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

Users wishing to use this library in proprietary products which are not 
themselves to be released under the GNU Public License should contact Xara for 
a license to do so. Such a license will normally be granted free of charge.

      Xara Group Ltd, Gaddesden Place, Hemel Hempstead, HP2 6EX, UK.
                        http://www.xara.com/ 

===========================XARAHEADEREND===================================*/

#ifndef INC_XARLIB
#define	INC_XARLIB

#include "camtypes.h"
#include "cxftags.h"
#include "cxfrec.h"

class CXaraFileRecord;
class CXaraFile;
class CCDiskFile;
class CCMemFile;

typedef BOOL (RecordHandler)(void*, CXaraFileRecord*);

class CXarImport
{
public:
	virtual ~CXarImport() {};
	virtual BOOL PrepareImport() = 0;
	virtual BOOL PrepareImport(TCHAR* pFileName) = 0;
#if defined(USE_COM_STREAM)
	virtual BOOL PrepareImport(IStream* pStream) = 0;
#endif	// defined(USE_COM_STREAM)
	virtual BOOL PrepareImport(BYTE* pBuffer, UINT32 Size) = 0;
	virtual BOOL SetHandler(void* pMagic, RecordHandler* pHandlerFunk) = 0;
	virtual BOOL DoImport() = 0;
};

class CXarExport
{
public:
	virtual ~CXarExport() {};
	virtual BOOL StartExport() = 0;
	virtual BOOL StartExport(TCHAR* pFileName) = 0;
#if defined(USE_COM_STREAM)
	virtual BOOL StartExport(IStream* pStream) = 0;
#endif	// defined(USE_COM_STREAM)
	virtual BOOL StartBufferedExport() = 0;
	virtual BOOL WriteRecord(CXaraFileRecord* pRecord, UINT32* pRecordNum = NULL) = 0;
	virtual BOOL WriteZeroSizedRecord(UINT32 Tag, UINT32* pRecordNum = NULL) = 0;
	virtual BOOL GetExportBuffer(BYTE** ppBuffer, UINT32* pSize) = 0;
};

namespace XarLibrary
{
	CXarImport* CreateImporter();
	CXarExport* CreateExporter();
}

#endif
