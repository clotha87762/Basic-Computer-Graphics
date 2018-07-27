#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <GL/glew.h>
#include <freeglut/glut.h>
#include "textfile.h"
#include "GLM.h"
#include <cmath>
#include <assert.h>

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
GLint iLocMVP;
GLint iLocNMVP;
GLint iLocNormal;
GLint iLocMDiffuse;
GLint iLocMAmbient;
GLint iLocMSpecular;
GLint iLocMShininess;

GLint iLocEye;

GLint iLocLType;

GLint iLocLAmbient;
GLint iLocLDiffuse;
GLint iLocLSpecular;
GLint iLocLConstatt;
GLint iLocLLinatt;
GLint iLocLPosition;
GLint iLocLSpotDir;
GLint iLocLSpotCosCutoff;
GLint iLocLSpotExp;
GLint iLocLightSwitch;
GLfloat Lambient[4]={0.3f,0.3f,0.3f,1.0f};
GLfloat Ldiffuse[4]={0.8f,0.8f,0.8f,1.0f};
GLfloat Lspecular[4]={1.0f,1.0f,1.0f,1.0f};
GLfloat Lposition[4]={0,0,-2.0f,0.0};

GLfloat cutoff=0.93f;
GLfloat spotExp=2.0f;
GLint type=1;
GLint lightSwitch=1;

GLfloat* normals;




GLfloat scaleX[3]={1,1,1},scaleY[3]={1,1,1},scaleZ[3]={1,1,1},
	rotateX[3]={0,0,0},rotateY[3]={0,0,0},rotateZ[3]={0,0,0},
	transX[3]={0,0,0},transY[3]={0,0,0},transZ[3]={0,0,0};


GLfloat Right=1,Left=-1,Top=1,Bottom=-1.0f,Far=3,Near=1;
GLfloat eye[3] = {0,0,2};
GLfloat up[3]={0,1,0};
GLfloat center[3]={0,0,1};

char* filename = "ColorModels/bunny5KN.obj";

				

GLMmodel* OBJ;
GLMmodel* OBJ1;
GLMmodel* OBJ2;
GLMmodel* OBJ3;

int nowRender =0;

GLfloat aMVP[16];
GLfloat MVP[16];
GLfloat NMVP[16];

GLfloat viewM[16];

GLint sign=1; // 1=pos ; -1=neg



enum Mode{G,V,P,L};
enum Gaction{s,t,r};
enum Vaction{e,c,u};
enum Paction{o,p};
enum XYZ{X,Y,Z};

bool multi=false;

Mode mode=L;
Gaction gaction=s;
Vaction vaction=e;
Paction paction=o;
XYZ xyz;

GLfloat* vertices[3];
GLfloat* vertice;
GLfloat* colors[3];
GLint verNum[3];


GLfloat scale[3];
GLfloat avgx[3];
GLfloat avgy[3];
GLfloat avgz[3];

void MM4(GLfloat* a){

	GLfloat c[16];

	int i,j,k=0;


	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			
			c[i*4+j]=0;
			for(k=0;k<4;k++){

				c[i*4+j] =c[i*4+j] + a[k*4+j] * MVP[i*4+k];
			}

		}
	}

	for(i=0;i<16;i++)
		MVP[i]=c[i];

}

bool inverse(GLfloat* mvp,GLfloat* Nmvp)
{
    // 填好參數化的部分
	 GLfloat matrix[4][4*2]; 

	   matrix[0][0]=mvp[0];
	   matrix[0][1]=mvp[4];
	   matrix[0][2]=mvp[8];
	   matrix[0][3]=mvp[12];
	   matrix[1][0]=mvp[1];
	   matrix[1][1]=mvp[5];
	   matrix[1][2]=mvp[9];
	   matrix[1][3]=mvp[13];
	   matrix[2][0]=mvp[2];
	   matrix[2][1]=mvp[6];
	   matrix[2][2]=mvp[10];
	   matrix[2][3]=mvp[14];
	   matrix[3][0]=mvp[3];
	   matrix[3][1]=mvp[7];
	   matrix[3][2]=mvp[11];
	   matrix[3][3]=mvp[15];


    for (int i=0; i<4; ++i)
        for (int j=0; j<4; ++j)
            matrix[i][4+j] = 0;

    for (int i=0; i<4; ++i)
        matrix[i][4+i] = 1;

    // 開始進行高斯喬登消去法
    // 內容幾乎與高斯消去法相同
    for (int i=0; i<4; ++i)
    {
        if (matrix[i][i] == 0)
            for (int j=i+1; j<4; ++j)
                if (matrix[j][i] != 0)
                {
                    for (int k=i; k<4*2; ++k)
                        std::swap(matrix[i][k], matrix[j][k]);
                    break;
                }

        // 反矩陣不存在。
        if (matrix[i][i] == 0) return false;

        double t = matrix[i][i];
        for (int k=i; k<4*2; ++k)
            matrix[i][k] /= t;

        // 消去時，所有的row都消去。
        for (int j=0; j<4; ++j)
            if (i != j && matrix[j][i] != 0)
            {
                double t = matrix[j][i];
                for (int k=i; k<4*2; ++k)
                    matrix[j][k] -= matrix[i][k] * t;
            }
    }

	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++){
	  Nmvp[i+j*4] = matrix[i][j+4];
	}

    return true;
}


