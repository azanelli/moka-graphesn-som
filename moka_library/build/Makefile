#############################################################################
# Makefile for building: libmoka.a
# Generated by qmake (2.01a) (Qt 4.8.1) on: Wed Jan 30 17:48:20 2013
# Project:  ../src/moka_library_1.0.2a.pro
# Template: lib
# Command: /usr/bin/qmake-qt4 -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile ../src/moka_library_1.0.2a.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_WEBKIT -DQT_NO_DEBUG
CFLAGS        = -pipe -O2 -fPIC -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -O2 -fPIC -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I../src -I/usr/include/qt4 -I../include -I/usr/include -I/usr/include -I/usr/include/openbabel-2.0 -I/usr/local/include -I/usr/include/libxml2 -I. -I../src -I.
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake-qt4
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = ../src/moka/dataset/csvdataset.cpp \
		../src/moka/dataset/dataset.cpp \
		../src/moka/dataset/datasetdispenser.cpp \
		../src/moka/dataset/genericdataset.cpp \
		../src/moka/dataset/multilabeledgraphdataset.cpp \
		../src/moka/dataset/vectordataset.cpp \
		../src/moka/ml/graphreservoir.cpp \
		../src/moka/ml/linearreadout.cpp \
		../src/moka/ml/supersom.cpp \
		../src/moka/model/model.cpp \
		../src/moka/model/modeldispenser.cpp \
		../src/moka/procedure/crossvalidation.cpp \
		../src/moka/structure/graph.cpp \
		../src/moka/structure/labeledgraph.cpp \
		../src/moka/structure/multilabeledgraph.cpp \
		../src/moka/util/parameters.cpp \
		../src/moka/util/info.cpp \
		../src/moka/util/timer.cpp \
		../src/moka/util/math.cpp \
		../src/moka/exception.cpp \
		../src/moka/global.cpp \
		../src/moka/log.cpp \
		../src/moka/model/graphesnsom.cpp 
OBJECTS       = csvdataset.o \
		dataset.o \
		datasetdispenser.o \
		genericdataset.o \
		multilabeledgraphdataset.o \
		vectordataset.o \
		graphreservoir.o \
		linearreadout.o \
		supersom.o \
		model.o \
		modeldispenser.o \
		crossvalidation.o \
		graph.o \
		labeledgraph.o \
		multilabeledgraph.o \
		parameters.o \
		info.o \
		timer.o \
		math.o \
		exception.o \
		global.o \
		log.o \
		graphesnsom.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		../libraries \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/staticlib.prf \
		/usr/share/qt4/mkspecs/features/static.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		../src/moka_library_1.0.2a.pro
QMAKE_TARGET  = moka
DESTDIR       = 
TARGET        = libmoka.a

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET) 

staticlib: $(TARGET)

$(TARGET):  $(OBJECTS) $(OBJCOMP) 
	-$(DEL_FILE) $(TARGET)
	$(AR) $(TARGET) $(OBJECTS)


Makefile: ../src/moka_library_1.0.2a.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		../libraries \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/staticlib.prf \
		/usr/share/qt4/mkspecs/features/static.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf
	$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile ../src/moka_library_1.0.2a.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
