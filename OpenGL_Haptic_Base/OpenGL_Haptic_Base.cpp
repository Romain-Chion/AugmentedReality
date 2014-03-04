    /********************************/
   /*      PONG 3D de la MORT      */
  /********************************/
 /* par Lenny Bill &Romain Chion */
/********************************/

//Bonjour Lenny Bill Je suis l'interface homme machine nommé...
//Quel est mon nom?


  /********************************/
 /*            INCLUDE           */
/********************************/

#include "read.h"
#include "stdafx.h"
#include "OpenGL_Haptic_Base.h"


  /********************************/
 /*      VARIABLES GLOBALES      */
/********************************/

//Facteurs de réduction 
#define REDUC_X 150
#define REDUC_Y 100
#define REDUC_Z 60

//Gestion de la souris
int anglex=0;
int angley=0;
int xold=0;
int yold=0;
int presse;

//Gestion haptique
hduVector3Dd positions(0,0,0);
hduVector3Dd angles(0,0,0);
hduVector3Dd force(0,0,0);
int boutonClair;
int boutonFonce;

//Le Temps
int tempsMaintenant = glutGet(GLUT_ELAPSED_TIME); // On créée 2 variables contenant un temps
int tempsAvant = glutGet(GLUT_ELAPSED_TIME);
int deltaT = 0;

//Rendu

	//Textures
GLuint textures[3];
	//Fichier Objets
OBJ_FILE MyObj[3];
	//Fichiers Son
FMOD_SYSTEM  *sys;
FMOD_SOUND   *sound[3];
FMOD_CHANNEL *channel;

//Paramètres de jeu
const int maximum = 6;		//nombre maximal de balles
float maxi = 6;
int n = 1;			//nombre de balles en jeu
int energie = 0;	//jauge de pouvoir
int s;				//mode de score
int score = 0;		//score actuel
int meilleur = 0;	//meilleur score
int temps = 0;		//temps de jeu
int p = 0;			//mode pokémon
int f = 1;		//Coefficient multiplicateur (forces et frottements)
int i = 0;			//Compteurs multiballe
int j = 0;			//Compteur local
char message[25];	//Message du score

//Création de la raquette
Raquette raquette;

//Création de la balle
Balle balle[maximum];

HDSchedulerHandle gCallbackHandle = 0;


  /********************************/
 /*             MAIN             */
/********************************/

int _tmain(int argc, _TCHAR* argv[])
{
	// Position initiale de la balle
	resetBalle();

	// Initialisation quadrillage
	//resetGeste();

	init_haptique();
	init_opengl(argc, argv);

	// entrée dans la boucle principale de glut
	glutMainLoop();
	return 0;
}


  /********************************/
 /*          FONCTIONS           */
/********************************/

