#ifndef CFILESOURCEDIALOG_H
#define CFILESOURCEDIALOG_H

#include <cmoduledialog.h>

// Qt includes

#include <qwidget.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <boost/shared_ptr.hpp>
#include <csubject.h>

using namespace aips;
using namespace std;
using namespace boost;

class CFileSource;

const uint EFileNameChangedEvent = 501;
const uint ELoadActivatedEvent = 502;

class CFileNameChangedEvent : public CEvent
{
public:
	CFileNameChangedEvent( CSubject* generator, string newFilename_ ) throw()
		: CEvent( generator, EFileNameChangedEvent, "CFileNameChangedEvent", "0.1", "CEvent" ), newFilename( newFilename_ )  {}
	~CFileNameChangedEvent() throw() {}
	string getFilename() { return newFilename; }
private:
	string newFilename;
};

class CLoadActivatedEvent : public CEvent
{
public:
	CLoadActivatedEvent( CSubject* generator ) throw()
		: CEvent( generator, ELoadActivatedEvent, "CCLoadActivatedEventEvent", "0.1", "CEvent" ) {}
	~CLoadActivatedEvent() throw() {}
};
	
/**
@author Hendrik Belitz
*/
class CFileSourceDialog : public QObject, public CModuleDialog
{
Q_OBJECT
public:
   CFileSourceDialog( CFileSource* parent_ ) throw();
   ~CFileSourceDialog() throw();
/* Dialog handling functions */
  /// Returns true because CSource always has a dialog window
  virtual bool hasDialog() const
    throw();
  /// Reimplemented from CPipelineItem
  virtual QWidget* getDialogHandle() const
    throw( NotPresentException );
  /// Reimplemented from CPipelineItem
  virtual void setCaption( std::string sCaption )
    throw( NotPresentException );
  /// Reimplemented from CPipelineItem
  virtual void showDialog()
    throw( NotPresentException );
  /// Reimplemented from CPipelineItem
  virtual void hideDialog()
    throw( NotPresentException );
  virtual bool isHidden()
    throw( NotPresentException );		
	virtual void activateDialog()
		throw( NotPresentException );		
	void setPath( string path_ ) { path = path_; }
	void setFileTypes( string ft ) { filetypes = ft; }
public slots:
  /// Opens a file dialog to allow selection of new source data
  void selectNewFile();
  /// Loads the new dataset
  void updateData();		
private:
  // Widgets
	string filetypes;
	string filename;
	string path;
  QWidget* theDisplay;          ///< The display window
  QVBox* aColumn;               ///< Layout manager
  QPushButton* fileNameButton;  ///< Button to call file selection dialog
  QPushButton* loadDataButton;  ///< Button to actually load the file		
	CFileSource* parent;
};

#endif
