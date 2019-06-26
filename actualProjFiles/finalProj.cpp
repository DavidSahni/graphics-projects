#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "glslprogram.h"




//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.


// title of these windows:

const char *WINDOWTITLE = { "OpenGL / GLUT Project 1 -- David Sahni" };
const char *GLUITITLE   = { "User Interface Window" };


// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };


// the escape key:

#define ESCAPE		0x1b

GLSLProgram	*Pattern;
float Time;
float bTime;
#define ANIM_CYCLE	9000
#define BNC_CYCLE 4000


// initial window size:

const int INIT_WINDOW_SIZE = { 600 };


// size of the box:

const float BOXSIZE = { 3.f };
const float HEXSIZE = { 1.f };



// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };

const float B_HEIGHT = .8;


// which projection:

enum Projections
{
	ORTHO,
	PERSP
};


// which button:

enum ButtonVals
{
	RESET,
	QUIT,
	TEXTURE,
	ANIMATE
};


// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };


// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };


// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};


// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to use the z-buffer
GLuint	BoxList;				// object display list
GLuint  HexList;
GLuint	OBorderList;
GLuint	IBorderList;
GLuint	tex0, tex1;
GLuint	SphereList;
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees			// height of one hex trapezoid (for calculating translations)
bool	texOn;
bool	ANIMATE_FLG;
float	animateVar;
bool	Distort;		// global -- true means to distort the texture
bool	invs;
bool	swapped;
int		axesCount;
float		AXES[] = { 1, 0, 1 };
int	animVert;
int	animFrag;
int mode;

float hThirdLen = HEXSIZE / 3.f;
float hh = HEXSIZE / 6.f;
float hx = HEXSIZE / 12;
float hy = (hh / 2)*sqrt(3); //height of one trapezoidal face
float hexHeight = hy ; //distance between two faces



// function prototypes:

void	Animate( );
void	Display( );
void	displayShine(float, bool);
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );
void	partyMode();
void	Axes( float );
void	HsvRgb( float[3], float [3] );
void	genShineList();
void	MjbSphere(float radius, int slices, int stacks);
unsigned char* BmpToTexture(char *filename, int *width, int *height);
struct point *	PtsPointer(int lat, int lng);
void	DrawPoint(struct point *p);
void	texSetup();
void	InitTextures();
void	genInnerBorder(float);
float*	MulArray3(float factor, float arr0[3]);
float*	Array3(float, float, float);
void	SetSpotLight(int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b);
void	SetPointLight(int ilight, float x, float y, float z, float r, float g, float b, bool);
void	SetMaterial(float r, float g, float b, float shiny);
float	degToRad(float);


// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	srand(time(NULL));

	glutInit( &argc, argv );


	// setup all the graphics stuff:

	InitGraphics( );


	// create the display structures that will not change:

	InitLists( );

	InitTextures();


	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );


	// setup all the user interface stuff:

	InitMenus( );


	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );


	// this is here to make the compiler happy:

	return 0;
}





void InitTextures() {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &tex0);

	glBindTexture(GL_TEXTURE_2D, tex0);
	texSetup();

	int width, height;
	unsigned char* Texture = BmpToTexture("tech_skin_1.bmp", &width, &height);
	printf("W H: %i, %i\n", width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	glGenTextures(1, &tex1);



}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void swapAxes() {
	if (invs == true && animVert) { //we are expanding
		AXES[axesCount] = 0;
		axesCount += 1;
		axesCount = axesCount % 3;
		AXES[axesCount] = 1;
	}

}

void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:


	// force a call to Display( ) next time it is convenient:
	if (ANIMATE_FLG) {
		float t;
		int ms = glutGet(GLUT_ELAPSED_TIME);
		int bms = ms;
		ms %= ANIM_CYCLE;
		bms %= BNC_CYCLE;
		Time = (float)ms / (float)(ANIM_CYCLE);
		t = (float)bms / (float)(BNC_CYCLE - 250);
		if (t <= 1) {
			swapped = false;
			if (invs) {
				bTime = t;
			}
			else {
				bTime = 1 - t;
			}
//			printf("%f", Time);
		}
		if (t > 1 && !swapped) {
			if (invs) {
				bTime = 1;
			}
			else {
				bTime = 0;
			}
			printf("swapped invs");
			invs = !invs;
			swapped = true;
		}

		
		


		glutSetWindow(MainWindow);
		glutPostRedisplay();

	}


}