void init_opengl(int argc, _TCHAR* argv[])
{
	//Initialisation de l'affichage
	glutInit(&argc,(char**)argv);								//initialisations de paramètres
	glutInitDisplayMode(GLUT_DOUBLE |GLUT_RGB | GLUT_DEPTH );	//initialisations des buffers
	glutInitWindowPosition(200,200);							//initialisations des positions
	glutInitWindowSize(500,500);								//initialisation de la taille de la fenetre
	glutCreateWindow("ULTIMATE DEATH PONG 3D");					//creation de la fenetre
	
	//Initialisation d'OpenGL
	glClearColor(200.0,200.0,200.0,200.0);	// fond
	glEnable(GL_DEPTH_TEST);				// utilisation du zbuffer
	glEnable(GL_TEXTURE_2D);				// utilisation des textures
//	glEnable(GL_COLOR_SUM);

	//Initialisation des textures
	glGenTextures(4,textures);					// Création du tableau de texture
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
	//textures[0]=LoadTexture("BRICK.bmp");	// Assigner le mur
	//textures[1]=LoadTexture("DARK.bmp");	// Assigner le toit
	//textures[2]=LoadTexture("GREY.bmp");	// Assigner le sol
	
	//Initialisation des objets
	Extract_Obj_File(MyObj[0],"RAQUETTE.obj");
	//Extract_Obj_File(MyObj[1],"ASPICOT.obj");
	//Extract_Obj_File(MyObj[2],"POKEBALL.obj");

	//Initialisation des fichiers sons
	FMOD_System_Create(&sys);
	FMOD_System_Init(sys, 1, FMOD_INIT_NORMAL, NULL);
	FMOD_System_CreateStream(sys, "FIREBALL.mp3", FMOD_HARDWARE | FMOD_LOOP_OFF | FMOD_2D, 0, &sound[0]);
	FMOD_System_CreateStream(sys, "FREEZE.mp3", FMOD_HARDWARE | FMOD_LOOP_OFF | FMOD_2D, 0, &sound[1]);
	FMOD_System_CreateStream(sys, "POKEBALL.mp3", FMOD_HARDWARE | FMOD_LOOP_OFF | FMOD_2D, 0, &sound[2]);
	FMOD_Channel_SetVolume(channel, 1.0);

	//3 pixels d'épaisseur pour les points et les lignes pour des raisons de clarté
	glPointSize(3.0);					

	//enregistrement des fonctions de rappel (callback)
	glutDisplayFunc(affichage);
	glutKeyboardFunc(clavier);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutMotionFunc(mousemotion);
	glutIdleFunc(idle);
}
void idle(void)
{
	glutPostRedisplay();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); //position et taille d'image à generer 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.01, 20.0); //modification de la perspective en fonction de la taille de la fentre
   glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state,int x,int y)
{	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)	//si on appuie sur le bouton gauche
	{	presse = 1;											//le booleen presse passe a 1 (vrai)
		xold=x;												//on sauvegarde la position de la souris
		yold=y;
		glutPostRedisplay();
	}
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)		//si on relache le bouton gauche
	{	presse=0;											//le booleen presse passe a 0 (faux)
		glutPostRedisplay();
	}
	
}

void mousemotion(int x,int y)
{
	if (presse) //si le bouton gauche est pressé
	{ /* on modifie les angles de rotation de l'objet en fonction de la position
	  actuelle de la souris et de la dernière position sauvegardée */
		anglex=anglex+(x-xold);
		angley=angley+(y-yold);

		if (angley < -45) //Limite les angles de vues aux murs 
		{
			angley = -45;
		}
		else if (angley > 45)
		{
			angley = 45;
		}
		if (anglex < -25)
		{
			anglex = -25;
		}
		else if (anglex > 25)
		{
			anglex = 25;
		}

		glutPostRedisplay();	//Rafraichissement de l'affichage
	}
	xold=x; yold=y;				//Valeurs courante de le position de la souris
	glutPostRedisplay();
}