../libraries:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/staticlib.prf:
/usr/share/qt4/mkspecs/features/static.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
qmake:  FORCE
	@$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile ../src/moka_library_1.0.2a.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/moka1.0.0 || $(MKDIR) .tmp/moka1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/moka1.0.0/ && $(COPY_FILE) --parents ../src/moka/dataset/csvdataset.h ../src/moka/dataset/dataset.h ../src/moka/dataset/datasetdispenser.h ../src/moka/dataset/genericdataset.h ../src/moka/dataset/genericdataset_impl.h ../src/moka/dataset/multilabeledgraphdataset.h ../src/moka/dataset/vectordataset.h ../src/moka/ml/graphreservoir.h ../src/moka/ml/graphreservoir_impl.h ../src/moka/ml/linearreadout.h ../src/moka/ml/supersom.h ../src/moka/model/model.h ../src/moka/model/modeldispenser.h ../src/moka/procedure/crossvalidation.h ../src/moka/structure/graph.h ../src/moka/structure/graph_impl.h ../src/moka/structure/labeledgraph.h ../src/moka/structure/multilabeledgraph.h ../src/moka/util/info.h ../src/moka/util/info_impl.h ../src/moka/util/math.h ../src/moka/util/math_impl.h ../src/moka/util/parameters.h ../src/moka/util/timer.h ../src/moka/exception.h ../src/moka/global.h ../src/moka/global_impl.h ../src/moka/log.h ../src/moka/model/graphesnsom.h .tmp/moka1.0.0/ && $(COPY_FILE) --parents ../src/moka/dataset/csvdataset.cpp ../src/moka/dataset/dataset.cpp ../src/moka/dataset/datasetdispenser.cpp ../src/moka/dataset/genericdataset.cpp ../src/moka/dataset/multilabeledgraphdataset.cpp ../src/moka/dataset/vectordataset.cpp ../src/moka/ml/graphreservoir.cpp ../src/moka/ml/linearreadout.cpp ../src/moka/ml/supersom.cpp ../src/moka/model/model.cpp ../src/moka/model/modeldispenser.cpp ../src/moka/procedure/crossvalidation.cpp ../src/moka/structure/graph.cpp ../src/moka/structure/labeledgraph.cpp ../src/moka/structure/multilabeledgraph.cpp ../src/moka/util/parameters.cpp ../src/moka/util/info.cpp ../src/moka/util/timer.cpp ../src/moka/util/math.cpp ../src/moka/exception.cpp ../src/moka/global.cpp ../src/moka/log.cpp ../src/moka/model/graphesnsom.cpp .tmp/moka1.0.0/ && (cd `dirname .tmp/moka1.0.0` && $(TAR) moka1.0.0.tar moka1.0.0 && $(COMPRESS) moka1.0.0.tar) && $(MOVE) `dirname .tmp/moka1.0.0`/moka1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/moka1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all:
compiler_moc_header_clean:
compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: 

####### Compile

csvdataset.o: ../src/moka/dataset/csvdataset.cpp ../src/moka/dataset/csvdataset.h \
		../src/moka/dataset/dataset.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/info.h \
		../src/moka/util/info_impl.h \
		../src/moka/util/parameters.h \
		../src/moka/dataset/vectordataset.h \
		../src/moka/dataset/genericdataset.h \
		../src/moka/dataset/genericdataset_impl.h \
		../src/moka/log.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o csvdataset.o ../src/moka/dataset/csvdataset.cpp

dataset.o: ../src/moka/dataset/dataset.cpp ../src/moka/dataset/dataset.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/info.h \
		../src/moka/util/info_impl.h \
		../src/moka/util/parameters.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o dataset.o ../src/moka/dataset/dataset.cpp

datasetdispenser.o: ../src/moka/dataset/datasetdispenser.cpp ../src/moka/dataset/datasetdispenser.h \
		../src/moka/dataset/dataset.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/info.h \
		../src/moka/util/info_impl.h \
		../src/moka/util/parameters.h \
		../src/moka/dataset/csvdataset.h \
		../src/moka/dataset/vectordataset.h \
		../src/moka/dataset/genericdataset.h \
		../src/moka/dataset/genericdataset_impl.h \
		../src/moka/log.h \
		../src/moka/dataset/multilabeledgraphdataset.h \
		../src/moka/structure/multilabeledgraph.h \
		../src/moka/structure/graph.h \
		../src/moka/util/math.h \
		../src/moka/util/math_impl.h \
		../src/moka/structure/graph_impl.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o datasetdispenser.o ../src/moka/dataset/datasetdispenser.cpp

