#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __linux__
    #include <GL/freeglut.h>
#elif __APPLE__
    #include "TargetConditionals.h"
    #ifdef TARGET_OS_MAC
        #include <GLUT/glut.h>
        #include <OpenGL/OpenGL.h>
    #endif
#elif defined _WIN32 || defined _WIN64
    #include <GL\glut.h>
#endif 

#include "config.d/gl2ps.h"
#include "config.d/gl2ps.c"

/* #define M_PI acos(-1.0) */

/* Output a cylinder with both lids. (Erwan Martin) */
#define SOLID_CLOSED_CYLINDER(QUAD, BASE, TOP, HEIGHT, SLICES, STACKS) \
gluCylinder(QUAD, BASE, TOP, HEIGHT, SLICES, STACKS); \
glRotatef(180, 1,0,0); \
gluDisk(QUAD, 0.0f, BASE, SLICES, 1); \
glRotatef(180, 1,0,0); \
glTranslatef(0.0f, 0.0f, HEIGHT); \
gluDisk(QUAD, 0.0f, TOP, SLICES, 1); \
glTranslatef(0.0f, 0.0f, -HEIGHT);

bool polydisperse = false;
float sAggl = 1.1; /* surface distance to consider particles as agglomerated (twice the surfactant layer) */

char fileBoxName[60] = "config.d/box-mono-uvan1.uion1.udip1.phi00470.b10000.txt";
char fileParticlesName[60] = "config.d/config-mono-uvan1.uion1.udip1.phi00470.b10000.txt";

/*
char fileBoxName[60] = "config.d/box-poli-uvan1.uion1.udip1.phi00470.b00000.txt";
char fileParticlesName[60] = "config.d/config-poli-uvan1.uion1.udip1.phi00470.b00000.txt";

char fileBoxName[60] = "config.d/box-poli-uvan1.uion1.udip1.phi00470.b10000.txt";
char fileParticlesName[60] = "config.d/config-poli-uvan1.uion1.udip1.phi00470.b10000.txt";

char fileBoxName[60] = "config.d/box-poli-uvan3.uion3.udip1.phi00470.b00000.txt";
char fileParticlesName[60] = "config.d/config-poli-uvan3.uion3.udip1.phi00470.b00000.txt";

char fileBoxName[60] = "config.d/box-poli-uvan3.uion3.udip1.phi00470.b10000.txt";
char fileParticlesName[60] = "config.d/config-poli-uvan3.uion3.udip1.phi00470.b10000.txt";
*/

