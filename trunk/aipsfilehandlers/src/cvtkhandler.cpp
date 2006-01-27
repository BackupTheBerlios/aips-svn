/************************************************************************
 * File: cvtkhandler.cpp                                               *
 * Project: AIPS - Basic file handlers plugin                           *
 * Description: A file header for the vtk image file format             *
 *                                                                      *
 * Author: Patrick Musmann (p.musmann@fz-juelich.de)                    *
 *         Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-05-21 (PM)                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#include "cvtkhandler.h"

#ifdef HAVE_VTK
#include "vtkUnsignedShortArray.h"
#include <cvtkadapter.h>
using namespace aips;
using namespace boost;
using namespace std;

/* VTK 4.4 defines vtkFloatingPointyType == double. VTK 4.2 uses float as
   a standard floating point type, so we need to define vtkFloatingPointyType
   == float. By this the code should get compiled with any VTK 4.x version */
#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif 

CVtkHandler::CVtkHandler() throw()
  : CBinaryFileHandler( "CVtkHandler", "0.4", "CBinaryFileHandler" )
{
  supportedFileTypesVec.push_back("VTK");
  supportedFileTypesVec.push_back("vtk");
}

CVtkHandler::~CVtkHandler() throw()
{
}

/**
 * \param aVtkArray VTK array to convert
 * \param dim size_t[3] array of resulting dataset dimensions
 * \returns the newly created dataset
 */
template<typename TVar, typename TSet, typename TArray> 
	boost::shared_ptr<TSet> CVtkHandler::convertVTKImage( TArray* aVtkArray, std::vector<size_t> dim ) const
{
	boost::shared_ptr<TSet> aSet ( new TSet( 3, dim ) );
	vtkIdType max = aVtkArray->GetNumberOfTuples();
	typename TSet::iterator it = aSet->begin();
	for( vtkIdType id = 0; id < max; ++id, ++it )
	{
		*it = *( aVtkArray->GetPointer( id ) );
		aSet->adjustDataRange( *it );		
	}
	return aSet;
}

/**
 * \param sFilename Name of the volume file
 * \exception FileException on any file error
 */
TDataFile CVtkHandler::load( const string& sFilename )
  const throw( FileException )
{
FBEGIN;
		// Initialize reader and load image
    shared_ptr<CImageHeader> aHeader( new CImageHeader );
		
    vtkStructuredPointsReader *spr = vtkStructuredPointsReader::New();
    spr->SetFileName( sFilename.c_str() );
		spr->Update();
    vtkStructuredPoints *sp = spr->GetOutput();		
		int *iVtkVolumeDimensions = sp->GetDimensions();
		std::vector<size_t> dimensionSize;
		dimensionSize.push_back( static_cast<size_t>( iVtkVolumeDimensions[0] ) );
		dimensionSize.push_back( static_cast<size_t>( iVtkVolumeDimensions[1] ) );
		dimensionSize.push_back( static_cast<size_t>( iVtkVolumeDimensions[2] ) );
cerr << "A";
		// Fill the Header
		aHeader->setExtents( dimensionSize );
		vtkFloatingPointType* origin = sp->GetOrigin();
		aHeader->setOrigin( origin[0], origin[1], origin[2] );
		vtkFloatingPointType* spacing = sp->GetSpacing();
		aHeader->setVoxelDimensions( spacing[0], spacing[1], spacing[2] );
		aHeader->setEndianess( false );
cerr << "B";		
		vtkPointData *p= sp->GetPointData();
cerr << "C";		
/*		TDataSetPtr aDataSet;
		// Convert vtkDataArray to CDataSet
		if ( sp->GetScalarType() == VTK_UNSIGNED_SHORT )
		{			
			vtkShortArray* myArray = static_cast<vtkShortArray*>( p->GetScalars() );
			aDataSet = this->convertVTKImage<short, TImage, vtkShortArray>( myArray, dimensionSize );
			aHeader->setVoxelType( "Int16" );
		}
		else if ( sp->GetScalarType() == VTK_FLOAT )
		{
			vtkFloatArray* myArray = static_cast<vtkFloatArray*>( p->GetScalars() );
			aDataSet = this->convertVTKImage<double, TField, vtkFloatArray>( myArray, dimensionSize );
			aHeader->setVoxelType( "Float16" );
		}		
		else if ( sp->GetScalarType() == VTK_DOUBLE )
		{
			vtkDoubleArray* myArray = static_cast<vtkDoubleArray*>( p->GetScalars() );
			aDataSet = this->convertVTKImage<double, TField, vtkDoubleArray>( myArray, dimensionSize );
			aHeader->setVoxelType( "Float32" );
		}		*/

    CVTKAdapter myAdapter;
    myAdapter.setExternalData( sp );
    TDataSetPtr aDataSet = myAdapter.convertToInternal();
cerr << "D";		
		// Clean up
    spr->Delete();
//DBG3( "refcnt " << sp->GetReferenceCount() );    
    //sp->Delete();
FEND;		
    return make_pair( aDataSet, aHeader );
}

