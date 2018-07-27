#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include <GL/glew.h>
#include <freeglut/glut.h>
#include "textfile.h"
#include "GLM.h"

#ifndef GLUT_WHEEL_UP
# define GLUT_WHEEL_UP   0x0003
# define GLUT_WHEEL_DOWN 0x0004
#endif

#ifndef max
# define max(a,b) (((a)>(b))?(a):(b))
# define min(a,b) (((a)<(b))?(a):(b))
#endif

// Shader attributes
GLint iLocPosition;
GLint iLocColor;

char* filename = "ColorModels/bunny5KC.obj";
char* files[] = {	"ColorModels/bunny5KC.obj",
					"ColorModels/teapot4KC.obj",
					"ColorModels/frog2KC.obj",
					"ColorModels/brain18KC.obj",
					"ColorModels/dragon10KC.obj",
					"ColorModels/hippo23KC.obj",
					"ColorModels/armadillo12KC.obj",
					"ColorModels/santa7KC.obj"
					};


GLMmodel* OBJ;
GLfloat* vertices;
GLfloat* colors;


void normalized (GLfloat scale){

	for(int p=0;p<(int)OBJ->numtriangles*9;p++){
		vertices[p] = (GLfloat)vertices[p+0]*scale;
		
	}

}

void displace(GLfloat dx,GLfloat dy,GLfloat dz){
	for(int p=0;p<(int)OBJ->numtriangles*9;p+=3){
		vertices[p] -=dx;
		vertices[p+1] -=dy;
		vertices[p+2] -= dz;
	}
}




