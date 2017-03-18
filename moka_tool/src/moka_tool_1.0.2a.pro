QT += core gui

MOKA_VERSION = 1.0.2a

CONFIG += console
CONFIG(debug, debug|release): DEFINES += _DEBUG

include(../libraries)

# Include paths
INCLUDEPATH += $$MOKA_INCLUDES/../include
DEPENDPATH  += $$MOKA_INCLUDES/../include
INCLUDEPATH += $$MOKA_INCLUDES
DEPENDPATH  += $$MOKA_INCLUDES
INCLUDEPATH += $$BOOST_INCLUDES
INCLUDEPATH += $$OPENBABEL_INCLUDES
INCLUDEPATH += $$LIBXML2_INCLUDES

# Static libraries links
CONFIG(debug, debug|release): LIBS += -L$$MOKA_DEBUG_LIBRARY -lmoka
CONFIG(debug, debug|release): PRE_TARGETDEPS += $$MOKA_DEBUG_LIBRARY/libmoka.a
CONFIG(release, debug|release): LIBS += -L$$MOKA_LIBRARY -lmoka
CONFIG(release, debug|release): PRE_TARGETDEPS += $$MOKA_LIBRARY/libmoka.a

# Shared libraries links
LIBS += -L$$OTHER_LIBRARIES_PATH
LIBS += -llapack -lblas -lxml2 -lpthread -lgfortran
LIBS += -L$$BOOST_LIBRARIES -lboost_filesystem -lboost_system \
        -lboost_program_options
LIBS += -L$$ARMADILLO_LIBRARIES -larmadillo
LIBS += -L$$OPENBABEL_LIBRARIES -lopenbabel
LIBS += -L$$MLPACK_LIBRARIES -lmlpack

# Runtime libraries paths
CONFIG(release, debug|release): LIBS += -Wl,-rpath,$$BOOST_LIBRARIES
CONFIG(release, debug|release): LIBS += -Wl,-rpath,$$ARMADILLO_LIBRARIES
CONFIG(release, debug|release): LIBS += -Wl,-rpath,$$OPENBABEL_LIBRARIES
CONFIG(release, debug|release): LIBS += -Wl,-rpath,$$MLPACK_LIBRARIES

TARGET = ../bin/moka
TEMPLATE = app

SOURCES += \
    moka_tool.cpp \
    gui/mokawindow.cpp \
    thread/loadgmm.cpp \
    gui/gmmviewer.cpp \
    gui/gmmgridview.cpp \
    util/hexagon.cpp \
    util/unitinfo.cpp \
    thread/loadunitinfo.cpp \
    gui/graphicshexcell.cpp \
    gui/unitinfowindow.cpp \
    gui/meleculereseultsdialog.cpp \
    modelequationdialog.cpp

HEADERS += \
    gui/mokawindow.h \
    thread/loadgmm.h \
    gui/gmmviewer.h \
    gui/gmmgridview.h \
    util/hexagon.h \
    util/unitinfo.h \
    thread/loadunitinfo.h \
    util/unitinfo_impl.h \
    gui/graphicshexcell.h \
    gui/unitinfowindow.h \
    gui/meleculereseultsdialog.h \
    modelequationdialog.h

FORMS += \
    gui/mokawindow.ui \
    gui/gmmviewer.ui \
    gui/unitinfowindow.ui \
    gui/meleculereseultsdialog.ui \
    modelequationdialog.ui

RESOURCES += \
    moka_tool.qrc

