AM_CPPFLAGS = -I$(srcdir)/src
# OpenCV headers contain an unused typedef.  Avoid warning about it.
BASE_FLAGS = -Wall -Wno-unused-local-typedefs \
	$(STATIC_KIT_CFLAGS) $(IPP_CFLAGS) $(OPENCV_CFLAGS) \
	$(GLIB_CFLAGS) $(OPENDIAMOND_CFLAGS)
AM_CFLAGS = $(BASE_FLAGS) -Werror-implicit-function-declaration
AM_CXXFLAGS = $(BASE_FLAGS)
LDADD = $(STATIC_KIT_LIBS) $(IPP_LIBS) $(OPENCV_LIBS) $(GLIB_LIBS) \
	$(OPENDIAMOND_LIBS) -lpng -ljpeg

noinst_PROGRAMS = src/autosegmenter

src_autosegmenter_SOURCES = \
	src/autosegmenter.cpp \
	src/diamond/diamond.cpp \
	src/diamond/diamond.h \
	src/diamond/rgb.h \
	src/Clustering/ClusteringUtilities.cpp \
	src/Clustering/ClusteringUtilities.h \
	src/Clustering/KMeans.cpp \
	src/Clustering/KMeans.h \
	src/DataStructure/Array2D.h \
	src/DataStructure/Region.h \
	src/DataStructure/Signature.h \
	src/IPPInterface/IPPImage_8u.cpp \
	src/IPPInterface/IPPImage_16s.cpp \
	src/IPPInterface/IPPImage_16u.cpp \
	src/IPPInterface/IPPImage_32f.cpp \
	src/IPPInterface/IPPImage.h \
	src/math/Vector.cpp \
	src/math/Vector.h \
	src/math/Matrix.cpp \
	src/math/Matrix.h \
	src/math/MatrixOperation.cpp \
	src/math/MatrixOperation.h \
	src/inpainting/inpainter.cpp \
	src/inpainting/inpainter.h \
	src/DissimilarityMeasure/EMD.c \
	src/DissimilarityMeasure/EMD.h \
	src/DissimilarityMeasure/EMDInterface.cpp \
	src/DissimilarityMeasure/EMDInterface.h \
	src/hairRemover/hairRemover.cpp \
	src/hairRemover/hairRemover.h \
	src/segmenter/segmenter.cpp \
	src/segmenter/segmenter.h \
	src/segmenter/segmenterEnums.h
