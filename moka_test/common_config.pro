MOKA_VERSION = 1.0.2a

CONFIG(debug, debug|release): DEFINES += _DEBUG

include(./libraries)

# Include paths
INCLUDEPATH += ../../include
DEPENDPATH  += ../../include
INCLUDEPATH += $$MOKA_INCLUDES
DEPENDPATH  += $$MOKA_INCLUDES
INCLUDEPATH += $$BOOST_INCLUDES
INCLUDEPATH += $$ARMADILLO_INCLUDES
INCLUDEPATH += $$OPENBABEL_INCLUDES
INCLUDEPATH += $$MLPACK_INCLUDES
INCLUDEPATH += $$LIBXML2_INCLUDES

# Static libraries links
CONFIG(release, debug|release): LIBS += -L../../build -lmoka
CONFIG(release, debug|release): PRE_TARGETDEPS += ../../build/libmoka.a
CONFIG(debug, debug|release): LIBS += -L../../debug -lmoka
CONFIG(debug, debug|release): PRE_TARGETDEPS += ../../debug/libmoka.a

# Shared libraries links
LIBS += -lboost_program_options
LIBS += -llapack -lblas -lxml2 -lpthread -lgfortran
LIBS += -L$$BOOST_LIBRARIES -lboost_filesystem -lboost_system
LIBS += -L$$ARMADILLO_LIBRARIES -larmadillo
LIBS += -L$$OPENBABEL_LIBRARIES -lopenbabel
LIBS += -L$$MLPACK_LIBRARIES -lmlpack

# Runtime libraries paths
CONFIG(release, debug|release): LIBS += -Wl,-rpath,$$BOOST_LIBRARIES
CONFIG(release, debug|release): LIBS += -Wl,-rpath,$$ARMADILLO_LIBRARIES
CONFIG(release, debug|release): LIBS += -Wl,-rpath,$$OPENBABEL_LIBRARIES
CONFIG(release, debug|release): LIBS += -Wl,-rpath,$$MLPACK_LIBRARIES
