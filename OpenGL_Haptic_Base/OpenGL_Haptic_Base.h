  /********************************/
 /*            INCLUDE           */
/********************************/

#include "stdafx.h"


  /********************************/
 /*          STRUCTURES          */
/********************************/

// Structure de la Raquette
struct Raquette
{
	hduVector3Dd posRaquette;
	hduVector3Dd angRaquette;
	hduVector3Dd posPrecRaquette;
	hduVector3Dd vitRaquette;
};

// Structure de la Balle
struct Balle
{
    hduVector3Dd posBalle;
    hduVector3Dd vitBalle;
	hduVector3Dd g;
	double r;
};


  /********************************/
 /*          PROTOTYPES          */
/********************************/

//Protypes de fonctions
void init_haptique();						//initialisation et d�marage haptique
void init_opengl(int argc, _TCHAR* argv[]);	//initialisation de l'interface

//Protypes callbacks
HDCallbackCode HDCALLBACK HapticCallback(void *pUserData);	//callback de l'interface haptique
void clavier(unsigned char touche,int x,int y);				//callback des �venements clavier
void affichage(void);										//callback de l'affichage
void mousemotion(int x,int y);								//callback du d�placement de la souris
void mouse(int button, int state,int x,int y);				//callback des �v�nements souris
void reshape (int w, int h);								//callback pour le redimentionnement de la fen�tre
void idle(void);											//callback glut quand on ne fait rien (fonction d'oisivet�)
void actualiserBalle(float deltaT);							//actualisation physique de la balle
void resetBalle(void);										//r�initialise les param�tres de la balle

/*int reconGeste(void); // Reconnaissance de gestes
void recordGeste(void); // Enregistrement de gestes
void resetGeste();  // Reinitialisation des gestes */