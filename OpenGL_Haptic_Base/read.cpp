  /********************************/
 /*            INCLUDE           */
/********************************/

#include "read.h"



  /********************************/
 /*          FONCTIONS           */
/********************************/
  
void DisplayFaces(OBJ_FILE TheObj){ //Affiche toutes les faces de l'argument TheObj contenant les données
	int i=0;
	for(i=0;i<3*TheObj.Nb_face;i=i+3){
		glColor3ub(TheObj.colors[i],TheObj.colors[i+1],TheObj.colors[i+2]);	
		glNormal3f(TheObj.normals[i],TheObj.normals[i+1],TheObj.normals[i+2]);
		glVertex3f(TheObj.vertices[3*TheObj.indices[i]], TheObj.vertices[3*TheObj.indices[i]+1], TheObj.vertices[3*TheObj.indices[i]+2]);
		glVertex3f(TheObj.vertices[3*TheObj.indices[i+1]], TheObj.vertices[3*TheObj.indices[i+1]+1], TheObj.vertices[3*TheObj.indices[i+1]+2]);
		glVertex3f(TheObj.vertices[3*TheObj.indices[i+2]], TheObj.vertices[3*TheObj.indices[i+2]+1], TheObj.vertices[3*TheObj.indices[i+2]+2]);
	};
}

int readstr(FILE *f,char *string) //Lit une ligne du fichier f dans string		
{
	do
	{
		fgets(string,255,f);
	} while ((string[0] == '/') || (string[0] == '\n'));
	if(feof(f)) return 0;
	return 1;
}

int face_count(char *File_name) // Compte les faces de File_name
{

	FILE *fp;
	int i;
	char oneline[255];
	int NbVertex=0;
	int faces=0;
								
	fp=fopen(File_name,"rt");
							
	readstr(fp,oneline);
	readstr(fp,oneline);	// hold the nbr of vertices
								
	sscanf(oneline, "%d\n", &NbVertex);
								
	for(i=NbVertex;i>0;i--) readstr(fp,oneline);	// vertices not needed
								
	while(readstr(fp,oneline))	++faces;	// count faces
								
	fclose(fp);
								
	return faces;
}


//////////////////////////////////////////////////
//	Calculate the normals of each vertices		//
//	MyObj	   - The structure which hold datas //
//////////////////////////////////////////////////

void CreatNormal(OBJ_FILE MyObj)
{
	float vx1,vy1,vz1;
	float vx2,vy2,vz2;
	float longueur;
	int i=0;

	// we calculate tow vectors from the polygone
/*	for(i=0;i<3*MyObj.Nb_face;i=i+3){
		vx1=MyObj.vertices[3*MyObj.indices[i]]-MyObj.vertices[3*MyObj.indices[i+1]];
		vy1=MyObj.vertices[3*MyObj.indices[i]+1]-MyObj.vertices[3*MyObj.indices[i+1]+1];
		vz1=MyObj.vertices[3*MyObj.indices[i]+2]-MyObj.vertices[3*MyObj.indices[i+1]+2];

		vx2=MyObj.vertices[3*MyObj.indices[i+1]]-MyObj.vertices[3*MyObj.indices[i+2]];
		vy2=MyObj.vertices[3*MyObj.indices[i+1]+1]-MyObj.vertices[3*MyObj.indices[i+2]+1];
		vz2=MyObj.vertices[3*MyObj.indices[i+1]+2]-MyObj.vertices[3*MyObj.indices[i+2]+2];
	
		// We calculate the dot product between the to vectors
		MyObj.normals[i]=( (vy1*vz2) - (vy2*vz1) );
		MyObj.normals[i+1]=( (vx2*vz1) - (vx1*vz2) );
		MyObj.normals[i+2]=( (vx1*vy2) - (vx2*vy1) );
	
		// magnetude
		longueur=(float) sqrt( (MyObj.normals[i]*MyObj.normals[i]) + (MyObj.normals[i+1]*MyObj.normals[i+1]) + (MyObj.normals[i+2]*MyObj.normals[i+2]) );
	
		// we "normalize" the normal
		MyObj.normals[di]=MyObj.normals[i]/longueur;
		MyObj.normals[i+1]=MyObj.normals[i+1]/longueur;
		MyObj.normals[i+2]=MyObj.normals[i+2]/longueur;
	};
	i=i-2;	// The loop has gone too far
	MyObj.normals[i]='\0';		// we close the array
*/}

//////////////////////////////////////////////////
//	The main fonction to read an obj file		//
//	MyObj	   - The structure which hold datas //
//	File_name  - File name				        //
//////////////////////////////////////////////////

