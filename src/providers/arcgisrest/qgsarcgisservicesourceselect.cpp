/***************************************************************************
    qgsarcgisservicesourceselect.cpp
    -------------------------
  begin                : Nov 26, 2015
  copyright            : (C) 2015 by Sandro Mani
  email                : smani@sourcepole.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsarcgisservicesourceselect.h"
#include "qgsowsconnection.h"
#include "qgsnewhttpconnection.h"
#include "qgsprojectionselectiondialog.h"
#include "qgsexpressionbuilderdialog.h"
#include "qgscontexthelp.h"
#include "qgsproject.h"
#include "qgscoordinatereferencesystem.h"
#include "qgscoordinatetransform.h"
#include "qgslogger.h"
#include "qgsmanageconnectionsdialog.h"
#include "qgsexception.h"
#include "qgssettings.h"
#include "qgsmapcanvas.h"

#include <QItemDelegate>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QRadioButton>
#include <QImageReader>

/**
 * Item delegate with tweaked sizeHint.
 */
class QgsAbstractDataSourceWidgetItemDelegate : public QItemDelegate
{
  public:
    //! Constructor
    QgsAbstractDataSourceWidgetItemDelegate( QObject *parent = 0 ) : QItemDelegate( parent ) { }
    QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
};


QgsArcGisServiceSourceSelect::QgsArcGisServiceSourceSelect( const QString &serviceName, ServiceType serviceType, QWidget *parent, Qt::WindowFlags fl, QgsProviderRegistry::WidgetMode widgetMode ):
  QgsAbstractDataSourceWidget( parent, fl, widgetMode ),
  mServiceName( serviceName ),
  mServiceType( serviceType ),
  mBuildQueryButton( 0 ),
  mImageEncodingGroup( 0 )
{
  setupUi( this );
  setWindowTitle( QStringLiteral( "Add %1 Layer from a Server" ).arg( mServiceName ) );

  mAddButton = buttonBox->addButton( tr( "&Add" ), QDialogButtonBox::ActionRole );
  mAddButton->setEnabled( false );
  connect( mAddButton, &QAbstractButton::clicked, this, &QgsArcGisServiceSourceSelect::addButtonClicked );

  if ( mServiceType == FeatureService )
  {
    mBuildQueryButton = buttonBox->addButton( tr( "&Build query" ), QDialogButtonBox::ActionRole );
    mBuildQueryButton->setDisabled( true );
    connect( mBuildQueryButton, &QAbstractButton::clicked, this, &QgsArcGisServiceSourceSelect::buildQueryButtonClicked );
  }

  connect( buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );
  connect( btnNew, &QAbstractButton::clicked, this, &QgsArcGisServiceSourceSelect::addEntryToServerList );
  connect( btnEdit, &QAbstractButton::clicked, this, &QgsArcGisServiceSourceSelect::modifyEntryOfServerList );
  connect( btnDelete, &QAbstractButton::clicked, this, &QgsArcGisServiceSourceSelect::deleteEntryOfServerList );
  connect( btnConnect, &QAbstractButton::clicked, this, &QgsArcGisServiceSourceSelect::connectToServer );
  connect( btnChangeSpatialRefSys, &QAbstractButton::clicked, this, &QgsArcGisServiceSourceSelect::changeCrs );
  connect( lineFilter, &QLineEdit::textChanged, this, &QgsArcGisServiceSourceSelect::filterChanged );
  populateConnectionList();
  mProjectionSelector = new QgsProjectionSelectionDialog( this );
  mProjectionSelector->setMessage( QString() );

  treeView->setItemDelegate( new QgsAbstractDataSourceWidgetItemDelegate( treeView ) );

  QgsSettings settings;
  restoreGeometry( settings.value( QStringLiteral( "Windows/SourceSelectDialog/geometry" ) ).toByteArray() );
  cbxUseTitleLayerName->setChecked( settings.value( QStringLiteral( "Windows/SourceSelectDialog/UseTitleLayerName" ), false ).toBool() );

  mModel = new QStandardItemModel();
  mModel->setHorizontalHeaderItem( 0, new QStandardItem( QStringLiteral( "Title" ) ) );
  mModel->setHorizontalHeaderItem( 1, new QStandardItem( QStringLiteral( "Name" ) ) );
  mModel->setHorizontalHeaderItem( 2, new QStandardItem( QStringLiteral( "Abstract" ) ) );
  if ( serviceType == FeatureService )
  {
    mModel->setHorizontalHeaderItem( 3, new QStandardItem( QStringLiteral( "Cache Feature" ) ) );
    mModel->setHorizontalHeaderItem( 4, new QStandardItem( QStringLiteral( "Filter" ) ) );
    gbImageEncoding->hide();
  }
  else
  {
    cbxFeatureCurrentViewExtent->hide();
    mImageEncodingGroup = new QButtonGroup( this );
  }

  mModelProxy = new QSortFilterProxyModel( this );
  mModelProxy->setSourceModel( mModel );
  mModelProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
  treeView->setModel( mModelProxy );

  connect( treeView, &QAbstractItemView::doubleClicked, this, &QgsArcGisServiceSourceSelect::treeWidgetItemDoubleClicked );
  connect( treeView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &QgsArcGisServiceSourceSelect::treeWidgetCurrentRowChanged );
}

