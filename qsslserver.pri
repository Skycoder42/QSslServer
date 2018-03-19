INCLUDEPATH += $$PWD

qpmx_static: message(configured for qpmx build)
else: message(configured for source build)

HEADERS += \
    $$PWD/qsslserver.h

SOURCES += \
    $$PWD/qsslserver.cpp

TRANSLATIONS +=
