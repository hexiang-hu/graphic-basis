# pragma once
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>
#include "unistd.h"
#include "include/Angel.h"
#include <vector>
#include <iomanip>
using namespace std;

// ============================================================================================
// macros for tile types


// ----------------------------------------------------------------------------------------------
// Option for compiling 2D version game or 3D version game
#define _3DGAME
//#define DEBUG
// ----------------------------------------------------------------------------------------------


#define TILE_TYPE_NUM 		3
#define TILE_COLOR_NUM		5

#define VBO_GRID_POSITION		0
#define VBO_GRID_COLOR			1
#define VBO_BOARD_POSITION		2
#define VBO_BOARD_COLOR			3
#define VBO_TILE_POSITION		4
#define VBO_TILE_COLOR 			5

// #define VBO_DROPTILE_COLOR 		6
// #define VBO_DROPTILE_POSITION 	7



#define VAO_GRID			0
#define VAO_BOARD			1
#define VAO_TILE			2
// #define VAO_DROPTILE		3

// #define _IN_BOUND(x, y)	 (y <= UP_BOUND && y >= DOWN_BOUND && x >= LEFT_BOUND && x <= RIGHT_BOUND)
#define _IN_BOUND(x, y)	 (y >= DOWN_BOUND && x >= LEFT_BOUND && x <= RIGHT_BOUND)

#define _ON_BOARD(x, y)  (y <= UP_BOUND && y >= DOWN_BOUND && x >= LEFT_BOUND && x <= RIGHT_BOUND)

#define _COLOR4_EQUAL(a,b) (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w) 

#define _MATCH_COLOR(color) ( _COLOR4_EQUAL(color, black) 	? "black" 	: \
							( _COLOR4_EQUAL(color, orange)	? "orange" 	: \
							( _COLOR4_EQUAL(color, red)		? "red" 	: \
							( _COLOR4_EQUAL(color, green)	? "green" 	: \
							( _COLOR4_EQUAL(color, purple)	? "purple"	: \
							( _COLOR4_EQUAL(color, yellow)	? "yellow"	: \
							( _COLOR4_EQUAL(color, white)	? "white"	: "Unknown color" \
								)))))))


typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

//  constant variable
// ============================================================================================

const int TILE_TYPE_L 	= 0;
const int TILE_TYPE_S 	= 1;
const int TILE_TYPE_I 	= 2;

// const for board boundaries 
const int LEFT_BOUND 	= 0;
const int RIGHT_BOUND 	= 9;
const int DOWN_BOUND 	= 0;
const int UP_BOUND  	= 19;


// const for board size
const int BOARD_WIDTH 	= 10;
const int BOARD_HEIGHT	= 20;

// colors
const color4 white  		= color4(1.0, 1.0, 1.0, 1.0);
const color4 black  		= color4(0.0, 0.0, 0.0, 1.0);

// extra colors for fruits 
const color4 orange 		= color4(1.0, 	0.549,	0.0,  	1.0); 
const color4 red 			= color4(1.0, 	0.0, 	0.0, 	1.0);
const color4 green 			= color4(0.486, 0.988, 	0.0, 	1.0);
const color4 purple			= color4(1.0, 	0.0, 	1.0, 	1.0);
const color4 yellow 		= color4(1.0, 	0.843, 	0.0, 	1.0);

const color4 tileColorsSet[5] = {
	orange, red, green, purple, yellow
};

// xsize and ysize represent the window size - updated if window is reshaped to prevent stretching of the game
int xsize = 600; 
int ysize = 800;

class Tile{
public:
	Tile(vec2 _pos, color4 _color) : Position(_pos), Color(_color){ }
	Tile(const Tile & _tile ) { this->Position = _tile.Position; this->Color = _tile.Color; }

	bool operator== (const Tile &other) const {
		return _COLOR4_EQUAL(this->Color, other.Color) && 
				( this->Position.x == other.Position.x && this->Position.y == other.Position.y );
  	}

	bool operator!=(const Tile &other) const {
		return !(*this == other);
	}

	vec2 Position;
	color4 Color;
};

vector<Tile> tiles;
vector< vector<Tile> > dropTiles;

// An array of 4 2d vectors representing displacement from a 'center' piece of the tile, on the grid
vec2 tilePos = vec2(5, 19);
// The position of the current tile using grid coordinates ((0,0) is the bottom left corner)
int tileType 		= TILE_TYPE_L;
int rotateType 		= 0;


GLuint  model_view;  // model-view matrix uniform shader variable location

#ifdef _3DGAME
// in 3d game, there will be 12 triangles per cube
const int TRIANGLE_VERTEX_NUM 	= 12;
const int TILE_VERTEX_NUM 		= 36;
const int QUAD_VERTEX_NUM 		= 6;
const int QUAD_LINE_VERTEX_NUM  = 8;
const int TILE_LINE_VERTEX_NUM  = 48;
const int GRID_LINE_VERTEX_NUM	= (BOARD_WIDTH + 1)*(BOARD_HEIGHT + 1)*TILE_LINE_VERTEX_NUM;
const GLfloat START_POINT_X = -200;
const GLfloat START_POINT_Y = -360;
const GLfloat START_POINT_Z = -33.0;

GLfloat eyex = 0.95, eyey 	= 0.95, eyez = 0.25;
GLfloat atx  = 1.0, aty 	= 1.0, atz  = 0;
#else
// in 2d game, there will be 2 triangles
const int TRIANGLE_VERTEX_NUM 	= 2;
const int TILE_VERTEX_NUM 		= 6;
const int GRID_LINE_VERTEX_NUM	= 64;
const GLfloat START_POINT_X 	= -200;
const GLfloat START_POINT_Y 	= -360;
const GLfloat START_POINT_Z 	= -33.0;

