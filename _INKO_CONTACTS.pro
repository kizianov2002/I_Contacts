#-------------------------------------------------
#
# Project created by QtCreator 2015-01-20T12:50:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = _INKO_CONTACT
TEMPLATE = app
QT += sql
QT += serialport

CONFIG -= embeded_manifest_dll

RC_FILE = inko_contacts.rc

SOURCES += main.cpp\
    connection_window.cpp \
    contacts_wnd.cpp \
    ki_sqlquerymodel.cpp \
    send2fond_wnd.cpp \
    print_monitoring_wnd.cpp \
    outerdb_wnd.cpp \
    packs_wnd.cpp \
    get_date_wnd.cpp \
    show_tab_wnd.cpp

HEADERS +=connection_window.h \
    contacts_wnd.h \
    ki_sqlquerymodel.h \
    ki_exec_query.h \
    s_data_app.h \
    send2fond_wnd.h \
    print_monitoring_wnd.h \
    s_data_contact.h \
    outerdb_wnd.h \
    packs_wnd.h \
    get_date_wnd.h \
    show_tab_wnd.h

FORMS += connection_window.ui \
    contacts_wnd.ui \
    send2fond_wnd.ui \
    print_monitoring_wnd.ui \
    outerdb_wnd.ui \
    packs_wnd.ui \
    get_date_wnd.ui \
    show_tab_wnd.ui

win32:LIBS             += -lsetupapi -ladvapi32 -luser32

DISTFILES += \
    inko_contacts.rc \
    inko-contacts.ico
