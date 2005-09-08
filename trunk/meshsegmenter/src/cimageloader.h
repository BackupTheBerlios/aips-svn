/***************************************************************************
 *   Copyright (C) 2004 by Hendrik Belitz                                  *
 *   h.belitz@fz-juelich.de                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CIMAGELOADER_H
#define CIMAGELOADER_H

#include <string>
#include <aipsnumeric.h>
#include <vtkShortArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkImageData.h>


using namespace aips;
/**
@author Hendrik Belitz
*/
class CImageLoader 
{
public:
  CImageLoader();
  ~CImageLoader();
	vtkImageData* load( std::string sFilename, TImagePtr& imagePtr );
private:
	std::string sFilename;
};

#endif
