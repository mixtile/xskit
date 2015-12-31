#-------------------------------------------------
#
# Project created by QtCreator 2015-10-08T10:34:57
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XSEditor
TEMPLATE = app
INCLUDEPATH += novile/include\
     ../../core/include\
    ../../pal/common/include\

SOURCES += main.cpp\
        xsmainwindow.cpp \
    xsdialog.cpp \
    xstreeview.cpp \
    xsfinddialog.cpp \
    xsfilesavedialog.cpp \
    xsgotolinedialog.cpp \
    xsappbuilder.cpp\
    ../../core/gen/object_def.cpp \
    ../../core/src/object.cpp \
    ../../core/src/container.cpp \
    ../../pal/qt/src/pal.cpp \
    ../../core/src/base.cpp \
    ../../core/src/string.cpp \
    ../../core/src/value.cpp \
    ../../core/src/array.cpp \
    ../../core/src/base32codec.cpp \
    ../../core/src/bon.cpp \
    ../../core/src/buffer.cpp \
    ../../core/src/bufferStream.cpp \
    ../../core/src/connection.cpp \
    ../../core/src/elementlist.cpp \
    ../../core/src/event.cpp \
    ../../core/src/filecache.cpp \
    ../../core/src/filestream.cpp \
    ../../core/src/httpconnection.cpp \
    ../../core/src/md5.cpp \
    ../../core/src/memory.cpp \
    ../../core/src/network.cpp \
    ../../core/src/path.cpp \
    ../../core/src/resourcestream.cpp \
    ../../core/src/stream.cpp \
    ../../core/src/uri.cpp \

HEADERS  += xsmainwindow.h\
    xsdialog.h \
    xstreeview.h \
    xsfinddialog.h \
    xsfilesavedialog.h \
    xsgotolinedialog.h \
    xsappbuilder.h \

FORMS    += xsmainwindow.ui \
    xsdialog.ui \
    xsfinddialog.ui \
    xsfilesavedialog.ui \
    xsgotolinedialog.ui

LIBS += -L"novile/lib"  -lnovile\

RESOURCES += \
    XSResource.qrc

DEFINES += _QT_IDE_
