
#include <stdio.h>
#include <string.h>
#include <math.h>
#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "mpi.h"

unsigned int* GRID;
int W = 200, H = 100;
int* KEY;
int GRID_WIDTH, GRID_HEIGHT;
unsigned int *FULL_GRID, *NEW_GRID, *THE_GRID;
int BOTTOM;
int TOP;
int         rank,size;
MPI_Status  status;
int         tag=0;
int xWidth = 400, yWidth = 400;

unsigned int get(int x, int y){
  if(x<0){
    x = GRID_WIDTH+x;
  }
  if(x>=GRID_WIDTH){
    x= x - GRID_WIDTH;
  }
  return FULL_GRID[(y+1)*GRID_WIDTH+x];
}
unsigned int setThe(int x, int y, unsigned int d){
  if(x<0) x = GRID_WIDTH+x;
  return THE_GRID[y*GRID_WIDTH+x] = d;
}
unsigned int set(int x, int y, unsigned int d){
  return NEW_GRID[(y+1)*GRID_WIDTH+x] = d;
}

void calc(){
  MPI_Send(FULL_GRID+GRID_WIDTH, GRID_WIDTH, MPI_UNSIGNED,TOP,tag,MPI_COMM_WORLD);
   //   printf("Column from %d to %d\n", rank, TOP);

      MPI_Send(FULL_GRID+GRID_HEIGHT*GRID_WIDTH, GRID_WIDTH, MPI_UNSIGNED,BOTTOM,tag,MPI_COMM_WORLD);
 //  printf("Column from %d to %d\n", rank, BOTTOM);

MPI_Recv(FULL_GRID+(1+GRID_HEIGHT)*GRID_WIDTH,GRID_WIDTH,MPI_UNSIGNED,BOTTOM,tag,MPI_COMM_WORLD,&status);
   //   printf("Got Column from %d to %d\n", BOTTOM, rank);

  MPI_Recv(FULL_GRID, GRID_WIDTH, MPI_UNSIGNED, TOP, tag,MPI_COMM_WORLD,&status);
   //   printf("Got Column from %d to %d\n", TOP, rank);

  int x, y;
  int sum = 0;
  /*
  for(y=-1; y<=GRID_HEIGHT; y++){
  for(x=-1; x<=GRID_WIDTH; x++){
    if(get(x, y)==0)
    printf(".");
    else
    printf("O");
  }
  printf("\n");
  }*/
  #pragma omp parallel for
  for(x=0; x<GRID_WIDTH; x++)
  for(y=0; y<GRID_HEIGHT; y++){
    sum=get(x-1, y)+get(x-1, y-1)+get(x-1, y+1)+get(x+1, y-1)+get(x+1, y+1)+get(x+1, y)+get(x, y-1)+get(x,y+1);
    set(x, y, (get(x, y)==1)?((sum==2 || sum==3)?1:0):((sum==3)?1:0) );
  }
  unsigned int* temp = FULL_GRID;
  FULL_GRID = NEW_GRID;
  NEW_GRID = temp;
}

void displayfunc(void)
{
  int i;
  int x, y;
  for(i = 1; i<=size; i++){
      MPI_Send(KEY,1,MPI_UNSIGNED,i,tag,MPI_COMM_WORLD);
//      printf("Sent request to %d\n", i);
  }
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_POINTS);
// double xSize = 1, ySize = 1;
  double xSize = (double)xWidth/W, ySize = (double)yWidth/H;
  for(i = 1; i<=size; i++){
    MPI_Recv(FULL_GRID+GRID_WIDTH, GRID_WIDTH*GRID_HEIGHT, MPI_UNSIGNED,
    MPI_ANY_SOURCE, tag,MPI_COMM_WORLD,&status);
    for(y = 0; y<GRID_HEIGHT; y++){
      for(x = 0; x<GRID_WIDTH; x++){
	if(get(x, y)){
	  glColor3f(1,0,0);
//	  printf("O");
	}
	else{
	  glColor3f(0,0,1);
//	  printf(".");
	}
	//glRectf(xSize*x, ySize*(y+GRID_HEIGHT*(status.MPI_SOURCE - 1)), xSize*(x+1), ySize*(y+GRID_HEIGHT*(status.MPI_SOURCE-1)+1)  );
	int k, l;
	for(k = 0; k<xSize; k++)
	for(l = 0; l<ySize; l++)
	glVertex2f(xSize*x+k, l+ySize*(y+GRID_HEIGHT*(status.MPI_SOURCE - 1)));
      }
//      printf("\n");
    }
  }

   glEnd();
   glutSwapBuffers();
}

