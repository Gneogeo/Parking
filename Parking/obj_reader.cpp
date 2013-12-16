#include "obj_reader.h"

#include "geometry.h"

#include <stdio.h>
#include <qdebug.h>
#include <ctime>


class ObjToken {
	char *line;
public:
	ObjToken() {line=0;}
	void startTracking(char *line) {
		this->line = line;
	}
	bool getNextToken(char *out) {
		int ret = 0;
		int i = 0;

		for (;;) {
			if (line[ret]=='\0') {
				out[0]=0;
				return false;
			}
			if (line[ret]=='/') {
				out[0]='/';
				out[1]=0;
				line = &line[ret+1];
				return true;
			} else if (line[ret]!=' ') {
				out[0]=line[ret];
				ret++;
				i=1;
				break;
			} else {
				ret++;
			}
		}
		for (;;) {
			if (line[ret]=='\0') {
				out[i]=0;
				return false;
			}
			if (line[ret]=='/' || line[ret]==' ') {
				out[i]=0;
				line = &line[ret];
				return true;
			} else {
				out[i]=line[ret];
				i++;
				ret++;
			}
		}
	}
};



void readOBJ(Geometry *geom,const char *name)
{

	clock_t t=clock();
	FILE *fp=fopen(name,"rb");
	if (!fp) return;
	char line[1000];

	

	while (!feof(fp)) {
		if (!fgets(line,1000,fp)) continue;
		if (line[0]=='#') continue;

		char token[1000];

		ObjToken Tok;
		Tok.startTracking(line);
		
		Tok.getNextToken(token);

		
		if (!strcmp(token,"v")) {
			float x,y,z;
			Tok.getNextToken(token);
			x=atof(token);
			Tok.getNextToken(token);
			y=atof(token);
			Tok.getNextToken(token);
			z=atof(token);
			geom->addGrid(x,y,z);
		} else if (!strcmp(token,"f")) {
			int a,b,c;
			Tok.getNextToken(token);
			a=atoi(token);
			Tok.getNextToken(token);
			if (!strcmp(token,"/")) { /* v./.vt, v./.vt/vn, v././vn */
				Tok.getNextToken(token); 
				if (!strcmp(token,"/")) {  /*  v/./.vn  */
					Tok.getNextToken(token);
					Tok.getNextToken(token);
				} else { /* v/.vt. , v/.vt./vn  */
					Tok.getNextToken(token);
					if (!strcmp(token,"/")) { /* v/vt./.vn */
						Tok.getNextToken(token);
						Tok.getNextToken(token);
					}
				}
			}

			b=atoi(token);

			Tok.getNextToken(token);
			if (!strcmp(token,"/")) { /* v./.vt, v./.vt/vn, v././vn */
				Tok.getNextToken(token);
				if (!strcmp(token,"/")) {  /*  v/./.vn  */
					Tok.getNextToken(token);
					Tok.getNextToken(token);
				} else { /* v/.vt. , v/.vt./vn  */
					Tok.getNextToken(token);
					if (!strcmp(token,"/")) { /* v/vt/vn */
						Tok.getNextToken(token);
						Tok.getNextToken(token);
					}
				}
			}
			c=atoi(token);

			geom->addTriangle(a-1,b-1,c-1,0);

				

		}

	
	}


	fclose(fp);

	qDebug("Time to read %s OBJ: %f msec",name,(clock()-t)/(CLOCKS_PER_SEC/1000.));
}