void init(void)
{
	glClearColor(0.3f, 0.5f, 0.4f, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	/* glEnable(GL_LIGHTING); */
}

void display(void)
{
	float Lx, Ly, Lz;
	float diam, rx, ry, rz, mx, my, mz;
	float raio, lado;
	float L, d, ux, uy, uz;
	float omega, A[3], U[3];
	float Z[3] = {0.0f, 0.0f, 1.0f};
	int iParticle, iSurfactant, iIon;
	int nParticles, nSurfactants, nIons;
	int jParticle;
	float surfDist;

	FILE * fileBox;
	FILE * fileParticles;

	double sphereDarkness,cylinderDarkness;

	char buf1[BUFSIZ] = "Garbage";
	char buf2[BUFSIZ] = "Garbage";
	char buf3[BUFSIZ] = "Garbage";

	fileBox = fopen(fileBoxName, "r");
	printf("\n fileBox: %s",fileBoxName);

	fileParticles = fopen(fileParticlesName, "r");
	printf("\n fileParticles: %s",fileParticlesName);

	if (fileBox != NULL)
	{
		if(fscanf(fileBox, "%f %f %f", &Lx, &Ly, &Lz)==3)
		{
			printf("\n Box sides: %f %f %f", Lx, Ly, Lz);
		}
		else
		{
			printf("\n Failed to read from fileBox.\n");
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        fscanf(fileParticles, " %d", &nParticles);

        float diamip[nParticles];
        float rxip[nParticles],ryip[nParticles],rzip[nParticles];
        float mxip[nParticles],myip[nParticles],mzip[nParticles];
        bool isParticleAggl[nParticles];

	while(fscanf(fileParticles, " %f %f %f %f %f %f %f", &diam, &ry, &rz, &rx, &my, &mz, &mx)==7) /* rotated view */
	{
	
		/* Reading particles: */ 
		 
		for (iParticle = 0; iParticle < nParticles; iParticle++) 
		{
			if(iParticle == 0)
			{
				diamip[iParticle] = diam;
				if(!polydisperse) {raio = diam/2.0e0;}
				
				rxip[iParticle] = rx;
				ryip[iParticle] = ry;
				rzip[iParticle] = rz;
				mxip[iParticle] = mx;
				myip[iParticle] = my;
				mzip[iParticle] = mz;
			}
			else
			if(fscanf(fileParticles, " %f %f %f %f %f %f %f", &diam, &ry, &rz, &rx, &my, &mz, &mx)==7)
			{
				if(polydisperse)
				{
					diamip[iParticle] = diam;
					raio = diam/2.0e0;
				}
				else
				{
					diamip[iParticle] = diamip[0];
				}
				
				rxip[iParticle] = rx;
				ryip[iParticle] = ry;
				rzip[iParticle] = rz;
				mxip[iParticle] = mx;
				myip[iParticle] = my;
				mzip[iParticle] = mz;
			}
			else
			{
				printf("\n Failed to read from fileParticles.\n");
			}
		}

		/* Drawing agglomerated particles: */

		/*
      printf("\n nParticles, nSurfactants, nIons: %d %d %d", nParticles, nSurfactants, nIons);
      printf("\n Press Any Key to Continue \n");  
      getchar();
		 */

		GLdouble eqn0[nParticles][2][4];

		/* Clean the screen and the depth buffer */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();
		
		for (iParticle = 0; iParticle < nParticles-1; iParticle++) 
		{		
			isParticleAggl[iParticle] = false;
		}

		for (iParticle = 0; iParticle < nParticles-1; iParticle++) 
		{
			for (jParticle = iParticle + 1; jParticle < nParticles; jParticle++) 
			{
				surfDist = pow((rxip[iParticle]-rxip[jParticle]),2);
				surfDist += pow((ryip[iParticle]-ryip[jParticle]),2);
				surfDist += pow((rzip[iParticle]-rzip[jParticle]),2);
							
				surfDist = sqrt(surfDist);
				surfDist = surfDist - diamip[iParticle]/2 - diamip[jParticle]/2;

/* DEBUG
      printf("\n iParticle, jParticle, : %d %d", iParticle, jParticle);
      printf("\n surfDist, sAggl: %f %f", surfDist, sAggl);
      printf("\n Press Any Key to Continue \n");  
      getchar();
*/
		 
				
				if(surfDist < sAggl)
				{
					isParticleAggl[iParticle] = true;
					isParticleAggl[jParticle] = true;
				}
			}
		}

		printf("\n ");

		for (iParticle = 0; iParticle < nParticles; iParticle++) 
		{
			if(isParticleAggl[iParticle])
			{
				printf("\n Agglomerated particle: %d", iParticle);

				diam = diamip[iParticle];
				if (!polydisperse) diam = diamip[0];
				rx = rxip[iParticle];
				ry = ryip[iParticle];
				rz = rzip[iParticle];
				mx = mxip[iParticle];
				my = myip[iParticle];
				mz = mzip[iParticle];
				
				raio = diam/2;
                        
				eqn0[iParticle][0][0] = mx;
				eqn0[iParticle][0][1] = my;
				eqn0[iParticle][0][2] = mz;
				eqn0[iParticle][0][3] = 0.0;
				eqn0[iParticle][1][0] = -mx;
				eqn0[iParticle][1][1] = -my;
				eqn0[iParticle][1][2] = -mz;
				eqn0[iParticle][1][3] = 0.0;

				glPushMatrix();

				glTranslated(rx, ry, rz);

				glEnable(GL_CLIP_PLANE0);

				sphereDarkness = rz/Lz;

				if (sphereDarkness > 1) { sphereDarkness = 1;}

				glClipPlane(GL_CLIP_PLANE0, eqn0[iParticle][0]);
				if (rz/Lz) glColor3f(0.0f, 0.0f, sphereDarkness);
				glutSolidSphere(raio, 160, 160);

				glClipPlane(GL_CLIP_PLANE0, eqn0[iParticle][1]);
				if (rz/Lz) glColor3f(sphereDarkness, 0.0f, 0.0f);
				glutSolidSphere(raio, 160, 160);

				glDisable(GL_CLIP_PLANE0);

				glPopMatrix();
			}
		}

		glFlush();

		/*
      printf("\n Press Any Key to Continue \n");  
      getchar();
		 */

		glPushMatrix();

		/*      glColor3f(0.3, 0.5, 0.4); */
		glColor3f(0.7, 0.7, 0.7);

		glNormal3f(1, 0, 0);
		glBegin(GL_QUADS);
		glVertex3f(0.0f, 0.0f, Lz);
		glVertex3f(0.0f, Ly, Lz);
		glVertex3f(0.0f, Ly, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();

		glNormal3f(0, 1, 0);
		glBegin(GL_QUADS);
		glVertex3f(Lx, 0.0f, 0.0f);
		glVertex3f(Lx, 0.0f, Lz);
		glVertex3f(0.0f, 0.0f, Lz);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();

		glNormal3f(0, 0, 100);
		glBegin(GL_QUADS);
		glVertex3f(Lx, 0.0f, 0.0f);
		glVertex3f(Lx, Ly, 0.0f);
		glVertex3f(0.0f, Ly, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();

		glPopMatrix();

		glFlush();

		/*
      printf("\n Press Any Key to Continue\n");  
      getchar();
		 */

	}

	printf("\n THE END \n");

}

static void keyboard(unsigned char key, int x, int y)
{
	FILE *fp;
	int state = GL2PS_OVERFLOW, buffsize = 0;

	(void) x;
	(void) y; /* not used */

	switch (key)
	{
	case 'q':
		exit(0);
		break;
	case 's':
		fp = fopen(" ',arqeps, ' ", "wb");
		printf("Escrevendo saida em ',arqeps,' ");
		while (state == GL2PS_OVERFLOW)
		{
			buffsize += 1024 * 1024;

			gl2psBeginPage("test", "gl2psTestSimple",
					NULL, GL2PS_EPS, GL2PS_SIMPLE_SORT,
					GL2PS_DRAW_BACKGROUND | GL2PS_USE_CURRENT_VIEWPORT,
					GL_RGBA, 0, NULL, 0, 0, 0, buffsize, fp, "out.eps");

			display();
			state = gl2psEndPage();
		}
		fclose(fp);
		printf("Pronto!\n");
		break;
	}
}

void reshape(int w, int h)
{
	float Lx, Ly, Lz;

	FILE * fileBox;
	fileBox = fopen(fileBoxName, "r");

	rewind(fileBox);
	if(fscanf(fileBox, "%f %f %f", &Lx, &Ly, &Lz)==3)
	{
	}
	else
	{
		printf("\n Failed to read from fileIons.\n");
	}

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float a1,a2;

	a1 = 0.05e0;
	a2 = 1.0e0 + a1;

	if (w <= h)
		glOrtho(-a1*Lx, a2*Lx, -a1*Ly, a2*Ly *h/w, -a2*Lz, a1*Lz);
	else
		glOrtho(-a1*Lx, a2*Lx *w/h, -a1*Ly, a2*Ly, -a2*Lz, a1*Lz);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(000, 000);
	glutCreateWindow(argv[0]);
	glutReshapeFunc(reshape);
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}


