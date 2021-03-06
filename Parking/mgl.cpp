#define WIN32_LEAN_AND_MEAN
#include <QtGui>
#include <QtOpenGL>
#include <cfloat>


#include <GL/glu.h>

#include <math.h>
#include <ctime>
#include "mgl.h"

#include "geometry.h"



float viewF=10;
const float pi=3.141593f;


GLWidget::GLWidget(QWidget *parent)
     : QGLWidget(parent)
{
     xRot = 0;
     yRot = 0;
     zRot = 0;
	 zoom=4;
}


GLWidget::~GLWidget()
{
	for (int i=0; i<geomlist.count(); i++) {
		delete geomlist.at(i);
	}

}

 
QSize GLWidget::minimumSizeHint() const
{
     return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
     return QSize(400, 400);
}

 

void GLWidget::initializeGL()
{
	GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat mat_ambient_color[] = { 0.8f, 0.8f, 0.2f, 1.0f };
	GLfloat mat_diffuse[] = { 0.1f, 0.5f, 0.8f, 1.0f };
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess[] = {30.0f};
	GLfloat light_position[] = { 0, 0, 1, 0 };
	glClearColor(1,1,1,1);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, no_mat);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, no_mat);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_mat);

	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);


	glEnable(GL_COLOR_MATERIAL);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);

	

	GLfloat pmat[16];
	glMatrixMode (GL_MODELVIEW);	
	glGetFloatv(GL_MODELVIEW_MATRIX,pmat);
	glLoadIdentity();
	glRotatef(-45.,1, 0,0);
	glRotatef(45.,0, 0,1);
	

	glMultMatrixf(pmat);
	glGetFloatv(GL_MODELVIEW_MATRIX,pmat);
	glLoadIdentity();
	transPos[0]=0; transPos[1]=0; transPos[2]=0;

	glTranslatef(transPos[0],transPos[1],transPos[2]);
	glMultMatrixf(pmat);


	
	zoom=5;

}



void GLWidget::resizeGL(int width, int height)
{
	int side = (width<height)? width : height ;
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float lim1=zoom*width/(float)side;
	float lim2=zoom*height/(float)side;

	float maxdepth=20;
	for (int i=0; i<geomlist.count(); i++) {
		Geometry *geom = geomlist.at(i);
		if (geom->visible) {

			if (maxdepth<-geom->minn[0]) maxdepth=-geom->minn[0];
			if (maxdepth<-geom->minn[1]) maxdepth=-geom->minn[1];
			if (maxdepth<-geom->minn[2]) maxdepth=-geom->minn[2];

			if (maxdepth<geom->maxx[0]) maxdepth=geom->maxx[0];
			if (maxdepth<geom->maxx[1]) maxdepth=geom->maxx[1];
			if (maxdepth<geom->maxx[2]) maxdepth=geom->maxx[2];
		}
	}
	

	glOrtho(-lim1,lim1,-lim2,lim2,-maxdepth,maxdepth);
}

static double pnt[3]={0};

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	lastReleasePos=event->pos();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	lastClickPos = event->pos();
	lastMovePos = event->pos();
	if (event->modifiers() & Qt::ShiftModifier) {
#if 0
		/*TODO*/
		int i;
		int x=lastClickPos.x();
		int y=lastClickPos.y();
		i=pickGrid(x-15,y-15,x+15,y+15);
		qDebug("%d",i);
		if (geom) geom->pickedGrid=i;
		updateGL();
#endif
	}
}



