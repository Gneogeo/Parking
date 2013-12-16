#include "parking.h"
#include "mgl.h"
#include <QGridLayout>
#include <QDialog>

#include <QFileDialog>

#include "q_bsplinesurf.h"
#include "q_geometries.h"
#include "geometry.h"


static parking *form=0;

parking::parking(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	form=this;
	ui.setupUi(this);
	QGridLayout *gd=new QGridLayout(ui.centralWidget);
	Widget=new GLWidget(ui.centralWidget);
	gd->addWidget(Widget);
	Widget->show();
	Widget->setFocusPolicy(Qt::StrongFocus);
	Widget->setFocus();
	
	
	geometries=new Q_Geometries(this);
	bsplinesurf=new Q_BSplineSurf(this);

	
	geometries->setFeatures(QDockWidget::AllDockWidgetFeatures);
	addDockWidget(Qt::LeftDockWidgetArea, geometries);
	geometries->hide();
	

#if 0
	bsplinesurf->setFeatures(QDockWidget::AllDockWidgetFeatures);
	addDockWidget(Qt::LeftDockWidgetArea, bsplinesurf);

	ui.menuWindow->addAction(bsplinesurf->toggleViewAction());
#else
	bsplinesurf->hide();
#endif

	
	connect(ui.action_LoadSTL,SIGNAL(triggered()),this,SLOT(loadSTL()));
	connect(ui.action_LoadOBJ,SIGNAL(triggered()),this,SLOT(loadOBJ()));
	connect(ui.action_LoadDXF,SIGNAL(triggered()),this,SLOT(loadDXF()));
	connect(ui.action_Load3DS,SIGNAL(triggered()),this,SLOT(load3DS()));
	connect(ui.action_LoadIGES,SIGNAL(triggered()),this,SLOT(loadIGES()));

	connect(ui.actionGeometries,SIGNAL(triggered()),this,SLOT(showGeometries()));

	connect(ui.actionExit,SIGNAL(triggered()),this,SLOT(exit()));

	fixView = ui.toolBar->addAction(QString::fromLocal8Bit("Fix View"));
	orthoView_XY = ui.toolBar->addAction(QString::fromLocal8Bit("XY View"));
	orthoView_YZ = ui.toolBar->addAction(QString::fromLocal8Bit("YZ View"));
	orthoView_ZX = ui.toolBar->addAction(QString::fromLocal8Bit("ZX View"));
	
	connect(fixView,SIGNAL(triggered()),this,SLOT(fixView_clicked()));
	connect(orthoView_XY,SIGNAL(triggered()),this,SLOT(orthoView_XY_clicked()));
	connect(orthoView_YZ,SIGNAL(triggered()),this,SLOT(orthoView_YZ_clicked()));
	connect(orthoView_ZX,SIGNAL(triggered()),this,SLOT(orthoView_ZX_clicked()));
	


}

parking::~parking()
{
}


void parking::exit()
{
	QApplication::exit(0);
}

static QDir *currentDir;

QString selectOpenFileName(const QString &filter)
{
	

	if (!currentDir) {
		currentDir = new QDir(QDir::home());
	}
	QString file = QFileDialog::getOpenFileName(form, QString::fromLocal8Bit("Open File..."),currentDir->absolutePath(),filter);
	if (!file.isEmpty()) {
		currentDir->setCurrent(file);
	}
	return file;
}

QStringList selectOpenFileNames(const QString &filter)
{
	

	if (!currentDir) {
		currentDir = new QDir(QDir::home());
	}
	QStringList files = QFileDialog::getOpenFileNames(form, QString::fromLocal8Bit("Open File..."),currentDir->absolutePath(),filter);
	if (!files.isEmpty()) {
		currentDir->setCurrent(files.at(0));
	}
	return files;
}




void parking::loadSTL()
{
	QString file=selectOpenFileName(QString::fromLocal8Bit("STL Files (*.stl)"));

	if (!file.isEmpty()) {
		Geometry *geom = new Geometry;
		Widget->geomlist.push_back(geom);
		geom->loadSTL(file.toLocal8Bit().data());
		bsplinesurf->refreshFromGeometry(geom);
	}
	Widget->updateGL();
}

void parking::loadOBJ()
{
	QStringList files=selectOpenFileNames(QString::fromLocal8Bit("OBJ Files (*.obj)"));

	if (!files.isEmpty()) {
		for (int i = 0; i<files.count(); i++) {
			QString file = files.at(i);
			Geometry *geom = new Geometry;
			Widget->geomlist.push_back(geom);

			geom->loadOBJ(file.toLocal8Bit().data());
			geom->name=file;
			geometries->addGeometry(geom);
			bsplinesurf->refreshFromGeometry(geom);
		}
	}
	Widget->updateGL();
} 

void parking::loadDXF()
{
	QString file=selectOpenFileName(QString::fromLocal8Bit("DXF Files (*.dxf)"));

	if (!file.isEmpty()) {
		Geometry *geom = new Geometry;
		Widget->geomlist.push_back(geom);

		geom->loadDXF(file.toLocal8Bit().data());
		bsplinesurf->refreshFromGeometry(geom);
	}
	Widget->updateGL();
}

void parking::load3DS()
{
	QString file=selectOpenFileName(QString::fromLocal8Bit("3DS Files (*.3ds)"));

	if (!file.isEmpty()) {
		Geometry *geom = new Geometry;
		Widget->geomlist.push_back(geom);

		geom->load3DS(file.toLocal8Bit().data());
		bsplinesurf->refreshFromGeometry(geom);
	}
	Widget->updateGL();
}


void parking::loadIGES()
{
	QString file=selectOpenFileName(QString::fromLocal8Bit("IGES Files (*.igs ; *.iges)"));

	if (!file.isEmpty()) {
		Geometry *geom = new Geometry;
		Widget->geomlist.push_back(geom);

		geom->loadIGES(file.toLocal8Bit().data());
		bsplinesurf->refreshFromGeometry(geom);
	}
	Widget->updateGL();
}

void parking::showGeometries()
{
	geometries->show();

}


void parking::fixView_clicked()
{
	if (Widget) {
		Widget->fixView();
	}
}

void parking::orthoView_XY_clicked()
{
	if (Widget) {
		Widget->orthoView(GLWidget::XY);
	}
}


void parking::orthoView_YZ_clicked()
{
	if (Widget) {
		Widget->orthoView(GLWidget::YZ);
	}
}


void parking::orthoView_ZX_clicked()
{
	if (Widget) {
		Widget->orthoView(GLWidget::ZX);
	}
}
