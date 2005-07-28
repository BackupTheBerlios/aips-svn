/************************************************************************
 * File: cplugindialog                                                  *
 * Project:                                                             *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz ($EMAIL)                                      *
 *                                                                      *
 * Created: Fre Apr 23 2004                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cplugindialog.h"

CPluginDialog::CPluginDialog(QWidget *parent, const char *name)
 : QDialog(parent, name)
{
	resize(620,400);
	rows= new QVBox( this );
	rows->resize(620,400);
	table = new QListView( rows );
	table->addColumn( "Plugin" );
	table->addColumn( "Version" );
	table->addColumn( "Documentation" );	
	table->addColumn( "Path" );	
	but1= new QHBox( rows );
	but1->setSpacing( 20 );
  but1->setMargin( 10 );
	but1->resize(620,50);
	but2= new QHBox( rows );
	but2->setSpacing( 20 );
  but2->setMargin( 10 );
	but2->resize(620,50);
	add = new QPushButton( "&Add plugin", but1 );
	add->setFixedSize(150,30);
	rem = new QPushButton( "&Remove plugin", but1 );
	rem->setFixedSize(150,30);
	ok = new QPushButton( "&O.K.", but2 );
	ok->setFixedSize(150,30);
	connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( add, SIGNAL( clicked() ), this, SLOT( openNewPlugin() ) );
	connect( rem, SIGNAL( clicked() ), this, SLOT( removeOldPlugin() ) );
	rows->show();
}

CPluginDialog::~CPluginDialog()
{
}

void CPluginDialog::addItem( SLibItem& l )
{
	QListViewItem* tmp;
	tmp = new QListViewItem( table, l.library, l.version, l.documentation, l.path );
}

void CPluginDialog::removeOldPlugin()
{
	QListViewItem* tmp = table->selectedItem();
	QString ts[4];
	SLibItem l;
	if ( tmp == NULL ) return;
	for ( int i = 0; i < 4; i++ )
		ts[i] = tmp->text(i);
	l.library = ts[0].ascii();	
	l.version = ts[1].ascii();
	l.path = ts[3].ascii();
	l.documentation = ts[2].ascii();
DBG(" Removing " << l.library );
	emit(removeItem(l));
	table->takeItem( tmp );
	delete tmp;	
}

void CPluginDialog::openNewPlugin()
{
	QString filename = QFileDialog::getOpenFileName( sPath.c_str(), "*.so", this );	
	if ( filename.length() < 4 ) return;
	DBG(" Adding " << filename );
	SLibItem l;
	emit(newItem(filename.ascii()));
}

void CPluginDialog::plugInList( std::vector<SLibItem> pluginlist )
{
	QListViewItem* tmp;
	table->clear();
	for( std::vector<SLibItem>::iterator it = pluginlist.begin(); it != pluginlist.end(); ++it )
	{
		tmp = new QListViewItem( table, it->library, it->version, it->documentation, it->path );
	}
}

void CPluginDialog::setPath( std::string sPluginPath )
{
	sPath = sPluginPath;
}

