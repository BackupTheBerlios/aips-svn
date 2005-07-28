#include "cfilesourcedialog.h"
#include <aipsnumeric.h>

CFileSourceDialog::CFileSourceDialog(  CFileSource* parent_ ) throw()
 : aips::CModuleDialog(), parent( parent_ )
{
	theDisplay = new QWidget();  
  theDisplay->setFixedSize( 300, 100 );
  theDisplay->hide();
  aColumn = new QVBox( theDisplay );
  aColumn->setGeometry(0,0,300,100);
  fileNameButton = new QPushButton( "...", aColumn );
  loadDataButton = new QPushButton( "Load", aColumn );
  filename = "";
	connect( loadDataButton, SIGNAL( clicked() ),
    this, SLOT( updateData() ) );
  connect( fileNameButton, SIGNAL( clicked() ),
    this, SLOT( selectNewFile() ) );
}


CFileSourceDialog::~CFileSourceDialog() throw()
{
	delete theDisplay;
}

/* Dialog methods */

bool CFileSourceDialog::hasDialog() const throw()
{
  return true;
}

QWidget* CFileSourceDialog::getDialogHandle() const throw( NotPresentException )
{
  return theDisplay;
}

void CFileSourceDialog::setCaption ( const std::string sCaption ) throw( NotPresentException )
{
  theDisplay->setCaption( sCaption.c_str() );
}

void CFileSourceDialog::showDialog() throw( NotPresentException )
{
  theDisplay->show();
}

void CFileSourceDialog::hideDialog() throw( NotPresentException )
{
  theDisplay->hide();
}

bool CFileSourceDialog::isHidden() throw( NotPresentException )
{
  return theDisplay->isHidden();
}

void CFileSourceDialog::selectNewFile()
{
  QString sTmp = QFileDialog::getOpenFileName( path.c_str(),
    filetypes );
  if ( sTmp == QString::null ) 
		return;
	filename = sTmp.ascii();	
	notify( shared_ptr<CFileNameChangedEvent>( new CFileNameChangedEvent( this, filename ) ) );
  fileNameButton->setText( sTmp );
}

void CFileSourceDialog::updateData()
{
	FBEGIN;
  if ( filename == "" )
  {
    QMessageBox::warning( 0, "Warning", "Could not load file with no name" );
    return;
  }
  notify( shared_ptr<CLoadActivatedEvent>( new CLoadActivatedEvent( this ) ) );
	FEND;
}

void CFileSourceDialog::activateDialog()	throw( NotPresentException )
{
	theDisplay->setActiveWindow();
}
