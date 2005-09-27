/************************************************************************
 * File: cplugindialog                                                       *
 * Project:                                                             *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz <h.belitz@fz-juelich.de>                                           *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Pre-Alpha                                                   *
 * Created: Fre Apr 23 2004                                                      *
 * Based on:                                                            *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CPLUGINDIALOG_H
#define CPLUGINDIALOG_H

#include <string>
#include <cbase.h>
#include <clog.h>
#include <vector>

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qfiledialog.h>
#include <qlibrary.h>

using namespace std;
using namespace aips;

struct SLibItem
{
	std::string library;
	std::string version;
	std::string path;
	std::string documentation;
	bool isHandlerLib;
	//QLibrary* lib;
	void* lib;
	bool operator==( const SLibItem& other )
	{
		return ( ( library == other.library ) && ( version == other.version )
			&& ( documentation == other.documentation ) && ( path == other.path ) );
	}
};

/**
@author Hendrik Belitz
*/
class CPluginDialog : public QDialog
{
Q_OBJECT
public:
  CPluginDialog(QWidget *parent = 0, const char *name = 0);
	~CPluginDialog();
	void plugInList( std::vector<SLibItem> pluginlist );
	void setPath( std::string sPluginPath );
public slots:
	void addItem( SLibItem& l );
	void removeOldPlugin( );
	void openNewPlugin( );
signals:
	void newItem( const std::string& l );
	void removeItem( const SLibItem& l );
private:
	QListView* table;
	QPushButton* add;
	QPushButton* rem;
	QPushButton* ok;
	QVBox* rows;
	QHBox* but1;
	QHBox* but2;
	std::string sPath;
};

#endif