/*
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
*/
void Cross(GLfloat* u, GLfloat* v, GLfloat* n)
{
   // assert(u); assert(v); assert(n);
    
    n[0] = u[1]*v[2] - u[2]*v[1];
    n[1] = u[2]*v[0] - u[0]*v[2];
    n[2] = u[0]*v[1] - u[1]*v[0];
}


void Normalize(GLfloat* v)
{
    GLfloat l;
    
    //assert(v);
    

    l = (GLfloat)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    
	v[0] /= l;
    v[1] /= l;
    v[2] /= l;
}




void geometric(int index);
void view();
void projection();



void changeToBasic(){
	
	paction=o;
	Right=1;Left=-1;Top=1;Bottom=-1;Near=-1;Far=1;
	for(int i=0;i<3;i++){
		scaleX[i] =scale[i];
		scaleY[i]=scale[i];
		scaleZ[i]=scale[i];
		transX[i] = -(avgx[i]);
		transY[i] = -(avgy[i]);
		transZ[i] = -(avgz[i]);
		rotateX[i]=0;
		rotateY[i]=0;
		rotateZ[i]=0;
		eye[i]=0;
	}
	up[0]=0;
	up[1]=1;
	up[2]=0;
	center[0]=0;
	center[1]=0;
	center[2]=-1;
}

void changeToAdv(){
	paction=o;
	Right=2;Left=-2;Top=2;Bottom=-2;Near=-2;Far=2;
	for(int i=0;i<3;i++){
		scaleX[i] =scale[i]/ 4 ;
		scaleY[i] =scale[i]/ 4 ;
		scaleZ[i]= scale[i]/ 4 ;
		transX[i] = -(avgx[i]);
		transY[i] = -(avgy[i]);
		transZ[i] = -(avgz[i]);
		rotateX[i]=0;
		rotateY[i]=0;
		rotateZ[i]=0;
		eye[i]=0;
	}
	//transY[0] -= 0.3f;
	//transY[2] += 0.3f;
	up[0]=0;
	up[1]=1;
	up[2]=0;
	center[0]=0;
	center[1]=0;
	center[2]=-1;
	
}


