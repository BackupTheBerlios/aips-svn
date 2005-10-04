//
// C++ Implementation: cvtkadapter
//
// Description: 
//
//
// Author: Hendrik Belitz <hendrik@darkon.info>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cvtkadapter.h"

#ifdef USE_VTK

#ifndef vtkFloatingPointType
#define VTK_OLD
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif 

namespace aips {

CVTKAdapter::CVTKAdapter( TDataSetPtr internalDataSPtr_ ) throw()
  : CStructuredDataAdapter( internalDataSPtr_, "CVTKAdapter", CVTKADAPTER_VERSION, "CStructuredDataAdapter" ),
    externalDataPtr( NULL )
{
}


CVTKAdapter::CVTKAdapter() throw()
  : CStructuredDataAdapter( "CVTKAdapter", CVTKADAPTER_VERSION, "CStructuredDataAdapter" ) 
{
}


CVTKAdapter::~CVTKAdapter() throw()
{
}

void CVTKAdapter::setExternalData( vtkStructuredPoints* aVTKImage ) throw( NullException )
{
  if ( !aVTKImage )
    throw( NullException("No image given") );
  externalDataPtr = aVTKImage;  
}
    
TDataSetPtr CVTKAdapter::convertToInternal()
{
  TDataSetPtr aDataSet;
  if ( !externalDataPtr )
  {
    alog << LWARN << "No external dataset given. Converted nothing." << endl;
    return aDataSet;
  }
  
  int *iVtkVolumeDimensions = externalDataPtr->GetDimensions();
  std::vector<size_t> dimensionSize;
  dimensionSize.push_back( static_cast<size_t>( iVtkVolumeDimensions[0] ) );
  dimensionSize.push_back( static_cast<size_t>( iVtkVolumeDimensions[1] ) );
  dimensionSize.push_back( static_cast<size_t>( iVtkVolumeDimensions[2] ) );

  // Get origin and spacing
  vtkFloatingPointType* origin = externalDataPtr->GetOrigin();
  vtkFloatingPointType* spacing = externalDataPtr->GetSpacing();

  vtkPointData *p= externalDataPtr->GetPointData();
    
  // Convert vtkDataArray to CDataSet
  if ( externalDataPtr->GetScalarType() == VTK_UNSIGNED_SHORT )
  {
    vtkShortArray* myArray = static_cast<vtkShortArray*>( p->GetScalars() );
    aDataSet = this->convertVTKImage<short, TImage, vtkShortArray>( myArray, dimensionSize );
  }
  else if ( externalDataPtr->GetScalarType() == VTK_FLOAT )
  {
    vtkFloatArray* myArray = static_cast<vtkFloatArray*>( p->GetScalars() );
    aDataSet = this->convertVTKImage<double, TField, vtkFloatArray>( myArray, dimensionSize );
  }
  else if ( externalDataPtr->GetScalarType() == VTK_DOUBLE )
  {
    vtkDoubleArray* myArray = static_cast<vtkDoubleArray*>( p->GetScalars() );
    aDataSet = this->convertVTKImage<double, TField, vtkDoubleArray>( myArray, dimensionSize );
  }
  if ( aDataSet )
  {
  /* FIXME Old VTK */
  #ifdef VTK_OLD
  #warning Origin not set since VTK is fairly old
  	cerr << "Origin not set since VTK is fairly old" << endl;
  #elseif
   	aDataSet->setBaseElementDimensions( spacing );
   	aDataSet->setOrigin( origin );
  #endif
  }
  // Clean up
  externalDataPtr->Delete();
  return aDataSet;
}

/**
 * \param aVtkArray VTK array to convert
 * \param dim size_t[3] array of resulting dataset dimensions
 * \returns the newly created dataset
 */
template<typename TVar, typename TSet, typename TArray> 
  boost::shared_ptr<TSet> CVTKAdapter::convertVTKImage( TArray* aVtkArray, std::vector<size_t> dim ) const
{
  boost::shared_ptr<TSet> aSet ( new TSet( 3, dim ) );
  vtkIdType max = aVtkArray->GetNumberOfTuples();
  typename TSet::iterator it = aSet->begin();
  for( vtkIdType id = 0; id < max; ++id, ++it )
  {
    *it = *( aVtkArray->GetPointer( id ) );
  }
  aSet->setMinimum( std::numeric_limits<TVar>::max() );
  aSet->setMaximum( 0 );
  it = aSet->begin(); 
  typename TSet::iterator endIt = aSet->end();
  while( it != endIt )
  {
    if ( *it > aSet->getMaximum() ) aSet->setMaximum( *it );
    if ( *it < aSet->getMinimum() ) aSet->setMinimum( *it );
    ++it;
  }
  return aSet;
}

vtkStructuredPoints* CVTKAdapter::convertToExternal() throw( NullException )
{
  if ( !internalDataSPtr )
    throw( NullException( "No pointer to internal data set" ) );
  // Determine dimension size
  size_t dimensionSize[3];
  dimensionSize[0] = internalDataSPtr->getExtent(0);
  dimensionSize[1] = internalDataSPtr->getExtent(1);
  if( internalDataSPtr->getDimension() > 2 )
    dimensionSize[2] = internalDataSPtr->getExtent(2);
  else
    dimensionSize[2] = 1;

  size_t siz = dimensionSize[0] * dimensionSize[1] * dimensionSize[2];
//cerr << dimensionSize[0] << " " << dimensionSize[1] << " " << dimensionSize[2] << endl;
  // Create vtk structured points structure
  vtkStructuredPoints* sp = vtkStructuredPoints::New();
  sp->SetDimensions( dimensionSize[0], dimensionSize[1], dimensionSize[2] );
  if ( internalDataSPtr->getDimension() == 2 )
  {
    sp->SetOrigin( internalDataSPtr->getOrigin(0), internalDataSPtr->getOrigin(1), 0.0 );
    sp->SetSpacing( internalDataSPtr->getBaseElementDimension(0),
      internalDataSPtr->getBaseElementDimension(1), 1.0 );
  }
  else
  {
    sp->SetOrigin( internalDataSPtr->getOrigin(0), internalDataSPtr->getOrigin(1), internalDataSPtr->getOrigin(2) );
    sp->SetSpacing( internalDataSPtr->getBaseElementDimension(0),
      internalDataSPtr->getBaseElementDimension(1),
      internalDataSPtr->getBaseElementDimension(2) );
  }
  sp->AllocateScalars();
  // Assign dataset to structured points
  vtkPointData* p=sp->GetPointData();
  vtkUnsignedShortArray* sArray = NULL;
  vtkDoubleArray* fArray = NULL;
  if ( checkType<TField>( internalDataSPtr ) )
  {
    TFieldPtr floatSet = boost::static_pointer_cast<TField>( internalDataSPtr );
    fArray = vtkDoubleArray::New();
    fArray->SetArray( floatSet->getArray(), siz, 1 );
    p->SetScalars( fArray );
  }
  else if ( checkType<TImage>( internalDataSPtr ) )
  {
    TImagePtr shortSet = boost::static_pointer_cast<TImage>( internalDataSPtr );
    TImage::iterator it = shortSet->begin();
    ushort* array = new ushort[siz];
    ushort* ot = array;
    while( it != shortSet->end() )
    {
      *ot = static_cast<ushort>( *it );
      ++it; ++ot;
    }
    sArray = vtkUnsignedShortArray::New();
    sArray->SetArray( array, siz, 1 );
    p->SetScalars( sArray );
    delete array;
  }
  else
  {
    sp->Delete();
    alog << LWARN << "Cannot convert given datatype" << endl;
    return NULL;
  }
  
  return sp;
}

}

#endif
