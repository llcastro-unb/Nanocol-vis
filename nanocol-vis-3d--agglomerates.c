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


char fileBoxName[60] = "config.d/box-poli-uvan3.uion3.udip1.phi00470.b00000.txt";
char fileParticlesName[60] = "config.d/config-poli-uvan3.uion3.udip1.phi00470.b00000.txt";

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

bool polydisperse = true;

float sAggl = 1.1; /* surface distance to consider particles as agglomerated (twice the surfactant layer) */

void init(void)
{
	glClearColor(0.3f, 0.5f, 0.4f, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glShadeModel(GL_FLAT);
	glEnable(GL_LIGHTING);
	GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0}; 
	GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};	   /* "cor" */
	GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};/* "brilho" */
	GLfloat posicaoLuz[4]={40.0, 20.0, 60.0, 1.0};
	GLfloat posicaoLuz2[4]={5.0, 10.0, 10.0, 1.0};

	/* Capacidade de brilho do material */
	GLfloat especularidade[4]={1.0,1.0,1.0,1.0}; 
	GLint especMaterial = 60;

	/* Habilita o modelo de colorização de Gouraud */
	glShadeModel(GL_SMOOTH);

	/* Define a refletância do material */
	glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
	/* Define a concentração do brilho */
	glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

	/* Ativa o uso da luz ambiente */
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	/* Define os parâmetros da luz de número 0 */
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente); 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );
	/* glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz2 ); */

	/* Habilita a definição da cor do material a partir da cor corrente */
	glEnable(GL_COLOR_MATERIAL);
	/* Habilita o uso de iluminação */
	glEnable(GL_LIGHTING);  
	/* Habilita a luz de número 0 */
	glEnable(GL_LIGHT0);
	/* Habilita o depth-buffering */
	glEnable(GL_DEPTH_TEST);
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

	while(fscanf(fileParticles, " %f %f %f %f %f %f %f", &diam, &ry, &rz, &rx, &my, &mz, &mx)==7) /* x, y, z rodados */
	{

		/* Reading particles: */ 
		 
		for (iParticle = 0; iParticle < nParticles; iParticle++) 
		{
			if(iParticle == 0)
			{
				diamip[iParticle] = diam;
				rxip[iParticle] = rx;
				ryip[iParticle] = ry;
				rzip[iParticle] = rz;
				mxip[iParticle] = mx;
				myip[iParticle] = my;
				mzip[iParticle] = mz;
			}
			else
			if(fscanf(fileParticles, " %f %f %f %f %f %f %f", &diam, &ry, &rz, &rx, &my, &mz, &mx)==7) /* x, y, z rodados */
			{
				diamip[iParticle] = diam;
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

				glClipPlane(GL_CLIP_PLANE0, eqn0[iParticle][0]);
				glColor3f(0.0f, 0.0f, 1.0f);
				glutSolidSphere(raio, 16, 16);

				glClipPlane(GL_CLIP_PLANE0, eqn0[iParticle][1]);
				glColor3f(1.0f, 0.0f, 0.0f);
				glutSolidSphere(raio, 16, 16);

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

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

		glNormal3f(0, 0, 1);
		glBegin(GL_QUADS);
		glVertex3f(Lx, 0.0f, 0.0f);
		glVertex3f(Lx, Ly, 0.0f);
		glVertex3f(0.0f, Ly, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(2);
		glColor3f(0.0, 0.0, 0.0);

		glBegin(GL_QUADS);
		glVertex3f(0.0f, 0.0f, Lz);
		glVertex3f(0.0f, Ly, Lz);
		glVertex3f(0.0f, Ly, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glVertex3f(Lx, 0.0f, Lz);
		glVertex3f(Lx, Ly, Lz);
		glVertex3f(Lx, Ly, 0.0f);
		glVertex3f(Lx, 0.0f, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glVertex3f(Lx, 0.0f, 0.0f);
		glVertex3f(Lx, 0.0f, Lz);
		glVertex3f(0.0f, 0.0f, Lz);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glVertex3f(Lx, Ly, 0.0f);
		glVertex3f(Lx, Ly, Lz);
		glVertex3f(0.0f, Ly, Lz);
		glVertex3f(0.0f, Ly, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glVertex3f(Lx, 0.0f, 0.0f);
		glVertex3f(Lx, Ly, 0.0f);
		glVertex3f(0.0f, Ly, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glVertex3f(Lx, 0.0f, Lz);
		glVertex3f(Lx, Ly, Lz);
		glVertex3f(0.0f, Ly, Lz);
		glVertex3f(0.0f, 0.0f, Lz);
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

	if (w <= h)
		glOrtho(-1.0e0 * Lx, 1.0e0 * Lx, -1.0e0 * Ly, 1.0e0 * Ly * h / w, -1.7e0 * Lz, 1.3e0 * Lz);
	else
		glOrtho(-1.0e0 * Lx, 1.0e0 * Lx * w / h, -1.0e0 * Ly, 1.0e0 * Ly, -1.7e0 * Lz, 1.3e0 * Lz);

/*	if (w <= h) 
		gluOrtho2D (-1.0e0 * Lx, 1.0e0 * Lx, -1.0e0 * Ly, 1.0e0 * Ly * h / w);
	else 
		gluOrtho2D (-1.0e0 * Lx, 1.0e0 * Lx * w / h, -1.0e0 * Ly, 1.0e0 * Ly ); */
	

	float eyeDist, atDist;

	eyeDist = 0.7e0;
	atDist = 0.5e0;

	gluLookAt(eyeDist*Lx*0.9e0,eyeDist*Ly*0.8e0,eyeDist*Lz,atDist*Lx,atDist*Ly,atDist*Lz,0,1,0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
        glTranslatef(Lx,Ly,Lz);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(000, 000);
	glutCreateWindow(argv[0]);
	glutReshapeFunc(reshape);

	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}


