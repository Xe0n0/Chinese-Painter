
#include "stdafx.h"
#include <stdio.h>
#include <GL/glut.h>
#include <gl/glext.h>
#include <stdio.h>
#include <iostream>
#include<string>
#include<fstream>
#include<sstream>
using namespace std;


int v_num=34835; 
int f_num=69666; 
int vn_num=34835;
GLfloat **point; 
int **fpoint; 
int **fnArr;
GLfloat **vnArr;
string s1,s2,s3,s4;
GLfloat f2,f3,f4;
struct vectors
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
int readfile(string addrstr) 
{
	int i;
	point=new GLfloat*[v_num];
	for (i=0;i<v_num;i++)
	{
		point[i]=new GLfloat[3];
	}
	vnArr=new GLfloat*[vn_num];
	for (i=0;i<vn_num;i++)
	{
		vnArr[i]=new GLfloat[3];
	}
	fpoint=new int*[f_num];
	fnArr=new int*[f_num];
	for (i=0;i<f_num;i++)
	{
		fpoint[i]=new int[3];
		fnArr[i]=new int[3];
	}
	ifstream infile(addrstr.c_str());
	string sline;
	int ii=0,jj=0,kk=0;
	
	while(getline(infile,sline))
	{
		
		if(sline.size()==0)continue;
		if(sline[0]=='v')
		{
			if(sline[1]=='n')
			{
				
				istringstream sin(sline);
				sin>>s1>>f2>>f3>>f4;
				vnArr[ii][0]=f2;
				vnArr[ii][1]=f3;
				vnArr[ii][2]=f4;
				ii++;
				
			}
			else
			{
				
				istringstream sin(sline);
				sin>>s1>>f2>>f3>>f4;
				point[jj][0]=f2;
				point[jj][1]=f3;
				point[jj][2]=f4;
				jj++;
				
			}
		}
		if (sline[0]=='f') 
		{
			
			istringstream in(sline);
			
			int a;
			in>>s1;
			
			int i,k;
			
			for(i=0;i<3;i++)
			{
				in>>s1;
				
				a=0;
				for(k=0;s1[k]!='/';k++)
				{
					a=a*10+(s1[k]-48);
				}
				fpoint[kk][i]=a;
				
				a=0;
				for(k=k+2;k<s1.size();k++)
				{
					a=a*10+(s1[k]-48);
				}
				fnArr[kk][i]=a;
			}	
		kk++;	
		}	
	}
	return 0;
}




void init(void)
{
	readfile("b.obj");
	glClearColor(1.0,1.0,1.0,0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,200,0,200); 
	glColor3f(0.0,0.0,0.0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
	vectors R,N,E,Mid;
	GLfloat m,u,v;
	Mid.x=0;
	Mid.y=-5;
	Mid.z=-30;
    for (int i=0;i<v_num;i++)
	{
		N.x=vnArr[i][0];
		N.y=vnArr[i][1];
		N.z=vnArr[i][2];//顶点法向量
		E.x=point[i][0];
		E.y=point[i][1];
		E.z=point[i][2]-30;
		float alfa=N.x*E.x+N.y*E.y+N.z*E.z;
		R.x=(2*alfa-1)*E.x;
		R.y=(2*alfa-1)*E.y;
		R.z=(2*alfa-1)*E.z;
		m=sqrt(R.x*R.x+R.y*R.y+(R.z+1)*(R.z+1));
		u=R.x/(2*m)+0.5;
		v=R.y/(2*m)+0.5;
		if(alfa<5)continue;
		if(alfa>20)
		{
			float k=(point[i][0]-Mid.x)*N.x+(point[i][1]-Mid.y)*N.y+(point[i][2]-Mid.z)*N.z;
			k=k*0.03+0.2;
			glColor3f(1-k,1-k,1-k);
			glPointSize(2.0f);  
			glBegin(GL_POINTS);
			glVertex2d(u*200, v*200);
			glEnd();
		}
		else
		{
		glColor3f(0.0,0.0,0.0);
		glPointSize(3.0f);  
		glBegin(GL_POINTS);
		glVertex2d(u*200, v*200);
		glEnd();  
		}
	}

	glFlush();
	glutSwapBuffers();
	
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}