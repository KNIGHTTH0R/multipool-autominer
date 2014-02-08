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

    setCentralWidget(_web_view);

    QMetaObject::invokeMethod(this, "loadStart", Qt::QueuedConnection);
}


// ========================================================================== //
// Функция загрузки web-страницы.
// ========================================================================== //
void AMainWindow::loadStart() {
    _web_view->load(QUrl(START_PAGE));
}
