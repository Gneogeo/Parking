#ifndef PARKING_H
#define PARKING_H


#include "ui_parking.h"

class Geometry;
class GLWidget;

class Q_Geometries;
class Q_BSplineSurf;

class parking : public QMainWindow
{
	Q_OBJECT

public:
	parking(QWidget *parent = 0, Qt::WFlags flags = 0);
	~parking();

	Q_Geometries *geometries;
	Q_BSplineSurf *bsplinesurf;

	GLWidget *Widget;

	QAction *fixView;
	QAction *orthoView_XY;
	QAction *orthoView_YZ;
	QAction *orthoView_ZX;


	public slots:
		void loadSTL();
		void loadOBJ();
		void loadDXF();
		void load3DS();
		void loadIGES();

		void showGeometries();

		void fixView_clicked();
		void orthoView_XY_clicked();
		void orthoView_YZ_clicked();
		void orthoView_ZX_clicked();



		void exit();

private:
	Ui::parkingClass ui;

};

#endif // PARKING_H