genericdataset.o: ../src/moka/dataset/genericdataset.cpp ../src/moka/dataset/genericdataset.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/dataset/dataset.h \
		../src/moka/util/info.h \
		../src/moka/util/info_impl.h \
		../src/moka/util/parameters.h \
		../src/moka/dataset/genericdataset_impl.h \
		../src/moka/log.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o genericdataset.o ../src/moka/dataset/genericdataset.cpp

multilabeledgraphdataset.o: ../src/moka/dataset/multilabeledgraphdataset.cpp ../src/moka/dataset/multilabeledgraphdataset.h \
		../src/moka/dataset/genericdataset.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/dataset/dataset.h \
		../src/moka/util/info.h \
		../src/moka/util/info_impl.h \
		../src/moka/util/parameters.h \
		../src/moka/dataset/genericdataset_impl.h \
		../src/moka/log.h \
		../src/moka/structure/multilabeledgraph.h \
		../src/moka/structure/graph.h \
		../src/moka/util/math.h \
		../src/moka/util/math_impl.h \
		../src/moka/structure/graph_impl.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o multilabeledgraphdataset.o ../src/moka/dataset/multilabeledgraphdataset.cpp

vectordataset.o: ../src/moka/dataset/vectordataset.cpp ../src/moka/dataset/vectordataset.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/dataset/genericdataset.h \
		../src/moka/dataset/dataset.h \
		../src/moka/util/info.h \
		../src/moka/util/info_impl.h \
		../src/moka/util/parameters.h \
		../src/moka/dataset/genericdataset_impl.h \
		../src/moka/log.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o vectordataset.o ../src/moka/dataset/vectordataset.cpp

graphreservoir.o: ../src/moka/ml/graphreservoir.cpp ../src/moka/ml/graphreservoir.h \
		../src/moka/exception.h \
		../src/moka/global.h \
		../src/moka/global_impl.h \
		../src/moka/structure/graph.h \
		../src/moka/util/math.h \
		../src/moka/util/math_impl.h \
		../src/moka/structure/graph_impl.h \
		../src/moka/ml/graphreservoir_impl.h \
		../src/moka/log.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o graphreservoir.o ../src/moka/ml/graphreservoir.cpp

linearreadout.o: ../src/moka/ml/linearreadout.cpp ../src/moka/ml/linearreadout.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/math.h \
		../src/moka/util/math_impl.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o linearreadout.o ../src/moka/ml/linearreadout.cpp

supersom.o: ../src/moka/ml/supersom.cpp ../src/moka/ml/supersom.h \
		../src/moka/exception.h \
		../src/moka/global.h \
		../src/moka/global_impl.h \
		../src/moka/util/math.h \
		../src/moka/util/math_impl.h \
		../src/moka/log.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o supersom.o ../src/moka/ml/supersom.cpp

model.o: ../src/moka/model/model.cpp ../src/moka/model/model.h \
		../src/moka/dataset/dataset.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/info.h \
		../src/moka/util/info_impl.h \
		../src/moka/util/parameters.h \
		../src/moka/log.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o model.o ../src/moka/model/model.cpp

modeldispenser.o: ../src/moka/model/modeldispenser.cpp ../src/moka/model/modeldispenser.h \
		../src/moka/model/model.h \
		../src/moka/dataset/dataset.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/info.h \
		../src/moka/util/info_impl.h \
		../src/moka/util/parameters.h \
		../src/moka/log.h \
		../src/moka/model/graphesnsom.h \
		../src/moka/dataset/multilabeledgraphdataset.h \
		../src/moka/dataset/genericdataset.h \
		../src/moka/dataset/genericdataset_impl.h \
		../src/moka/structure/multilabeledgraph.h \
		../src/moka/structure/graph.h \
		../src/moka/util/math.h \
		../src/moka/util/math_impl.h \
		../src/moka/structure/graph_impl.h \
		../src/moka/ml/graphreservoir.h \
		../src/moka/ml/graphreservoir_impl.h \
		../src/moka/ml/linearreadout.h \
		../src/moka/ml/supersom.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o modeldispenser.o ../src/moka/model/modeldispenser.cpp

