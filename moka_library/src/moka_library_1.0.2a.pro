QT -= qt gui core

MOKA_VERSION = 1.0.2a

TARGET = moka
TEMPLATE = lib

CONFIG += staticlib
CONFIG(debug, debug|release): DEFINES += _DEBUG

# DEFINES += MOKA_TMP_CODE

include(../libraries)

# Include paths
INCLUDEPATH += ../include
DEPENDPATH  += ../include
INCLUDEPATH += $$BOOST_INCLUDES
INCLUDEPATH += $$ARMADILLO_INCLUDES
INCLUDEPATH += $$OPENBABEL_INCLUDES
INCLUDEPATH += $$MLPACK_INCLUDES
INCLUDEPATH += $$LIBXML2_INCLUDES

SOURCES += \
    moka/dataset/csvdataset.cpp \
    moka/dataset/dataset.cpp \
    moka/dataset/datasetdispenser.cpp \
    moka/dataset/genericdataset.cpp \
    moka/dataset/multilabeledgraphdataset.cpp \
    moka/dataset/vectordataset.cpp \
    moka/ml/graphreservoir.cpp \
    moka/ml/linearreadout.cpp \
    moka/ml/supersom.cpp \
    moka/model/model.cpp \
    moka/model/modeldispenser.cpp \
    moka/procedure/crossvalidation.cpp \
    moka/structure/graph.cpp \
    moka/structure/labeledgraph.cpp \
    moka/structure/multilabeledgraph.cpp \
    moka/util/parameters.cpp \
    moka/util/info.cpp \
    moka/util/timer.cpp \
    moka/util/math.cpp \
    moka/exception.cpp \
    moka/global.cpp \
    moka/log.cpp \
    moka/model/graphesnsom.cpp

HEADERS += \
    moka/dataset/csvdataset.h \
    moka/dataset/dataset.h \
    moka/dataset/datasetdispenser.h \
    moka/dataset/genericdataset.h \
    moka/dataset/genericdataset_impl.h \
    moka/dataset/multilabeledgraphdataset.h \
    moka/dataset/vectordataset.h \
    moka/ml/graphreservoir.h \
    moka/ml/graphreservoir_impl.h \
    moka/ml/linearreadout.h \
    moka/ml/supersom.h \
    moka/model/model.h \
    moka/model/modeldispenser.h \
    moka/procedure/crossvalidation.h \
    moka/structure/graph.h \
    moka/structure/graph_impl.h \
    moka/structure/labeledgraph.h \
    moka/structure/multilabeledgraph.h \
    moka/util/info.h \
    moka/util/info_impl.h \
    moka/util/math.h \
    moka/util/math_impl.h \
    moka/util/parameters.h \
    moka/util/timer.h \
    moka/exception.h \
    moka/global.h \
    moka/global_impl.h \
    moka/log.h \
    moka/model/graphesnsom.h
