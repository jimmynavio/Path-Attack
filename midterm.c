#define USE_FONTS
#define MAXSIZE 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <GL/glfw.h>
#include "defs.h"

#ifdef USE_FONTS
#include "fonts.h"
#endif

//INIT AND DISPLAY FUNCTIONS
int init_glfw(void);
void init_opengl(void);
void init_board(void);
void init_squares(void);
void render(void);
void check_mouse(void);
void GLFWCALL mouse_click(int, int);
void GLFWCALL key_press(int, int);
void shut_down(int);
GLuint loadBMP(const char *imagepath);

//GAMEPLAY FUNCTIONS
void resetChecked(void);
void checkBoard(void);
int checkV(int, int);
int checkH(int, int);
int outOfBounds(int position);
int randomNum(int max);
void ai_play(void);
void compTurn(char player);
void humanTurn(int x, int y, char player);

//STRUCTURES
typedef struct t_board{
  char winner;
  int movesLeft;
  int xMid;
  int yMid;
  int bottom;
  int top;
  int left;
  int right;
} Board;

typedef struct t_square{
  char status;
  int checked;
  int winPath;
  int over;
  
  int xMid;
  int yMid;
  int bottom;
  int top;
  int left;
  int right;
} Square;

//GLOBAL VARIABLES
int xres=640;
int yres=480;
int boardArea, boardHalf, squareArea, squareHalf;
int gameSize, AImode=-1;
Board board;
Square grid[MAXSIZE][MAXSIZE];
GLuint Htexture;
GLuint Vtexture;

int main(int argc, char *argv[])
{
  if(argc < 2){
    gameSize = 10;
  }else{
    gameSize = atoi(argv[1]);
  }
  
  if(gameSize > MAXSIZE){
    printf("Game size must not exceed %d\n", MAXSIZE);
    exit(EXIT_FAILURE);
  }

  if (init_glfw()) {
		exit(EXIT_FAILURE);
	}
	init_opengl();
	init_board();
	
  #ifdef USE_FONTS
	initialize_fonts();
	#endif
  
  srand(time(NULL));
	while(1){
    if(AImode == 1 && board.winner == 0){
      compTurn('V');
      compTurn('H');
    }
    check_mouse();
		render();
		glfwSwapBuffers();
		if (!glfwGetWindowParam(GLFW_OPENED)) break;
	}
	shut_down(0);
}

void shut_down(int return_code){
  glfwTerminate();
  
  #ifdef USE_FONTS
	cleanup_fonts();
	#endif
  
  exit(return_code);
}

int init_glfw(void){
	int nmodes;
	GLFWvidmode glist[256];
	if (!glfwInit()){
		printf("Failed to initialize GLFW\n");
		return 1;
	}
  
	//get the monitor native full-screen resolution
	nmodes = glfwGetVideoModes(glist, 250);
	xres = glist[nmodes-1].Width;
	yres = glist[nmodes-1].Height;
  
	//create a window
	//if (!glfwOpenWindow(xres, yres, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
	if (!glfwOpenWindow(xres,yres,8,8,8,0,32,0,GLFW_FULLSCREEN)) {
		glfwTerminate();
		return 1;
	}
	glfwSetWindowTitle("Game Board and Grid");
	glfwSetWindowPos(0, 0);
	
  //make sure we see the escape key pressed
	glfwEnable(GLFW_STICKY_KEYS);
	glfwSetMouseButtonCallback(mouse_click);
  glfwSetKeyCallback(key_press);
	glfwEnable(GLFW_MOUSE_CURSOR);
	
  //enable vertical sync (on cards that support it)
	glfwSwapInterval(1);
	return 0;
}

void init_opengl(void){
	//OpenGL initialization
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_COLOR_MATERIAL);
	
	//choose one of these
	//glShadeModel(GL_FLAT);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glEnable(GL_TEXTURE_2D);
	Htexture = loadBMP("resources/H.bmp");
	Vtexture = loadBMP("resources/V.bmp");
	printf("tex: %i %i\n",Htexture,Vtexture);
}