QgsArcGisServiceSourceSelect::~QgsArcGisServiceSourceSelect()
{
  QgsSettings settings;
  settings.setValue( QStringLiteral( "Windows/SourceSelectDialog/geometry" ), saveGeometry() );
  settings.setValue( QStringLiteral( "Windows/SourceSelectDialog/UseTitleLayerName" ), cbxUseTitleLayerName->isChecked() );

  delete mProjectionSelector;
  delete mModel;
  delete mModelProxy;
}


void QgsArcGisServiceSourceSelect::populateImageEncodings( const QStringList &availableEncodings )
{
  QLayoutItem *item = nullptr;
  while ( ( item = gbImageEncoding->layout()->takeAt( 0 ) ) != nullptr )
  {
    delete item->widget();
    delete item;
  }
  bool first = true;
  QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
  foreach ( const QString &encoding, availableEncodings )
  {
    bool supported = false;
    foreach ( const QByteArray &fmt, supportedFormats )
    {
      if ( encoding.startsWith( fmt, Qt::CaseInsensitive ) )
      {
        supported = true;
      }
    }
    if ( !supported )
    {
      continue;
    }

    QRadioButton *button = new QRadioButton( encoding, this );
    button->setChecked( first );
    gbImageEncoding->layout()->addWidget( button );
    mImageEncodingGroup->addButton( button );
    first = false;
  }
}

QString QgsArcGisServiceSourceSelect::getSelectedImageEncoding() const
{
  return mImageEncodingGroup ? mImageEncodingGroup->checkedButton()->text() : QString();
}

void QgsArcGisServiceSourceSelect::populateConnectionList()
{
  QStringList conns = QgsOwsConnection::connectionList( mServiceName );
  cmbConnections->clear();
  foreach ( const QString &item, conns )
  {
    cmbConnections->addItem( item );
  }
  bool connectionsAvailable = !conns.isEmpty();
  btnConnect->setEnabled( connectionsAvailable );
  btnEdit->setEnabled( connectionsAvailable );
  btnDelete->setEnabled( connectionsAvailable );
  btnSave->setEnabled( connectionsAvailable );

  //set last used connection
  QString selectedConnection = QgsOwsConnection::selectedConnection( mServiceName );
  int index = cmbConnections->findText( selectedConnection );
  if ( index != -1 )
  {
    cmbConnections->setCurrentIndex( index );
  }
}

QString QgsArcGisServiceSourceSelect::getPreferredCrs( const QSet<QString> &crsSet ) const
{
  if ( crsSet.size() < 1 )
  {
    return QLatin1String( "" );
  }

  //first: project CRS
  QgsCoordinateReferenceSystem projectRefSys = QgsProject::instance()->crs();
  //convert to EPSG
  QString ProjectCRS;
  if ( projectRefSys.isValid() )
  {
    ProjectCRS = projectRefSys.authid();
  }

  if ( !ProjectCRS.isEmpty() && crsSet.contains( ProjectCRS ) )
  {
    return ProjectCRS;
  }

  //second: WGS84
  if ( crsSet.contains( GEO_EPSG_CRS_AUTHID ) )
  {
    return GEO_EPSG_CRS_AUTHID;
  }

  //third: first entry in set
  return *( crsSet.constBegin() );
}

void QgsArcGisServiceSourceSelect::refresh()
{
  populateConnectionList();
}

void QgsArcGisServiceSourceSelect::addEntryToServerList()
{

  QgsNewHttpConnection nc( 0, QStringLiteral( "qgis/connections-%1/" ).arg( mServiceName.toLower() ) );
  nc.setWindowTitle( tr( "Create a New %1 Connection" ).arg( mServiceName ) );

  if ( nc.exec() )
  {
    populateConnectionList();
    emit connectionsChanged();
  }
}

