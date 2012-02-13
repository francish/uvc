#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <glui.h>
/* function that reads images */ 
GLubyte* readPPMfile(char*);

/* pointer to image for grid texture */
static GLubyte* grid;

/* place to store OpenGL-created names of texture objects */
static GLuint gridTex;

/* current view matrix */
static GLfloat curview[16] = {1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1};  /* foreground */

/* cursor position */ 
static int oldx;
static int oldy;

/* camera x-y position */
static GLfloat cameraPosition[3] = {0.0, 0.0, 5.0};

/* angle of camera around the y axis */
static double angle = 0.0;

/* x-y direction camera is pointing */
static GLfloat cameraDirection[2] = {0.0, 1.0};

/**** User IDs for callbacks ****/
#define RESET_VIEW     500


/*Main GL Window*/
  int win;
/********************************************************************************/

/* draw the sea floor as a texture-mapped square */
void seaFloor(void) {
  GLfloat X = 0.0;
  GLfloat Y = 0.0;     /* center of group of tiles drawn */ 
  
  /* turn on texture mapping and use the texture object created at initialization */
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D,gridTex);

  /* texture coordinates place 5x5 copies of the texture on the rectangle */

  glBegin(GL_QUADS);
   glTexCoord2f(-7.0,-7.0);
   glVertex3f(X-35.0, Y-35.0, 0.0);
   glTexCoord2f(-7.0,7.0);
   glVertex3f(X-35.0, Y+35.0, 0.0);
   glTexCoord2f(7.0,7.0);
   glVertex3f(X+35.0, Y+35.0, 0.0);
   glTexCoord2f(7.0,-7.0);
   glVertex3f(X+35.0, Y-35.0, 0.0);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

void initLighting(void)
{
    static float light1_ambient[] = {0.05, 0.05, 0.05, 1.0};
    static float light1_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    static float light1_specular[] = {2.0, 2.0, 2.0, 1.0};

    static float mat_shininess[] = {100.0};

    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glEnable(GL_LIGHT1);

    glEnable(GL_LIGHTING);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void setLight(void)
{
    static float light0_position[] = {0.0, -1.0, 1.0, 0.0};
    static float light1_position[] = {-0.5, 1.0, 0.5, 0.0};

    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

}

void initTextures(void) {
  /* read in image for texture */ 
  grid = readPPMfile("grid.ppm");
  /* create a texture object */
  glGenTextures(1,&gridTex);
  /* make it the current texture */
  glBindTexture(GL_TEXTURE_2D,gridTex);
  /* make it wrap around */
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
  /* make it filter nicely */
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  /* and put the image data into the texure object */
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,128,128,0,
	       GL_RGB,GL_UNSIGNED_BYTE,grid);
}


/* Should put viewing parameters into Modelview matrix which produce
   transformation required by cameraPosition and cameraAngle parameters */
void placeCamera() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotatef(-90.0,1.0,0.0,0.0); /* rotate to point forward */  
 glTranslatef(0.0, 0.0, -5.0); /* raise camera above floor */
 cameraPosition[0]=0;
 cameraPosition[1] =0;
 cameraPosition[2]=5;
 glTranslatef(cameraPosition[0], cameraPosition[1], -cameraPosition[2]); /* raise camera above floor */
  glGetFloatv(GL_MODELVIEW_MATRIX,curview);
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  initLighting();	
  initTextures();

  /* initialize projection matrix */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-.5,.5,-.5,.5,1.0,35.0);

  /* initialize viewing transform matrix (= camera position) and save */
  placeCamera();

}

void background(void) {

  /* place camera in current position */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixf(curview);

  seaFloor();

}


void foreground(void) {

  /* postion camera */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixf(curview);

  setLight();	     /* position lights after camera transformation */

  /* draw objects after this line */
}

