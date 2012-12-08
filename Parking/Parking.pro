#-------------------------------------------------
#
# Project created by QtCreator 2012-11-24T13:22:32
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = Parking
TEMPLATE = app


SOURCES += chunck3ds_reader.cpp  \
	   dxf_reader.cpp  \
	   geometry.cpp  \
	   iges_reader.cpp \
	   main.cpp  \
	   mgl.cpp  \
	   parking.cpp \
	   stl_reader.cpp

HEADERS  += chunck3ds_reader.h  \
	    coord_system.h \
	    dxf_reader.h  \
	    geometry.h  \
	    iges_reader.h \
	    mgl.h  \
	    parking.h	\
	    stl_reader.h  \
	    vector3d.h

FORMS    += parking.ui
