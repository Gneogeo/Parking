#include "obj_reader.h"

#include "geometry.h"

#include <stdio.h>
#include <qdebug.h>

static char *get_next_token(char *in,char *out)
{
	int ret = 0;
	int i = 0;
	for (;;) {
		if (in[ret]=='\0') {
			out[i]=0;
			return NULL;
		}
		if (i==0) {
			if (in[ret]=='/') {
				out[0]='/';
				out[1]=0;
				return &in[1];
			} else if (in[ret]!=' ') {
				out[i]=in[ret];
				ret++;
				i++;
			} else {
				ret++;
			}
		} else {
			if (in[ret]=='/' || in[ret]==' ') {
				out[i]=0;
				return &in[ret];
			} else {
				out[i]=in[ret];
				i++;
				ret++;
			}
		}
	}
}

void readOBJ(Geometry *geom,const char *name)
{
	FILE *fp=fopen(name,"rb");
	if (!fp) return;
	char line[1000];

	while (!feof(fp)) {
		if (!fgets(line,1000,fp)) continue;
		if (line[0]=='#') continue;

		char token[1000];
		char *pline;
		pline = line;

		pline = get_next_token(pline,token);

		if (!strcmp(token,"v")) {
			float x,y,z;
			pline = get_next_token(pline,token);
			x=atof(token);
			pline = get_next_token(pline,token);
			y=atof(token);
			pline = get_next_token(pline,token);
			z=atof(token);
			geom->addGrid(x,y,z);
		} else if (!strcmp(token,"f")) {
			int a,b,c;
			pline = get_next_token(pline,token);
			a=atoi(token);
			pline = get_next_token(pline,token);
			if (!strcmp(token,"/")) { /* v./.vt, v./.vt/vn, v././vn */
				pline = get_next_token(pline,token);
				if (!strcmp(token,"/")) {  /*  v/./.vn  */
					pline = get_next_token(pline,token);
					pline = get_next_token(pline,token);
				} else { /* v/.vt. , v/.vt./vn  */
					pline = get_next_token(pline,token);
					if (!strcmp(token,"/")) { /* v/vt./.vn */
						pline = get_next_token(pline,token);
						pline = get_next_token(pline,token);
					}
				}
			}

			b=atoi(token);

			pline = get_next_token(pline,token);
			if (!strcmp(token,"/")) { /* v./.vt, v./.vt/vn, v././vn */
				pline = get_next_token(pline,token);
				if (!strcmp(token,"/")) {  /*  v/./.vn  */
					pline = get_next_token(pline,token);
					pline = get_next_token(pline,token);
				} else { /* v/.vt. , v/.vt./vn  */
					pline = get_next_token(pline,token);
					if (!strcmp(token,"/")) { /* v/vt/vn */
						pline = get_next_token(pline,token);
						pline = get_next_token(pline,token);
					}
				}
			}
			c=atoi(token);

			geom->addTriangle(a-1,b-1,c-1,0);

				

		}

	
	}


	fclose(fp);

	
}