/************************************************************************
 * File: cxmlinterpreter.cpp                                            *
 * Project: AIPS                                                        *
 * Description: Simple interpreter for xml files                        *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status:  Alpha                                                       *
 * Created: 2004-04-20                                                  *
 * Changed: 2004-06-22 Corrected an error on reading escaped strings    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CXMLINTERPRETER_H
#define CXMLINTERPRETER_H

#include <vector>
#include <cexception.h>
#include <aipsmacros.h>
#include <fstream>
#include <clog.h>

using namespace aips;

class CXMLInterpreter : public CBase
{
public:
	CXMLInterpreter( std::string sFilename ) throw( FileException );
	~CXMLInterpreter() throw();
  std::string getContent();
  std::string getTag();
  std::string getParam( ushort usNumber );
  bool isOnlyOpen();
  bool isOnlyClose();
  void nextTag()
  {
  	tagIt++;
  }	
private:
	struct XMLTag 
	{
		std::string sId;
		std::string sContent;
		std::vector<std::pair<std::string,std::string> > sParamVec;
		ushort usComplete;
	};
	std::vector<XMLTag> tags;	
	std::vector<XMLTag>::iterator tagIt;
private:
  void parseLine( std::string sLine ) throw();
	std::vector<std::string> split( std::string merged, std::string sep = "/" );
};

#endif