void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	
	int dx = event->x() - lastMovePos.x();
	int dy = event->y() - lastMovePos.y();

	lastMovePos=event->pos();

	GLfloat pmat[16];
	int buttons=event->buttons();
	int Ctrl=(event->modifiers() & Qt::ControlModifier) ==Qt::ControlModifier;
	

	if (buttons == Qt::LeftButton && Ctrl) {
		
			
		glMatrixMode (GL_MODELVIEW);
		glGetFloatv(GL_MODELVIEW_MATRIX,pmat);
		glLoadIdentity();

		glTranslatef(-transPos[0],-transPos[1],-transPos[2]);

		glMultMatrixf(pmat);

		glGetFloatv(GL_MODELVIEW_MATRIX,pmat);
		glLoadIdentity();
			
		glRotatef(dy,1, 0,0);
		glRotatef(dx, 0,1,0);

		glMultMatrixf(pmat);
		glGetFloatv(GL_MODELVIEW_MATRIX,pmat);
		glLoadIdentity();
		glTranslatef(transPos[0],transPos[1],transPos[2]);

		glMultMatrixf(pmat);


		

		updateGL();
	} else if (buttons == Qt::RightButton && Ctrl) {

		glMatrixMode (GL_MODELVIEW);	
		glGetFloatv(GL_MODELVIEW_MATRIX,pmat);
		glLoadIdentity();
		QSize size=this->size();
		
		glTranslatef(zoom*dx/200.,-zoom*dy/200.,0);

		transPos[0]+=zoom*dx/200.; transPos[1]-=zoom*dy/200.;

		glMultMatrixf(pmat);
		
		updateGL();
	
	} else if ((buttons & Qt::LeftButton) && (buttons & Qt::RightButton) && Ctrl) {
		zoom=zoom*(1+dx/100.);

		QSize size=this->size();
		resizeGL(size.width(),size.height());
		updateGL();
	}
}