GLfloat eyez = 0.5;

#endif

//board[x][y] represents whether the cell (x,y) is occupied
bool board[BOARD_WIDTH][BOARD_HEIGHT];
color4 boardColors[BOARD_WIDTH][BOARD_HEIGHT];
color4 boardVertexColors[BOARD_WIDTH * BOARD_HEIGHT * TILE_VERTEX_NUM];

// location of vertex attributes in the shader program
GLuint vPosition;
GLuint vColor;

// locations of uniform variables in shader program
GLuint locxsize;
GLuint locysize;

// VAO and VBO
GLuint vaoIDs[3]; 
// One VAO for each object: 0. the grid 1. the board 2. the current piece

GLuint vboIDs[6]; 
// Two Vertex Buffer Objects for each VAO (specifying vertex positions and colours, respectively)

GLfloat velocity 	= -1.0;
GLfloat step 		=  1.0;


// Game control signal
bool ifPause 		= false;
bool ifGameStop 	= false;

// Glut time control float
GLfloat deltaTime 	= 0.0f;		// Time between current frame and last frame
GLfloat lastFrame 	= 0.0f;  	// Time of last frame

// Velocity for each timer movement and step for keyboard movement

// The 'tile' array will always be some element [i][j] of this array (an array of vec2)
int allRotationShapeSize[TILE_TYPE_NUM] = {4, 4, 4};

vec2 allRotationsLshape[4][4] = 
	{
		{vec2(-1,-1), vec2(-1, 0), vec2( 0, 0), vec2( 1,  0)},
		{vec2( 1,-1), vec2( 0,-1), vec2( 0, 0), vec2( 0,  1)},
		{vec2( 1, 1), vec2( 1, 0), vec2( 0, 0), vec2(-1,  0)},
		{vec2(-1, 1), vec2( 0, 1), vec2( 0, 0), vec2( 0, -1)}
	};

// All rotations for S and L shapes
// ============================================================================================
vec2 allRotationsSshape[4][4] = 
	{
		{vec2(-1, -1), vec2( 0,-1), vec2(0, 0), vec2( 1, 0)},
		{vec2( 1, -1), vec2( 1, 0), vec2(0, 0), vec2( 0, 1)},
		{vec2( 1,  1), vec2( 0, 1), vec2(0, 0), vec2(-1, 0)},
		{vec2(-1,  1), vec2(-1, 0), vec2(0, 0), vec2( 0,-1)}
	};

vec2 allRotationsIshape[4][4] = 
	{
		{vec2(-2, 0), vec2(-1, 0), vec2(0, 0), vec2( 1, 0)},
		{vec2( 0,-2), vec2( 0,-1), vec2(0, 0), vec2( 0, 1)},
		{vec2( 2, 0), vec2( 1, 0), vec2(0, 0), vec2(-1, 0)},
		{vec2( 0, 2), vec2( 0, 1), vec2(0, 0), vec2( 0,-1)}

	};

// Get Tile Bound 
//-------------------------------------------------------------------------------------------------------------------
struct TileBound{
	GLfloat left, right, up, down;
	TileBound(int _left, int _right, int _up, int _down):
		left(_left), right(_right), up(_up), down(_down){}
};

// ===========================================================================================
// Utility function 
void genBoardVertexColorFromBoardColor(int x ,int y, color4 _color);
void genBoardVertexColorsFromBoardColors();

void genColorVertexFromTileColor(color4 * pPointColor, color4 _color);

color4 genRandomColor();
const TileBound getTileBound( vec2 * pTile);

bool checkInBound(vec2 newPos);
bool checkTileGridCollision( int x, int y);
bool checkTilesGridsCollision(vec2 newPos);
bool testRotation(vec2 currentTilePos);

// ===========================================================================================
// Function Declaration
void updateBoard();
void updateTiles();
void newTile();
bool rotateTile();
void shiftTileColor();
void unsetTiles();

void moveDownRow(int startRow);
void moveDownRows(bool eliminatedRows[]);
void eliminateFullRow(int row);
bool checkFullRow(int row);
void setTile();
bool moveTile(vec2 direction);
void moveDownTileToEnd();

void quad( point4 * pPoints, point4 p1, point4 p2, point4 p3, point4 p4);
void initGrid();
void initBoard();
void initCurrentTile();
void init();

void processDisplay();
void processReshape(GLsizei width, GLsizei height);
void processSpecialKey(int key, int x, int y);
void processKeyboard(unsigned char key, int x, int y);
void processTimer(int val);
void processIdle();

void newGame();
void pauseResumeGame();
bool tryStopGame();
void restartGame();
bool checkEndOfGame();

bool matchFruitTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT]);
void printBoolBoardSizeArray(bool eliminatedFruitTiles[][BOARD_HEIGHT]);
void cleanUpEliminationTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT]);
void moveDownFruitTilesCols(bool eliminatedFruitTiles[][BOARD_HEIGHT]);
bool eliminateFruitTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT]);

void addUnsupportedTilesToDropTiles();
bool checkFruitMatchAndEliminate();
void checkFullRowsAndEliminate();
bool fallTiles();
void updateDropTiles();
void setDropTiles(vector<Tile> &_tile);
bool searchConnectToBottom(vec2 vertex);
void addTileToDropTiles(Tile _newDropTile);
void addTilesToDropTiles(vector<Tile> _newDropTiles);
