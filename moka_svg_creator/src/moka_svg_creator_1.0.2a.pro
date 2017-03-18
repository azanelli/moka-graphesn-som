TEMPLATE  = app
CONFIG   += console
CONFIG   -= qt app_bundle
QT       -= qt gui core

MOKA_VERSION = 1.0.2a

TARGET = ../bin/moka-svg-creator

CONFIG(debug, debug|release): DEFINES += _DEBUG

include(../libraries)

# Include paths
INCLUDEPATH += ../include
DEPENDPATH  += ../include
INCLUDEPATH += $$MOKA_INCLUDES/../include
DEPENDPATH  += $$MOKA_INCLUDES/../include
INCLUDEPATH += $$MOKA_INCLUDES
DEPENDPATH  += $$MOKA_INCLUDES
INCLUDEPATH += $$BOOST_INCLUDES
INCLUDEPATH += $$OPENBABEL_INCLUDES
INCLUDEPATH += $$LIBXML2_INCLUDES

# Static libraries links
CONFIG(debug, debug|release): \
    LIBS += -L$$MOKA_DEBUG_LIBRARY -lmoka
CONFIG(debug, debug|release): \
    PRE_TARGETDEPS += $$MOKA_DEBUG_LIBRARY/libmoka.a
CONFIG(release, debug|release): \
    LIBS += -L$$MOKA_LIBRARY -lmoka
CONFIG(release, debug|release): \
    PRE_TARGETDEPS += $$MOKA_LIBRARY/libmoka.a

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

SOURCES += \
    moka_svg_creator.cpp

