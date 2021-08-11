QT += core gui widgets network

CONFIG += c++11

SOURCES += \
    crypto.cpp \
    discoveryservice.cpp \
    filetransferdialog.cpp \
    filetransferreceiver.cpp \
    filetransfersender.cpp \
    filetransferserver.cpp \
    filetransfersession.cpp \
    main.cpp \
    mainui.cpp \
    navigater.cpp \
    onlinedeviceitem.cpp \
    qrencode/bitstream.c \
    qrencode/mask.c \
    qrencode/mmask.c \
    qrencode/mqrspec.c \
    qrencode/qrencode.c \
    qrencode/qrinput.c \
    qrencode/qrspec.c \
    qrencode/rsecc.c \
    qrencode/split.c \
    sendfilewindow.cpp \
    settings.cpp \
    trayicon.cpp \
    webreceive.cpp \
    websend.cpp \
    util.cpp \
    # start of httpserver and templeteengine
    httpserver/httpglobal.cpp \
    httpserver/httplistener.cpp \
    httpserver/httpconnectionhandler.cpp \
    httpserver/httpconnectionhandlerpool.cpp \
    httpserver/httprequest.cpp \
    httpserver/httpresponse.cpp \
    httpserver/httpcookie.cpp \
    httpserver/httprequesthandler.cpp \
    httpserver/httpsession.cpp \
    httpserver/httpsessionstore.cpp \
    httpserver/staticfilecontroller.cpp \
    templateengine/template.cpp \
    templateengine/templateloader.cpp \
    templateengine/templatecache.cpp
    # end of httpserver and templeteengine

HEADERS += \
    crypto.h \
    discoveryservice.h \
    filetransferdialog.h \
    filetransferreceiver.h \
    filetransfersender.h \
    filetransferserver.h \
    filetransfersession.h \
    mainui.h \
    navigater.h \
    onlinedeviceitem.h \
    qrencode/bitstream.h \
    qrencode/config.h \
    qrencode/mask.h \
    qrencode/mmask.h \
    qrencode/mqrspec.h \
    qrencode/qrencode.h \
    qrencode/qrencode_inner.h \
    qrencode/qrinput.h \
    qrencode/qrspec.h \
    qrencode/rsecc.h \
    qrencode/split.h \
    sendfilewindow.h \
    settings.h \
    trayicon.h \
    webreceive.h \
    websend.h \
    util.h \
    # start of httpserver and templeteengine
    httpserver/httpglobal.h \
    httpserver/httplistener.h \
    httpserver/httpconnectionhandler.h \
    httpserver/httpconnectionhandlerpool.h \
    httpserver/httprequest.h \
    httpserver/httpresponse.h \
    httpserver/httpcookie.h \
    httpserver/httprequesthandler.h \
    httpserver/httpsession.h \
    httpserver/httpsessionstore.h \
    httpserver/staticfilecontroller.h\
    templateengine/templateglobal.h \
    templateengine/template.h \
    templateengine/templateloader.h \
    templateengine/templatecache.h
    # end of httpserver and templeteengine

FORMS += \
    filetransferdialog.ui \
    mainui.ui \
    sendfilewindow.ui \
    webreceive.ui \
    websend.ui

RESOURCES += \
    icons.qrc

RC_ICONS = icons/app.ico
ICON = icons/app.icns

unix {
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib -lsodium

    PREFIX = $$(PREFIX)
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }

    binary.path = $$PREFIX/bin
    binary.files = $$OUT_PWD/landrop
    binary.extra = cp "$$OUT_PWD/LANDrop" "$$OUT_PWD/landrop"
    binary.CONFIG = no_check_exist executable

    icon.path = $$PREFIX/share/icons/hicolor/scalable/apps
    icon.files = $$OUT_PWD/landrop.svg
    icon.extra = cp "$$PWD/icons/app.svg" "$$OUT_PWD/landrop.svg"
    icon.CONFIG = no_check_exist 

    desktop.path = $$PREFIX/share/applications
    desktop.files = $$OUT_PWD/landrop.desktop
    desktop.extra = \
        cp "$$PWD/../misc/LANDrop.desktop" "$$OUT_PWD/landrop.desktop" && \
        sed -i 's/Exec=LANDrop/Exec=landrop/g' "$$OUT_PWD/landrop.desktop" && \
        sed -i 's/Icon=LANDrop/Icon=landrop/g' "$$OUT_PWD/landrop.desktop"
    desktop.CONFIG = no_check_exist 

    INSTALLS += binary icon desktop
}

QMAKE_INFO_PLIST = Info.plist

OTHER_FILES += etc/* etc/static/*
