/*************************************************************************************************
 *                                                                                                *
 *  file: addfilmwindow.cpp                                                                       *
 *                                                                                                *
 *  Alexandra Video Library                                                                       *
 *  Copyright (C) 2014-2015 Eugene Melnik <jeka7js@gmail.com>                                     *
 *                                                                                                *
 *  Alexandra is free software; you can redistribute it and/or modify it under the terms of the   *
 *  GNU General Public License as published by the Free Software Foundation; either version 2 of  *
 *  the License, or (at your option) any later version.                                           *
 *                                                                                                *
 *  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     *
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     *
 *  See the GNU General Public License for more details.                                          *
 *                                                                                                *
 *  You should have received a copy of the GNU General Public License along with this program.    *
 *  If not, see <http://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
  *************************************************************************************************/

#include "addfilmwindow.h"
#include "filesextensions.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCryptographicHash>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QStringList>

AddFilmWindow::AddFilmWindow( AlexandraSettings* s, QWidget* parent ) : QDialog( parent )
{
    setupUi( this );
    bOpenFile->setFocus();

    settings = s;

    connect( bOpenFile, &QPushButton::clicked, this, &AddFilmWindow::OpenFilmFileClicked );
    connect( bOpenPoster, &QPushButton::clicked, this, &AddFilmWindow::OpenPosterFileClicked );
    connect( bOk, &QPushButton::clicked, this, &AddFilmWindow::OkButtonClicked );
}

void AddFilmWindow::show()
{
    QDialog::show();
    bOpenPoster->setText( tr( "Open" ) );
    filmId = QString( QCryptographicHash::hash( QByteArray::number( qrand() ), QCryptographicHash::Sha1 ).toHex() );
}

void AddFilmWindow::closeEvent( QCloseEvent* event )
{
    ClearFields();
    event->accept();
}

void AddFilmWindow::OpenFilmFileClicked()
{
    QString lastFilmPath = settings->GetLastFilmPath();

    QFileInfo fileName = QFileDialog::getOpenFileName( this,
                         tr( "Select film" ),
                         lastFilmPath,
                         tr( "Video files (%1)" ).arg( FilesExtensions().GetFilmExtensionsForFilter() ) );

    if( fileName.isFile() )
    {
        eFilmFileName->setText( fileName.absoluteFilePath() );
        eTitle->setText( fileName.completeBaseName() );

        QString posterFileName = FilesExtensions().SearchForEponymousImage( fileName.absoluteFilePath() );
        if( !posterFileName.isEmpty() )
        {
            ePosterFileName->setText( posterFileName );
            bOpenPoster->setText( tr( "Clear" ) );
        }

        settings->SetLastFilmPath( fileName.absolutePath() );
        settings->sync();
    }
}

void AddFilmWindow::OpenPosterFileClicked()
{
    if( bOpenPoster->text() == tr( "Open" ) ) // Open poster
    {
        QString lastPosterPath = settings->GetLastPosterPath();

        QFileInfo fileName = QFileDialog::getOpenFileName( this,
                             tr( "Select image" ),
                             lastPosterPath,
                             tr( "Images (%1)" ).arg( FilesExtensions().GetImageExtensionsForFilter() ) );

        if( fileName.isFile() )
        {
            ePosterFileName->setText( fileName.absoluteFilePath() );
            bOpenPoster->setText( tr( "Clear" ) );

            settings->SetLastPosterPath( fileName.absolutePath() );
            settings->sync();
        }
    }
    else // Clear poster
    {
        QString posterPath = QFileInfo( ePosterFileName->text() ).path();
        QString postersDirPath = settings->GetPostersDirPath();

        if( posterPath == postersDirPath )
        {
            int res = QMessageBox::question( this, tr( "Clear poster" ), tr( "Remove image file?" ) );

            if( res == QMessageBox::Yes )
            {
                QFile( ePosterFileName->text() ).remove();
            }
        }

        ePosterFileName->clear();
        bOpenPoster->setText( tr( "Open" ) );
    }
}

void AddFilmWindow::OkButtonClicked()
{
    // Checking necessary fields
    if( eFilmFileName->text().isEmpty() )
    {
        QMessageBox::information( this, tr( "Adding film" ), tr( "You must choose file on the disk." ) );
        eFilmFileName->setFocus();
        return;
    }
    if( eTitle->text().isEmpty() )
    {
        QMessageBox::information( this, tr( "Adding film" ), tr( "Field \"Title\" can't be empty." ) );
        eTitle->setFocus();
        return;
    }

    // Text data
    Film f;
    f.SetId( filmId );
    f.SetFileName( eFilmFileName->text() );
    f.SetTitle( eTitle->text() );
    f.SetOriginalTitle( eOriginalTitle->text() );
    f.SetTagline( eTagline->text() );
    f.SetYearFromStr( eYear->text() );
    f.SetCountry( eCountry->text() );
    f.SetGenre( eGenre->text() );
    f.SetRatingFromStr( cbRating->currentText() );
    f.SetDirector( eDirector->text() );
    f.SetProducer( eProducer->text() );
    f.SetStarring( tStarring->toPlainText() );
    f.SetDescription( tDescription->toPlainText() );
    f.SetTags( eTags->text() );
    f.SetIsViewed( cIsViewed->isChecked() );
    f.SetIsFavourite( cIsFavourite->isChecked() );

    // Manipulations with poster
    QString posterFileName = ePosterFileName->text();

    if( !posterFileName.isEmpty() )
    {
        f.SetIsPosterExists( true );

        QString postersDir = settings->GetPostersDirPath();
        int newHeight = settings->GetScalePosterToHeight();

        if( QFileInfo( posterFileName ).absolutePath() != postersDir )
        {
            // Creating posters' directory if not exists
            if( !QDir().exists( postersDir ) )
            {
                QDir().mkdir( postersDir );
            }

            QPixmap p( posterFileName );

            // Scale to height
            if( newHeight != 0 && newHeight < p.height() )
            {
                p = p.scaledToHeight( newHeight, Qt::SmoothTransformation );
            }

            // Move to posters' folder
            QString newPosterFileName = postersDir + "/" + f.GetPosterName();

            if( !p.save( newPosterFileName ) )
            {
                f.SetIsPosterExists( false );
                emit PosterMovingError();
            }
        }
    }

    close();
    emit Done( f );
}

void AddFilmWindow::ClearFields()
{
    eFilmFileName->clear();
    ePosterFileName->clear();
    eTitle->clear();
    eOriginalTitle->clear();
    eTagline->clear();
    eYear->clear();
    eCountry->clear();
    eGenre->clear();
    cbRating->setCurrentIndex( 0 );
    eDirector->clear();
    eProducer->clear();
    tStarring->clear();
    tDescription->clear();
    eTags->clear();
    cIsViewed->setChecked( false );
    cIsFavourite->setChecked( false );
}