void GLWidget::fixView()
{
	
	QSize size=this->size();
	float width=size.width();
	float height=size.height();
	
	GLfloat pmat[16];
	glMatrixMode (GL_MODELVIEW);	
	glGetFloatv(GL_MODELVIEW_MATRIX,pmat);

    unsigned int i,j;
	float xmin,xmax,ymin,ymax,zmin,zmax;
	xmin=FLT_MAX;
	xmax=-FLT_MAX;
	ymin=FLT_MAX;
	ymax=-FLT_MAX;
	zmin=FLT_MAX;
	zmax=-FLT_MAX;

	int first=1;
		
	for (int k0=0; k0<geomlist.count(); k0++) {
		Geometry *geom = geomlist.at(k0);
		if (geom->visible) {
			for (i=0; i<geom->grids.length(); i++) {
				float *pvec,vec[3];
				pvec=geom->grids.at(i).coords;

				vec[0]=pvec[0]*pmat[0]+pvec[1]*pmat[4]+pvec[2]*pmat[8]+pmat[12];
				vec[1]=pvec[0]*pmat[1]+pvec[1]*pmat[5]+pvec[2]*pmat[9]+pmat[13];
				vec[2]=pvec[0]*pmat[2]+pvec[1]*pmat[6]+pvec[2]*pmat[10]+pmat[14];

				xmin = vec[0]<xmin ? vec[0] : xmin;
				xmax = vec[0]>xmax ? vec[0] : xmax;
				ymin = vec[1]<ymin ? vec[1] : ymin;
				ymax = vec[1]>ymax ? vec[1] : ymax;
				zmin = vec[2]<zmin ? vec[2] : zmin;
				zmax = vec[2]>zmax ? vec[2] : zmax;

			}

			for (i=0; i<geom->bsplines.length(); i++) {
				float *pvec,vec[3];
				const BSpline &BS=geom->bsplines.at(i);
				for (j=0; j<BS.total_coords; j++) {
					pvec=BS.coords[j];
					vec[0]=pvec[0]*pmat[0]+pvec[1]*pmat[4]+pvec[2]*pmat[8]+pmat[12];
					vec[1]=pvec[0]*pmat[1]+pvec[1]*pmat[5]+pvec[2]*pmat[9]+pmat[13];
					vec[2]=pvec[0]*pmat[2]+pvec[1]*pmat[6]+pvec[2]*pmat[10]+pmat[14];

					xmin = vec[0]<xmin ? vec[0] : xmin;
					xmax = vec[0]>xmax ? vec[0] : xmax;
					ymin = vec[1]<ymin ? vec[1] : ymin;
					ymax = vec[1]>ymax ? vec[1] : ymax;
					zmin = vec[2]<zmin ? vec[2] : zmin;
					zmax = vec[2]>zmax ? vec[2] : zmax;
				}
			}

			for (i=0; i<geom->bsplinesurfs.length(); i++) {
				float *pvec,vec[3];
				const BSplineSurf &BSS=geom->bsplinesurfs.at(i);
				for (j=0; j<BSS.total_coords; j++) {
					pvec=BSS.coords[j];
					vec[0]=pvec[0]*pmat[0]+pvec[1]*pmat[4]+pvec[2]*pmat[8]+pmat[12];
					vec[1]=pvec[0]*pmat[1]+pvec[1]*pmat[5]+pvec[2]*pmat[9]+pmat[13];
					vec[2]=pvec[0]*pmat[2]+pvec[1]*pmat[6]+pvec[2]*pmat[10]+pmat[14];

					xmin = vec[0]<xmin ? vec[0] : xmin;
					xmax = vec[0]>xmax ? vec[0] : xmax;
					ymin = vec[1]<ymin ? vec[1] : ymin;
					ymax = vec[1]>ymax ? vec[1] : ymax;
					zmin = vec[2]<zmin ? vec[2] : zmin;
					zmax = vec[2]>zmax ? vec[2] : zmax;
				}
			}
		}
	}
	if (xmin>xmax) xmin=xmax=0;
	if (ymin>ymax) ymin=ymax=0;

	float cx=(xmin+xmax)*0.5;
	float cy=(ymin+ymax)*0.5;
	float cz=(zmin+zmax)*0.5;
	float dx=(xmax-xmin)*0.5;
	float dy=(ymax-ymin)*0.5;
	pmat[12]-=cx;
	pmat[13]-=cy;
	pmat[14]-=cz;
		
	glLoadMatrixf(pmat);

	zoom= dx > dy ? dx : dy;
	resizeGL(size.width(),size.height());


	/*
TODO: Must correct the fixView Algorithm

	[X1;1]	= [M p;0 1]*[X;1]
	X1=M*X+p
	
	[X2;1] = [P q;0 1]*[X1;1]
	X2=P*X1+q

	If glOrtho, P=[1/r 0 0;0 1/t 0;0 0 -2/(f-n)]
	q=[0; 0; -(f+n)/(f-n)];


	P*X1=[X1(1)/r; X1(2)/t 0]
	X1(1)/r
Prepei:

	-lim1<X1(1)<lim1
	-lim2<X1(2)<lim2
	
	lim1=zoom*width/side
	lim2=zoom*height/side

	

	


	
	*/
	
	updateGL();
}


void GLWidget::orthoView(GLWidget::Ortho o)
{
	GLfloat pmat[16];
	glMatrixMode (GL_MODELVIEW);
	glGetFloatv(GL_MODELVIEW_MATRIX,pmat);
	switch (o) {
		case XY :
			pmat[0]=1; pmat[4]=0; pmat[8]=0;
			pmat[1]=0; pmat[5]=1; pmat[9]=0;
			pmat[2]=0; pmat[6]=0; pmat[10]=1;
			glLoadMatrixf(pmat);
			break;
		case YZ :
			pmat[0]=0; pmat[4]=0; pmat[8]=1;
			pmat[1]=1; pmat[5]=0; pmat[9]=0;
			pmat[2]=0; pmat[6]=1; pmat[10]=0;
			glLoadMatrixf(pmat);
			break;
		case ZX :
			pmat[0]=0; pmat[4]=1; pmat[8]=0;
			pmat[1]=0; pmat[5]=0; pmat[9]=1;
			pmat[2]=1; pmat[6]=0; pmat[10]=0;
			glLoadMatrixf(pmat);
			break;
	}
	fixView();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		
	}
}

float pmat02,pmat12,pmat22;