void texSetup() {

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

}


// draw the complete scene:

void
Display()
{
	if (DebugOn != 0)
	{
		fprintf(stderr, "Display\n");
	}


	// set which window we want to do the graphics into:

	glutSetWindow(MainWindow);

	// erase the background:

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (DepthBufferOn != 0)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);


	// specify shading to be flat:

	glShadeModel(GL_SMOOTH);


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (WhichProjection == ORTHO)
		glOrtho(-3., 3., -3., 3., 0.1, 1000.);
	else
		gluPerspective(90., 1., 0.1, 1000.);


	// place the objects into the scene:
	float White[] = { 1, 1, 1, 1 };
	// place the objects into the scene:
	if (mode > 3) {
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, MulArray3(.2f, White));
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();



	//mode 1: just the shine original shine
	//mode 2: three shines
	//mode 3: three shines with animation
	//mode 4: three shines animation lighting
	//mode 5: final product
	// set the eye position, look-at position, and up-vector:

	gluLookAt(0, .2, 5, 0., 0., 0., 0., 1., 0.);



	// rotate the scene:


	glRotatef((GLfloat)Yrot, 0., 1., 0.);
	glRotatef((GLfloat)Xrot, 1., 0., 0.);




	// uniformly scale the scene:

	if (Scale < MINSCALE)
		Scale = MINSCALE;

	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

	if (mode > 3){
		SetPointLight(GL_LIGHT0, 0, -.5, 3, 1, 1, 1, Distort);
	//SetPointLight(GL_LIGHT2, 0, 2, 0, 1, 1, 1, true);
	SetPointLight(GL_LIGHT1, 3, -3, 0, 1, 1, 1, Distort);
	}


	// set the fog parameters:

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}


	// possibly draw the axes:

	if (AxesOn != 0)
	{
		glColor3fv(&Colors[WhichColor][0]);
		glCallList(AxesList);
	}


	// since we are using glScalef( ), be sure normals get unitized:

	glEnable(GL_NORMALIZE);

	if (mode > 3) {
		glEnable(GL_LIGHTING);
		SetMaterial(22 / 256., 243 / 256., 255 / 256., 45);
	}
	else {
			glColor3f(22 / 256., 243 / 256., 255 / 256.);

	}


	// draw the current object:
	//if (texOn) {
	//	int xColor = Xmouse;
	//	int yColor = Ymouse;

	//	int zColor = (Xmouse + Ymouse) / 2;
	//	xColor = (xColor % 360);
	//	yColor = (yColor % 256);
	//	float rgb[3];
	//	float hsv[3] = { xColor, yColor / 256., .75 };
	//	HsvRgb(hsv, rgb);
	//	glColor3fv(rgb);

	//}
	//else {
	//	glColor3f(22 / 256., 243 / 256., 255 / 256.);
	//}

	if (mode > 3) {
		Pattern->Use(1);
	}
	if (mode > 1){
		glPushMatrix();
		glTranslatef(0, bTime * B_HEIGHT, 0);
		glTranslatef(-HEXSIZE / 2, 0, 0);
		glRotatef(-90, 1, 0, 0);

		glPushMatrix();
		glTranslatef(0, 0, -hexHeight);
		glRotatef(25, 1, 0, 0);
		if (ANIMATE_FLG) {
			float ang = 360 * Time;
			glRotatef(ang, 0, 0, 1);
		}
		displayShine(0, true);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(HEXSIZE / 2, HEXSIZE/2 + (2* hexHeight), 0);
		glRotatef(45, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		if (ANIMATE_FLG) {
			float ang = 360 * Time;
			glRotatef(-ang, 0, 0, 1);
		}
		displayShine(0, true);
	
		glPopMatrix();

		glPushMatrix();
		glTranslatef(HEXSIZE, HEXSIZE, HEXSIZE-hexHeight);
		//glRotatef(45, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		if (ANIMATE_FLG) {
			float ang = 360 * Time;
			glRotatef(ang, 0, 0, 1);
		}
		displayShine(0, true);
		glPopMatrix();

		glPopMatrix();
	}
	else {
		displayShine(0, true);
	}
	if (mode > 4) {
		Pattern->Use(0);

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);

		SetMaterial(255 / 256., 255 / 256., 255 / 256., 100);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex0);
		texSetup();
		glPushMatrix();
		glTranslatef(0, -2.5, 0);
		glCallList(BoxList);
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
	}



	//if( DepthFightingOn != 0 )
	//{
	//	glPushMatrix( );
	//		glRotatef( 90.,   0., 1., 0. );
	//		glCallList( HexList );
	//	glPopMatrix( );
	//}


	//DoRasterString( 5., 5., 0., "Text That Doesn't" );


	// swap the double-buffered framebuffers:

	glutSwapBuffers( );


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}