void colorModel(int num)
{
	
	int i;

	// TODO:
	//// You should traverse the vertices and the colors of each vertex.
	//// Normalize each vertex into [-1, 1], which will fit the camera clipping window.

	// number of triangles
	verNum[num]=3*OBJ->numtriangles;

	// number of vertices
	
	// The center position of the model
	// (should be calculated by yourself)
	GLfloat sx,sy,sz;
	sx=OBJ->position[0];
	sy=OBJ->position[1];
	sz=OBJ->position[2];
	printf("%f %f %f \n",sx,sy,sz);
   
	
	int j=0;
	vertices[num] = new GLfloat[(int)OBJ->numtriangles*9];
	normals = new GLfloat[(int)OBJ->numtriangles*9];
	printf("numnormal %d\n",(int)OBJ->numnormals);
	//colors[num] = new GLfloat[(int)OBJ->numtriangles*9];
	GLfloat maxx=-1e9,maxy=-1e9,maxz=-1e9,minx=1e9,miny=1e9,minz=1e9;
	/*
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
		vertices[num][j*3+0] = vx;
		vertices[num][j*3+1] = vy;
		vertices[num][j*3+2] = vz;
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
		
		vertices[num][(j+1)*3+0] = vx;
		vertices[num][(j+1)*3+1] = vy;
		vertices[num][(j+1)*3+2] = vz;

		if(vx>maxx)maxx=vx;
		if(vx<minx)minx=vx;

		if(vy>maxy)maxy=vy;
		if(vy<miny)miny=vy;

		if(vz>maxz)maxz=vz;
		if(vz<minz)minz=vz;


		vx= OBJ->vertices[indv3*3+0];
		vy= OBJ->vertices[indv3*3+1];
		vz= OBJ->vertices[indv3*3+2];
		
		vertices[num][(j+2)*3+0] = vx;
		vertices[num][(j+2)*3+1] = vy;
		vertices[num][(j+2)*3+2] = vz;

		if(vx>maxx)maxx=vx;
		if(vx<minx)minx=vx;


		if(vy>maxy)maxy=vy;
		if(vy<miny)miny=vy;

		if(vz>maxz)maxz=vz;
		if(vz<minz)minz=vz;
		

		
		



		// colors
		
		colors[num][j*3+0] = OBJ->colors[indv1*3+0];
		colors[num][j*3+1] = OBJ->colors[indv1*3+1];
		colors[num][j*3+2] = OBJ->colors[indv1*3+2];

		colors[num][(j+1)*3+0] = OBJ->colors[indv2*3+0];
		colors[num][(j+1)*3+1] = OBJ->colors[indv2*3+1];
		colors[num][(j+1)*3+2] = OBJ->colors[indv2*3+2];

		colors[num][(j+2)*3+0] = OBJ->colors[indv3*3+0];
		colors[num][(j+2)*3+1] = OBJ->colors[indv3*3+1];
		colors[num][(j+2)*3+2] = OBJ->colors[indv3*3+2];
		
	}*/
	

	GLMgroup* group = OBJ->groups;
	int t=0;
	while (group)
	{
		//Get material data here (p.5)
			
			glUniform4fv(iLocMDiffuse,1,OBJ->materials[group->material].diffuse);
			glUniform4fv(iLocMAmbient,1,OBJ->materials[group->material].ambient);
			glUniform4fv(iLocMSpecular,1,OBJ->materials[group->material].specular);
			glUniform1fv(iLocMShininess,1,&OBJ->materials[group->material].shininess);
			printf("numtriangle %d\n",group->numtriangles);
			printf("ambient %f\n",OBJ->materials[group->material].ambient[0]);
			printf("diffuse %f\n",OBJ->materials[group->material].diffuse[0]);
			printf("specular %f\n",OBJ->materials[group->material].specular[0]);

		for (i = 0; i<(int)group->numtriangles; i++,t+=3)
		{
		//Get OBJ data here (p.6-7)
			//Triangle index
			
			int triangleID = group->triangles[i];
			//the index of each vertices
			int indv1 = OBJ->triangles[triangleID].vindices[0];
			int indv2 = OBJ->triangles[triangleID].vindices[1];
			int indv3 = OBJ->triangles[triangleID].vindices[2];
			
			GLfloat vx, vy, vz;
			vx = OBJ->vertices[indv1*3+0];
			vy= OBJ->vertices[indv1*3+1];
			vz= OBJ->vertices[indv1*3+2];
			vertices[num][t*3+0] = vx;
			vertices[num][t*3+1] = vy;
			vertices[num][t*3+2] = vz;
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
		
			vertices[num][(t+1)*3+0] = vx;
			vertices[num][(t+1)*3+1] = vy;
			vertices[num][(t+1)*3+2] = vz;

			if(vx>maxx)maxx=vx;
			if(vx<minx)minx=vx;

			if(vy>maxy)maxy=vy;
			if(vy<miny)miny=vy;

			if(vz>maxz)maxz=vz;
			if(vz<minz)minz=vz;


			vx= OBJ->vertices[indv3*3+0];
			vy= OBJ->vertices[indv3*3+1];
			vz= OBJ->vertices[indv3*3+2];
		
			vertices[num][(t+2)*3+0] = vx;
			vertices[num][(t+2)*3+1] = vy;
			vertices[num][(t+2)*3+2] = vz;

			if(vx>maxx)maxx=vx;
			if(vx<minx)minx=vx;


			if(vy>maxy)maxy=vy;
			if(vy<miny)miny=vy;

			if(vz>maxz)maxz=vz;
			if(vz<minz)minz=vz;
			
			
			
			//the index of the face normal of the triangle i
			int indfn = OBJ->triangles[triangleID].findex;
			//the face normal
			OBJ->facetnorms[indfn*3+2];
			OBJ->facetnorms[indfn*3];
			OBJ->facetnorms[indfn*3+1]; 

			//the index of each normals
			int indn1 = OBJ->triangles[triangleID].nindices[0];
			int indn2 = OBJ->triangles[triangleID].nindices[1];
			int indn3 = OBJ->triangles[triangleID].nindices[2];
			// the vertex normal
			normals[t*3+0]=OBJ->normals[indn1*3];
			normals[t*3+1]=OBJ->normals[indn1*3+1];
			normals[t*3+2]=OBJ->normals[indn1*3+2];
		
			normals[(t+1)*3+0]=OBJ->normals[indn2*3];
			normals[(t+1)*3+1]=OBJ->normals[indn2*3+1];
			normals[(t+1)*3+2]=OBJ->normals[indn2*3+2];
		
			normals[(t+2)*3+0]=OBJ->normals[indn3*3];
			normals[(t+2)*3+1]=OBJ->normals[indn3*3+1];
			normals[(t+2)*3+2]=OBJ->normals[indn3*3+2]; 
			
		}

		group = group->next;


		}








	GLfloat max=0;
	GLfloat xx = maxx-minx;
	GLfloat yy = maxy-miny;
	GLfloat zz = maxz-minz;
	 avgx[num] = (maxx+minx)/2.0f;
	 avgy[num] = (maxy+miny)/2.0f;
	 avgz[num] = (maxz+minz)/2.0f;
	
	if(xx>max) {
		max = xx;
	
	}
	if(yy>max){
		max = yy;
		
	}
	if(zz>max) max = zz;
	scale[num] = 2.0f/max;
	printf("%f\n",scale);
	/*normalized(scale);

	GLfloat deltax =  avgx*scale;
	GLfloat deltay =  avgy*scale;
	GLfloat deltaz =  avgz*scale;
	//printf("%f %f %f \n",deltax,deltay,deltaz);
	displace(deltax,deltay,deltaz);
	*/
	
	scaleX[num] =scale[num];
	scaleY[num]=scale[num];
	scaleZ[num]=scale[num];
	transX[num] = -(avgx[num]);
	transY[num] = -(avgy[num]);
	transZ[num] = -(avgz[num]);
	rotateX[num]=0;
	rotateY[num]=0;
	rotateZ[num]=0;

	
}



void loadOBJModel(int num)
{
	// read an obj model here
	if(num==0)
		filename = "ColorModels/duck4KN.obj";
	/*else if(num==1)
		filename = "ColorModels/bunny5KN.obj";
	else if(num==2)
		filename = "ColorModels/bunny5KN.obj";
		*/
	OBJ = glmReadOBJ(filename);
	printf("%s\n", filename);           
	//glmReadMTL(OBJ,"bunny5KN.obj");

	// traverse the color model
	
	colorModel(num);

}

void idle()
{
	glutPostRedisplay();
}


