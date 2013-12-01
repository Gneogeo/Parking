#-------------------------------------------------
#
# Project created by QtCreator 2012-11-24T13:22:32
#
#-------------------------------------------------

QT       += core gui opengl

#LIBS += -lGLU

TARGET = Parking
TEMPLATE = app


SOURCES += bspline.cpp \
	   chunck3ds_reader.cpp  \
	   dxf_reader.cpp  \
	   geometry.cpp  \
	   iges_reader.cpp \
	   main.cpp  \
	   mgl.cpp  \
	   parking.cpp \
	   q_bsplinesurf.cpp \
	   stl_reader.cpp \
    conic.cpp

HEADERS  += bspline.h \
	    chunck3ds_reader.h  \
	    coord_system.h \
	    dxf_reader.h  \
	    geometry.h  \
	    iges_reader.h \
	    matrix.h \
	    mgl.h  \
	    myvector.h  \
	    parking.h	\
	    q_bsplinesurf.h \
	    stl_reader.h  \
	    vector3d.h \
    conic.h

FORMS    += parking.ui \
	    q_bsplinesurf.ui
