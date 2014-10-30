#ifndef ADDFILMWINDOW_H
#define ADDFILMWINDOW_H

#include "ui_addfilmwindow.h"

#include "film.h"

#include <QDialog>
#include <QShowEvent>

class AddFilmWindow : public QDialog, public Ui::AddFilmWindow
{
    Q_OBJECT

    public:
        AddFilmWindow( QWidget* parent = nullptr );

    protected:
        void showEvent( QShowEvent* event );

    signals:
        void AddFilm( Film f );

    private slots:
        void OpenFilmFileClicked();
        void OpenPosterFileClicked();
        void OkButtonClicked();

    private:
        void ConfigureRatingComboBox();
        void ClearFields();
};

#endif // ADDFILMWINDOW_H