void loadIdentity(){
	MVP[0] = 1;	MVP[4] = 0;	MVP[8] = 0;		MVP[12] = 0;
	MVP[1] = 0;	MVP[5] = 1;	MVP[9] = 0;		MVP[13] = 0;
	MVP[2] = 0;	MVP[6] = 0;	MVP[10] = 1;	MVP[14] = 0;
	MVP[3] = 0;	MVP[7] = 0;	MVP[11] = 0;	MVP[15] = 1;

}

void transpose(GLfloat* matrix){
	for(int i=0;i<4;i++)
		for(int j=i;j<4;j++)
			std::swap(matrix[i*4+j],matrix[i+j*4]);
}

void renderScene(void)
{
	///TODO(HW2):
	// Calculate your transform matrices and multiply them.
	// Finally, assign the MVP matrix to aMVP.
	// NOTE:
	// You should add comment to describe where are your Model,
	// Viewing, and Projection transform matrices.



	///Example of displaying colorful triangle
	// clear canvas
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(iLocPosition);
	glEnableVertexAttribArray(iLocNormal);

	static GLfloat triangle_color[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	static GLfloat triangle_vertex[] = {
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f
	};
	
	
	MVP[0] = 1;	MVP[4] = 0;	MVP[8] = 0;		MVP[12] = 0;
	MVP[1] = 0;	MVP[5] = 1;	MVP[9] = 0;		MVP[13] = 0;
	MVP[2] = 0;	MVP[6] = 0;	MVP[10] = 1;	MVP[14] = 0;
	MVP[3] = 0;	MVP[7] = 0;	MVP[11] = 0;	MVP[15] = 1;


	// Move example triangle to left by 0.5
	/*aMVP[0] = 1;	aMVP[4] = 0;	aMVP[8] = 0;		aMVP[12] = -0.5;
	aMVP[1] = 0;	aMVP[5] = 1;	aMVP[9] = 0;		aMVP[13] = 0;
	aMVP[2] = 0;	aMVP[6] = 0;	aMVP[10] = -1;	aMVP[14] = 0;
	aMVP[3] = 0;	aMVP[7] = 0;	aMVP[11] = 0;	aMVP[15] = 1;*/
	
	if(!multi){
		
		geometric(nowRender);
		view();
		projection();
		glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices[nowRender]);
		glVertexAttribPointer(iLocNormal,3,GL_FLOAT,GL_FALSE,0,normals);

		/*for(int i=0;i<OBJ->numtriangles*9;i++)
			printf("%f \n",normals[i]);
		system("Pause");
		*/
		glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, MVP);
		inverse(MVP,NMVP);
		transpose(NMVP);
		glUniformMatrix4fv(iLocNMVP,1,GL_FALSE,NMVP);
		
		// draw the array we just bound
		GLMgroup* group ;
		glUniform4fv(iLocLAmbient,1,Lambient);
		glUniform4fv(iLocLDiffuse,1,Ldiffuse);
		glUniform4fv(iLocLSpecular,1,Lspecular);
		glUniform4fv(iLocLPosition,1,Lposition);
		glUniform4fv(iLocEye,1,eye);
		glUniform1iv(iLocLType,1,&type);
		glUniform1fv(iLocLSpotCosCutoff,1,&cutoff);
		glUniform1fv(iLocLSpotExp,1,&spotExp);
		glUniform1iv(iLocLightSwitch,1,&lightSwitch);
		//glUniform4fv(iLocLConstatt,1,OBJ->materials[group->material].constantAttenuation);
		//glUniform4fv(iLocLLinatt,1,OBJ->materials[group->material].linearAttenuation);
		//glUniform4fv(iLocLSpotDir,1,OBJ->materials[group->material].spotDirection);
		//glUniform4fv(iLocLSpotExp,1,OBJ->materials[group->material].spotExponent);
		//glUniform4fv(iLocLSpotCosCutoff,1,OBJ->materials[group->material].spotCosCutoff);
		int count;
		count=0;
		group= OBJ->groups;
		while(group){
			//printf("aa %f \n",OBJ->materials[group->material].ambient[0]);
			//printf("bb %f \n",OBJ->materials[group->material].ambient[1]);
			//printf("cc %f \n",OBJ->materials[group->material].ambient[2]);
			//printf("dd %f \n",OBJ->materials[group->material].ambient[3]);
			glUniform4fv(iLocMAmbient,1,OBJ->materials[group->material].ambient);
			glUniform4fv(iLocMDiffuse,1,OBJ->materials[group->material].diffuse);
			glUniform4fv(iLocMSpecular,1,OBJ->materials[group->material].specular);
			glUniform1fv(iLocMShininess,1,&OBJ->materials[group->material].shininess);
			//printf("shininess %f \n",OBJ->materials[group->material].shininess);
			
			glDrawArrays(GL_TRIANGLES, 0, group->numtriangles*3);
			count+= (group->numtriangles*9);
			//printf("count %d \n",count);
			//system("Pause");
			glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices[nowRender]+count);
			glVertexAttribPointer(iLocNormal,3,GL_FLOAT,GL_FALSE,0,normals+count);
			
			group = group->next;
		}
	
	}
	else if(multi){
		for(int i=0;i<3;i++){
			loadIdentity();
			geometric(i);
			view();
			projection();
			glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices[i]);
			glVertexAttribPointer(   iLocColor, 3, GL_FLOAT, GL_FALSE, 0, colors[i]);
			glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, MVP);
			glDrawArrays(GL_TRIANGLES, 0, verNum[i]);
		}
	}

	glutSwapBuffers();
	
}