void drawRectangle(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3,float x4,float y4,float z4)
{
	float norm[3];

	norm[0]=(y1-y3)*(z2-z4)-(z1-z3)*(y2-y4);
	norm[1]=(z1-z3)*(x2-x4)-(x1-x3)*(z2-z4);
	norm[2]=(x1-x3)*(y2-y4)-(y1-y3)*(x2-x4);
	vec_normalize(norm);
	float vert[3];

	glBegin(GL_QUADS);
	glNormal3fv(norm);
	vert[0]=x1; vert[1]=y1; vert[2]=z1; glVertex3fv(vert);
	vert[0]=x2; vert[1]=y2; vert[2]=z2; glVertex3fv(vert);
	vert[0]=x3; vert[1]=y3; vert[2]=z3; glVertex3fv(vert);
	vert[0]=x4; vert[1]=y4; vert[2]=z4; glVertex3fv(vert);
	glEnd();
}

int processHits (GLint hits, GLuint buffer[])
{
	unsigned int i, j;
	float z1,z2;
	int hit;

	float minZ;
	int ret=0;
	GLuint ii, jj, names, *ptr;
	qDebug ("hits = %d\n", hits);
	ptr = (GLuint *) buffer;
	for (i = 0; i < hits; i++) { /* for each hit */
		names = *ptr;
		qDebug (" number of names for this hit = %d\n", names);
		ptr++;
		z1=(float)*ptr/0x7fffffff;
		qDebug(" z1 is %g;", z1); 
		ptr++;
		z2=(float)*ptr/0x7fffffff;
		qDebug(" z2 is %g\n", z2); 
		ptr++;
		qDebug (" names are ");
		for (j = 0; j < names; j++) { /* for each name */
			hit=*ptr;
			qDebug ("%d ", *ptr);
			if (j == 0) /* set row and column */
			ii = *ptr;
			else if (j == 1)
			jj = *ptr;
			ptr++;
		}
		qDebug ("\n");
		if (i==0) {
			ret=hit;
			minZ=(z1<z2)?z1:z2;
		} else {
			if (z1<minZ || z2<minZ) {
				ret=hit;
				minZ=(z1<z2)?z1:z2;
			}
		}

		
	}
	qDebug("MinZ=%f, minHit=%d",minZ,ret);
	return ret;
}

int GLWidget::pickGrid(int x1,int y1,int x2,int y2)
{
#if 0
	/*TODO*/
	if (geom && geom->grids.length()) {
		int x=(x1+x2)/2;
		int y=(y1+y2)/2;
		int dx=(x2-x1)/2;
		int dy=(y2-y1)/2;
		
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT,viewport);
			
		glMatrixMode (GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluPickMatrix(x,viewport[3]+2*viewport[1]-y,dx,dy,viewport);
		float side = (size().width()<size().height())? size().width() : size().height() ;

		float lim1=zoom*size().width()/side;
		float lim2=zoom*size().height()/side;
		glOrtho(-lim1,lim1,-lim2,lim2,-20,20);

		
		unsigned int *buffer;
		int buflen=geom->grids.length();
		buffer=new unsigned int[buflen];

		glSelectBuffer(buflen,buffer);
		glRenderMode(GL_SELECT);
		glInitNames();
		
		
		glPushName(100);
		
                unsigned int k;
		for (k=0; k<geom->grids.length(); k++) {
			glLoadName(k);
			glBegin(GL_POINTS); 
			glVertex3fv(geom->grids.at(k).coords); 
			glEnd();
		}

		int hits=glRenderMode(GL_RENDER);

		GLuint *ptr;
		ptr = (GLuint *) buffer;
		int i;
		float z1,z2;
		float mz1=1e+40,mz2=1e+40;
		
		int ret=-1;
		int mret;
		for (i=0; i<hits; i++) {
			ptr++;
			z1=(float)*ptr/0x7fffffff;
			ptr++;
			z2=(float)*ptr/0x7fffffff;
			ptr++;
			mret=*ptr;
			ptr++;
			if (z1<mz1 || z2<mz2) {
				mz1=z1;
				mz2=z2;
				ret=mret;
			}
		}
		
		
		glMatrixMode (GL_PROJECTION);
		glPopMatrix();
		
		delete []buffer;
		
		return ret;
	} else return -1;
#endif
	return -1;
}