void affichage(void)
{	
	GLUquadric* params =gluNewQuadric();

	//Initialisation des pixels = effacement de la scène et du z-buffer
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	//Calcul du temps
	tempsMaintenant = glutGet(GLUT_ELAPSED_TIME);
	deltaT = tempsMaintenant - tempsAvant;
	temps += deltaT;

	//Paramètres Raquette
	raquette.posRaquette = positions;
	raquette.posRaquette[2] += 75;
	raquette.angRaquette = angles;
	//raquette.angleVirtuel[0] = 1.5*angles[0];

	raquette.angRaquette[0] /= 0.0175;  // Conversion radians degrés
	raquette.angRaquette[1] /= 0.0175;
	raquette.angRaquette[2] /= 0.0175;

	raquette.angRaquette[1] =0;			// La rotation selon l'axe de la raquette est bloquée

	//Facteur de réduction des déplacement
	raquette.posRaquette[0] /= REDUC_X;
	raquette.posRaquette[1] /= REDUC_Y;
	raquette.posRaquette[2] /= REDUC_Z;

	if ((boutonClair=0)&&(energie>20)) //Fireball
	{
		energie -= 5*deltaT;
		FMOD_System_PlaySound(sys, FMOD_CHANNEL_FREE, sound[0], 0, &channel);
//		glSecondaryColor3i(1,0,0);
		textures[3]=textures[0];
		textures[0]=LoadTexture("FIRE.bmp");
	}
	else if ((boutonFonce=0)&&(energie>20)) //Freeze
	{
		energie -= 5*deltaT;
		FMOD_System_PlaySound(sys, FMOD_CHANNEL_FREE, sound[1], 0, &channel);
//		glSecondaryColor3i(0,0,1);
		textures[3]=textures[0];
		textures[0]=LoadTexture("ICE.bmp");
	}
	else
	{
		textures[0]=textures[3];
	}

	glPushMatrix();
		glRasterPos3f(-0.4f,-0.4f,-1.);
		if (energie+deltaT<100) energie += deltaT;
		else energie = 100;
		glColor3ub(0,255,0);
		glVertex3f(0,0,0);
		glVertex3f(0,energie,0);
		glVertex3f(5,energie,0);
		glVertex3f(5,0,0);
		glColor3ub(255,255,255);
		glVertex3f(0,100,0);
		glVertex3f(0,energie,0);
		glVertex3f(5,energie,0);
		glVertex3f(5,100,0);
	glPopMatrix();

	glLoadIdentity();
    gluLookAt (sin(anglex/100.0)*3, angley/100.0, cos(anglex/100.0)*3, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //point de vue
	
	//Energie

	//Score
	

		glPushMatrix();
			glColor3ub(255,255,255);
			glLoadIdentity();
			glRasterPos3f(-0.4f,-0.4f,-1.);									//déplace la zone de texte
			sprintf (message,"score: %d meilleur: %d",score,meilleur);		//texte à afficher
			for (i=0;i<strlen(message);i++)									//défile les characteres
			{
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*(message+i));
			}
		glPopMatrix();


		glPushMatrix();
			glColor3ub(255,255,255);
			glLoadIdentity();
			glRasterPos3f(-0.4f,-0.4f,-1.);									//déplace la zone de texte
			sprintf (message,"score: %d temps: %d s",score,temps);			//texte à afficher
			for (i=0;i<strlen(message);i++)									//défile les characteres
			{
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*(message+i));
			}
		glPopMatrix();


	// Murs

		//Fond
	glBegin(GL_QUADS);
		glColor3ub(0,0,153);
		//glBindTexture(GL_TEXTURE_2D,textures[0]);
		glTexCoord2f(0.0,0.0);glVertex3f(-0.75,-0.5,0);
		glTexCoord2f(0.0,1.0);glVertex3f(-0.75,0.5,0);
		glTexCoord2f(1.0,1.0);glVertex3f(0.75,0.5,0);
		glTexCoord2f(1.0,0.0);glVertex3f(0.75,-0.5,0);
	glEnd();

		//Arrière
	/*glBegin(GL_POLYGON);
		glVertex3f(0.75,-0.5,3);
		glVertex3f(0.75,0.5,3);
		glVertex3f(-0.75,0.5,3);
		glVertex3f(-0.75,-0.5,3);
	glEnd();*/

		//Droite
	glBegin(GL_POLYGON);
		glColor3ub(51,51,255);
		//glBindTexture(GL_TEXTURE_2D,textures[0]);
		glTexCoord2f(1.0,0.0);glVertex3f(0.75,0.5,0);
		glTexCoord2f(0.0,0.0);glVertex3f(0.75,-0.5,0);
		glColor3ub(51,153,255);
		glTexCoord2f(0.0,1.0);glVertex3f(0.75,-0.5,3);
		glTexCoord2f(1.0,1.0);glVertex3f(0.75,0.5,3);
	glEnd();

		//Gauche
	glBegin(GL_POLYGON);
		glColor3ub(51,51,255);
		//glBindTexture(GL_TEXTURE_2D,textures[0]);
		glTexCoord2f(1.0,0.0);glVertex3f(-0.75,0.5,0);
		glTexCoord2f(1.0,0.0);glVertex3f(-0.75,-0.5,0);
		glColor3ub(51,153,255);
		glTexCoord2f(0.0,1.0);glVertex3f(-0.75,-0.5,3);
		glTexCoord2f(1.0,1.0);glVertex3f(-0.75,0.5,3);
	glEnd();

		//Sol
	glBegin(GL_POLYGON);
		glColor3ub(255,153,0);
		//glBindTexture(GL_TEXTURE_2D,textures[2]);
		glTexCoord2f(1.0,0.0);glVertex3f(0.75,-0.5,0);
		glTexCoord2f(0.0,0.0);glVertex3f(-0.75,-0.5,0);
		glColor3ub(255,204,51);
		glTexCoord2f(0.0,1.0);glVertex3f(-0.75,-0.5,3);
		glTexCoord2f(1.0,1.0);glVertex3f(0.75,-0.5,3);
	glEnd();
	
		//Plafond
	glBegin(GL_POLYGON);
		glColor3ub(255,204,0);
		//glBindTexture(GL_TEXTURE_2D,textures[1]);
		glTexCoord2f(1.0,0);glVertex3f(0.75,0.5,0);
		glTexCoord2f(0.0,0.0);glVertex3f(-0.75,0.5,0);
		glColor3ub(255,255,204);
		glTexCoord2f(0.0,1.0);glVertex3f(-0.75,0.5,3);
		glTexCoord2f(1.0,1.0);glVertex3f(0.75,0.5,3);
	glEnd();

	// Raquette

		//Affichage
	glPushMatrix();
		glTranslatef(raquette.posRaquette[0],raquette.posRaquette[1],raquette.posRaquette[2]);
		glRotatef(-90,0,1,0);
		glRotatef(raquette.angRaquette[0],0,1,0);	// Rotation selon une droite perpendiculaire au sol
		glRotatef(raquette.angRaquette[1],0,0,1);	// Rotation selon l'axe de la raquette (Bloqué)
		glRotatef(raquette.angRaquette[2],1,0,0);	// Rotation selon l'autre droite
		glBegin(GL_TRIANGLES);						// Affichages des petits polygones
			// Initialisation gluQuadric
		params = gluNewQuadric();
		gluQuadricDrawStyle(params,GLU_FILL);
		
		// MancheRaquette
		glColor3ub(136,66,29);
		gluCylinder(params,0.02,0.02,0.1,20,1);
		
		// Bouchon manche
		glPushMatrix();
			glColor3ub(240,195,0);
			glTranslatef(0,0,0.1);
			gluDisk(params,0,0.02,20,1);
		glPopMatrix();

		// Tamis de la raquette 
		glPushMatrix();
			glPushMatrix();
				glColor3ub(240,195,0);
				glTranslatef(-0.01,0,-0.08); // On décale du rayon par rapport au bout du manche, et on recentre le tamis
				glRotatef(90,0,1,0);
				gluCylinder(params,0.08,0.08,0.02,20,1);
			glPopMatrix();
			glPushMatrix();
				glColor3ub(10,10,10);
				glTranslatef(0.01,0,-0.08);
				glRotatef(90,0,1,0);
				gluDisk(params,0,0.08,20,1);
			glPopMatrix();
			glPushMatrix();
				glColor3ub(219,23,2);
				glTranslatef(-0.01,0,-0.08);
				glRotatef(90,0,1,0);
				gluDisk(params,0,0.08,20,1);
			glPopMatrix();
		glPopMatrix();
			if (p=1) DisplayFaces(MyObj[1]);
			else DisplayFaces(MyObj[0]);
		glEnd();
	glPopMatrix();

		//Ombre
	glPushMatrix();
		glTranslatef(raquette.posRaquette[0],-0.49,raquette.posRaquette[2]);
		glRotatef(90,1,0,0);
		glColor3ub(50,50,50);
		gluDisk(params,0,0.01,20,1);
	glPopMatrix();

		// BoundingBox raquette 