void geometric(int index){
	GLfloat rotateM[16];
	GLfloat scaleM[16];
	GLfloat transM[16];


	transM[0]=1;	transM[4]=0;	transM[8]=0;	transM[12]=transX[index];
	transM[1]=0;	transM[5]=1;	transM[9]=0;	transM[13]=transY[index];
	transM[2]=0;	transM[6]=0;	transM[10]=1;	transM[14]=transZ[index];
	transM[3]=0;	transM[7]=0;	transM[11]=0;	transM[15]=1;

	MM4(transM);
	
	
	rotateM[0]=1;	rotateM[4]=0;					rotateM[8]=0; rotateM[12]=0;
	rotateM[1]=0;	rotateM[5]=cos(rotateX[index]);	rotateM[9]=-sin(rotateX[index]); rotateM[13]=0;	
	rotateM[2]=0;	rotateM[6]=sin(rotateX[index]);	rotateM[10]=cos(rotateX[index]); rotateM[14]=0;
	rotateM[3]=0;	rotateM[7]=0;					rotateM[11]=0; rotateM[15]=1;
	
	MM4(rotateM); // rotate along x axis

	rotateM[0]=cos(rotateY[index]);	rotateM[4]=0;	rotateM[8]=sin(rotateY[index]); rotateM[12]=0;
	rotateM[1]=0;					rotateM[5]=1;	rotateM[9]=0;					rotateM[13]=0;	
	rotateM[2]=-sin(rotateY[index]);rotateM[6]=0;	rotateM[10]=cos(rotateY[index]); rotateM[14]=0;
	rotateM[3]=0;					rotateM[7]=0;	rotateM[11]=0;					rotateM[15]=1;

	MM4(rotateM);

	rotateM[0]=cos(rotateZ[index]);	rotateM[4]=-sin(rotateZ[index]);rotateM[8]=0; rotateM[12]=0;
	rotateM[1]=sin(rotateZ[index]);	rotateM[5]=cos(rotateZ[index]);	rotateM[9]=0; rotateM[13]=0;	
	rotateM[2]=0;					rotateM[6]=0;					rotateM[10]=1; rotateM[14]=0;
	rotateM[3]=0;					rotateM[7]=0;					rotateM[11]=0; rotateM[15]=1;

	MM4(rotateM);

	scaleM[0]=scaleX[index];	scaleM[4]=0;				scaleM[8]=0;	scaleM[12]=0;
	scaleM[1]=0;				scaleM[5]=scaleY[index];	scaleM[9]=0;	scaleM[13]=0;
	scaleM[2]=0;				scaleM[6]=0;				scaleM[10]=scaleZ[index];	scaleM[14]=0;
	scaleM[3]=0;				scaleM[7]=0;				scaleM[11]=0;	scaleM[15]=1;

	MM4(scaleM);

	if(multi && index==0){
		transM[0]=1;	transM[4]=0;	transM[8]=0;	transM[12]=0;
		transM[1]=0;	transM[5]=1;	transM[9]=0;	transM[13]=0.5f;
		transM[2]=0;	transM[6]=0;	transM[10]=1;	transM[14]=0;
		transM[3]=0;	transM[7]=0;	transM[11]=0;	transM[15]=1;

		MM4(transM);

	}
	else if(multi && index==2){
		transM[0]=1;	transM[4]=0;	transM[8]=0;	transM[12]=0;
		transM[1]=0;	transM[5]=1;	transM[9]=0;	transM[13]=-0.5f;
		transM[2]=0;	transM[6]=0;	transM[10]=1;	transM[14]=0;
		transM[3]=0;	transM[7]=0;	transM[11]=0;	transM[15]=1;

		MM4(transM);

	}

	

}

void view(){
	GLfloat Rz[3];
	
	Rz[0] = center[0]-eye[0];
	Rz[1] = center[1]-eye[1];
	Rz[2] = center[2]-eye[2];
	
	Normalize(Rz);
	
	GLfloat Rx[3];
	Cross(Rz,up,Rx);
	Normalize(Rx);

	
	GLfloat Ry[3];
	Cross(Rx,Rz,Ry);

	viewM[0]=Rx[0];		viewM[4]=Rx[1];		viewM[8]=Rx[2];		viewM[12]=0;
	viewM[1]=Ry[0];		viewM[5]=Ry[1];		viewM[9]=Ry[2];		viewM[13]=0;
	viewM[2]=-Rz[0];	viewM[6]=-Rz[1];	viewM[10]=-Rz[2];	viewM[14]=0;
	viewM[3]=0;			viewM[7]=0;			viewM[11]=0;		viewM[15]=1;

	GLfloat eyeM[16];
	eyeM[0]=1;	eyeM[4]=0;	eyeM[8]=0;	eyeM[12]=-eye[0];
	eyeM[1]=0;	eyeM[5]=1;	eyeM[9]=0;	eyeM[13]=-eye[1];
	eyeM[2]=0;	eyeM[6]=0;	eyeM[10]=1;	eyeM[14]=-eye[2];
	eyeM[3]=0;	eyeM[7]=0;	eyeM[11]=0;	eyeM[15]=1;
	
	MM4(eyeM);
	MM4(viewM);
	
	
	
}

