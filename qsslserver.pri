INCLUDEPATH += $$PWD

HEADERS += \
	$$PWD/qsslserver.h

SOURCES += \
	$$PWD/qsslserver.cpp

QDEP_PACKAGE_EXPORTS += Q_SSL_SERVER_EXPORT
!qdep_build: DEFINES += "Q_SSL_SERVER_EXPORT="
