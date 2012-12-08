#include "parking.h"
#include "mgl.h"
#include <QGridLayout>
#include <QDialog>

#include <QFileDialog>

#include "geometry.h"

parking::parking(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	QGridLayout *gd=new QGridLayout(ui.centralWidget);
	Widget=new GLWidget(ui.centralWidget);
	gd->addWidget(Widget);
	Widget->show();
	Widget->setFocusPolicy(Qt::StrongFocus);
	Widget->setFocus();


	Widget->geom=0;
	
	connect(ui.action_LoadSTL,SIGNAL(triggered()),this,SLOT(loadSTL()));
	connect(ui.action_LoadDXF,SIGNAL(triggered()),this,SLOT(loadDXF()));
	connect(ui.action_Load3DS,SIGNAL(triggered()),this,SLOT(load3DS()));
	connect(ui.action_LoadIGES,SIGNAL(triggered()),this,SLOT(loadIGES()));

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


void parking::loadSTL()
{
	QString file=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("Open File..."),
		QString::fromLocal8Bit(""),QString::fromLocal8Bit("STL Files (*.stl)"));
	if (!file.isEmpty()) {
		if (!Widget->geom) {
			delete Widget->geom;
		}
		Widget->geom=new Geometry;

		Widget->geom->loadSTL(file.toLocal8Bit().data());
	}
	
}

void parking::loadDXF()
{
	QString file=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("Open File..."),
		QString::fromLocal8Bit(""),QString::fromLocal8Bit("DXF Files (*.dxf)"));
	if (!file.isEmpty()) {
		if (!Widget->geom) {
			delete Widget->geom;
		}
		Widget->geom=new Geometry;

		Widget->geom->loadDXF(file.toLocal8Bit().data());
	}
}

void parking::load3DS()
{
	QString file=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("Open File..."),
		QString::fromLocal8Bit(""),QString::fromLocal8Bit("3DS Files (*.3ds)"));
	if (!file.isEmpty()) {
		if (!Widget->geom) {
			delete Widget->geom;
		}
		Widget->geom=new Geometry;

		Widget->geom->load3DS(file.toLocal8Bit().data());
	}
}


void parking::loadIGES()
{
        QString file=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("Open File..."),
                QString::fromLocal8Bit(""),QString::fromLocal8Bit("IGES Files (*.ig*s)"));
        if (!file.isEmpty()) {
                if (!Widget->geom) {
                        delete Widget->geom;
                }
                Widget->geom=new Geometry;

                Widget->geom->loadIGES(file.toLocal8Bit().data());
        }
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
