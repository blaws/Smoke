// Smoke.c
// blaws

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <GLUT/glut.h>

int wsize = 750;
bool paused = false;
int squaresize = 16;
GLubyte square[16][16][2];
GLuint textureObj;
std::vector<int> arrayOfSquares;  // contains x- and y-coords, x- and y- velocity, current rotation, rotation velocity, and frames since creation
int smokeAmt=5,varAmt=0;  // control amount and placement of smoke
int wind=0;

// creates random, smoothed noise texture to use as the basis of the smoke
void makeSquare(){
  int i,j,k,coeffs[6];
  double offsets[6];
  for(i=0;i<6;i++){
    offsets[i] = 2*M_PI * (rand()%1000)/1000.0;
  }

  for(i=0;i<squaresize;i++){
    for(j=0;j<squaresize;j++){
      square[i][j][0] = 0;
      for(k=0;k<6;k++) square[i][j][0]+=40*sin(sin(i+offsets[k])*(j+1)*2);
      square[i][j][1] = 255*(1-pow(hypot(squaresize/2-i,squaresize/2-j)/hypot(squaresize/2,squaresize/2),.5));
    }
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  glGenTextures(1,&textureObj);
  glBindTexture(GL_TEXTURE_2D,textureObj);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE_ALPHA,squaresize,squaresize,0,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,square);
}

void reshape(int w,int h){
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0,wsize,0,wsize);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void display(){
  int i;
  double alpha,size;
  glClear(GL_COLOR_BUFFER_BIT);

  glBindTexture(GL_TEXTURE_2D,textureObj);
  glMatrixMode(GL_MODELVIEW_MATRIX);
  glLoadIdentity();

  for(i=arrayOfSquares.size()-7;i>=0;i-=7){
    // place
    glLoadIdentity();
    glTranslatef(arrayOfSquares[i],arrayOfSquares[i+1],0.0);
    glRotatef(arrayOfSquares[i+4],0.0,0.0,1.0);

    // set apparent alpha based on time since creation
    alpha = 1.0 - arrayOfSquares[i+6]/60.0;
    glColor4f(alpha,alpha,alpha,alpha);

    // draw with size based on time
    size = .075 * (1+arrayOfSquares[i+6]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0,0.0);
    glVertex2f(-squaresize*size,-squaresize*size);
    glTexCoord2f(1.0,0.0);
    glVertex2f(squaresize*size,-squaresize*size);
    glTexCoord2f(1.0,1.0);
    glVertex2f(squaresize*size,squaresize*size);
    glTexCoord2f(0.0,1.0);
    glVertex2f(-squaresize*size,squaresize*size);
    glEnd();

    // reset location
    glLoadIdentity;
  }

  glutSwapBuffers();
}

// runs every 50 ms; moves the smoke squares
void increment(int t){
  int i,j;
  while(rand()%(smokeAmt+1)){  // add square
    arrayOfSquares.push_back(wsize/2+rand()%(2*varAmt+1)-varAmt);  // X
    arrayOfSquares.push_back(wsize/4+rand()%(2*varAmt+1)-varAmt);  // Y
    arrayOfSquares.push_back(rand()%5-2);  // X-vel
    arrayOfSquares.push_back(rand()%3);  // Y-vel
    arrayOfSquares.push_back(rand()%360);  // rotation
    arrayOfSquares.push_back(rand()%11-5);  // rotation-vel
    arrayOfSquares.push_back(0);  // timer
  }

  for(i=0;i<arrayOfSquares.size();i+=7){
    // delete if out of screen or at time of 3 sec
    if(arrayOfSquares[i]<(-squaresize*4) || arrayOfSquares[i]>wsize+squaresize*4 || arrayOfSquares[i+1]>wsize+squaresize*4 || arrayOfSquares[i+6]>60){
      for(j=0;j<7;j++){
	arrayOfSquares.erase(arrayOfSquares.begin()+i);
      }
      i -= 7;
      continue;
    }

    // add velocities to current position
    arrayOfSquares[i] += arrayOfSquares[i+2];
    arrayOfSquares[i+1] += arrayOfSquares[i+3];
    arrayOfSquares[i+4] += arrayOfSquares[i+5];

    // accelerate
    if(rand()%5==0) arrayOfSquares[i+3]++;
    if(wind && rand()%(abs(wind)+1) && rand()%5==0) arrayOfSquares[i+2]+=(wind)/abs(wind);

    // add to timer
    arrayOfSquares[i+6]++;
  }

  // reset timer
  if(!paused) glutTimerFunc(50,increment,0);
  glutPostRedisplay();
}

void reset(){
  arrayOfSquares.clear();
  makeSquare();
  paused = false;
  wind = 0;
  smokeAmt = 5;
  varAmt = 0;
}

void keyboard(unsigned char key,int x,int y){
  switch(key){
  case 'q':
  case 'Q':
  case 27:
    exit(0);
    break;
  case 'r':
  case 'R':
    reset();
    break;
  case 'p':
  case 'P':
    if(paused){
      glutTimerFunc(50,increment,0);
      paused = false;
    }
    else paused=true;
    break;
  default:
    break;
  }
}

void keyboardSpecials(int key,int x,int y){
  switch(key){
  case GLUT_KEY_UP:
    if(smokeAmt<10) smokeAmt++;
    break;
  case GLUT_KEY_DOWN:
    if(smokeAmt>0) smokeAmt--;
    break;
  case GLUT_KEY_LEFT:
    if(wind>-10) wind--;
    break;
  case GLUT_KEY_RIGHT:
    if(wind<10) wind++;
    break;
  default:
    break;
  }
}

int main(int argc,char* argv[]){
  srand(time(NULL));

  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(wsize,wsize);
  glutInitWindowPosition((1280-wsize)/2,(800-wsize)/2);
  glutCreateWindow("Smoke");

  glClearColor(1.0,1.0,1.0,1.0);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(keyboardSpecials);

  reset();
  glutTimerFunc(50,increment,0);
  glutMainLoop();
}