void projection(){

	if(paction==o){

		GLfloat orthM[16];

		orthM[0]=2/(Right-Left);	orthM[4]=0;					orthM[8]=0;					orthM[12]=-(Right+Left)/(Right-Left);
		orthM[1]=0;					orthM[5]=2/(Top-Bottom);	orthM[9]=0;					orthM[13]=-(Top+Bottom)/(Top-Bottom);
		orthM[2]=0;					orthM[6]=0;					orthM[10]=-2/(Far-Near);	orthM[14]=-(Far+Near)/(Far-Near);
		orthM[3]=0;					orthM[7]=0;					orthM[11]=0;				orthM[15]=1;

		MM4(orthM);
	}
	 else if(paction==p){
		GLfloat persM[16];

		persM[0]=(2.0f*Near)/(Right-Left);	persM[4]=0;						persM[8]=(Right+Left)/(Right-Left);	persM[12]=0;
		persM[1]=0;						persM[5]=(2.0f*Near)/(Top-Bottom);	persM[9]=(Top+Bottom)/(Top-Bottom);	persM[13]=0;
		persM[2]=0;						persM[6]=0;						persM[10]=-(Far+Near)/(Far-Near);	persM[14]=-(2.0f*Near*Far)/(Far-Near);	
		persM[3]=0;						persM[7]=0;						persM[11]=-1.0f;						persM[15]=0;


		MM4(persM);
	}


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

	vs = textFileRead("sample.vert");
	fs = textFileRead("sample.frag");

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

	glEnableVertexAttribArray(iLocNormal);
	
	iLocPosition = glGetAttribLocation (p, "av4position");
	//iLocColor    = glGetAttribLocation (p, "av3color");
	iLocNormal	=glGetAttribLocation(p,"normal4");
	iLocMVP		 = glGetUniformLocation(p, "mvp");
	iLocNMVP = glGetUniformLocation(p,"nmvp");

	
	iLocMDiffuse = glGetUniformLocation(p,"Material.diffuse");
	iLocMAmbient = glGetUniformLocation(p,"Material.ambient");
	iLocMSpecular = glGetUniformLocation(p,"Material.specular");
	iLocMShininess = glGetUniformLocation(p,"Material.shininess");

	iLocEye = glGetUniformLocation(p,"eye");

	 iLocLAmbient = glGetUniformLocation(p,"LightSource.ambient");
	 iLocLDiffuse = glGetUniformLocation(p,"LightSource.diffuse");
	 iLocLSpecular= glGetUniformLocation(p,"LightSource.specular");
	 iLocLConstatt= glGetUniformLocation(p,"LightSource.constantAttenuation");
	 iLocLLinatt= glGetUniformLocation(p,"LightSource.linearAttenuation");
	 iLocLPosition= glGetUniformLocation(p,"LightSource.position");
	 iLocLSpotDir= glGetUniformLocation(p,"LightSource.spotDirection");
	 iLocLSpotCosCutoff= glGetUniformLocation(p,"LightSource.spotCosCutoff");
	 iLocLSpotExp= glGetUniformLocation(p,"LightSource.spotExponent");
	 iLocLType=glGetUniformLocation(p,"type");
	  iLocLightSwitch=glGetUniformLocation(p,"lightSwitch");


	glUseProgram(p);
}

int preX,preY;
void processMouse(int who, int state, int x, int y)
{
	printf("(%d, %d) ", x, y);

	switch(who){
	case GLUT_LEFT_BUTTON:  
		printf("left button   "); 
		preX=x;
		preY=y;
		break;
	case GLUT_MIDDLE_BUTTON: printf("middle button "); break;
	case GLUT_RIGHT_BUTTON:  printf("right button  "); break; 
	case GLUT_WHEEL_UP:      
		
		if(mode==G){

			if(gaction==s){
				scaleZ[nowRender] +=0.03f;
			}
			else if(gaction==t){
				transZ[nowRender] +=0.03f;
			}
			else if(gaction==r){
				rotateZ[nowRender] +=0.03f;
			}

		}
		else if(mode==V){
			eye[2] += 0.03f;
		}

		break;
	case GLUT_WHEEL_DOWN:    
		if(mode==G){

			if(gaction==s){
				scaleZ[nowRender] -=0.03f;
			}
			else if(gaction==t){
				transZ[nowRender] -=0.03f;
			}
			else if(gaction==r){
				rotateZ[nowRender] -=0.03f;
			}

		}
		else if(mode==V){
			eye[2] -= 0.03f;
		}

		break;
	default:                 printf("%-14d", who);     break;
	}

	switch(state){
	case GLUT_DOWN:          printf("start ");         break;
	case GLUT_UP:            printf("end   ");         break;
	}

	printf("\n");
}

