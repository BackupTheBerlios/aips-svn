/************************************************************************
 * File: plugin.cpp                                                     *
 * Project: AIPS basic sources plugin library                           *
 * Description: Library functions to access the plugin modules          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Beta                                                        *
 * Created: 27.08.03                                                    *
 * Changed: 28.01.04 Documented and clarified code                      *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include <map>
#include <boost/shared_ptr.hpp>

#include "libid.h"
#include "cswapaxes.h"
#include "cmultiply.h"
#include "cinvert.h"
#include "cgaussiannoise.h"
#include "cextractintensity.h"
#include "cdivideintensities.h"
#include "cdatamirror.h"
#include "ccropimage.h"
#include "ccorrelation.h"
#include "ccombineintensities.h"
#include "caddsubtract.h"
#include "caddborder.h"
#include "cimagescaler.h"
#include "cmaximum.h"
#include "cnoiseestimator.h"

using namespace std;
using namespace boost;

vector<string> classNames;
map<string,shared_ptr<CPipelineItem> > factoryMap;

extern "C" {

/** \return library documentation string */
std::string libraryDoc()
{
	return std::string("Filters which operate on whole images");
}

/** \return the library version */
std::string libraryID()
{
	return sLibID;
}

void initFactory()
{
	shared_ptr<CPipelineItem> workPtr;
	workPtr.reset( new CSwapAxes( 0 ) );
	factoryMap["CSwapAxes"] = workPtr;
	classNames.push_back("CSwapAxes");	
	workPtr.reset( new CMultiply( 0 ) );
	factoryMap["CMultiply"] = workPtr;
	classNames.push_back("CMultiply");	
	workPtr.reset( new CInvert( 0 ) );
	factoryMap["CInvert"] = workPtr;
	classNames.push_back("CInvert");	
	workPtr.reset( new CGaussianNoise( 0 ) );
	factoryMap["CGaussianNoise"] = workPtr;
	classNames.push_back("CGaussianNoise");	
	workPtr.reset( new CExtractIntensity( 0 ) );
	factoryMap["CExtractIntensity"] = workPtr;
	classNames.push_back("CExtractIntensity");	
	workPtr.reset( new CDivideIntensities( 0 ) );
	factoryMap["CDivideIntensities"] = workPtr;
	classNames.push_back("CDivideIntensities");	
	workPtr.reset( new CDataMirror( 0 ) );
	factoryMap["CDataMirror"] = workPtr;
	classNames.push_back("CDataMirror");	
	workPtr.reset( new CCropImage( 0 ) );
	factoryMap["CCropImage"] = workPtr;
	classNames.push_back("CCropImage");	
	workPtr.reset( new CCorrelation( 0 ) );
	factoryMap["CCorrelation"] = workPtr;
	classNames.push_back("CCorrelation");	
	workPtr.reset( new CCombineIntensities( 0 ) );
	factoryMap["CCombineIntensities"] = workPtr;
	classNames.push_back("CCombineIntensities");	
	workPtr.reset( new CAddSubtract( 0 ) );
	factoryMap["CAddSubtract"] = workPtr;
	classNames.push_back("CAddSubtract");	
	workPtr.reset( new CAddBorder( 0 ) );
	factoryMap["CAddBorder"] = workPtr;
	classNames.push_back("CAddBorder");		
	workPtr.reset( new CImageScaler( 0 ) );
	factoryMap["CImageScaler"] = workPtr;
	classNames.push_back("CImageScaler");		
	workPtr.reset( new CMaximum( 0 ) );
	factoryMap["CMaximum"] = workPtr;
	classNames.push_back("CMaximum");		
	workPtr.reset( new CNoiseEstimator( 0 ) );
	factoryMap["CNoiseEstimator"] = workPtr;
	classNames.push_back("CNoiseEstimator");
}

void unloadFactory()
{
	factoryMap.clear();
	classNames.clear();
}

/** \return the number of modules in the library */
uint numberOfModules()
{
  return classNames.size();
}

/**
 * Create and retrieve a new module
 * \param uiID number of module
 * \return new module instance
 */
shared_ptr<CPipelineItem> createModuleByNumber( const uint uiID )
{
	if ( uiID < classNames.size() )
	{		
		return (factoryMap[ classNames[ uiID ] ]);
	}
	else
	{
		shared_ptr<CPipelineItem> dummy; dummy.reset();
		return dummy;
	}
}

shared_ptr<CPipelineItem> createModuleByString( const string& sID )
{
	if ( factoryMap.find( sID ) != factoryMap.end() )
		return (factoryMap[ sID ]);
	else
	{
		shared_ptr<CPipelineItem> dummy; dummy.reset();
		return dummy;
	}
}

}