//	glPushMatrix();
//		glBegin(GL_POLYGON);			// boundingBox face normale
//			glColor3ub(255,255,255);
//			glVertex3f(raquette.posVirtuelle[0]/*-0.08*sin(0.01745*raquette.angleVirtuel[1])*/,raquette.posVirtuelle[1]/*-0.08*cos(0.01745*raquette.angleVirtuel[1])*/,raquette.posVirtuelle[2]/*+0.08*abs(sin(0.01745*raquette.angleVirtuel[1]))*/);
//			glVertex3f(raquette.posVirtuelle[0]/*+0.08*sin(0.01745*raquette.angleVirtuel[1])*/,raquette.posVirtuelle[1]/*+0.08*cos(0.01745*raquette.angleVirtuel[1])*/,raquette.posVirtuelle[2]/*-0.08*abs(sin(0.01745*raquette.angleVirtuel[1]))*/);
//			glVertex3f(raquette.posVirtuelle[0]+0.16*cos(0.01745*raquette.angleVirtuel[0])/*+0.08*sin(0.01745*raquette.angleVirtuel[1])*/,raquette.posVirtuelle[1]+0.08*cos(0.01745*raquette.angleVirtuel[1]),raquette.posVirtuelle[2]-0.16*sin(0.01745*raquette.angleVirtuel[0])/*-0.08*abs(sin(0.01745*raquette.angleVirtuel[1]))*/);
//			glVertex3f(raquette.posVirtuelle[0]+0.16*cos(0.01745*raquette.angleVirtuel[0])/*-0.08*sin(0.01745*raquette.angleVirtuel[1])*/,raquette.posVirtuelle[1]-0.08*cos(0.01745*raquette.angleVirtuel[1]),raquette.posVirtuelle[2]-0.16*sin(0.01745*raquette.angleVirtuel[0])/*+0.08*abs(sin(0.01745*raquette.angleVirtuel[1]))*/);
//		glEnd();
//	glPopMatrix();

	// Balle

		//Actualisation des paramètres
	actualiserBalle(deltaT);
	for(i=0;i<n;i++)
	{
		//Affichage
		glPushMatrix();
			glTranslatef(balle[i].posBalle[0],balle[i].posBalle[1],balle[i].posBalle[2]);
			glColor3ub(0,125*((float) i)/(maxi),250*((float) i)/(maxi));
			glutSolidSphere(balle[i].r,20,20);
		glPopMatrix();

		//Ombre
		glPushMatrix();
			glTranslatef(balle[i].posBalle[0],-0.49,balle[i].posBalle[2]);
			glRotatef(90,1,0,0);
			glColor3ub(40,40,40);
			gluDisk(params,0,balle[i].r-0.03*(balle[i].posBalle[1]+0.5),20,1);
		glPopMatrix();
	}

	// On échange le buffer de dessin et d'affichage
	glutSwapBuffers(); //en double buffer
	tempsAvant = tempsMaintenant;
}

