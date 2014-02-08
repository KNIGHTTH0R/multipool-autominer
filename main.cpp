#include <QtCore/QTextCodec>
#include <QtCore/QLocale>

#include <QtWidgets/QApplication>

#include "amainwindow.h"

// ========================================================================== //
//
// ========================================================================== //
int main(int argc, char *argv[]) {
    QCoreApplication::setOrganizationName("AlexisSoft");
    QCoreApplication::setOrganizationDomain("free-lance.ru");
    QCoreApplication::setApplicationName("easyminer");

    QApplication app(argc, argv);

    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::RussianFederation));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    AMainWindow main_window;
    main_window.resize(640, 480);
    main_window.show();

    return app.exec();
}
