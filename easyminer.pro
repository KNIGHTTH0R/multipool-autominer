QT += core gui widgets webkitwidgets

TARGET = easyminer

TEMPLATE = app

SOURCES += main.cpp \
	amainwindow.cpp \
	alogger.cpp

HEADERS  += amainwindow.h \
	alogger.h \
	asingleton.h

OTHER_FILES += \
	README.md \
    error.html

RESOURCES += \
    images.qrc
