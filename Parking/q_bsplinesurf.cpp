#include "q_bsplinesurf.h"
#include "geometry.h"

Q_BSplineSurf::Q_BSplineSurf(QWidget *parent, Qt::WFlags flags) : QDockWidget(parent) 
{  
	ui.setupUi(this);

}


Q_BSplineSurf::~Q_BSplineSurf()
{
}

void Q_BSplineSurf::refreshFromGeometry(const Geometry *geom)
{
	ui.treeWidget->clear();
	
	int i;
	for (i=0; i<geom->bsplinesurfs.length(); i++) {
		QTreeWidgetItem *item=new QTreeWidgetItem(ui.treeWidget);
		item->setText(0,QString::number(i));
	}
}
