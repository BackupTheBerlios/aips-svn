/************************************************************************
 * File: cconverter.cpp                                                 *
 * Project: AIPS                                                        *
 * Description: Base class for all conversion modules                   *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 28.01.04                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cconverter.h"

using namespace aips;

/**
 * \param ulID unique module id in pipeline
 * \param usFanIn number of input ports
 * \param usFanOut number of output ports
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 */
CConverter::CConverter( ulong ulID, ushort usFanIn, ushort usFanOut, const std::string &sClassName_, 
		const std::string &sClassVersion_, const std::string &sDerivedFrom_ )
  throw() : CPipelineItem ( ulID, usFanIn, usFanOut, sClassName_, sClassVersion_, sDerivedFrom_ )
{
  setModuleName("Converter");
  setType( ITypeConverter );
}

CConverter::~CConverter() throw()
{
}