void display(void) {
  static int loop = 0;
  /* erase the frame buffer and the depth buffer */
  glClearColor(0.0,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
//   placeCamera();
   glTranslatef(cameraPosition[0], cameraPosition[1], cameraPosition[2]);
   //   glMultMatrixf(curview);
  loop++;
  if (loop>100) loop = 0;
  if (loop == 1)
  printf("Camera: %.2f %.2f %.2f\n", cameraPosition[0], cameraPosition[1], cameraPosition[2]);
  /* draw background */
  background();

  /* draw foreground */
  foreground();

  glutSwapBuffers();
}

void reshape (int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glutPostRedisplay();
}


/***************************************** myGlutIdle() ***********/

void myGlutIdle( void )
{
  /* According to the GLUT specification, the current window is
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
  if ( glutGetWindow() != win )
    glutSetWindow(win);

  /*  GLUI_Master.sync_live_all();  -- not needed - nothing to sync in this
      application  */

  glutPostRedisplay();
}


void keyboard (unsigned char key, int x, int y){
  GLint params[2];

  switch(key){
//   case 's':
//     scale += .1;
//     glutPostRedisplay();
//     break;
//   case 'd':
//     scale-=.1;
//     glutPostRedisplay();
//     break;
  case 'q':
    exit(0);
    break;
  default:
    break;
  }
}

int main(int argc, char** argv) {


  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500,500);
  glutInitWindowPosition(100,100);
  win =  glutCreateWindow(argv[0]);

  /* initialize background, objects, camera, etc. */
  init();

  /* callback functions */
  GLUI_Master.set_glutDisplayFunc(display);
  GLUI_Master.set_glutReshapeFunc(reshape);
  GLUI_Master.set_glutKeyboardFunc(keyboard);

  /*  GLUI Code */
  //Subwindow doesn't seem to work with mac.. refresh is not correct
  //  GLUI *glui = GLUI_Master.create_glui_subwindow(win, GLUI_SUBWINDOW_RIGHT);
  GLUI *glui = GLUI_Master.create_glui("Controls"/*name*/,0/*flags*/,100+500,100 /*x y*/);
  glui->set_main_gfx_window(win);
  GLUI_Rotation *view_rot = glui->add_rotation("Camera", curview);
  view_rot->set_spin(1.0);
  GLUI_Translation *trans_xy = glui->add_translation("XY", GLUI_TRANSLATION_XY, cameraPosition);
  trans_xy->set_speed(0.005);
  
  GLUI_Translation *trans_x = 
    glui->add_translation( "Objects X", GLUI_TRANSLATION_X, cameraPosition );
  trans_x->set_speed( .005 );
  
  GLUI_Translation *trans_y = 
    glui->add_translation( "Objects Y", GLUI_TRANSLATION_Y, &cameraPosition[1] );
  trans_y->set_speed( .005 );
  
  GLUI_Translation *trans_z = 
    glui->add_translation( "Objects Z", GLUI_TRANSLATION_Z, &cameraPosition[2] );
  trans_z->set_speed( .005 );
  
  glui->add_button("Reset View", RESET_VIEW, placeCamera);

  GLUI_Master.set_glutIdleFunc(myGlutIdle);
  glutMainLoop();
  return(0);
}

/********************************************************************************/
/* function to read in a  RAW, FULL COLOR, .ppm file
/********************************************************************************/

GLubyte* readPPMfile(char* filename) {
  FILE* input;
  int w,h,max,i,j,k;
  char rgb[3];
  GLubyte* vol;
  char buffer[200]; 

  if ((input = fopen(filename,"r")) == NULL) {
    fprintf(stderr,"Cannot open file %s \n",filename);
    exit(1);
  }
  /* read a line of input */
  fgets(buffer,200,input);
  if (strncmp(buffer,"P6",2) != 0) {
    fprintf(stderr,"%s is not a binary PPM file \n",filename);
    exit(1);
  }

  /* get second line, ignoring comments */
  do {
    fgets(buffer,200,input);
  } while (strncmp(buffer,"#",1) == 0);

  if (sscanf(buffer,"%d %d",&w,&h) != 2) {
    fprintf(stderr,"can't read sizes! \n");
    exit(1);
  }

  /* third line, ignoring comments */
  do {
    fgets(buffer,200,input);
  } while (strncmp(buffer,"#",1) == 0);

  if (sscanf(buffer,"%d",&max) != 1) {
    fprintf(stderr,"what about max size? \n");
    exit(1);
  }

  fprintf(stderr,"reading %d columns %d rows \n",w,h);

  vol = (GLubyte*) malloc(w*h*3*sizeof(GLubyte));
  for (i=h-1; i>=0; i--) {
    for (j=0; j<w; j++) {
      fread(rgb,sizeof(char),3,input);
      for (k=0; k<3; k++)
	*(vol+i*w*3+j*3+k) = (GLubyte) rgb[k];
    }
  }

  return(vol);
}