void GLWidget::paintGL()
{
	clock_t t=clock();

	float ww=3.14159f/180.f;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_LIGHTING);
	int render_mode;
	glGetIntegerv(GL_RENDER_MODE,&render_mode);

	float pmat[4][4];
	glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat *)pmat);
	pmat02=pmat[0][2]; pmat12=pmat[1][2]; pmat22=pmat[2][2];


	
	/*Drawing the corners*/
	glColor3f(1,0,0);
	
	drawRectangle(-5, 0,0,-4.5, 0,0,-4.5, 0,.5,-5, 0,.5);
	drawRectangle( 5, 0,0, 4.5, 0,0, 4.5, 0,.5, 5, 0,.5);
	drawRectangle( 5,10,0, 4.5,10,0, 4.5,10,.5, 5,10,.5);
	drawRectangle(-5,10,0,-4.5,10,0,-4.5,10,.5,-5,10,.5);

	drawRectangle(-5,0,0, -5,0.5,0, -5,0.5,.5, -5,0,.5);
	drawRectangle( 5,0,0,  5,0.5,0,  5,0.5,.5,  5,0,.5);
	drawRectangle(-5,10,0, -5,9.5,0, -5,9.5,.5, -5,10,.5);
	drawRectangle( 5,10,0,  5,9.5,0,  5,9.5,.5,  5,10,.5);

	drawRectangle(-5,0,0, -4.5,0,0, -4.5,.5,0, -5,.5,0);
	drawRectangle( 5,0,0,  4.5,0,0,  4.5,.5,0,  5,.5,0);
	drawRectangle(-5,10,0, -4.5,10,0, -4.5,9.5,0, -5,9.5,0);
	drawRectangle( 5,10,0,  4.5,10,0,  4.5,9.5,0,  5,9.5,0);

	drawRectangle(-5, 0,5,-4.5, 0,5,-4.5, 0,4.5,-5, 0,4.5);
	drawRectangle( 5, 0,5, 4.5, 0,5, 4.5, 0,4.5, 5, 0,4.5);
	drawRectangle( 5,10,5, 4.5,10,5, 4.5,10,4.5, 5,10,4.5);
	drawRectangle(-5,10,5,-4.5,10,5,-4.5,10,4.5,-5,10,4.5);

	drawRectangle(-5,0,5, -5,0.5,5, -5,0.5,4.5, -5,0,4.5);
	drawRectangle( 5,0,5,  5,0.5,5,  5,0.5,4.5,  5,0,4.5);
	drawRectangle(-5,10,5, -5,9.5,5, -5,9.5,4.5, -5,10,4.5);
	drawRectangle( 5,10,5,  5,9.5,5,  5,9.5,4.5,  5,10,4.5);

	drawRectangle(-5,0,5, -4.5,0,5, -4.5,.5,5, -5,.5,5);
	drawRectangle( 5,0,5,  4.5,0,5,  4.5,.5,5,  5,.5,5);
	drawRectangle(-5,10,5, -4.5,10,5, -4.5,9.5,5, -5,9.5,5);
	drawRectangle( 5,10,5,  4.5,10,5,  4.5,9.5,5,  5,9.5,5);



	float Origin[3],Normal[3],Perp[3];
	Origin[0]=0; Origin[1]=0; Origin[2]=0;
	Normal[0]=0; Normal[1]=0; Normal[2]=1;
	Perp[0]=1; Perp[1]=0; Perp[2]=0;

	glDisable(GL_LIGHTING);


	glLineWidth(3);
	glBegin(GL_LINES);
	glColor3f(1,0,0); glVertex3f(0,0,0); glVertex3f(1,0,0);
	glColor3f(0,1,0); glVertex3f(0,0,0); glVertex3f(0,1,0);
	glColor3f(0,0,1); glVertex3f(0,0,0); glVertex3f(0,0,1);
	
	glEnd();
	glLineWidth(1);

	glColor3f(0,0,0);

	glBegin(GL_LINES);
	glVertex3d(0,0,0);
	glVertex3dv(pnt);
	glEnd();

	for (int i=0; i<geomlist.count(); i++) {
		Geometry *geom = geomlist.at(i);
			
		if (geom->visible) {
			glEnable(GL_LIGHTING);

			if (geom->hasSmoothNormals) {
				glShadeModel(GL_SMOOTH);
			}
			glColor3f(.7f,.6f,.4f);
			
			float *norm,*cnorm;

			float cosf;


#if 0		
			glBegin(GL_TRIANGLES);
			for (int k=0; k<geom->triangles.length(); k++) {
				Triangle *tria;
				tria = &geom->triangles.at(k);
				norm=tria->normal.data;
				if (!geom->hasSmoothNormals) {
					glNormal3fv(norm);
					for (int k1=0; k1<3; k1++) {
						glVertex3fv(geom->grids.at(tria->node[k1]).coords);
					}
				} else {
					for (int k1=0; k1<3; k1++) {
						cnorm=geom->smooth_normals.at( tria->node[k1] ).data;
						cosf=norm[0]*cnorm[0]+norm[1]*cnorm[1]+norm[2]*cnorm[2];
						if (1 || (cosf>-.94 && cosf<.94)) {
							glNormal3fv(norm);
						} else {
							glNormal3fv(cnorm);
						}
						glVertex3fv(geom->grids.at(geom->triangles.at(k).node[k1]).coords);
					}
				}
			}
			glEnd();

#else
			int *vert_ar = &geom->triaStripVertex.at(0);
			int *elem_ar = &geom->triaStripElement.at(0);
			float (*normstrip)[3]=0;
			normstrip = (float(*)[3]) malloc(geom->grids.length()*sizeof(float[3]));
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3,GL_FLOAT,sizeof(Grid),&geom->grids.at(0).coords);
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT,0,normstrip);
			
			while (vert_ar[0]) {
				int total_ar=vert_ar[0]; vert_ar++; elem_ar++;
				
				for (int k=0; k<total_ar; k++) {
					memcpy(normstrip[vert_ar[k]],geom->triangles.at(elem_ar[k]).normal.data,sizeof(float[3]));
				}
				
								
				glDrawElements(GL_TRIANGLE_STRIP,total_ar,GL_UNSIGNED_INT,vert_ar);
				
				vert_ar+=total_ar;
				elem_ar+=total_ar;
			}
			free(normstrip);
