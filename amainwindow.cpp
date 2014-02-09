#include <QtCore/QProcess>
#include <QtCore/QDir>

#include <QtGui/QMouseEvent>

#include <QtWidgets/QApplication>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QMenu>

#include <QtWebKit/QWebElement>

#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKitWidgets/QWebView>

#include "amainwindow.h"
#include "alogger.h"

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
    _web_view->installEventFilter(this);

    connect(_web_view, SIGNAL(loadFinished(bool))
        , this, SLOT(onWebViewLoadFinished(bool)));

    _text_edit = new QTextEdit(this);
    _text_edit->setReadOnly(true);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(_web_view);
    splitter->addWidget(_text_edit);
    splitter->setStretchFactor(0, 1);

    setCentralWidget(splitter);

    _miner_process = new QProcess(this);
    _miner_process->setReadChannel(QProcess::StandardOutput);
    connect(_miner_process, SIGNAL(readyReadStandardOutput())
        , this, SLOT(onMinerProcessReadyRead()), Qt::QueuedConnection);

    QMetaObject::invokeMethod(this, "trayInit", Qt::QueuedConnection);

    QString fname = QCoreApplication::applicationDirPath() +"/page.html";
    fname = QDir::toNativeSeparators(fname);
    QMetaObject::invokeMethod(this, "loadStart", Qt::QueuedConnection
        , Q_ARG(QUrl,QUrl::fromLocalFile(fname)));
}


// ========================================================================== //
// Событие фильтра.
// ========================================================================== //
bool AMainWindow::eventFilter(QObject *object, QEvent *event) {
    if(event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
        if(mouse_event->button() == Qt::LeftButton) {
            QWebFrame *frame = _web_view->page()->frameAt(mouse_event->pos());
            if(frame) {
                QPoint pos = _web_view->mapFromGlobal(mouse_event->globalPos());

                QWebElementCollection buttons
                    = frame->documentElement().findAll("button");

                for(int i = 0, n = buttons.count(); i < n; ++i) {
                    QWebElement button = buttons.at(i);
                    if(button.isNull()) continue;

                    if(!button.geometry().contains(pos)) continue;

                    if(button.hasClass("miner_start")) {
                        if(!button.hasAttribute("miner"))    continue;
                        if(!button.hasAttribute("prefixes")) continue;
                        if(!button.hasAttribute("params"))   continue;

                        if(_miner_process->state() != QProcess::NotRunning)
                            continue;

                        QStringList prefixes
                            = button.attribute("prefixes")
                                .split('|', QString::SkipEmptyParts);

                        if(!prefixes.isEmpty()) {
                            QListIterator<QString> prefixes_iter(prefixes);
                            while(prefixes_iter.hasNext()) {
                                const QString &prefix = prefixes_iter.next();
                                _miner_process->start(prefix);
                                _miner_process->waitForFinished();
                            }
                        }

                        QString fname = QCoreApplication::applicationDirPath();
                        fname.append("/");
                        fname.append(button.attribute("miner"));
                        fname.append("/");
                        fname.append(button.attribute("miner"));
                        fname.append(".exe ");

                        fname = QDir::toNativeSeparators(fname);

                        fname.append(button.attribute("params"));

                        _miner_process->start(fname);

                        break;
                    }

                    if(button.hasClass("miner_stop")) {
                        if(_miner_process->state() == QProcess::Running)
                            _miner_process->terminate();
                            _miner_process->kill();

                        break;
                    }
                }
            }
        }
    }

    return QObject::eventFilter(object, event);
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

    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason))
        , this, SLOT(onTrayActivated(QSystemTrayIcon::ActivationReason)));

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
// Слот активации иконки в системном трее.
// ========================================================================== //
void AMainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    if(reason != QSystemTrayIcon::Context) show();
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


// ========================================================================== //
// Слот вывода содержимого стандартного потока.
// ========================================================================== //
void AMainWindow::onMinerProcessReadyRead() {
    if(_miner_process->state() != QProcess::Running) return;

    QByteArray data = _miner_process->readAllStandardOutput();
    _text_edit->setPlainText(QString::fromLocal8Bit(data));
}