void processMouseMotion(int x, int y){  // callback on mouse drag
	//printf("(%d, %d) mouse move\n", x, y);
	if(x-preX>0){
		
		if(mode==G){

			if(gaction==s){
				scaleX[nowRender] += 0.02f;
			}
			else if(gaction==t){
				transX[nowRender] += 0.02f;
			}
			else if(gaction==r){
				rotateX[nowRender] += 0.02f;
			}

		}
		else if(mode==V){

			eye[0] += 0.03f;

		}

	}
	else if(x-preX<0){
		
		if(mode==G){

			if(gaction==s){
				scaleX[nowRender] -= 0.02f;
			}
			else if(gaction==t){
				transX[nowRender] -= 0.02f;
			}
			else if(gaction==r){
				rotateX[nowRender] -= 0.02f;
			}

		}
		else if(mode==V){

			eye[0]-=0.03f;
		}
	}

	

	if(y-preY<0){
		
		if(mode==G){

			if(gaction==s){
				scaleY[nowRender] += 0.02f;
			}
			else if(gaction==t){
				transY[nowRender] += 0.02f;
			}
			else if(gaction==r){
				rotateY[nowRender] += 0.02f;
			}

		}
		else if(mode==V){

			eye[1] += 0.03f;

		}

	}
	else if(y-preY>0){
		
		if(mode==G){

			if(gaction==s){
				scaleY[nowRender] -= 0.02f;
			}
			else if(gaction==t){
				transY[nowRender] -= 0.02f;
			}
			else if(gaction==r){
				rotateY[nowRender] -= 0.02f;
			}

		}
		else if(mode==V){

			eye[1]-=0.03f;
		}
	}



	preX=x;
	preY=y;
}