void resetBalle(void)
{
	for(i=0;i<maximum;i++)
	{
		balle[i].r=0.04;
		balle[i].posBalle[0]=(1.25*((float) i/(maxi))-0.50);	
		balle[i].posBalle[1]=0;
		balle[i].posBalle[2]=0.3;
		balle[i].vitBalle[0]=0.0005;
		balle[i].vitBalle[1]=0.0005;
		balle[i].vitBalle[2]=0.0005;
		balle[i].g[0]=0;
		balle[i].g[1]=0.0000025;
		balle[i].g[2]=0;
	}
}


void actualiserBalle(float deltaT)
{
	hduVector3Dd posBalle_Raquette; // Balle dans le repere de la raquette
	hduVector3Dd vitBalle_Raquette;
	
		// Calcul de la vitesse de la raquette;
	raquette.vitRaquette[0] = (raquette.posRaquette[0] - raquette.posPrecRaquette[0])/deltaT;
	raquette.vitRaquette[1] = (raquette.posRaquette[1] - raquette.posPrecRaquette[1])/deltaT;
	raquette.vitRaquette[2] = (raquette.posRaquette[2] - raquette.posPrecRaquette[2])/deltaT;
	raquette.posPrecRaquette = raquette.posRaquette;

	for (i=0;i<n;i++)
	{
		if (boutonClair)
		{
			//FIREBALL!!!
			balle[i].vitBalle[1] *= 1.05;
			balle[i].g[1]=0;
			f=1;
		}
	
		else if (boutonFonce)
		{
			//Freeze...
			f=0.99;
		}

		// Gravité
		balle[i].vitBalle += - balle[i].g*deltaT;
		balle[i].vitBalle *= f;

		// Actualisation de la position de la balle
		balle[i].posBalle[0] += balle[i].vitBalle[0]*deltaT;
		balle[i].posBalle[1] += balle[i].vitBalle[1]*deltaT;
		balle[i].posBalle[2] += balle[i].vitBalle[2]*deltaT;
			
		//Collisions

			// Murs
	
			if (fabs(balle[i].posBalle[0])>=0.69)	balle[i].vitBalle[0] = -balle[i].vitBalle[0]; //cotés
			if (fabs(balle[i].posBalle[1])>=0.44)	balle[i].vitBalle[1] = -balle[i].vitBalle[1]; //sol&plafond
			if (balle[i].posBalle[2]<=0.05)		
			{
				balle[i].vitBalle[2] = -balle[i].vitBalle[2];	//mur du fond
				score+=1;										//amélioration du score
				if (score>meilleur) meilleur=score;
			}
			if (balle[i].posBalle[2]>=2.95) 
			{
				balle[i].vitBalle[2] = -balle[i].vitBalle[2];	//mur arrière
				score=0;										//remise à zéro du score
			}

			if ((balle[i].posBalle[1]<=-0.46)&&(balle[i].vitBalle[1]+balle[i].vitBalle[2]+balle[i].vitBalle[3]<=0.00001))
			{
				balle[i].posBalle[1]=-0.45;
				balle[i].vitBalle[1]=0;
				balle[i].g[1]=0;
			}

			// Raquette

			for (j=0 ; j<3 ; j++)
			{
				posBalle_Raquette[j] = balle[i].posBalle[j] - raquette.posRaquette[j];
				vitBalle_Raquette[j] = balle[i].vitBalle[j] - 0.2*raquette.vitRaquette[j];
			}
	
			if (posBalle_Raquette[0]*posBalle_Raquette[0] + posBalle_Raquette[1]*posBalle_Raquette[1] <= 0.2*0.2 && abs(posBalle_Raquette[2] - balle[i].r) < 0.1)
			{
				force=vitBalle_Raquette*deltaT*50;
				vitBalle_Raquette[2] *= -0.5;
	
				for (j=0 ; j<3 ; j++)
				{
					balle[i].vitBalle[j] = vitBalle_Raquette[j] + 0.4*raquette.vitRaquette[j];
				}
			}
			else force*=0;
	}
}