/**
 * \param sFilename Name of the volume file
 * \param theData Pair of data set and header (will be ignored for this file type) to be saved
 * \exception FileException on any file error
 */
void CVtkHandler::save( const string& sFilename, const TDataFile& theData )
  const throw( FileException )
{
FBEGIN;
    // Determine dimension size
//     size_t dimensionSize[3];
//     dimensionSize[0] = theData.first->getExtent(0);
//     dimensionSize[1] = theData.first->getExtent(1);
//     if( theData.first->getDimension() > 2 )
//         dimensionSize[2] = theData.first->getExtent(2);
//     else
//         dimensionSize[2] = 1;

/*		size_t siz = dimensionSize[0] * dimensionSize[1] * dimensionSize[2];
cerr << dimensionSize[0] << " " << dimensionSize[1] << " " << dimensionSize[2] << endl;        		*/
    // Create vtk structured points structure
    CVTKAdapter myAdapter( theData.first );
    vtkImageData* sp = myAdapter.convertToExternal();
         
//     vtkStructuredPoints* sp = vtkStructuredPoints::New();
//     ushort* array = new ushort[siz];
//     sp->SetDimensions( dimensionSize[0], dimensionSize[1], dimensionSize[2] );
//     sp->AllocateScalars();
//     // Assign dataset to structured points
//     vtkPointData* p=sp->GetPointData();
// 		vtkUnsignedShortArray* sArray = NULL;
// 		vtkDoubleArray* fArray = NULL;
// 		CDataSet& inputData = *(theData.first);
// 		if ( checkType<TField>( inputData ) )
// 		{
// 			TFieldPtr floatSet = static_pointer_cast<TField>( theData.first );
// 			fArray = vtkDoubleArray::New();
// 			fArray->SetArray( floatSet->getArray(),
// 				dimensionSize[0] * dimensionSize[1] * dimensionSize[2], 1 );
// 			p->SetScalars( fArray );
// 		}
// 		else 
// 		{
// 			
// 			if ( checkType<TImage>( inputData ) )
// 			{
// 				TImagePtr shortSet = static_pointer_cast<TImage>( theData.first );				
// 				TImage::iterator it = shortSet->begin();
// 				ushort* ot = array;
// 				while( it != shortSet->end() )
// 				{					
// 					*ot = static_cast<ushort>( *it );
// 					++it; ++ot;
// 				}
// 				sArray = vtkUnsignedShortArray::New();
// 				sArray->SetArray( array,
// 					dimensionSize[0]*dimensionSize[1]*dimensionSize[2], 1 );
// 				p->SetScalars( sArray );
// 			}		
// 			else
// 			{
// 		    sp->Delete();		
// 		    delete array;
// 				throw( FileException( "CVTkHandler - Unknown image format in dataset. Image was not saved" ) );
// 			}
// 		}
//     
 		// Save generated vtkStructuredPoints structure
    vtkStructuredPointsWriter *spw = vtkStructuredPointsWriter::New();
    spw->SetFileName( sFilename.c_str() );
    spw->SetInput( sp );
    spw->SetFileTypeToBinary();    // spw->SetFileTypeToASCII();
    spw->Write();
		
	  // Clean up
    spw->Delete();
    sp->Delete();
// 		if ( sArray != NULL ) sArray->Delete();
// 		if ( fArray != NULL ) fArray->Delete();
// 		delete array;
FEND;		
}
#endif