void colorModel()
{
	int i;

	// TODO:
	//// You should traverse the vertices and the colors of each vertex.
	//// Normalize each vertex into [-1, 1], which will fit the camera clipping window.

	// number of triangles
	OBJ->numtriangles;

	// number of vertices
	
	// The center position of the model
	// (should be calculated by yourself)
	GLfloat sx,sy,sz;
	sx=OBJ->position[0];
	sy=OBJ->position[1];
	sz=OBJ->position[2];
	printf("%f %f %f \n",sx,sy,sz);
   
	
	int j=0;
	vertices = new GLfloat[(int)OBJ->numtriangles*9];
	colors = new GLfloat[(int)OBJ->numtriangles*9];
	GLfloat maxx=-1e9,maxy=-1e9,maxz=-1e9,minx=1e9,miny=1e9,minz=1e9;
	
	for(i=0; i<(int)OBJ->numtriangles; i++,j+=3)
	{
		// the index of each vertex
		int indv1 = OBJ->triangles[i].vindices[0];
		int indv2 = OBJ->triangles[i].vindices[1];
		int indv3 = OBJ->triangles[i].vindices[2];

		

		// the index of each color
		int indc1 = indv1;
		int indc2 = indv2;
		int indc3 = indv3;

		// vertices
		GLfloat vx, vy, vz;
		vx = OBJ->vertices[indv1*3+0];
		vy= OBJ->vertices[indv1*3+1];
		vz= OBJ->vertices[indv1*3+2];
		vertices[j*3+0] = vx;
		vertices[j*3+1] = vy;
		vertices[j*3+2] = vz;
		if(vx>maxx)maxx=vx;
		if(vx<minx)minx=vx;

		if(vy>maxy)maxy=vy;
		if(vy<miny)miny=vy;

		if(vz>maxz)maxz=vz;
		if(vz<minz)minz=vz;

		//printf("%f %f %f \n",vertices[j*3+0],vertices[j*3+1],vertices[j*3+2]);
	

		vx= OBJ->vertices[indv2*3+0];
		vy= OBJ->vertices[indv2*3+1];
		vz= OBJ->vertices[indv2*3+2];
		
		vertices[(j+1)*3+0] = vx;
		vertices[(j+1)*3+1] = vy;
		vertices[(j+1)*3+2] = vz;

		if(vx>maxx)maxx=vx;
		if(vx<minx)minx=vx;

		if(vy>maxy)maxy=vy;
		if(vy<miny)miny=vy;

		if(vz>maxz)maxz=vz;
		if(vz<minz)minz=vz;


		vx= OBJ->vertices[indv3*3+0];
		vy= OBJ->vertices[indv3*3+1];
		vz= OBJ->vertices[indv3*3+2];
		
		vertices[(j+2)*3+0] = vx;
		vertices[(j+2)*3+1] = vy;
		vertices[(j+2)*3+2] = vz;

		if(vx>maxx)maxx=vx;
		if(vx<minx)minx=vx;


		if(vy>maxy)maxy=vy;
		if(vy<miny)miny=vy;

		if(vz>maxz)maxz=vz;
		if(vz<minz)minz=vz;
		
		// colors
		GLfloat c1, c2, c3;
		colors[j*3+0] = OBJ->colors[indv1*3+0];
		colors[j*3+1] = OBJ->colors[indv1*3+1];
		colors[j*3+2] = OBJ->colors[indv1*3+2];

		colors[(j+1)*3+0] = OBJ->colors[indv2*3+0];
		colors[(j+1)*3+1] = OBJ->colors[indv2*3+1];
		colors[(j+1)*3+2] = OBJ->colors[indv2*3+2];

		colors[(j+2)*3+0] = OBJ->colors[indv3*3+0];
		colors[(j+2)*3+1] = OBJ->colors[indv3*3+1];
		colors[(j+2)*3+2] = OBJ->colors[indv3*3+2];
	}
	GLfloat max=0;
	GLfloat xx = maxx-minx;
	GLfloat yy = maxy-miny;
	GLfloat zz = maxz-minz;
	GLfloat avgx = (maxx+minx)/2.0f;
	GLfloat avgy = (maxy+miny)/2.0f;
	GLfloat avgz = (maxz+minz)/2.0f;
	
	if(xx>max) {
		max = xx;
	
	}
	if(yy>max){
		max = yy;
		
	}
	if(zz>max) max = zz;
	GLfloat scale = 2.0f/max;
	printf("%f\n",scale);
	normalized(scale);

	GLfloat deltax =  avgx*scale;
	GLfloat deltay =  avgy*scale;
	GLfloat deltaz =  avgz*scale;
	//printf("%f %f %f \n",deltax,deltay,deltaz);
	displace(deltax,deltay,deltaz);
	
	
	
}


void loadOBJModel()
{
	// read an obj model here
	OBJ = glmReadOBJ(filename);
	printf("%s\n", filename);

	// traverse the color model
	colorModel();
}

void idle()
{
	glutPostRedisplay();
}

void renderScene(void)
{
	// clear canvas
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(iLocPosition);
	glEnableVertexAttribArray(iLocColor);

	static GLfloat triangle_color[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		
	};

	static GLfloat triangle_vertex[] = {
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 -1.0f, 1.0f, 0.0f,
		 0.0f,  -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};


	/*glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, triangle_vertex);
	glVertexAttribPointer(iLocColor, 3, GL_FLOAT, GL_FALSE, 0, triangle_color);

	// draw the array we just bound
	glDrawArrays(GL_TRIANGLES, 0, 3);*/
	int k=0;
	while(k<(int)OBJ->numtriangles){
		glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices+k*9);
		glVertexAttribPointer(iLocColor, 3, GL_FLOAT, GL_FALSE, 0, colors+k*9);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		k++;

	}
	

	
	glutSwapBuffers();
}

void showShaderCompileStatus(GLuint shader, GLint *shaderCompiled){
	glGetShaderiv(shader, GL_COMPILE_STATUS, shaderCompiled);
	if(GL_FALSE == (*shaderCompiled))
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character.
		GLchar *errorLog = (GLchar*) malloc(sizeof(GLchar) * maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
		fprintf(stderr, "%s", errorLog);

		glDeleteShader(shader);
		free(errorLog);
	}
}

