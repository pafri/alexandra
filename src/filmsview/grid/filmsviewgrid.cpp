
#include "filmsviewgrid.h"

#include <QScrollBar>


FilmsViewGrid::FilmsViewGrid( QWidget* parent )
    : QListView( parent ),
      proxyModel( new FilmsViewGridProxyModel( this ) )
{
    QListView::setModel( proxyModel );

      // Appearance
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setEditTriggers( QAbstractItemView::NoEditTriggers );
    setContextMenuPolicy( Qt::CustomContextMenu );
    setResizeMode( QListView::Adjust );
    setViewMode( QListView::IconMode );
    setWrapping( true );
    setSpacing( 5 );

//    setVerticalScrollMode( QAbstractItemView::ScrollPerPixel ); // WTF: Qt bug?
}


void FilmsViewGrid::setModel( QAbstractItemModel* model )
{
    proxyModel->setSourceModel( model );
    proxyModel->SetCacheSize( model->rowCount() );
}


void FilmsViewGrid::LoadSettings()
{
    ReloadSettings();
}

void FilmsViewGrid::ReloadSettings()
{
    setStyleSheet( QString( "font-size: %1pt" ).arg( AlexandraSettings::GetInstance()->GetGridFontSize() ) );
}


void FilmsViewGrid::updateGeometries()
{
    QListView::updateGeometries();
    verticalScrollBar()->setSingleStep( AlexandraSettings::GetInstance()->GetGridItemSize() / 5 );
}
