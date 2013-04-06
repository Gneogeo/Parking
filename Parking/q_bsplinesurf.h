#ifndef BSPLINESURF_H
#define BSPLINESURF_H

#include "ui_q_bsplinesurf.h"


class Geometry;

class Q_BSplineSurf : public QDockWidget
{
	Q_OBJECT

	public:
		Q_BSplineSurf(QWidget *parent = 0, Qt::WFlags flags = 0);
		~Q_BSplineSurf();
		void refreshFromGeometry(const Geometry *geom);


	private:
		Ui::Q_BSplineSurfClass ui;

};



#endif
