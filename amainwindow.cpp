#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

#include <QtWidgets/QStatusBar>

#include <QtWebKitWidgets/QWebView>

#include "amainwindow.h"
#include "alogger.h"

#define START_PAGE "http://google.ru"

// ========================================================================== //
// Конструктор.
// ========================================================================== //
AMainWindow::AMainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("easyminer");

    connect(&ALogger::instance(), SIGNAL(sigInfo(QString))
        , statusBar(), SLOT(showMessage(QString)), Qt::QueuedConnection);
    connect(&ALogger::instance(), SIGNAL(sigWarn(QString))
        , statusBar(), SLOT(showMessage(QString)), Qt::QueuedConnection);
    connect(&ALogger::instance(), SIGNAL(sigCrit(QString))
        , statusBar(), SLOT(showMessage(QString)), Qt::QueuedConnection);

    _web_view = new QWebView(this);
    connect(_web_view, SIGNAL(loadFinished(bool))
        , this, SLOT(onWebViewLoadFinished(bool)));

    setCentralWidget(_web_view);

    QMetaObject::invokeMethod(this, "loadStart", Qt::QueuedConnection
        , Q_ARG(QUrl,QUrl(START_PAGE)));
}


// ========================================================================== //
// Функция загрузки web-страницы.
// ========================================================================== //
void AMainWindow::loadStart(const QUrl &url) {
    _web_view->load(url);
}


// ========================================================================== //
// Слот завершения загрузки web-страницы.
// ========================================================================== //
void AMainWindow::onWebViewLoadFinished(bool ok) {
    if(!ok) {
        QString fname = QCoreApplication::applicationDirPath() +"/error.html";
        fname = QDir::toNativeSeparators(fname);

        QUrl url = QUrl::fromLocalFile(fname);

        if(_web_view->url() != url) {
            QMetaObject::invokeMethod(this, "loadStart", Qt::QueuedConnection
                , Q_ARG(QUrl,url));

            return;
        }
    }
}