void QgsArcGisServiceSourceSelect::modifyEntryOfServerList()
{
  QgsNewHttpConnection nc( 0, QStringLiteral( "qgis/connections-%1/" ).arg( mServiceName.toLower() ), cmbConnections->currentText() );
  nc.setWindowTitle( tr( "Modify %1 Connection" ).arg( mServiceName ) );

  if ( nc.exec() )
  {
    populateConnectionList();
    emit connectionsChanged();
  }
}

void QgsArcGisServiceSourceSelect::deleteEntryOfServerList()
{
  QString msg = tr( "Are you sure you want to remove the %1 connection and all associated settings?" )
                .arg( cmbConnections->currentText() );
  QMessageBox::StandardButton result = QMessageBox::information( this, tr( "Confirm Delete" ), msg, QMessageBox::Ok | QMessageBox::Cancel );
  if ( result == QMessageBox::Ok )
  {
    QgsOwsConnection::deleteConnection( mServiceName, cmbConnections->currentText() );
    cmbConnections->removeItem( cmbConnections->currentIndex() );
    emit connectionsChanged();
    bool connectionsAvailable = cmbConnections->count() > 0;
    btnConnect->setEnabled( connectionsAvailable );
    btnEdit->setEnabled( connectionsAvailable );
    btnDelete->setEnabled( connectionsAvailable );
    btnSave->setEnabled( connectionsAvailable );
  }
}

void QgsArcGisServiceSourceSelect::connectToServer()
{
  bool haveLayers = false;
  btnConnect->setEnabled( false );
  mModel->setRowCount( 0 );
  mAvailableCRS.clear();

  QgsOwsConnection connection( mServiceName, cmbConnections->currentText() );

  setCursor( Qt::WaitCursor );
  bool success = connectToService( connection );
  unsetCursor();
  if ( success )
  {
    haveLayers = mModel->rowCount() > 0;

    if ( haveLayers )
    {
      for ( int i = 0; i < treeView->header()->count(); ++i )
      {
        treeView->resizeColumnToContents( i );
        if ( i < 2 && treeView->columnWidth( i ) > 300 )
        {
          treeView->setColumnWidth( i, 300 );
        }
      }
      treeView->selectionModel()->select( mModel->index( 0, 0 ), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows );
      treeView->setFocus();
    }
    else
    {
      QMessageBox::information( 0, tr( "No Layers" ), tr( "The query returned no layers." ) );
    }
  }

  btnConnect->setEnabled( true );
  mAddButton->setEnabled( haveLayers );
  if ( mServiceType == FeatureService )
  {
    mBuildQueryButton->setEnabled( haveLayers );
  }
  btnChangeSpatialRefSys->setEnabled( haveLayers );
}

void QgsArcGisServiceSourceSelect::addButtonClicked()
{
  if ( treeView->selectionModel()->selectedRows().isEmpty() )
  {
    return;
  }

  QgsOwsConnection connection( mServiceName, cmbConnections->currentText() );

  QString pCrsString( labelCoordRefSys->text() );
  QgsCoordinateReferenceSystem pCrs( pCrsString );
  //prepare canvas extent info for layers with "cache features" option not set
  QgsRectangle extent;
  QgsCoordinateReferenceSystem canvasCrs;
  if ( mapCanvas() )
  {
    extent = mapCanvas()->extent();
    canvasCrs = mapCanvas()->mapSettings().destinationCrs();
  }
  //does canvas have "on the fly" reprojection set?
  if ( pCrs.isValid() && canvasCrs.isValid() )
  {
    try
    {
      extent = QgsCoordinateTransform( canvasCrs, pCrs ).transform( extent );
      QgsDebugMsg( QString( "canvas transform: Canvas CRS=%1, Provider CRS=%2, BBOX=%3" )
                   .arg( canvasCrs.authid(), pCrs.authid(), extent.asWktCoordinates() ) );
    }
    catch ( const QgsCsException & )
    {
      // Extent is not in range for specified CRS, leave extent empty.
    }
  }

  //create layers that user selected from this feature source
  QModelIndexList list = treeView->selectionModel()->selectedRows();
  for ( int i = 0; i < list.size(); i++ )
  {
    //add a wfs layer to the map
    QModelIndex idx = mModelProxy->mapToSource( list[i] );
    if ( !idx.isValid() )
    {
      continue;
    }
    int row = idx.row();
    QString layerTitle = mModel->item( row, 0 )->text(); //layer title/id
    QString layerName = mModel->item( row, 1 )->text(); //layer name
    bool cacheFeatures = mServiceType == FeatureService ? mModel->item( row, 3 )->checkState() == Qt::Checked : false;
    QString filter = mServiceType == FeatureService ? mModel->item( row, 4 )->text() : QLatin1String( "" ); //optional filter specified by user
    if ( cbxUseTitleLayerName->isChecked() && !layerTitle.isEmpty() )
    {
      layerName = layerTitle;
    }
    QgsRectangle layerExtent;
    if ( mServiceType == FeatureService && ( cbxFeatureCurrentViewExtent->isChecked() || !cacheFeatures ) )
    {
      layerExtent = extent;
    }
    QString uri = getLayerURI( connection, layerTitle, layerName, pCrsString, filter, layerExtent );

    QgsDebugMsg( "Layer " + layerName + ", uri: " + uri );
    emit addLayer( uri, layerName );
  }
  accept();
}