void processNormalKeys(unsigned char key, int x, int y) {
	printf("(%d, %d) ", x, y);
	printf("/n %d ",key);
	switch(key) {
		case'h':
		case'H':
			printf("------- HELP MENU FOR HW3 -----\n\n");
			printf("Press 1 to change to Directional Light \n");
			printf("Press 2 to change to Point Light \n");
			printf("Press 3 to change to Spot Light\n");
			printf("(All three kinds of light use the same position)\n");
			printf("Press 4 to add the CosineCutoff of the spot light\n");
			printf("Press 5 to substract the CosineCutoff of the spot light\n");
			printf("Press 6 to add spot exponent of the spot light\n");
			printf("Press 7 to substract spot exponent of the spot light\n");
			printf("Press 8 to turn on/off the light (thus we can only see the global ambient light)\n");
			printf("Press x/y/z to adjust the position of the light\n");
			printf("(Automatically,light position adjustment  is set to add, press space to change between add and substract)\n");
			printf("\n");
			printf("Viewing Position can also be adjusted by first pressing 'v'&'e' ,and press x/y/z/space to adjust \n");
			printf("(Press L if you want to adjust the light position again)\n");
			printf("\n");
			printf("Press P to change to Perspective Projection\n");
			printf("Press O to change to Orthogonal Projection\n");
			/*printf("\n-----HELP MENU-----\n\n");
			printf("There are two modes : single model and multi model. Press m/M to change mode \n");
			printf("(when mode changed,adjusment made to models would be reset)\n");
			printf("In single model mode,press 'g' 'v' to select current transform to geometric/view transform\n\n");
			printf("------Geometric Transform-----\n");
			printf("Press 's' 't' 'r' to make current manipulation to 'Scale' 'Transfer' or 'Rotate'\n");
			printf("Use x/y/z buttons to adjust the x/y/z component of current manipulation\n");
			printf("Pressing space can change between +/-  \n");
			printf("\n");
			printf("For Example : Press 'g' --> 's' --> space --> 'x' can reduce the x scale component of the model \n\n");
			printf("------Viewing Transform-----\n");
			printf("Similar to Geometric transform,except 's' 't' 'r' replaced by 'e' 'c' 'u'\n");
			printf("(which means eye/center coordinate/up vector component of the model)\n");
			printf("-----Projection-----\n");
			printf("Press 'o' 'p' to change between orthogoanl/perspective projection\n");
			printf("When switching between P and O , some initial arguments would be set automatically to offer better result\n");
			printf("To set projection arguments,in orthogonal mode and press o again, or in Perspective mode and press p again\n");
			printf("and then input 6 arguments in cmd 'Left' 'Right' 'Top 'Bottom' 'Far' 'Near' in order\n\n");
			//printf("Some suggested argument settings please refer to my report >.<\n\n");
			printf("In single model mode, pressing 1 2 3 can switch between three different models which you want to displayed\n");
			printf("In mulit models mode, pressing 1 2 3 can switch between models that you want to manipulate\n\n");
			printf("Mouse usage is also supported:\n");
			printf("Press left button and drag horizontally --> adjust x component of current manipulation\n");
			printf("Press left button and drag vertically --> adjust y component of current manipulation\n");
			printf("Middle wheel  --> adjust z component of current manipulation\n\n");
			*/
			break;
		case 27: /* the Esc key */ 
			exit(0); 
			break;
		case 32:
			if(sign==1)
				sign=-1;
			else if(sign==-1)
				sign = 1;
			break;
		case 49:
			type=1;
			break;
		case 50:
			type=2;
			break;
		case 51:
			type=3;
			break;
		case 52:
			cutoff+=0.01;
			break;
		case 53:
			cutoff-=0.01;
			break;
		case 54:
			spotExp+=0.5f;
			break;
		case 55:
			spotExp-=0.5f;
			break;
		case 56:
			lightSwitch = (lightSwitch+1)%2;
			break;
		case 'l':
		case 'L':
			mode=L;
			printf("Mode changed to light position\n");
			break;
		case'm':
		case'M':
		/*	multi=!multi;
			if(multi)changeToAdv();
			else if(!multi)changeToBasic();
			*/
			break;
		case 'g':
		case 'G':
			mode =G;
			break;
		case'v':
		case'V':
			mode = V;
			break;
		case'p':
		case'P':
			if(paction==p){
			//printf("Please input Perspective Projection Parameters: left, right, bottom, up, near, far\n");
			//scanf("%f %f %f %f %f %f",&Left,&Right,&Bottom,&Top,&Near,&Far);
			}
			else if(paction==o){
			printf("Perspective Projection!!\n");
			paction=p;
			Left=-1; Right=1;Top=1; Bottom=-1;Near=1; Far=3;
			eye[0] = 0;
			eye[1] = 0;
			eye[2] = 2;
			}
			break;
		case'o':
		case'O':
			if(paction==o){
			//printf("Please input Orthogonal Projection Parameters: left, right, bottom, up, near, far\n");
			//scanf("%f %f %f %f %f %f",&Left,&Right,&Bottom,&Top,&Near,&Far);
			}
			else if(paction==p){
			printf("Orthogonal Projection!!\n");
			paction=o;
			Left=-1; Right=1;Top=1; Bottom=-1;Near=1; Far=3;
			eye[0] = 0;
			eye[1] = 0;
			eye[2] = 2;
			}
			break;
		case's':
		case'S':
			gaction=s;
			break;
		case't':
		case'T':
			gaction=t;
			break;
		case'r':
		case'R':
			gaction=r;
			break;
		case'e':
		case'E':
			vaction=e;
			break;
		case'c':
		case'C':
			vaction=c;
			break;
		case'u':
		case'U':
			vaction=u;
			break;
		case'x':
		case'X':
			if(mode==G){

				if(gaction==s){
					scaleX[nowRender] += sign*0.1*scaleX[nowRender];
				}
				else if(gaction==t){
					transX[nowRender] += sign*10;
				}
				else if(gaction==r){
					rotateX[nowRender] += sign*0.05;
				}

			}
			else if(mode==V){
					
				if(vaction==e){
					eye[0] += sign*0.1;
				}
				else if(vaction==c){
					center[0] += sign*0.1;
				}
				else if(vaction==u){
					up[0] += sign*0.1;
				}



			}
			else if(mode==L){
				Lposition[0] +=sign*0.1f;
			}

			break;
		case'y':
		case'Y':
			if(mode==G){

				if(gaction==s){
					scaleY[nowRender] += sign*0.1*scaleY[nowRender];
				}
				else if(gaction==t){
					transY[nowRender] += sign*10;
				}
				else if(gaction==r){
					rotateY[nowRender] += sign*0.1;
				}

			}
			else if(mode==V){
				if(vaction==e){
					eye[1] += sign*0.1;
				}
				else if(vaction==c){
					center[1] += sign*0.1;
				}
				else if(vaction==u){
					up[1] += sign*0.1;
				}
			}
			else if(mode==L){
				Lposition[1] +=sign*0.1f;
			}
			break;
		case'z':
		case'Z':
			if(mode==G){

				if(gaction==s){
					scaleZ[nowRender] += sign*0.1*scaleZ[nowRender];
				}
				else if(gaction==t){
					transZ[nowRender] += sign*10;
				}
				else if(gaction==r){
					rotateZ[nowRender] += sign*0.1;
				}

			}
			else if(mode==V){
				if(vaction==e){
					eye[2] += sign*0.1;
				}
				else if(vaction==c){
					center[2] += sign*0.1;
				}
				else if(vaction==u){
					up[2] += sign*0.1;
				}
			}
			else if(mode==L){
				Lposition[2] +=sign*0.1f;
			}
			break;
		

	}
	printf("NowOperation: ");
	if(mode==G){
		printf("Geometric Transform -- ");
		
		switch(gaction){

			case s:
				printf("Scale！\n"); 
			break;
			case t:
				printf("Translate！\n");
			break;
			case r:
				printf("Rotate！\n");
			break;

		}

	}
	else if(mode==V){
		printf("Viewing Transform -- ");

		switch(vaction){

			case e:
				printf("EyeCoordinate！\n (%f %f %f)\n",eye[0],eye[1],eye[2]); 
			break;
			case u:
				printf("UpVector！\n (%f %f %f)\n",up[0],up[1],up[2]);
			break;
			case c:
				printf("CenterCoordinate！\n (%f %f %f)\n",center[0],center[1],center[2]);
			break;

		}
	}
	else if(mode==P){
		printf("Projection Transform -- ");

	}
	else if(mode==L){
		printf(" light position moving\n");
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
	glutCreateWindow("10320 CS550000 CG HW2 TA");

	glewInit();
	if(glewIsSupported("GL_VERSION_2_0")){
		printf("Ready for OpenGL 2.0\n");
	}else{
		printf("OpenGL 2.0 not supported\n");
		system("pause");
		exit(1);
	}
	setShaders();
	// load obj models through glm
	loadOBJModel(0);
	//loadOBJModel(1);
	//loadOBJModel(2);
	
	// register glut callback functions
	
	glutDisplayFunc (renderScene);
	
	glutIdleFunc    (idle);
	
	glutKeyboardFunc(processNormalKeys);
	
	glutMouseFunc   (processMouse);

	glutMotionFunc  (processMouseMotion);
	
	glEnable(GL_DEPTH_TEST);
	
	// set up shaders here
	//setShaders();
	
	// main loop
	glutMainLoop();
	
	// free
	glmDelete(OBJ);

	return 0;
}