void init_board(void){
  board.winner = 0;
  board.movesLeft = gameSize*gameSize;
  
  boardArea = yres-240;
  while(boardArea%gameSize != 0){
    boardArea++;
  }
  boardHalf = boardArea / 2;
  squareArea = boardArea / gameSize;
  squareHalf = squareArea / 2;
  
  board.xMid = xres/2 - 120;
  board.yMid = yres/2 - 48;
  
  board.bottom = board.yMid - boardHalf;
  board.top = board.yMid + boardHalf;
  board.left = board.xMid - boardHalf;
  board.right = board.xMid + boardHalf;
  
  init_squares();
}

void init_squares(void){
  int i, j;
  for (i=0; i<gameSize; i++) {
		for (j=0; j<gameSize; j++) {
      
      //initialize all states to 0
      grid[i][j].status = 0;
      grid[i][j].checked = 0;
      grid[i][j].winPath = 0;
      grid[i][j].over = 0;
      
      //find middle of square based on i * j
      int xMidTemp = board.left + squareHalf + (squareArea*i);
      int yMidTemp = board.bottom + squareHalf + (squareArea*j);
      
      //Make square size slightly smaller than grid size
      grid[i][j].xMid = xMidTemp;
      grid[i][j].yMid = yMidTemp;
      grid[i][j].bottom = yMidTemp - squareHalf + 1;
      grid[i][j].top = yMidTemp + squareHalf - 1;
      grid[i][j].left = xMidTemp - squareHalf + 1;
      grid[i][j].right = xMidTemp + squareHalf - 1;
		}
	}
}

void check_mouse(void){	
	if(board.winner != 0 || AImode == 1)
    return;
  
  int x,y;
	int i,j;
  
	glfwGetMousePos(&x, &y);
  //reverse the y position
	y = yres - y;

	//is the mouse over any grid squares?
  for (i=0; i<gameSize; i++){
		for (j=0; j<gameSize; j++){
			grid[i][j].over=0;
		}
	}
  
  // OPTOMIZATION?!
  // i = (x-board.left)/squareArea;
  // j = (y-board.bottom)/squareArea;
  // if (outOfBounds(i) || outOfBounds(j)) return;
  // grid[i][j].over=1;
  
	for (i=0; i<gameSize; i++) {
		for (j=0; j<gameSize; j++) {
     if (x >= grid[i][j].left &&
					x <= grid[i][j].right &&
					y >= grid[i][j].bottom &&
					y <= grid[i][j].top) {
				grid[i][j].over=1;
				break;
			}
		}
		if (grid[i][j].over) break;
	}
}

void GLFWCALL mouse_click(int button, int action){  
  if(board.winner != 0 || AImode == 1)
    return;
  
  int x,y;
  if(action == GLFW_PRESS){
		glfwGetMousePos(&x, &y);
		//reverse the y position
		y = yres - y;
    
    int i,j,kill=0;		
    for (i=0; i<gameSize; i++){
			for(j=0; j<gameSize; j++){
        if(x >= grid[i][j].left &&
           x <= grid[i][j].right &&
           y >= grid[i][j].bottom &&
           y <= grid[i][j].top){
					if(button == GLFW_MOUSE_BUTTON_LEFT) humanTurn(i, j, 'V');
					if(button == GLFW_MOUSE_BUTTON_RIGHT) humanTurn(i, j, 'H');
					kill = 1;
					break;
				}
			}
			if (kill) break;
		}
	}
}

void GLFWCALL key_press(int key, int action){
  if(action == GLFW_PRESS){
    switch(key){
      case 'Q':
      case GLFW_KEY_ESC:
        shut_down(0);
        break;
      case 'C':
        AImode *= -1;
        break;
      case 'N':
        init_board();
        break;
      case '1':
      case GLFW_KEY_KP_1:
        gameSize = 10;
        init_board();
        break;
      case '2':
      case GLFW_KEY_KP_2:
        gameSize = 20;
        init_board();
        break;
      case '3':
      case GLFW_KEY_KP_3:
        gameSize = 30;
        init_board();
        break;
      case '4':
      case GLFW_KEY_KP_4:
        gameSize = 40;
        init_board();
        break;
      case '5':
      case GLFW_KEY_KP_5:
        gameSize = 50;
        init_board();
        break;
      case '6':
      case GLFW_KEY_KP_6:
        gameSize = 60;
        init_board();
        break;
      case '7':
      case GLFW_KEY_KP_7:
        gameSize = 70;
        init_board();
        break;
      case '8':
      case GLFW_KEY_KP_8:
        gameSize = 80;
        init_board();
        break;
      case '9':
      case GLFW_KEY_KP_9:
        gameSize = 90;
        init_board();
        break;
      case '0':
      case GLFW_KEY_KP_0:
        gameSize = 100;
        init_board();
        break;
    }
  }
}

