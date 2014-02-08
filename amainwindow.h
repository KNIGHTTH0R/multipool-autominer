#ifndef AMAINWINDOW_H
#define AMAINWINDOW_H

#include <QtWidgets/QMainWindow>

class QWebView;

class AMainWindow : public QMainWindow {
    Q_OBJECT

    public:
        //! Конструктор.
        AMainWindow(QWidget *parent = NULL);

        //! Деструктор.
        virtual ~AMainWindow() {}

    private:
        QWebView *_web_view;

        //! Функция загрузки web-страницы.
        Q_INVOKABLE void loadStart();

};

#endif
