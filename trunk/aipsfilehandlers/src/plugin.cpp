/************************************************************************
 * File: plugin.cpp                                                     *
 * Project: AIPS basic filters plugin library                           *
 * Description: Library functions to access the plugin modules          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Beta                                                        *
 * Created: 27.08.03                                                    *
 * Changed: 29.01.04 Documented and clarified code                      *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "libid.h"
#include <cstdio>
#include <map>
#include <boost/shared_ptr.hpp>
#include "aipsfilehandlers_config.h"
#include "canalyzehandler.h"
#include "cdatahandler.h"
#include "csimpledathandler.h"
#include "cdf3handler.h"
#include "ccnthandler.h"
#include "cucfhandler.h"
/*#include "cadfhandler.h"*/
#include "cvinterfilehandler.h"
#ifdef HAVE_VTK
#include "cvtkhandler.h"
#endif
#ifdef HAVE_ITK
#include "citkhandler.h"
#endif
#ifdef HAVE_IM
#include "ccommonimagehandler.h"
#endif

using namespace std;
using namespace boost;

vector<string> classNames;
map<string,shared_ptr<CFileHandler> > factoryMap;

extern "C" {

/** \return library documentation string */
std::string libraryDoc()
{
	return std::string("File handlers for data, dat, analyze 7.5 and common 2d image formats");
}

/** \return the library version */
std::string libraryID()
{
	return sLibID;
}

void initFactory()
{
	classNames.clear();
	factoryMap.clear();
	shared_ptr<CFileHandler> workPtr;
#ifdef HAVE_ITK	
	workPtr.reset( new CITKHandler() );
	factoryMap["CITKHandler"] = workPtr;
	classNames.push_back("CITKHandler");
#endif	
	workPtr.reset( new CDataHandler() );
	factoryMap["CDataHandler"] = workPtr;
	classNames.push_back("CDataHandler");
	workPtr.reset( new CSimpleDatHandler() );
	factoryMap["CSimpleDatHandler"] = workPtr;
	classNames.push_back("CSimpleDatHandler");
	workPtr.reset( new CAnalyzeHandler() );
	factoryMap["CAnalyzeHandler"] = workPtr;
	classNames.push_back("CAnalyzeHandler");
	workPtr.reset( new CDF3Handler() );
	factoryMap["CDF3Handler"] = workPtr;
	classNames.push_back("CDF3Handler");
	workPtr.reset( new CVInterfileHandler() );
	factoryMap["CVInterfileHandler"] = workPtr;
	classNames.push_back("CVInterfileHandler");
	workPtr.reset( new CCntHandler() );
	factoryMap["CCntHandler"] = workPtr;
	classNames.push_back("CCntHandler");
	workPtr.reset( new CUcfHandler() );
	factoryMap["CUcfHandler"] = workPtr;
	classNames.push_back("CUcfHandler");
#ifdef HAVE_VTK
	workPtr.reset( new CVtkHandler() );
	factoryMap["CVtkHandler"] = workPtr;
	classNames.push_back("CVtkHandler");
#else
#warning No VTK support present
#endif	
#ifdef HAVE_IM
	workPtr.reset( new CCommonImageHandler() );
	factoryMap["CCommonImageHandler"] = workPtr;
	classNames.push_back("CCommonImageHandler");
#else
#warning No ImageMagick support present
#endif	
}

void unloadFactory()
{
	factoryMap.clear();
	classNames.clear();
}

/** \return the number of modules in the library */
uint numberOfModules()
{
	cerr << "<DBG: Class names: " << endl;
	for( vector<string>::iterator it = classNames.begin(); it != classNames.end(); ++it )
	cerr << "     " << *it << endl;
	cerr << ">" << endl;
  return classNames.size();
}

/**
 * Create and retrieve a new module
 * \param uiID number of module
 * \return new module instance
 */
shared_ptr<CFileHandler> createHandlerByNumber( const uint uiID )
{
	if ( uiID < classNames.size() )
	{		
		return (factoryMap[ classNames[ uiID ] ]);
	}
	else
	{
		shared_ptr<CFileHandler> dummy; dummy.reset();
		return dummy;
	}
}

shared_ptr<CFileHandler> createHandlerByString( const string& sID )
{
	if ( factoryMap.find( sID ) != factoryMap.end() )
		return (factoryMap[ sID ]);
	else
	{
		shared_ptr<CFileHandler> dummy; dummy.reset();
		return dummy;
	}
}

}
