#ifndef AMAINWINDOW_H
#define AMAINWINDOW_H

#include <QtWidgets/QSystemTrayIcon>
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

        //! Функция вывода иконки в трей.
        Q_INVOKABLE void trayInit();

        //! Функция загрузки web-страницы.
        Q_INVOKABLE void loadStart(const QUrl &url);

    private slots:
        //! Слот активации иконки в системном трее.
        void onTrayActivated(QSystemTrayIcon::ActivationReason reason);

        //! Слот завершения загрузки web-страницы.
        void onWebViewLoadFinished(bool ok);

};

#endif