#endif

			glShadeModel(GL_FLAT);

			geom->drawRevolveLines();
			geom->drawBSplineSurfs();

			glDisable(GL_LIGHTING);

		
			/*The lines should be a bit closer to viewer*/
			glMatrixMode (GL_MODELVIEW);
			glPushMatrix();
			glTranslatef(pmat02*5e-3*zoom,pmat12*5e-3*zoom,pmat22*5e-3*zoom);


			geom->drawEdgeStrip();
			geom->drawLineStrip();
			geom->drawCircles();
			geom->drawArcs();
			geom->drawSplines();
			geom->drawBSplines();
			geom->drawArcEllipses();
			geom->drawArcHyperbolas();


			glPointSize(4);
			glBegin(GL_POINTS);
			for (int k=0; k<geom->points.length(); k++) {
				glVertex3fv(geom->grids.at(geom->points.at(k)).coords);
			}
			glEnd();

			if (geom->pickedGrid!=-1) {
				glBegin(GL_POINTS);
				glVertex3fv(geom->grids.at(geom->pickedGrid).coords);
				glEnd();
			}

			glPointSize(1);

			glPopMatrix();

		}

	}

	float frame_time = (clock()-t)/(CLOCKS_PER_SEC/1000.);
	qDebug("FPS: %f",1000./frame_time);
	
}