void mousefunc(int button,int state,int xscr,int yscr)
{
  if(state==GLUT_DOWN){
   if(button==GLUT_LEFT_BUTTON)
      {
      }
      else if(button == GLUT_RIGHT_BUTTON){
      }
  }
}

void motionfunc(int xscr,int yscr)
{
   printf("(x,y)=(%d,%d)\n",xscr,yscr);
}

void keyfunc(unsigned char key,int xscr,int yscr)
{
 switch(key){

   }

         glutPostRedisplay();
}

void closefunc(void)
{
   printf("Window closed.\n");
}

void reshapefunc(int wscr,int hscr)
{
  xWidth = wscr;yWidth = hscr;
   glViewport(0,0,(GLsizei)wscr,(GLsizei)hscr);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0,(GLsizei)wscr,0,(GLsizei)hscr);
   glMatrixMode(GL_MODELVIEW);
}

void import(FILE* f){
 int x = 0, y = 0;
 char bin;
 char last = '\0';
 while(fread(&bin, sizeof(char),1,f)!=-1){
   printf("%c", bin, EOF);
   if(bin==EOF || (bin==last && last == '\n')) break;
   if(bin=='\n'){x = 0; y++;}
   else{
   if(bin=='0' || bin=='*' || bin=='O' || bin=='o')  THE_GRID[y*W+x]= 1;
     x++;
   }
   last = bin;
 }
}

void printTheGrid(){
  int i, j;
   for(j = 0; j<H; j++){
 for(i = 0; i<W; i++){
     printf("%c", (THE_GRID[j*W+i])?'O':'.');
   }
   printf("\n");
 }

}
void printG(){
int i, j;
printf("---------------------------------------\n");
for(j = 0; j<GRID_HEIGHT; j++)
{
for(i = 0; i<W; i++)
printf("%c",(get(i, j))?'O':'.');
printf("\n");
}
}
int main(int argc,char* argv[])
{
    KEY = (unsigned int*)malloc(sizeof(unsigned int));
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  size--;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  printf("*** STARTING %d/%d\n",rank,size);
    GRID_WIDTH = W;
    GRID_HEIGHT = H/size;
  if(rank == 0){
    FULL_GRID = (unsigned int*)malloc(sizeof(unsigned int)*GRID_WIDTH*(GRID_HEIGHT+2));
    THE_GRID = (unsigned int*)malloc(sizeof(unsigned int)*W*H);
    memset(THE_GRID, 0, sizeof(unsigned int)*W*H);
    import(fopen("glider (copy).txt", "r"));
    printf("IMPORT COMPLETE\n");
   // printTheGrid();
    int i;
    for(i = 1; i<=size; i++){
   MPI_Send(THE_GRID+(i-1)*GRID_WIDTH*GRID_HEIGHT,GRID_HEIGHT*GRID_WIDTH,MPI_UNSIGNED,i,tag,MPI_COMM_WORLD);
    }
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(xWidth, yWidth);
    glutCreateWindow("Conway's game of life");
    glClearColor(1.0,1.0,1.0,0.0);
    glShadeModel(GL_SMOOTH);

    glutDisplayFunc(displayfunc);    // register callback functions
    glutIdleFunc(NULL);             // no animation
    glutMouseFunc(mousefunc);
    glutKeyboardFunc(keyfunc);
    glutReshapeFunc(reshapefunc);
    glutWMCloseFunc(closefunc);
   //
    glutMainLoop();
  }
  else{
    TOP = (rank-1+size)%size;
    BOTTOM = (rank+1)%size;
    if(BOTTOM==0) BOTTOM = size;
    if(TOP==0) TOP = size;
    NEW_GRID = (unsigned int*)malloc(sizeof(unsigned int)*GRID_WIDTH*(GRID_HEIGHT+2));
    FULL_GRID = (unsigned int*)malloc(sizeof(unsigned int)*GRID_WIDTH*(GRID_HEIGHT+2));


MPI_Recv(FULL_GRID+GRID_WIDTH,GRID_HEIGHT*GRID_WIDTH,MPI_UNSIGNED,0,tag,MPI_COMM_WORLD,&status);
//printf("%d-----------\n", FULL_GRID[GRID_WIDTH]);
//printG();
 while(1){
      MPI_Recv(KEY,1,MPI_UNSIGNED,0,tag,MPI_COMM_WORLD,&status);
//      printf("%d Received request\n", rank);
      calc();
      MPI_Send(FULL_GRID+GRID_WIDTH,GRID_HEIGHT*GRID_WIDTH,MPI_UNSIGNED,0,tag,MPI_COMM_WORLD);
  //    printf("%d Sent data\n", rank);
    }
  }
  return 0;
}