crossvalidation.o: ../src/moka/procedure/crossvalidation.cpp ../src/moka/procedure/crossvalidation.h \
		../src/moka/dataset/dataset.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/info.h \
		../src/moka/util/info_impl.h \
		../src/moka/util/parameters.h \
		../src/moka/model/model.h \
		../src/moka/log.h \
		../src/moka/util/math.h \
		../src/moka/util/math_impl.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o crossvalidation.o ../src/moka/procedure/crossvalidation.cpp

graph.o: ../src/moka/structure/graph.cpp ../src/moka/structure/graph.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/math.h \
		../src/moka/util/math_impl.h \
		../src/moka/structure/graph_impl.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o graph.o ../src/moka/structure/graph.cpp

labeledgraph.o: ../src/moka/structure/labeledgraph.cpp ../src/moka/structure/labeledgraph.h \
		../src/moka/structure/graph.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/math.h \
		../src/moka/util/math_impl.h \
		../src/moka/structure/graph_impl.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o labeledgraph.o ../src/moka/structure/labeledgraph.cpp

multilabeledgraph.o: ../src/moka/structure/multilabeledgraph.cpp ../src/moka/structure/multilabeledgraph.h \
		../src/moka/structure/graph.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/math.h \
		../src/moka/util/math_impl.h \
		../src/moka/structure/graph_impl.h \
		../src/moka/log.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o multilabeledgraph.o ../src/moka/structure/multilabeledgraph.cpp

parameters.o: ../src/moka/util/parameters.cpp ../src/moka/util/parameters.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/info.h \
		../src/moka/util/info_impl.h \
		../src/moka/log.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o parameters.o ../src/moka/util/parameters.cpp

info.o: ../src/moka/util/info.cpp ../src/moka/util/info.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/info_impl.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o info.o ../src/moka/util/info.cpp

timer.o: ../src/moka/util/timer.cpp ../src/moka/util/timer.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/log.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o timer.o ../src/moka/util/timer.cpp

math.o: ../src/moka/util/math.cpp ../src/moka/util/math.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/util/math_impl.h \
		../src/moka/log.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o math.o ../src/moka/util/math.cpp

exception.o: ../src/moka/exception.cpp ../src/moka/exception.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o exception.o ../src/moka/exception.cpp

global.o: ../src/moka/global.cpp ../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/log.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o global.o ../src/moka/global.cpp

log.o: ../src/moka/log.cpp ../src/moka/log.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o log.o ../src/moka/log.cpp

graphesnsom.o: ../src/moka/model/graphesnsom.cpp ../src/moka/model/graphesnsom.h \
		../src/moka/dataset/multilabeledgraphdataset.h \
		../src/moka/dataset/genericdataset.h \
		../src/moka/global.h \
		../src/moka/exception.h \
		../src/moka/global_impl.h \
		../src/moka/dataset/dataset.h \
		../src/moka/util/info.h \
		../src/moka/util/info_impl.h \
		../src/moka/util/parameters.h \
		../src/moka/dataset/genericdataset_impl.h \
		../src/moka/log.h \
		../src/moka/structure/multilabeledgraph.h \
		../src/moka/structure/graph.h \
		../src/moka/util/math.h \
		../src/moka/util/math_impl.h \
		../src/moka/structure/graph_impl.h \
		../src/moka/ml/graphreservoir.h \
		../src/moka/ml/graphreservoir_impl.h \
		../src/moka/ml/linearreadout.h \
		../src/moka/ml/supersom.h \
		../src/moka/model/model.h \
		../src/moka/util/timer.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o graphesnsom.o ../src/moka/model/graphesnsom.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