OBJ_FILE Extract_Obj_File(OBJ_FILE MyObj,char *File_name) // Lit le fichier File_name en .obj dans la variable OBJ_FILE
{
	int j=0;
	FILE *fp;		// The file
	int i;
	unsigned char tmpchar[6];	// hold one color (hexa)
	int nbcolor;		// number of digits of the color number
	char oneline[255];

	// The structure
	MyObj.Ngone=0;		// Not use in this program	
	MyObj.Nb_vertex=0;
	MyObj.Nb_face=0;
	MyObj.indices=NULL;
	MyObj.vertices=NULL;
	MyObj.normals=NULL;
	MyObj.colors=NULL;

	MyObj.Nb_face=face_count(File_name);
	
	// We count the faces and allocate the memory we need
	MyObj.indices=(int*)malloc(3*MyObj.Nb_face*sizeof(int)); 
	MyObj.normals=(float*)malloc(3*MyObj.Nb_face*sizeof(float)); 
	MyObj.colors=(int*)malloc(3*MyObj.Nb_face*sizeof(int)); 

	// We begin to read the file
	fp=fopen(File_name,"r");

	readstr(fp,oneline);	// This ligne is not usefull there
	readstr(fp,oneline);	// This ligne is usefull
	sscanf(oneline, "%d\n", &MyObj.Nb_vertex); // The number of vertices
	
	// We allocate the memory we need
	MyObj.vertices=(float*)malloc(3*MyObj.Nb_vertex*sizeof(float));

	// we stock all the vertices line after line
	for(i=MyObj.Nb_vertex;i>0;i--){
		readstr(fp,oneline);
		sscanf(oneline, "%f %f %f\n", &MyObj.vertices[j],&MyObj.vertices[j+1],&MyObj.vertices[j+2]);
		j=j+3;
	}

	MyObj.vertices[j]='\0';		// We close the array
	j=0;
	
	// we stock all the indices line after line
	while(readstr(fp,oneline)){
		
		////////////// Indices /////////
		sscanf(oneline, "%d %d %d %d 0x%s\n",&MyObj.Ngone,&MyObj.indices[j],&MyObj.indices[j+1],&MyObj.indices[j+2],tmpchar);
		
		char enterred[100];
		int inputLength = strlen(enterred)-1;

		///////////// Colors ///////////
		nbcolor=0;
		while(tmpchar[nbcolor]!='\0')	
			nbcolor=nbcolor+1;
			
		/////////Red//////
		if( isdigit(tmpchar[0]) )  
			MyObj.colors[j] = ( tmpchar[0]-48)*16;
		else MyObj.colors[j] = (( tmpchar[0]-'a'+10)*16 );
		

		if(isdigit(tmpchar[1])) 
			MyObj.colors[j] = MyObj.colors[j] + (tmpchar[1]-48) ;
		else MyObj.colors[j] = MyObj.colors[j] + (( tmpchar[1]-'a'+10) );
		
		//////////Green/////
		if( isdigit(tmpchar[2]) )  
			MyObj.colors[j+1] = ( tmpchar[2]-48)*16;
		else MyObj.colors[j+1] = (( tmpchar[2]-'a'+10)*16 );

		if(isdigit(tmpchar[3])) 
			MyObj.colors[j+1] = MyObj.colors[j+1] + (tmpchar[3]-48) ;
		else MyObj.colors[j+1] = MyObj.colors[j+1] + (( tmpchar[3]-'a'+10) );
		
		///////////Blue////////
		if(nbcolor==6)	// the blue color has 2 digits
		{
			if( isdigit(tmpchar[4]) )  
				MyObj.colors[j+2] = ( tmpchar[4]-48)*16;
			else MyObj.colors[j+2] = (( tmpchar[4]-'a'+10)*16 );
			if(isdigit(tmpchar[5])) 
				MyObj.colors[j+2] = MyObj.colors[j+2] + (tmpchar[5]-48) ;
			else MyObj.colors[j+2] = MyObj.colors[j+2] + (( tmpchar[5]-'a'+10));
		}
		else	// the blue color has 1 digit
		{
			if( isdigit(tmpchar[4]) )  
				MyObj.colors[j+2] = ( tmpchar[4]-48);
			else MyObj.colors[j+2] = ( tmpchar[4]-'a'+10);
		}			
		j=j+3;
	}

	MyObj.indices[j]='\0';		// We close the array
	MyObj.colors[j]='\0';		// We close the array

	fclose(fp);					// We close the file

	// we calculate the normals
	CreatNormal(MyObj);

	return MyObj;
}

GLuint LoadTexture( const char * filename )
{

  GLuint texture;

  int width, height;

  unsigned char * data;

  FILE * file;

  file = fopen( filename, "rb" );

  if ( file == NULL ) return 0;
  width = 1024;
  height = 512;
  data = (unsigned char *)malloc( width * height * 3 );
  //int size = fseek(file,);
  fread( data, width * height * 3, 1, file );
  fclose( file );

 for(int i = 0; i < width * height ; ++i)
{
   int index = i*3;
   unsigned char B,R;
   B = data[index];
   R = data[index+2];

   data[index] = R;
   data[index+2] = B;

}


glGenTextures( 1, &texture );
glBindTexture( GL_TEXTURE_2D, texture );
glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );


glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );
free( data );

return texture;
}