void displayShine(float ang, bool border){
	float inAng = ang;
	float x = -HEXSIZE / 2.;
	float y =-HEXSIZE + hexHeight;

	for (int i = 0; i < 6; i++){
		glPushMatrix();

		
		glTranslatef(x, y, 0);
		glRotatef(ang, 0, 0, 1);
		glCallList(HexList);
		glPopMatrix();
		if (border){
			glPushMatrix();
			glTranslatef(x, y, 0);
			glRotatef(ang, 0, 0, 1);
			glRotatef(90, 1, 0, 0);
			glCallList(OBorderList);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(x, y, 0);
			glRotatef(ang, 0, 0, 1);
			glTranslatef(hx, hy, 0);
			glRotatef(90, 1, 0, 0);
			glCallList(IBorderList);
			glPopMatrix();
		}

		if (i == 0) {
			x += HEXSIZE;
		}
		else if (i == 1) {
			y += HEXSIZE - hexHeight;
			x += HEXSIZE / 2;
		}
		else if (i == 2) {
			y += HEXSIZE - hexHeight;
			x -= HEXSIZE / 2;
		}
		else if (i == 3) {
			//y += HEXSIZE / 2;
			x -= HEXSIZE;
		}
		else {
			y -= HEXSIZE - hexHeight;
			x -= HEXSIZE / 2;
		}
		ang += 60;
	}
	if (border) {
		glPushMatrix();
		glFrontFace(GL_CW);
		glTranslatef(0, 0, hexHeight);
		displayShine(inAng, false);
	}
	else {
		glFrontFace(GL_CCW);

		glPopMatrix();
	}
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case TEXTURE:
			partyMode();
			break;

		case ANIMATE:
			ANIMATE_FLG = !ANIMATE_FLG;
			Time = 0;
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void partyMode() {
	texOn = !texOn;
}

void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );
	glutAddMenuEntry("Toggle Texture", TEXTURE);
	glutAddMenuEntry("Animate", ANIMATE);

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}





// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( Animate );





	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	Pattern = new GLSLProgram();
	bool valid = Pattern->Create("pattern.vert", "pattern.frag");
	if (!valid)
	{
		fprintf(stderr, "Shader cannot be created!\n");
		DoMainMenu(QUIT);
	}
	else
	{
		fprintf(stderr, "Shader created.\n");
	}
	Pattern->SetVerbose(false);

}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )


void genShineList() {



	HexList = glGenLists(1);
	glNewList(HexList, GL_COMPILE);
	glBegin(GL_TRIANGLE_STRIP);
	float drawx = 0;
	float drawy = 0;
	for (int i = 0; i < 13; i++) {
		glNormal3f(0, 1, 0);
		glVertex3f(drawx, drawy, 0);
		drawx += hx;
		if (i % 2 == 0) {
			drawy += hy;
		}
		else {
			drawy -= hy;
		}
	}
	printf("drawx: %f\n", drawx - hx);
	glEnd();
	glEndList();

	float borderL =hx;
	float borderW = hexHeight;
	OBorderList = glGenLists(1);
	glNewList(OBorderList, GL_COMPILE);
	glBegin(GL_TRIANGLE_STRIP);
	drawx = 0;
	drawy = borderW;
	glNormal3f(0, -1, 0);
	glVertex3f(drawx, drawy, 0);
	drawy -= borderW;
	int count = 0;
	while (count < 13) {
		glNormal3f(0, -1, 0);
		glVertex3f(drawx, drawy, 0);
		if (drawy > 0) {
			drawy = 0;
		}
		else {
			drawy = borderW;
		}
		drawx += borderL;
		printf("drawx: %f", drawx);
		count++;
	}
	glNormal3f(0, -1, 0);
	glVertex3f(drawx - borderL, drawy, 0);
	glEnd();
	glEndList();
	genInnerBorder(borderW);

}

void genInnerBorder(float borderW) {
	IBorderList = glGenLists(1);
	glNewList(IBorderList, GL_COMPILE);
	glBegin(GL_TRIANGLE_STRIP);
	float drawx = 0;
	float drawy = 0;
	float borderL = hx;
	glNormal3f(0, -1, 0);
	glVertex3f(drawx, drawy, 0);
	drawy += borderW;
	int count = 0;
	while (count < 11) {
		glNormal3f(0, -1, 0);
		glVertex3f(drawx, drawy, 0);
		if (drawy > 0) {
			drawy = 0;
		}
		else {
			drawy = borderW;
		}
		drawx += borderL;
		printf("drawx: %f", drawx);
		count++;
	}
	glNormal3f(0, -1, 0);
	glVertex3f(drawx - borderL, drawy, 0);
	glEnd();
	glEndList();
}







void
InitLists( )
{
	glutSetWindow(MainWindow);
	
	genShineList();

	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;

	// create the object:

	BoxList = glGenLists( 1 );
	glNewList( BoxList, GL_COMPILE );
	glPushMatrix();

		glBegin( GL_QUADS );

			glNormal3f( 0., 0.,  1. );
				glTexCoord2f(0., 0.);
				glVertex3f( -dx, -dy,  dz );
				glTexCoord2f(0., 1.);
				glVertex3f(  dx, -dy,  dz );
				glTexCoord2f(1., 1.);
				glVertex3f(  dx,  dy,  dz );
				glTexCoord2f(1., 0.);
				glVertex3f( -dx,  dy,  dz );

			glNormal3f( 0., 0., -1. );
				glTexCoord2f( 0., 0. );
				glVertex3f( -dx, -dy, -dz );
				glTexCoord2f( 0., 1. );
				glVertex3f( -dx,  dy, -dz );
				glTexCoord2f( 1., 1. );
				glVertex3f(  dx,  dy, -dz );
				glTexCoord2f( 1., 0. );
				glVertex3f(  dx, -dy, -dz );

			glNormal3f(  1., 0., 0. );
				glTexCoord2f(0., 0.);
				glVertex3f(  dx, -dy,  dz );
				glTexCoord2f(0., 1.);
				glVertex3f(  dx, -dy, -dz );
				glTexCoord2f(1., 1.);
				glVertex3f(  dx,  dy, -dz );
				glTexCoord2f(1., 0.);
				glVertex3f(  dx,  dy,  dz );

			glNormal3f( -1., 0., 0. );
			glTexCoord2f(0., 0.);
				glVertex3f( -dx, -dy,  dz );
				glTexCoord2f(0., 1.);
				glVertex3f( -dx,  dy,  dz );
				glTexCoord2f(1., 1.);
				glVertex3f( -dx,  dy, -dz );
				glTexCoord2f(1., 0.);
				glVertex3f( -dx, -dy, -dz );

			glNormal3f( 0.,  1., 0. );
			glTexCoord2f(0., 0.);
				glVertex3f( -dx,  dy,  dz );
				glTexCoord2f(0., 1.);
				glVertex3f(  dx,  dy,  dz );
				glTexCoord2f(1., 1.);
				glVertex3f(  dx,  dy, -dz );
				glTexCoord2f(1., 0.);
				glVertex3f( -dx,  dy, -dz );

			glNormal3f( 0., -1., 0. );
			glTexCoord2f(0., 0.);
				glVertex3f( -dx, -dy,  dz );
				glTexCoord2f(0., 1.);
				glVertex3f( -dx, -dy, -dz );
				glTexCoord2f(1., 1.);
				glVertex3f(  dx, -dy, -dz );
				glTexCoord2f(1., 0.);
				glVertex3f(  dx, -dy,  dz );

		glEnd( );
		glPopMatrix();

	glEndList( );


	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );


	



}



// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'l':
		case 'L':
			Distort = !Distort;
			break;

		case 'f':
		case 'F':
			ANIMATE_FLG = !ANIMATE_FLG;
			break;
		case 'V':
			animFrag = 0;
			animVert = 1;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler
		case '1':
			mode = 1;
			glutPostRedisplay();
			break;
		case '2':
			mode = 2;
			glutPostRedisplay();
			break;
		case '3':
			mode = 3;
			ANIMATE_FLG = true;
			glutPostRedisplay();
			break;
		case '4':
			mode = 4;
			ANIMATE_FLG = true;
			glutPostRedisplay();
			break;
		case '5':
			mode = 5;
			ANIMATE_FLG = true;
			glutPostRedisplay();
			break;

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}


	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 0;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	WhichColor = WHITE;
	WhichProjection = ORTHO;
	Xrot = Yrot = 0.;
	texOn = false;
	ANIMATE_FLG = false;
	animateVar = true;
	Distort = true;
	invs = true;
	swapped = false;
	axesCount = 0;
	AXES[0] = 1;
	AXES[1] = 0;
	AXES[2] = 1;
	Time = 1;
	bTime = 0;
	animVert = 0;
	animFrag = 0;
	mode = 0;
}


// called when user resizes the window:

void
Resize(int width, int height)
{
	if (DebugOn != 0)
		fprintf(stderr, "ReSize: %d, %d\n", width, height);

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


float degToRad(float deg) {
	float rad = deg * (M_PI / 180.);
	return rad;
}

void SetMaterial(float r, float g, float b, float shiny) {

	float White[] = { 1.,1.,1.,1. };

		glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
		glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4, White));
		glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., White));
		glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
		glMaterialf(GL_BACK, GL_SHININESS, 5.);

		glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
		glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
		glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.8f, White));
		glMaterialf(GL_FRONT, GL_SHININESS, shiny);


}

void SetPointLight(int ilight, float x, float y, float z, float r, float g, float b, bool sphere) {
	if (sphere) {
		glPushMatrix();
		glTranslatef(x, y, z);
		glColor3f(102. / 255., 1, 102. / 255.);
		glutSolidSphere(.01, 30, 30);
		glPopMatrix();
	}
	
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_AMBIENT, Array3(0, 0, 0));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0);

}

void SetSpotLight(int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b) {

	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_SPOT_DIRECTION, Array3(xdir, ydir, zdir));
	glLightf(ilight, GL_SPOT_EXPONENT, 10);
	glLightf(ilight, GL_SPOT_CUTOFF, 30.);
	glLightfv(ilight, GL_AMBIENT, Array3(0, 0, 0));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0);

}

float * Array3(float a, float b, float c) {
	static float arr[4];
	arr[0] = a;
	arr[1] = b;
	arr[2] = c;
	arr[3] = 1.;
	return arr;

}

float * MulArray3(float factor, float arr0[3]) {
	static float arr[4];
	arr[0] = factor * arr0[0];
	arr[1] = factor * arr0[1];
	arr[2] = factor * arr0[2];
	arr[3] = 1.;
	return arr;

}


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r, g, b;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}



struct point {
	float x, y, z;		// coordinates
	float nx, ny, nz;	// surface normal
	float s, t;		// texture coords
};

int		NumLngs, NumLats;
struct point *	Pts;

