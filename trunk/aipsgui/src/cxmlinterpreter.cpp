/************************************************************************
 * File: cxmlinterpreter.cpp                                            *
 * Project: AIPS                                                        *
 * Description: Simple interpreter for xml files                        *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-04-20                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cxmlinterpreter.h"
#include <boost/algorithm/string.hpp>

using namespace std;

CXMLInterpreter::CXMLInterpreter( std::string sFilename ) throw( FileException ) 
	: CBase( "CXMLInterpreter", "0.2", "CBase" )
{
	ifstream theFile( sFilename.c_str() );
	if ( !theFile.is_open() )
		throw( FileException( "Could not open file" ) );
	while ( !theFile.eof() )
	{		
		char buffer[255];
		theFile.getline( buffer, 255 );
		string sLine ( buffer );
		if ( !sLine.empty() ) parseLine( sLine );
	}
	theFile.close();
	tagIt = tags.begin();
}


CXMLInterpreter::~CXMLInterpreter() throw()
{
}

/*!
    \fn CXMLInterpreter::getContent()
 */
std::string CXMLInterpreter::getContent()
{
	return tagIt->sContent;
}


/*!
    \fn CXMLInterpreter::getID()
 */
std::string CXMLInterpreter::getTag()
{
	return tagIt->sId;
}


/*!
    \fn CXMLInterpreter::getParam( ushort usNumber )
 */
std::string CXMLInterpreter::getParam( ushort usNumber )
{
	if ( usNumber < tagIt->sParamVec.size() )
		return tagIt->sParamVec[usNumber].second;
	return "";
}

/*!
    \fn CXMLInterpreter::isOnlyOpen()
 */
bool CXMLInterpreter::isOnlyOpen()
{
	return ( tagIt->usComplete == 1 );
}


/*!
    \fn CXMLInterpreter::isOnlyClose()
 */
bool CXMLInterpreter::isOnlyClose()
{
  return ( tagIt->usComplete == 2 );
}


/*!
    \fn CXMLInterpreter::parseLine( std::string sLine ) throw()
 */
void CXMLInterpreter::parseLine( std::string sLine ) throw()
{
	// Read first tag in Line
	string::size_type beg, end;
	beg = sLine.find( "<", 0 );
	end = sLine.find( ">", beg );		
	if ( beg == string::npos || end == string::npos )
		return;
	vector<string> tagVec;
	string subStr = sLine.substr( beg+1, end-beg-1 );
	boost::split( tagVec, subStr, boost::is_any_of(" ") );	
	XMLTag actTag;
	actTag.sId = tagVec[0];
DBG( "Found tag <" << tagVec[0] << ">" );	
	if ( actTag.sId[0] == '/' )
		actTag.usComplete = 2;
	else
	{
		// Read all parameters of tag
		if ( tagVec.size() > 1 )
		{
			for ( uint i = 1; i < tagVec.size(); ++i )
			{
				beg = tagVec[i].find( "=" );		
				string paramName = tagVec[i].substr( 0, beg );
				string paramValue = tagVec[i].substr( 
					beg + 2, ( tagVec[i].length() - (paramName.length() + 3) ) );
				actTag.sParamVec.push_back( make_pair( paramName, paramValue ) );
DS( " Found param <" << paramName << ">=<" << paramValue << ">"  << ( tagVec[i].length() -
(paramName.length() + 3 )));
			}
		}
		beg = end + 1;
		end = sLine.find( "<", beg );
		if ( end == string::npos )
		{
			actTag.usComplete = 1;
		}
		else
		{
			actTag.usComplete = 0;
			actTag.sContent = sLine.substr( beg, end - beg );
DS( " Content is <" << actTag.sContent << ">" );
		}
	}
	tags.push_back( actTag );
}
