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
#ifndef CIMAGEDATA_H
#define CIMAGEDATA_H

#include <cimageloader.h>
#include <aipsnumeric.h>
// #include <edgedetectors/csobelgradient.h>
// #include <edgedetectors/cgaussderivative.h>
// #include <basicconverters/cfieldtoimage.h>
// #include <basicconverters/creducevector.h>
// #include <fieldfilters/cvectorflow.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkLookupTable.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToColors.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkTexture.h>
#include <vtkProperty.h>

/**
@author Hendrik Belitz
*/
class CImageData : public vtkObjectBase {
public:
  CImageData();
  ~CImageData();
	vtkActor* getOutlines();
	vtkActor* getSlice( uint orientation );
	void setSlicePosition( uint orientation, uint position );
	void setColorTable( vtkLookupTable* lut );
	void loadDataSet( std::string sFilename );
	uint getExtent( uint orientation );
	TField3DPtr computeExternalForces();
	double getMinimum();
	double getMaximum();
private:
	vtkImageData *imageData;
	vtkActor* sliceActors[3];
	vtkActor* dataOutlinesActor;
	vtkStructuredGridOutlineFilter* dataOutlines;
	vtkImageReslice* slicers[3];
	vtkLookupTable* colorTable;
	vtkImageMapToColors* colorMappers[3];
	vtkPlaneSource* planeSources[3];
	vtkPolyDataMapper* planeMappers[3];
	vtkTexture* planeTextures[3];
	uint extent[3];	
	CImageLoader imageLoader;
	TImagePtr theImage;
	double scalarRange[2];
};

#endif
