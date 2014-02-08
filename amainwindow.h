#ifndef AMAINWINDOW_H
#define AMAINWINDOW_H

#include <QtWidgets/QMainWindow>

class AMainWindow : public QMainWindow {
    Q_OBJECT

    public:
        //! Конструктор.
        AMainWindow(QWidget *parent = NULL);

        //! Деструктор.
        virtual ~AMainWindow() {}

};

#endif
