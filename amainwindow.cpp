#include <QtCore/QDir>

#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMenu>

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

    QMetaObject::invokeMethod(this, "trayInit", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "loadStart", Qt::QueuedConnection
        , Q_ARG(QUrl,QUrl(START_PAGE)));
}


// ========================================================================== //
// Функция вывода иконки в трей.
// ========================================================================== //
void AMainWindow::trayInit() {
    QMenu *tray_menu = new QMenu(this);
    tray_menu->addAction(tr("Показать easyminer"), this, SLOT(show()));
    tray_menu->addAction(tr("Выйти"), qApp, SLOT(quit()));

    // QtBug: придётся использовать костыль.
    QSet<QWidget*> before = QApplication::topLevelWidgets().toSet();

    QSystemTrayIcon *tray
        = new QSystemTrayIcon(QIcon(":/images/tray.png"), this);
    tray->setContextMenu(tray_menu);
    tray->show();

    QSet<QWidget*> after = QApplication::topLevelWidgets().toSet();

    after -= before;
    if(!after.isEmpty()) {
        QWidget *wdg = (*after.begin());
        wdg->setWindowFlags(
            Qt::Window|Qt::FramelessWindowHint|Qt::SplashScreen);
        wdg->show();
        wdg->hide();
        qApp->processEvents();
        wdg->show();
    }
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