void render(void){
  int linePos;
  
  //set the viewing area on screen
  glViewport(0, 0, xres, yres);
	//clear color buffer
	glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//init matrices
	glMatrixMode (GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
  //this sets to 2D mode (no perspective)
	glOrtho(0, xres, 0, yres, -1, 1);
  
  //draw larger square
  glColor3f(0.8f, 0.6f, 0.2f);
  glBegin(GL_QUADS);
		glVertex2i(board.left-5, board.bottom-5);
		glVertex2i(board.right+5, board.bottom-5);
    glVertex2i(board.right+5, board.top+5);
    glVertex2i(board.left-5, board.top+5);
	glEnd();
  
  //draw slightly smaller square for gridlines
  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_QUADS);
		glVertex2i(board.left, board.bottom);
		glVertex2i(board.right, board.bottom);
    glVertex2i(board.right, board.top);
    glVertex2i(board.left, board.top);
	glEnd();
  
  //draw the grid
  glLineWidth(1);
  int i, j;
  for (i=0; i<gameSize; i++) {
		for (j=0; j<gameSize; j++) {
      glColor3f(0.5f, 0.1f, 0.1f);
      if (grid[i][j].over){
				glColor3f(1.0f, 1.0f, 0.0f);
			}
      
      //check if square is used & set color & texture
      glBindTexture(GL_TEXTURE_2D, 0);
      if(grid[i][j].status == 'V'){
				glColor3f(0.1, 0.8f, 0.1f);
        glBindTexture(GL_TEXTURE_2D, Vtexture);
			}else if(grid[i][j].status == 'H'){
				glColor3f(0.1f, 0.1f, 0.8f);
        glBindTexture(GL_TEXTURE_2D, Htexture);
			}
			
      if(grid[i][j].winPath == 1){
        glColor3f(1.0f, 1.0f, 1.0f);
      }
      
      //draw square
      glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex2i(grid[i][j].left, grid[i][j].bottom);
				glTexCoord2f(1.0f, 0.0f); glVertex2i(grid[i][j].right, grid[i][j].bottom);
				glTexCoord2f(1.0f, 1.0f); glVertex2i(grid[i][j].right, grid[i][j].top);
				glTexCoord2f(0.0f, 1.0f); glVertex2i(grid[i][j].left, grid[i][j].top);
			glEnd();
      glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
  
  Rect rTop;
  rTop.left = board.left;
  rTop.bot  = board.top + 90;
  rTop.center = 0;
  ggprint16(&rTop, 24, 0x00ffffff, "Gameplay:");
  ggprint16(&rTop, 24, 0x00bbaa33, "V's objective: Find a path from top to bottom.");
  ggprint16(&rTop, 24, 0x00bbaa33, "H's objective: Find a path from left to right.");
  
  Rect rLeft;
  rLeft.left = board.right + 50;
  rLeft.bot  = board.top - 50;
  rLeft.center = 0;
  ggprint16(&rLeft, 24, 0x00ffffff, "Winner: %c", board.winner);
  ggprint16(&rLeft, 24, 0x00bbaa33, "Press C for AI mode: %s", AImode==1? "ON": "OFF");
  ggprint16(&rLeft, 24, 0x00ccaa33, "Press N for new game");
  ggprint16(&rLeft, 24, 0x00ddaa33, "Press Q to quit");
  
  Rect rRight;
  rRight.left = board.right + 50;
  rRight.bot  = board.top - 200;
  rRight.center = 0;
  ggprint16(&rRight, 24, 0x00ffffff, "Game Size: %d", gameSize);
  ggprint16(&rRight, 24, 0x00ffffff, "Possible moves: %d", board.movesLeft);
  ggprint16(&rRight, 24, 0x00bbaa33, "Press 0-9 to change size");
}

GLuint loadBMP(const char *imagepath){
	//When you create your texture files, please specify
	//type: BMP
	//color depth: 24-bit
	unsigned int retval;
	unsigned char header[54];
	//Each BMP file begins by a 54-bytes header
	//Position in the file where the actual data begins
	unsigned int dataPos;
	unsigned int width, height;
	unsigned int imageSize;
	// = width*height*3
	//RGB data will go in this
	unsigned char *data; 
	//
	printf("loadBMP(%s)...\n",imagepath);
	//Log("opening file **%s**\n",imagepath);
	FILE * file = fopen(imagepath,"r");
	if (!file) {
		printf("Image could not be opened\n");
		return 0;
	} 
	if (fread(header, 1, 54, file)!=54) {
		// If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0]!='B' || header[1]!='M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	dataPos   = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width     = *(int*)&(header[0x12]);
	height    = *(int*)&(header[0x16]); 
	//Some BMP files are misformatted, guess missing information
	if (imageSize==0) imageSize=width*height*3;
	if (dataPos==0) dataPos=54; 
	data = (unsigned char *)malloc(imageSize+1);
	//Read the actual data from the file into the buffer
	retval = fread(data,1,imageSize,file);
	fclose(file); 
	//In glTexImage2D, the GL_RGB indicates that we are talking
	//about a 3-component color, and GL_BGR says how exactly
	//it is represented in RAM. As a matter of fact, BMP does
	//not store Red->Green->Blue but Blue->Green->Red, so we
	//have to tell it to OpenGL.
	#define GL_BGR 0x80E0
	//Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	free(data);
	return textureID;
}

void resetChecked(void){
  int i, j;
  for(i=0; i<gameSize; i++){
    for(j=0; j<gameSize; j++){
      grid[i][j].checked = 0;
    }
  }  
}

void checkBoard(void){
  int i;
  if(board.winner != 0)
    return;
  
  for(i=0; i<gameSize; i++){
    resetChecked();
    if(checkV(i,0) > -1){
      board.winner = 'V';
    }    
    
    if(checkH(0,i) > -1){
      board.winner = 'H';
    }
  }
}

int checkV(int i, int j){
  if (outOfBounds(i) || outOfBounds(j)) return -1;
  if (grid[i][j].status != 'V') return -1;
  if (grid[i][j].checked == 1) return -1;
  
  grid[i][j].winPath = 1;
  if (j == gameSize-1) return 1;
  
  grid[i][j].checked = 1;
  if (checkV(i-1, j-1) == 1) return 1;
  if (checkV(i  , j-1) == 1) return 1;
  if (checkV(i+1, j-1) == 1) return 1;
  if (checkV(i+1, j  ) == 1) return 1;
  if (checkV(i+1, j+1) == 1) return 1;
  if (checkV(i  , j+1) == 1) return 1;
  if (checkV(i-1, j+1) == 1) return 1;
  if (checkV(i-1, j  ) == 1) return 1;
  
  grid[i][j].winPath = 0;
  return -1;
}

int checkH(int i, int j){
  if (outOfBounds(i) || outOfBounds(j)) return -1;
  if (grid[i][j].status != 'H') return -1;
  if (grid[i][j].checked == 1) return -1;
  
  grid[i][j].winPath = 1;
  if (i == gameSize-1) return 1;
  
  grid[i][j].checked = 1;
  if (checkH(i-1, j-1) == 1) return 1;
  if (checkH(i  , j-1) == 1) return 1;
  if (checkH(i+1, j-1) == 1) return 1;
  if (checkH(i+1, j  ) == 1) return 1;
  if (checkH(i+1, j+1) == 1) return 1;
  if (checkH(i  , j+1) == 1) return 1;
  if (checkH(i-1, j+1) == 1) return 1;
  if (checkH(i-1, j  ) == 1) return 1;
  
  grid[i][j].winPath = 0;
  return -1;
}

int outOfBounds(int position){
  return position >= gameSize || position < 0;
}

int randomNum(int max){
  return (rand() % (max));
}

void compTurn(char player){
  int i = randomNum(gameSize);
  int j = randomNum(gameSize);
  
  while(grid[i][j].status != 0){
    i = randomNum(gameSize);
    j = randomNum(gameSize);
  }
  
  grid[i][j].status = player;
  board.movesLeft -= 1;
  checkBoard();
}

void humanTurn(int i, int j, char player){
  if(grid[i][j].status != 0){
    return;
  }
  
  grid[i][j].status = player;
  board.movesLeft -= 1;
  checkBoard();
}







