#include <aipsnumeric.h>
#include <cmainwindow.h>
#include <qapplication.h>

int main( int argc, char* argv[] ) 
{
  QApplication myApp( argc, argv );
	string s;
	cerr << argc << endl;
	if ( argc > 1 )
		s = argv[1];
	else
		s = "/data/data/arbeitsgruppe/gudrun/xch2_reo_p101_r.dat";
	CMainWindow *mw = new CMainWindow( s );
	mw->setCaption( "Template mesh generation" );
	mw->resize( 600, 300 );	 
	myApp.setMainWidget( mw );
	mw->show();
	std::cerr << "Running" << endl;
  int nReturn = myApp.exec();  

  return nReturn;

}
