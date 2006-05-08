#include <string>
#include <iostream>
#include <itkOrientImageFilter.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

using namespace itk;
using namespace std;

itk::SpatialOrientation::ValidCoordinateOrientationFlags
	determineOrientation( std::string theOrientation )
{
	if ( theOrientation == "RIP" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP;
	else if ( theOrientation == "LIP" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LIP;
	else if ( theOrientation == "RSP" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSP; 	
	else if ( theOrientation == "LSP" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LSP; 	
	else if ( theOrientation == "RIA" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIA; 	
	else if ( theOrientation == "LIA" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LIA; 	
	else if ( theOrientation == "RSA" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSA; 	
	else if ( theOrientation == "LSA" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LSA; 	
	else if ( theOrientation == "IRP" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IRP; 	
	else if ( theOrientation == "ILP" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ILP; 	
	else if ( theOrientation == "SRP" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SRP; 	
	else if ( theOrientation == "SLP" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SLP; 	
	else if ( theOrientation == "IRA" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IRA; 	
	else if ( theOrientation == "ILA" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ILA; 	
	else if ( theOrientation == "SRA" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SRA; 	
	else if ( theOrientation == "SLA" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SLA; 	
	else if ( theOrientation == "RPI" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPI; 	
	else if ( theOrientation == "LPI" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPI; 	
	else if ( theOrientation == "RAI" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI; 	
	else if ( theOrientation == "LAI" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LAI; 	
	else if ( theOrientation == "RPS" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPS; 	
	else if ( theOrientation == "LPS" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPS; 	
	else if ( theOrientation == "RAS" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAS; 	
	else if ( theOrientation == "LAS" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LAS; 	
	else if ( theOrientation == "PRI" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PRI; 	
	else if ( theOrientation == "PLI" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PLI; 	
	else if ( theOrientation == "ARI" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ARI; 	
	else if ( theOrientation == "ALI" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ALI; 	
	else if ( theOrientation == "PRS" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PRS; 	
	else if ( theOrientation == "PLS" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PLS; 	
	else if ( theOrientation == "ARS" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ARS; 	
	else if ( theOrientation == "ALS" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ALS; 	
	else if ( theOrientation == "IPR" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IPR; 	
	else if ( theOrientation == "SPR" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SPR; 	
	else if ( theOrientation == "IAR" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IAR; 	
	else if ( theOrientation == "SAR" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SAR; 	
	else if ( theOrientation == "IPL" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IPL; 	
	else if ( theOrientation == "SPL" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SPL; 	
	else if ( theOrientation == "IAL" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IAL; 	
	else if ( theOrientation == "SAL" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SAL; 	
	else if ( theOrientation == "PIR" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIR; 	
	else if ( theOrientation == "PSR" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PSR; 	
	else if ( theOrientation == "AIR" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_AIR; 	
	else if ( theOrientation == "ASR" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ASR; 	
	else if ( theOrientation == "PIL" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIL; 	
	else if ( theOrientation == "PSL" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PSL; 	
	else if ( theOrientation == "AIL" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_AIL; 	
	else //if ( theOrientation == "ASL" )
		return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ASL; 
}

int main(int argc, char *argv[])
{
  typedef Image<short, 3> ImageType;
  typedef Image<short, 3> ImageType2;
  typedef ImageFileReader<ImageType> ReaderType;
  typedef ImageFileWriter<ImageType2> WriterType;
	itk::SpatialOrientation::ValidCoordinateOrientationFlags inputDir;
	itk::SpatialOrientation::ValidCoordinateOrientationFlags outputDir;
	if ( argc < 4 || argc > 5 )
	{
		cout << "Usage: reorient [ABC] XYZ inputfile outputfile\n"
			<< " with ABC/XYZ being a three-letter-code for the three axes:\n"
			<< " The fastest-varying axis is given first.\n"
			<< " A - Anterior / P - Posterior \n"
			<< " S - Superior / I - Inferior \n"
			<< " R - Right / L - Left \n"
			<< " ABC may be ommited. In that case, we try to determine the image orientation "
			<< " automatocally (This WILL fail in most cases)." << endl;
		return 0;
	}
	OrientImageFilter<ImageType,ImageType>::Pointer orienter =
  	OrientImageFilter<ImageType,ImageType>::New();
  ReaderType::Pointer myReader = ReaderType::New();
  WriterType::Pointer myWriter = WriterType::New();
	if ( argc == 4 )
	{
		orienter->UseImageDirectionOn();
		outputDir = determineOrientation( argv[1] );
		cout << argv[2] << " ( ??? ) -> " << argv[3] << " ( " << argv[1] << " )" << endl;
	  orienter->SetDesiredCoordinateOrientation( outputDir );
	  myReader->SetFileName( argv[2] );
  	myWriter->SetFileName( argv[3] );
	}
	else if ( argc == 5 )
	{
		inputDir = determineOrientation( argv[1] );
		outputDir = determineOrientation( argv[2] );
	  cout << argv[3] << " (" << argv[1] << ") -> " << argv[4] << " (" << argv[2] 
			<< ")" << endl;
  	orienter->SetGivenCoordinateOrientation( inputDir );
	  orienter->SetDesiredCoordinateOrientation( outputDir ); 
  	myReader->SetFileName( argv[3] );
  	myWriter->SetFileName( argv[4] );
	}

  myReader->Update();  
  orienter->SetInput(myReader->GetOutput());
  orienter->Update();
  
  myWriter->SetInput( orienter->GetOutput() );
  myWriter->Update();
  return EXIT_SUCCESS;
}