void clavier(unsigned char touche,int x,int y)
{
	switch (touche)
	{
	case 'r' :
	case 'R' : // Reset de la balle
		textures[0]=LoadTexture("WALL.bmp");
		resetBalle(); break;
	case 's' :
	case 'S' :
		if (s<0) s=(s+1)%3;
		break;
	case 'm' :
	case 'M' : // balle de tailles multiples
		for (i=0;i<maximum;i++)
		{
			balle[i].r=0.035*((float) i)/(maxi)+0.015;
		}; break;
	case 'p' :
	case 'P' : // Pokemon
			FMOD_System_PlaySound(sys, FMOD_CHANNEL_FREE, sound[2], 0, &channel);
			textures[0]=LoadTexture("PIKA.bmp");
			break;
	case 'c' :
	case 'C' : // Carrés pleins
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glutPostRedisplay(); break;
	case 'b' :
	case 'B' : // Supprime une balle
		if (n>1) n--; break;
	case 'n' :
	case 'N' : // Nouvelle balle
		if (n<maximum) n++; break;
	case 'f' :
	case 'F' : // Fil de fer
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glutPostRedisplay(); break;
	case 'g' :
	case 'G' : // Green texture
		textures[0]=LoadTexture("IVY.bmp");
		textures[2]=LoadTexture("HERB.bmp");
		break;
	case 'w' :
	case 'W' : // Wall texture
		textures[0]=LoadTexture("BRICK.bmp");
		textures[1]=LoadTexture("DARK.bmp");
		textures[2]=LoadTexture("GREY.bmp");break;
	case 'e' :
	case 'E' : // Plein écran
		glutFullScreen(); break;
	case 27  : // Retourne en mode fenêtre
		glutReshapeWindow(640,480);
		glutPositionWindow(100,100); break;
	case 'h' :
	case 'H' : // Help
		break;
	case 'q' :
	case 'Q' : // Quitte le programme
		exit(0);
	}
}