struct point *
	PtsPointer(int lat, int lng)
{
	if (lat < 0)	lat += (NumLats - 1);
	if (lng < 0)	lng += (NumLngs - 1);
	if (lat > NumLats - 1)	lat -= (NumLats - 1);
	if (lng > NumLngs - 1)	lng -= (NumLngs - 1);
	return &Pts[NumLngs*lat + lng];
}



void
DrawPoint(struct point *p)
{
	glNormal3f(p->nx, p->ny, p->nz);
	glTexCoord2f(p->s, p->t);
	glVertex3f(p->x, p->y, p->z);
}

void
MjbSphere(float radius, int slices, int stacks)
{
	//glutSetWindow(MainWindow);
	struct point top, bot;		// top, bottom points
	struct point *p;

	// set the globals:

	NumLngs = slices;
	NumLats = stacks;

	if (NumLngs < 3)
		NumLngs = 3;

	if (NumLats < 3)
		NumLats = 3;


	// allocate the point data structure:

	Pts = new struct point[NumLngs * NumLats];


	// fill the Pts structure:
	int rev = 1;
	for (int ilat = 0; ilat < NumLats; ilat++)
	{
		float lat = -M_PI / 2. + M_PI * (float)ilat / (float)(NumLats - 1);
		float xz = cos(lat);
		float y = sin(lat);
		for (int ilng = 0; ilng < NumLngs; ilng++)
		
		{
			float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NumLngs - 1);
			float x = xz * cos(lng);
			float z = -xz * sin(lng);
			p = PtsPointer(ilat, ilng);
			p->x = radius * x;
			p->y = radius * y;
			p->z = radius * z;
			p->nx = x;
			p->ny = y;
			p->nz = z;
			//p->s = (lng + M_PI) / (2.*M_PI);
			//p->t = (lat + M_PI / 2.) / M_PI;
			float ss, tt;
			ss = (float)rand() / (float)(RAND_MAX / 3);
			if (rand() % 2 == 0) {
				tt = 1;
			}
			else {
				tt = -1;
			}
			p->s = ss;
			p->t = tt;
		}
		rev = rev * -1;
	}

	top.x = 0.;		top.y = radius;	top.z = 0.;
	top.nx = 0.;		top.ny = 1.;		top.nz = 0.;
	top.s = 0.;		top.t = 1.;

	bot.x = 0.;		bot.y = -radius;	bot.z = 0.;
	bot.nx = 0.;		bot.ny = -1.;		bot.nz = 0.;
	bot.s = 0.;		bot.t = 0.;


	// connect the north pole to the latitude NumLats-2:

	glBegin(GL_QUADS);
	for (int ilng = 0; ilng < NumLngs - 1; ilng++)
	{
		p = PtsPointer(NumLats - 1, ilng);
		DrawPoint(p);

		p = PtsPointer(NumLats - 2, ilng);
		DrawPoint(p);

		p = PtsPointer(NumLats - 2, ilng + 1);
		DrawPoint(p);

		p = PtsPointer(NumLats - 1, ilng + 1);
		DrawPoint(p);
	}
	glEnd();

	// connect the south pole to the latitude 1:

	//glBindTexture(GL_TEXTURE_2D, tex0);

	glBegin(GL_QUADS);
	for (int ilng = 0; ilng < NumLngs - 1; ilng++)
	{
		p = PtsPointer(0, ilng);
		DrawPoint(p);

		p = PtsPointer(0, ilng + 1);
		DrawPoint(p);

		p = PtsPointer(1, ilng + 1);
		DrawPoint(p);

		p = PtsPointer(1, ilng);
		DrawPoint(p);
	}
	glEnd();


	// connect the other 4-sided polygons:
	//glBindTexture(GL_TEXTURE_2D, tex0);

	glBegin(GL_QUADS);
	for (int ilat = 2; ilat < NumLats - 1; ilat++)
	{
		for (int ilng = 0; ilng < NumLngs - 1; ilng++)
		{
			p = PtsPointer(ilat - 1, ilng);
			DrawPoint(p);

			p = PtsPointer(ilat - 1, ilng + 1);
			DrawPoint(p);

			p = PtsPointer(ilat, ilng + 1);
			DrawPoint(p);

			p = PtsPointer(ilat, ilng);
			DrawPoint(p);
		}
	}
	glEnd();

	delete[] Pts;
	Pts = NULL;

}


