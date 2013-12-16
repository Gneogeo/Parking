#ifndef MGL_H
#define MGL_H

#include <QGLWidget>
#include <QVector>

class Geometry;

class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent = 0);
	~GLWidget();
	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	QVector<Geometry *>geomlist;
	
	int pickGrid(int x1,int y1,int x2,int y2);

	void fixView();

	enum Ortho {
		XY,
		YZ,
		ZX
	};

	void orthoView(Ortho o);
	


protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent( QKeyEvent * event );

private:

	float xRot;
	float yRot;
	float zRot;
	QPoint lastClickPos;
	QPoint lastMovePos;
	QPoint lastReleasePos;
	float zoom;

	float transPos[3];

};




#endif