void init_haptique()
{
	//init phantom
	HDErrorInfo error;

    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        fprintf(stderr, "\nPas d'interface\n");
       	}
	else
	{
		fprintf(stderr, "\nInterface détectée\n");
    
	gCallbackHandle = hdScheduleAsynchronous(
        HapticCallback, 0, HD_MAX_SCHEDULER_PRIORITY);
	hdEnable(HD_FORCE_OUTPUT);
	hdStartScheduler();
	

    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        fprintf(stderr, "\nErreur hdStartScheduler\n");
    }
	}
}

HDCallbackCode HDCALLBACK HapticCallback(void *pUserData)
{
       
    HDint nCurrentButtons, nLastButtons;
	
    hdBeginFrame(hdGetCurrentDevice()); // on prend un instantané de l'état de l'interface (positions)

	hdGetDoublev(HD_CURRENT_POSITION,positions); // on récupère les infos
    hdGetIntegerv(HD_CURRENT_BUTTONS, &nCurrentButtons);
	hdGetIntegerv(HD_LAST_BUTTONS, &nLastButtons);
	hdGetDoublev(HD_CURRENT_GIMBAL_ANGLES,angles);	

	// Definition physique des murs de la salle
	if (positions[2] <= -75)
	{
		force[2] = -positions[2]-75;
	}

	if (positions[0] >= 0.75*REDUC_X)
	{
		force[0] = -positions[0]+0.75*REDUC_X;
	}
	else if (positions[0] <= -0.75*REDUC_X)
	{
		force[0] = -positions[0]-0.75*REDUC_X;
	}

	if (positions[1] <= -0.5*REDUC_Y)
	{
		force[1] = -positions[1]-0.5*REDUC_Y;
	}
	else if (positions[1] >= 0.5*REDUC_Y)
	{
		force[1] = -positions[1]+0.5*REDUC_Y;
	}

	// Gestion des boutons
	if ((nCurrentButtons & HD_DEVICE_BUTTON_2) != 0)
	{
		boutonClair = 1;
	}
	else
	{
		boutonClair = 0;
	}
	if ((nCurrentButtons & HD_DEVICE_BUTTON_1) != 0 && (nLastButtons & HD_DEVICE_BUTTON_1)==0)
	{
		boutonFonce= 1;
	}
	else
	{
		boutonFonce = 0;
	}
	hdSetDoublev(HD_CURRENT_FORCE, force);
	hdEndFrame(hdGetCurrentDevice());// on met à jour l'état de l'interface (application des forces)
    return HD_CALLBACK_CONTINUE;
};