void setShaders()
{
	GLuint v, f, p;
	char *vs = NULL;
	char *fs = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("shader.vert");
	fs = textFileRead("shader.frag");

	glShaderSource(v, 1, (const GLchar**)&vs, NULL);
	glShaderSource(f, 1, (const GLchar**)&fs, NULL);

	free(vs);
	free(fs);

	// compile vertex shader
	glCompileShader(v);
	GLint vShaderCompiled;
	showShaderCompileStatus(v, &vShaderCompiled);
	if(!vShaderCompiled) system("pause"), exit(123);

	// compile fragment shader
	glCompileShader(f);
	GLint fShaderCompiled;
	showShaderCompileStatus(f, &fShaderCompiled);
	if(!fShaderCompiled) system("pause"), exit(456);

	p = glCreateProgram();

	// bind shader
	glAttachShader(p, f);
	glAttachShader(p, v);

	// link program
	glLinkProgram(p);

	iLocPosition = glGetAttribLocation (p, "av4position");
	iLocColor    = glGetAttribLocation (p, "av3color");

	glUseProgram(p);
}


void processMouse(int who, int state, int x, int y)
{
	printf("(%d, %d) ", x, y);

	switch(who){
	case GLUT_LEFT_BUTTON:   printf("left button   "); break;
	case GLUT_MIDDLE_BUTTON: printf("middle button "); break;
	case GLUT_RIGHT_BUTTON:  printf("right button  "); break;
	case GLUT_WHEEL_UP:      printf("wheel up      "); break;
	case GLUT_WHEEL_DOWN:    printf("wheel down    "); break;
	default:                 printf("%-14d", who);     break;
	}

	switch(state){
	case GLUT_DOWN:          printf("start ");         break;
	case GLUT_UP:            printf("end   ");         break;
	}

	printf("\n");

}

void processMouseMotion(int x, int y){  // callback on mouse drag
	printf("(%d, %d) mouse move\n", x, y);
}

void processNormalKeys(unsigned char key, int x, int y) {
	static int index = 0;
	printf("(%d, %d) ", x, y);
	switch(key) {
		case 27: /* the Esc key */
			exit(0);
			break;
		case 'a':
		case 'A':
			if(index>0)
			index -=1;
			//printf("Left Key \n");
			filename = files[index];
			loadOBJModel();
			break;
		case 'd':
		case 'D':
			if(index<7)
			index +=1;
			//printf("Right Key \n");
			filename = files[index];
			loadOBJModel();
			break;
		case 'w':
		case 'W':
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			break;
		
		case 's':
		case 'S':
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			break;
		case 'h':
		case 'H':
			printf("HELP MENU¡G \n");
			printf("A Key¡G Previous Model \n");
			printf("D Key: Next Model \n");
			printf("S key¡GTurn to solid mode \n");
			printf("W key¡GTurn to wirefrane mode \n");

			break;
		
	}
	printf("\n");

}

int main(int argc, char **argv) {
	// glut init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	// create window
	glutInitWindowPosition(460, 40);
	glutInitWindowSize(800, 800);
	glutCreateWindow("10320 CS550000 CG HW1 TA");

	glewInit();
	if(glewIsSupported("GL_VERSION_2_0")){
		printf("Ready for OpenGL 2.0\n");
	}else{
		printf("OpenGL 2.0 not supported\n");
		system("pause");
		exit(1);
	}

	// load obj models through glm
	loadOBJModel();

	// register glut callback functions
	glutDisplayFunc (renderScene);
	glutIdleFunc    (idle);
	glutKeyboardFunc(processNormalKeys);
	glutMouseFunc   (processMouse);
	glutMotionFunc  (processMouseMotion);

	glEnable(GL_DEPTH_TEST);

	// set up shaders here
	setShaders();

	// main loop
	glutMainLoop();

	// free
	glmDelete(OBJ);

	return 0;
}