int	ReadInt(FILE *);
short	ReadShort(FILE *);


struct bmfh
{
	short bfType;
	int bfSize;
	short bfReserved1;
	short bfReserved2;
	int bfOffBits;
} FileHeader;

struct bmih
{
	int biSize;
	int biWidth;
	int biHeight;
	short biPlanes;
	short biBitCount;
	int biCompression;
	int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	int biClrUsed;
	int biClrImportant;
} InfoHeader;

const int birgb = { 0 };



/**
 ** read a BMP file into a Texture:
 **/

unsigned char *
BmpToTexture(char *filename, int *width, int *height)
{

	int s, t, e;		// counters
	int numextra;		// # extra bytes each line in the file is padded with
	FILE *fp;
	unsigned char *texture;
	int nums, numt;
	unsigned char *tp;


	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open Bmp file '%s'\n", filename);
		return NULL;
	}

	FileHeader.bfType = ReadShort(fp);


	// if bfType is not 0x4d42, the file is not a bmp:

	if (FileHeader.bfType != 0x4d42)
	{
		fprintf(stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType);
		fclose(fp);
		return NULL;
	}


	FileHeader.bfSize = ReadInt(fp);
	FileHeader.bfReserved1 = ReadShort(fp);
	FileHeader.bfReserved2 = ReadShort(fp);
	FileHeader.bfOffBits = ReadInt(fp);


	InfoHeader.biSize = ReadInt(fp);
	InfoHeader.biWidth = ReadInt(fp);
	InfoHeader.biHeight = ReadInt(fp);

	nums = InfoHeader.biWidth;
	numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort(fp);
	InfoHeader.biBitCount = ReadShort(fp);
	InfoHeader.biCompression = ReadInt(fp);
	InfoHeader.biSizeImage = ReadInt(fp);
	InfoHeader.biXPelsPerMeter = ReadInt(fp);
	InfoHeader.biYPelsPerMeter = ReadInt(fp);
	InfoHeader.biClrUsed = ReadInt(fp);
	InfoHeader.biClrImportant = ReadInt(fp);


	// fprintf( stderr, "Image size found: %d x %d\n", ImageWidth, ImageHeight );


	texture = new unsigned char[3 * nums * numt];
	if (texture == NULL)
	{
		fprintf(stderr, "Cannot allocate the texture array!\b");
		return NULL;
	}


	// extra padding bytes:

	numextra = 4 * (((3 * InfoHeader.biWidth) + 3) / 4) - 3 * InfoHeader.biWidth;


	// we do not support compression:

	if (InfoHeader.biCompression != birgb)
	{
		fprintf(stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression);
		fclose(fp);
		return NULL;
	}



	rewind(fp);
	fseek(fp, 14 + 40, SEEK_SET);

	if (InfoHeader.biBitCount == 24)
	{
		for (t = 0, tp = texture; t < numt; t++)
		{
			for (s = 0; s < nums; s++, tp += 3)
			{
				*(tp + 2) = fgetc(fp);		// b
				*(tp + 1) = fgetc(fp);		// g
				*(tp + 0) = fgetc(fp);		// r
			}

			for (e = 0; e < numextra; e++)
			{
				fgetc(fp);
			}
		}
	}

	fclose(fp);

	*width = nums;
	*height = numt;
	return texture;
}



int
ReadInt(FILE *fp)
{
	unsigned char b3, b2, b1, b0;
	b0 = fgetc(fp);
	b1 = fgetc(fp);
	b2 = fgetc(fp);
	b3 = fgetc(fp);
	return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}


short
ReadShort(FILE *fp)
{
	unsigned char b1, b0;
	b0 = fgetc(fp);
	b1 = fgetc(fp);
	return (b1 << 8) | b0;
}


