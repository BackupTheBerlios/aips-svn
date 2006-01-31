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
#warning "Old VTK detected. This will still work, although upgrading to VTK 4.4+ is strongly recommended
#define VTK_OLD
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#elseif
#undefine VTK_OLD
#endif 

using namespace std;
using namespace boost;

namespace aips {

CVTKAdapter::CVTKAdapter( TDataSetPtr internalDataSPtr_ ) throw()
  : CStructuredDataAdapter( internalDataSPtr_, "CVTKAdapter", CVTKADAPTER_VERSION, "CStructuredDataAdapter" ), 
    externalDataPtr( NULL )
{
}


CVTKAdapter::CVTKAdapter() throw()
  : CStructuredDataAdapter( "CVTKAdapter", CVTKADAPTER_VERSION, "CStructuredDataAdapter" ), 
  	externalDataPtr( NULL )
{
}


CVTKAdapter::~CVTKAdapter() throw()
{
  if ( externalDataPtr )
  {
DBG3( "New external data refcnt: " << externalDataPtr->GetReferenceCount() );
    externalDataPtr->UnRegister( NULL );
  }
}

void CVTKAdapter::setExternalData( vtkImageData* aVTKImage ) throw( NullException )
{
FBEGIN;
  if ( !aVTKImage )
    throw( NullException("No image given") );
    
    
  /* FIXME This is a possible leak because CVTKAdapter is not of Type vtkObjectBase */
  // Update VTK smart pointer information and retrieve VTK image
  if ( externalDataPtr )
  {
DBG3( "Old external data refcnt: " << externalDataPtr->GetReferenceCount() );  
    externalDataPtr->UnRegister( NULL );
  }
  externalDataPtr = aVTKImage;
  if ( externalDataPtr )
  {
  	externalDataPtr->Register( NULL );
DBG3( "New external data refcnt: " << externalDataPtr->GetReferenceCount() );
  }
FEND;  
}
    
TDataSetPtr CVTKAdapter::convertToInternal()
{
 FBEGIN;
  TDataSetPtr aDataSet;
  if ( !externalDataPtr )
  {
    alog << LWARN << "No external dataset given. Converted nothing." << endl;
    return aDataSet;
  }
  
  int *iVtkVolumeDimensions = externalDataPtr->GetDimensions();
DBG3( "VTK dimensions are " << iVtkVolumeDimensions[0] << " x " << iVtkVolumeDimensions[1] << " x " 
	<< 	iVtkVolumeDimensions[2] );
	
  std::vector<size_t> dimensionSize;
  for( int i = 0; i < 3; ++i )
  	if ( iVtkVolumeDimensions[i] > 0 )
    	dimensionSize.push_back( static_cast<size_t>( iVtkVolumeDimensions[i] ) );

  // Get origin and spacing
  vtkFloatingPointType* origin = externalDataPtr->GetOrigin();
  vtkFloatingPointType* spacing = externalDataPtr->GetSpacing();

  vtkPointData* p = externalDataPtr->GetPointData();
    
  // Convert vtkDataArray to CDataSet
	switch( externalDataPtr->GetScalarType() )
	{
		case VTK_BIT:
DBG3( "Data type is bit" );
	    vtkBitArray* myBitArray = static_cast<vtkBitArray*>( p->GetScalars() );
  	  aDataSet = this->convertVTKImage<TImage, vtkBitArray>( myBitArray, dimensionSize );
  	  break;
		case VTK_CHAR:
DBG3( "Data type is char" );
	    vtkCharArray* myCharArray = static_cast<vtkCharArray*>( p->GetScalars() );
  	  aDataSet = this->convertVTKImage<TImage, vtkCharArray>( myCharArray, dimensionSize );
  	  break;
		case VTK_UNSIGNED_CHAR:
DBG3( "Data type is uchar" );
	    vtkUnsignedCharArray* myUCharArray = static_cast<vtkUnsignedCharArray*>( p->GetScalars() );
  	  aDataSet = this->convertVTKImage<TImage, vtkUnsignedCharArray>( myUCharArray, dimensionSize );
  	  break;
		case VTK_SHORT:
DBG3( "Data type is short" );
	    vtkShortArray* myShortArray = static_cast<vtkShortArray*>( p->GetScalars() );
  	  aDataSet = this->convertVTKImage<TImage, vtkShortArray>( myShortArray, dimensionSize );
  	  break;
		case VTK_UNSIGNED_SHORT:
DBG3( "Data type is ushort" );
	    vtkUnsignedShortArray* myUShortArray = static_cast<vtkUnsignedShortArray*>( p->GetScalars() );
  	  aDataSet = this->convertVTKImage<TField, vtkUnsignedShortArray>( myUShortArray, dimensionSize );
  	  break;
		case VTK_INT:
DBG3( "Data type is int" );
	    vtkIntArray* myIntArray = static_cast<vtkIntArray*>( p->GetScalars() );
  	  aDataSet = this->convertVTKImage<TField, vtkIntArray>( myIntArray, dimensionSize );
  	  break;
		case VTK_UNSIGNED_INT:
DBG3( "Data type is uint" );
	    vtkUnsignedIntArray* myUIntArray = static_cast<vtkUnsignedIntArray*>( p->GetScalars() );
  	  aDataSet = this->convertVTKImage<TField, vtkUnsignedIntArray>( myUIntArray, dimensionSize );
  	  break;
		case VTK_LONG:
DBG3( "Data type is long" );
	    vtkLongArray* myLongArray = static_cast<vtkLongArray*>( p->GetScalars() );
  	  aDataSet = this->convertVTKImage<TField, vtkLongArray>( myLongArray, dimensionSize );
  	  break;
		case VTK_UNSIGNED_LONG:
DBG3( "Data type is ulong" );
	    vtkUnsignedLongArray* myULongArray = static_cast<vtkUnsignedLongArray*>( p->GetScalars() );
  	  aDataSet = this->convertVTKImage<TField, vtkUnsignedLongArray>( myULongArray, dimensionSize );
  	  break;
		case VTK_FLOAT:
DBG3( "Data type is float" );
    	vtkFloatArray* myFloatArray = static_cast<vtkFloatArray*>( p->GetScalars() );
    	aDataSet = this->convertVTKImage<TField, vtkFloatArray>( myFloatArray, dimensionSize );
    	break;
		case VTK_DOUBLE:		
DBG3( "Data type is double" );
  	  vtkDoubleArray* myDoubleArray = static_cast<vtkDoubleArray*>( p->GetScalars() );
	    aDataSet = this->convertVTKImage<TField, vtkDoubleArray>( myDoubleArray, dimensionSize );
	    break;
		default:
			alog << LERR << "VTK data type is unknown" << endl;
			return aDataSet;
	}
  if ( aDataSet )
  {
DBG3( "Setting origin and base element dimension" );
  /* FIXME Old VTK */
#ifdef VTK_OLD
#warning Origin not set since your VTK is fairly old
  	cerr << "Origin not set since your VTK is fairly old" << endl;
#endif
#ifndef VTK_OLD
   	aDataSet->setBaseElementDimensions( spacing );
   	aDataSet->setOrigin( origin );
#endif
  }
DBG3( "External data refcnt: " << externalDataPtr->GetReferenceCount() );  
FEND;
  return aDataSet; 
}

/**
 * \param aVtkArray VTK array to convert
 * \param dim size_t[3] array of resulting dataset dimensions
 * \returns the newly created dataset
 */
template<typename TSet, typename TArray> 
  boost::shared_ptr<TSet> CVTKAdapter::convertVTKImage( TArray* aVtkArray, std::vector<size_t> dim ) const
{
FBEGIN;
  boost::shared_ptr<TSet> aSet ( new TSet( dim.size(), dim ) );
  
  vtkIdType maximumId = aVtkArray->GetMaxId();
cerr << "Converting " << maximumId << " values from " << aVtkArray->GetClassName() << " to " << typeid( typename TSet::TDataType ).name() << endl;  
  typename TSet::iterator it = aSet->begin();
  for( vtkIdType id = 0; id < maximumId; ++id, ++it )
  {
    *it = static_cast<typename TSet::TDataType>( aVtkArray->GetValue( id ) );
    aSet->adjustDataRange( *it );
  }
FEND;
  return aSet;
}

vtkImageData* CVTKAdapter::convertToExternal() throw( NullException )
{
  if ( !internalDataSPtr )
    throw( NullException( "No pointer to internal data set" ) );
    
  // Determine dimension size
  uint uiDataDimensionality = internalDataSPtr->getDimension();
  if ( uiDataDimensionality > 3 )
  	alog << LWARN << "Can only convert 1D to 3D images to vtkImageData!" << endl;
  	
  vector<size_t> dimensionSize( 3 );
  size_t dataSize = 1; 
  for( uint i = 0; i < internalDataSPtr->getDimension(); ++i )
  {
  	dimensionSize[i] = internalDataSPtr->getExtent(i);	
  	dataSize *= dimensionSize[i];
  }
  if ( uiDataDimensionality < 3 )
		dimensionSize[2] = 1;
  if ( uiDataDimensionality < 2 )
		dimensionSize[1] = 1;

  // Create vtk structured points structure
  vtkImageData* sp = vtkImageData::New();
  sp->SetDimensions( dimensionSize[0], dimensionSize[1], dimensionSize[2] );
  if ( internalDataSPtr->getDimension() == 1 )
  {
    sp->SetOrigin( internalDataSPtr->getOrigin(0), 
    	0.0, 
    	0.0 );
    sp->SetSpacing( internalDataSPtr->getBaseElementDimension(0),
      1.0, 
      1.0 );
  }
  else if ( internalDataSPtr->getDimension() == 2 )
  {
    sp->SetOrigin( internalDataSPtr->getOrigin(0), 
    	internalDataSPtr->getOrigin(1), 
    	0.0 );
    sp->SetSpacing( internalDataSPtr->getBaseElementDimension(0),
      internalDataSPtr->getBaseElementDimension(1), 
      1.0 );
  }
  else
  {
    sp->SetOrigin( internalDataSPtr->getOrigin(0), 
    	internalDataSPtr->getOrigin(1), 
    	internalDataSPtr->getOrigin(2) );
    sp->SetSpacing( internalDataSPtr->getBaseElementDimension(0),
      internalDataSPtr->getBaseElementDimension(1),
      internalDataSPtr->getBaseElementDimension(2) );
  }
  sp->AllocateScalars();
  
  // Assign dataset to structured points
  vtkPointData* p = sp->GetPointData();
    
  if ( checkType<TField>( internalDataSPtr ) )
  {
DBG3("Converting to double");
    TFieldPtr floatSet = static_pointer_cast<TField>( internalDataSPtr );
    vtkDoubleArray* fArray = vtkDoubleArray::New();
    fArray->SetArray( floatSet->getArray(), dataSize, 1 );
    p->SetScalars( fArray );
  }
  else if ( checkType<TImage>( internalDataSPtr ) )
  {
DBG3("Converting to short");    
    TImagePtr shortSet = static_pointer_cast<TImage>( internalDataSPtr );
    vtkShortArray* sArray = vtkShortArray::New();
    sArray->SetArray( shortSet->getArray(), dataSize, 1 );
    p->SetScalars( sArray );
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