void QgsArcGisServiceSourceSelect::changeCrs()
{
  if ( mProjectionSelector->exec() )
  {
    QString crsString = mProjectionSelector->crs().authid();
    labelCoordRefSys->setText( crsString );
  }
}

void QgsArcGisServiceSourceSelect::changeCrsFilter()
{
  QgsDebugMsg( "changeCRSFilter called" );
  //evaluate currently selected typename and set the CRS filter in mProjectionSelector
  QModelIndex currentIndex = treeView->selectionModel()->currentIndex();
  if ( currentIndex.isValid() )
  {
    QString currentTypename = currentIndex.sibling( currentIndex.row(), 1 ).data().toString();
    QgsDebugMsg( QString( "the current typename is: %1" ).arg( currentTypename ) );

    QMap<QString, QStringList>::const_iterator crsIterator = mAvailableCRS.find( currentTypename );
    if ( crsIterator != mAvailableCRS.end() )
    {
      QSet<QString> crsNames;
      foreach ( const QString &crsName, crsIterator.value() )
      {
        crsNames.insert( crsName );
      }
      if ( mProjectionSelector )
      {
        mProjectionSelector->setOgcWmsCrsFilter( crsNames );
        QString preferredCRS = getPreferredCrs( crsNames ); //get preferred EPSG system
        if ( !preferredCRS.isEmpty() )
        {
          QgsCoordinateReferenceSystem refSys = QgsCoordinateReferenceSystem::fromOgcWmsCrs( preferredCRS );
          mProjectionSelector->setCrs( refSys );

          labelCoordRefSys->setText( preferredCRS );
        }
      }
    }
  }
}

void QgsArcGisServiceSourceSelect::on_cmbConnections_activated( int index )
{
  Q_UNUSED( index );
  QgsOwsConnection::setSelectedConnection( mServiceName, cmbConnections->currentText() );
}

void QgsArcGisServiceSourceSelect::treeWidgetItemDoubleClicked( const QModelIndex &index )
{
  QgsDebugMsg( "double-click called" );
  QgsOwsConnection connection( mServiceName, cmbConnections->currentText() );
  buildQuery( connection, index );
}

void QgsArcGisServiceSourceSelect::treeWidgetCurrentRowChanged( const QModelIndex &current, const QModelIndex &previous )
{
  Q_UNUSED( previous )
  QgsDebugMsg( "treeWidget_currentRowChanged called" );
  changeCrsFilter();
  if ( mServiceType == FeatureService )
  {
    mBuildQueryButton->setEnabled( current.isValid() );
  }
  mAddButton->setEnabled( current.isValid() );
}

void QgsArcGisServiceSourceSelect::buildQueryButtonClicked()
{
  QgsDebugMsg( "mBuildQueryButton click called" );
  QgsOwsConnection connection( mServiceName, cmbConnections->currentText() );
  buildQuery( connection, treeView->selectionModel()->currentIndex() );
}

void QgsArcGisServiceSourceSelect::filterChanged( const QString &text )
{
  QgsDebugMsg( "FeatureType filter changed to :" + text );
  QRegExp::PatternSyntax mySyntax = QRegExp::PatternSyntax( QRegExp::RegExp );
  Qt::CaseSensitivity myCaseSensitivity = Qt::CaseInsensitive;
  QRegExp myRegExp( text, myCaseSensitivity, mySyntax );
  mModelProxy->setFilterRegExp( myRegExp );
  mModelProxy->sort( mModelProxy->sortColumn(), mModelProxy->sortOrder() );
}

QSize QgsAbstractDataSourceWidgetItemDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  QVariant indexData = index.data( Qt::DisplayRole );
  if ( indexData.isNull() )
  {
    return QSize();
  }
  QSize size = option.fontMetrics.boundingRect( indexData.toString() ).size();
  size.setHeight( size.height() + 2 );
  return size;
}

void QgsArcGisServiceSourceSelect::on_buttonBox_helpRequested() const
{
  QgsContextHelp::run( metaObject()->className() );
}
