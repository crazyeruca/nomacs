/*******************************************************************************************************
 DkPluginManager.h
 Created on:	20.05.2013
 
 nomacs is a fast and small image viewer with the capability of synchronizing multiple instances
 
 Copyright (C) 2011-2013 Markus Diem <markus@nomacs.org>
 Copyright (C) 2011-2013 Stefan Fiel <stefan@nomacs.org>
 Copyright (C) 2011-2013 Florian Kleber <florian@nomacs.org>

 This file is part of nomacs.

 nomacs is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 nomacs is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 *******************************************************************************************************/

#pragma once

#include <QtGui/QWidget>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>
#include <QtGui/QProgressDialog>
#include <QtGui/QTabWidget>
#include <QPluginLoader>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QScrollBar>

#include "DkViewPort.h"
#include "DkPluginInterface.h"

namespace nmc {

class DkPluginTableWidget;
class DkInstalledPluginsModel;

enum pluginManagerTabs {
	tab_installed_plugins,
	tab_download_plugins,
};

enum installedPluginsColumns {
	ip_column_name,
	ip_column_version,
	ip_column_enabled,
	ip_column_uninstall,
	ip_column_size,
};

enum downloadPluginsColumns {
	dp_column_name,
	dp_column_version,
	dp_column_install,
	dp_column_size,
};

struct XmlPluginData {
	QString id;
	QString name;
	QString version;
	QString decription;
	QString previewImgUrl;
	QString downloadSrc;
	QString downloadX86;
	QString downloadX64;
};

struct QPairFirstComparer {
	template<typename T1, typename T2>
	bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const {
		return a.first < b.first;
	}
};

struct QPairSecondComparer {
	template<typename T1, typename T2>
	bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const {
		return a.second < b.second;
	}
};

// Plug-in manager dialog for enabling/disabling plug-ins and downloading new ones
class DkPluginManager : public QDialog {

Q_OBJECT

public:
	DkPluginManager(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	~DkPluginManager();
		
	void loadPlugins();
	QMap<QString, DkPluginInterface *> getPlugins();
	QList<QString> getPluginIdList();
	QMap<QString, QString> getPluginFileNames();
	void setPluginIdList(QList<QString> newPlugins);
	QMap<QString, QString> getRunId2PluginId();
	void setRunId2PluginId(QMap<QString, QString> newMap);
	void deletePlugin(QString pluginID);

protected slots:
	void closePressed();
	void tabChanged(int tab);

protected:
	int dialogWidth;
	int dialogHeight;
	QTabWidget* tabs;
	QMap<QString, DkPluginInterface *> loadedPlugins;
	QMap<QString, QPluginLoader *> pluginLoaders;	// needed for unloading plug-ins when uninstalling them
	DkPluginTableWidget* tableWidgetInstalled;
	DkPluginTableWidget* tableWidgetDownload;
	QMap<QString, QString> pluginFiles;
	QList<QString> pluginIdList;
	QMap<QString, QString> runId2PluginId;

	void init();
	void createLayout();
	void showEvent(QShowEvent *event);
		
};

// widget with all plug-in information
class DkPluginTableWidget: public QWidget {

Q_OBJECT

public:    
	DkPluginTableWidget(int tab, DkPluginManager* manager, QWidget* parent);
	~ DkPluginTableWidget();

	void clearTableFilters();
	void updateModels();
	DkPluginManager* getPluginManager();
	int getOpenedTab();
	
private:
	void createLayout();

	int openedTab;
	DkPluginManager* pluginManager;
	QSortFilterProxyModel *proxyModel;
	//DkInstalledPluginsModel* model;
	QAbstractTableModel* model;

	QTableView* tableView;
	QLineEdit* filterEdit;

private slots:
	void updatePlugins();
	void filterTextChanged();
	void uninstallPlugin(const QModelIndex &index);

};

// model for the table in the installed plug-ins tab 
class DkInstalledPluginsModel : public QAbstractTableModel {

Q_OBJECT

public:
    DkInstalledPluginsModel(QObject *parent=0);
    DkInstalledPluginsModel(QList<QString> data, QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
	
	QList<QString> getPluginData();
	void setDataToInsert(QString newData);
	void setEnabledData(QMap<QString, bool> enabledData);
	QMap<QString, bool> getEnabledData();
	void loadPluginsEnabledSettings();
	void savePluginsEnabledSettings();
	
private:
	QList<QString> pluginData;
	QMap<QString, bool> pluginsEnabled;
	QString dataToInsert;
	DkPluginTableWidget* parentTable;

};

class DkDownloadPluginsModel : public QAbstractTableModel {

Q_OBJECT

public:
    DkDownloadPluginsModel(QObject *parent=0);
    DkDownloadPluginsModel(QList<QString> data, QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
	
	QList<QString> getPluginData();
	void setDataToInsert(QString newData);
	void setEnabledData(QMap<QString, bool> enabledData);
	QMap<QString, bool> getEnabledData();
	void loadPluginsEnabledSettings();
	void savePluginsEnabledSettings();
	
private:
	QList<QString> pluginData;
	QMap<QString, bool> pluginsEnabled;
	QString dataToInsert;
	DkPluginTableWidget* parentTable;
	QList<XmlPluginData> xmlPluginData;
};


// checkbox delegate based on code from http://stackoverflow.com/questions/3363190/qt-qtableview-how-to-have-a-checkbox-only-column
class DkCheckBoxDelegate : public QStyledItemDelegate {

Q_OBJECT

public:
	DkCheckBoxDelegate(QObject *parent = 0);
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
	QTableView* parentTable;
};

// pushbutton delegate : adds buttons in the table column
class DkPushButtonDelegate : public QStyledItemDelegate {

Q_OBJECT

public:
	DkPushButtonDelegate(QObject *parent = 0);
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void buttonClicked(const QModelIndex &index);

private:
	QTableView* parentTable;
	int currRow;
	QStyle::State  pushButonState;
};

// delegate for download table: icon if already downloaded or button if not
class DkDownloadDelegate : public QStyledItemDelegate {

Q_OBJECT

public:
	DkDownloadDelegate(QObject *parent = 0);
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void buttonClicked(const QModelIndex &index);

private:
	QTableView* parentTable;
	int currRow;
	QStyle::State  pushButonState;
};

// text edit connected to tables selection
class DkDescriptionEdit : public QTextEdit {

Q_OBJECT

public:
	DkDescriptionEdit(QAbstractTableModel* data, QSortFilterProxyModel* proxy, QItemSelectionModel* selection, QWidget *parent = 0);

protected slots:
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
	void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
	QAbstractTableModel* dataModel;
	QSortFilterProxyModel* proxyModel;
	QItemSelectionModel* selectionModel;
	DkPluginTableWidget* parentTable;
	QString defaultString;
	void updateText();
};

// label connected to table selections
class DkDescriptionImage : public QLabel {

Q_OBJECT

public:
	DkDescriptionImage(QAbstractTableModel* data, QSortFilterProxyModel* proxy, QItemSelectionModel* selection, QWidget *parent = 0);

protected slots:
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
	void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
	QAbstractTableModel* dataModel;
	QSortFilterProxyModel* proxyModel;
	QItemSelectionModel* selectionModel;
	DkPluginTableWidget* parentTable;
	QImage defaultImage;
	void updateImage();
};

};
