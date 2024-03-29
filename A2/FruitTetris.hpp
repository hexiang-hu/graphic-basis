# pragma once
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>
#include "unistd.h"
#include "include/Angel.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
using namespace std;

//----------------------------------------------------------------------------------------------------------
// macros for tile types


//----------------------------------------------------------------------------------------------------------
// Option for compiling 2D version game or 3D version game
// #define DEBUG

//----------------------------------------------------------------------------------------------------------
// Macro for Tile Attribute

#define TILE_TYPE_NUM 			3
#define TILE_COLOR_NUM			5

//----------------------------------------------------------------------------------------------------------
// Macro for Vertex Buffer Object

#define VBO_GRID_POSITION		0
#define VBO_GRID_COLOR			1
#define VBO_BOARD_POSITION		2
#define VBO_BOARD_COLOR			3
#define VBO_TILE_POSITION		4
#define VBO_TILE_COLOR 			5
#define VBO_ROBOTARM_POSITION 	6
#define VBO_ROBOTARM_COLOR 		7

// ----------------------------------------------------------------------------------------------
// Macro for Vertex Array Object

#define VAO_GRID				0
#define VAO_BOARD				1
#define VAO_TILE				2
#define VAO_ROBOTARM			3


// ----------------------------------------------------------------------------------------------
// 

#define _IN_BOUND(x, y)	 (y >= DOWN_BOUND && x >= LEFT_BOUND && x <= RIGHT_BOUND)
#define _ON_BOARD(x, y)  (y <= UP_BOUND && y >= DOWN_BOUND && x >= LEFT_BOUND && x <= RIGHT_BOUND)

#define _COLOR4_EQUAL(a,b) (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w) 

#define _MATCH_COLOR(color) ( _COLOR4_EQUAL(color, black) 	? "black" 	: \
							( _COLOR4_EQUAL(color, orange)	? "orange" 	: \
							( _COLOR4_EQUAL(color, red)		? "red" 	: \
							( _COLOR4_EQUAL(color, green)	? "green" 	: \
							( _COLOR4_EQUAL(color, purple)	? "purple"	: \
							( _COLOR4_EQUAL(color, yellow)	? "yellow"	: \
							( _COLOR4_EQUAL(color, white)	? "white"	: \
							( _COLOR4_EQUAL(color, grey)   	? "grey"    : \
							( _COLOR4_EQUAL(color, magenta) ? "magenta" : \
							( _COLOR4_EQUAL(color, cyan) 	? "cyan" 	: "Unknown color" \
								))))))))))


typedef glm::vec4  color4;
typedef glm::vec4  point4;

//----------------------------------------------------------------------------------------------------------
// global constant variable

// const for tile type
const int TILE_TYPE_L 		= 0;
const int TILE_TYPE_S 		= 1;
const int TILE_TYPE_I 		= 2;

// const for board boundaries 
const int LEFT_BOUND 		= 0;
const int RIGHT_BOUND 		= 9;
const int DOWN_BOUND 		= 0;
const int UP_BOUND  		= 19;


// const for board size
const int BOARD_WIDTH 		= 10;
const int BOARD_HEIGHT		= 20;

// basic colors
const color4 white  		= color4(1.0, 	  1.0, 	 1.0, 	1.0);
const color4 black  		= color4(0.0, 	  0.0, 	 0.0, 	1.0);
const color4 grey			= color4(0.827, 0.827, 0.827, 	1.0);

// extra colors for fruits 
const color4 orange 		= color4(1.0, 	0.549,	0.0,  	1.0); 
const color4 red 			= color4(1.0, 	0.0, 	0.0, 	1.0);
const color4 green 			= color4(0.180, 0.545, 	0.341, 	1.0);
const color4 purple			= color4(0.502, 0.0, 	0.502, 	1.0);
const color4 yellow 		= color4(1.0, 	0.843, 	0.0, 	1.0);

// Extra colors constants
const color4 blue 			= color4(0.0,	0.0,	1.0,	1.0);
const color4 cyan			= color4(0.0,	1.0,	1.0,	1.0);
const color4 magenta		= color4(1.0,	0.0,	1.0,	1.0);
const color4 paleTurquoise	= color4(0.686,	0.933,	0.933,	1.0); 

// Tile color constants array 
const color4 tileColorsSet[5] = {
	orange, red, green, magenta, yellow
};

//----------------------------------------------------------------------------------------------------------
// OpenGL drawing related structure

// xsize and ysize represent the window size
int xsize = 720; 
int ysize = 800;

// Local ID for Shader Matrix


// in 3D game, there will be 12 triangles per cube
const int TRIANGLE_VERTEX_NUM 		= 12;
const int TILE_VERTEX_NUM 			= 36;
const int QUAD_VERTEX_NUM 			= 6;
const int QUAD_LINE_VERTEX_NUM  	= 8;
const int TILE_LINE_VERTEX_NUM  	= 48;
const int GRID_LINE_VERTEX_NUM		= (BOARD_WIDTH + 1)*(BOARD_HEIGHT + 1)*TILE_LINE_VERTEX_NUM;

// Edge length of each tile
const GLfloat EDGE_LEN 				= 1.0;

// Time bound constant defining the maximum time bound for robot arm to hold tile
const GLfloat RELEASED_TIMEBOUND 	= 5.0f;

// Global structure for different shader program [Originally decided for adding texture feature to robot arm]
GLuint UniversalShader;
GLuint RobotArmShader;

//board[x][y] represents whether the cell (x,y) is occupied
bool board[BOARD_WIDTH][BOARD_HEIGHT];
color4 boardColors[BOARD_WIDTH][BOARD_HEIGHT];
color4 boardVertexColors[BOARD_WIDTH * BOARD_HEIGHT * TILE_VERTEX_NUM];

// Vertex Array Object and Vertex Buffer Object

GLuint vaoIDs[4]; 
// One VAO for each object: 0. the grid 1. the board 2. the current tiles 3. robot arm

GLuint vboIDs[8]; 
// Two Vertex Buffer Objects for each VAO (specifying vertex positions and colours, respectively)

//---------------------------------------------------------------------------------------------------------
// Game control global variable

// Controlling tile dropping velocity
GLfloat dropVelocity 		= -1.0;
GLfloat moveStep 			=  1.0;

// Game control signal
bool ifPause 				= false;
bool ifGameStop 			= false;

// Glut time control float
GLfloat deltaTime 			= 0.0f;		// Time between current frame and last frame
GLfloat lastFrame 			= 0.0f;  	// Time of last frame
GLfloat dt 					= 0.0f;

//---------------------------------------------------------------------------------------------------------
// Tile Structure

// Structure for handling each individual tile
class Tile{
public:
	Tile(glm::vec2 _pos, color4 _color) : Position(_pos), Color(_color){ }
	Tile(const Tile & _tile ) { this->Position = _tile.Position; this->Color = _tile.Color; }

	bool operator== (const Tile &other) const {
		return _COLOR4_EQUAL(this->Color, other.Color) && 
				( this->Position.x == other.Position.x && this->Position.y == other.Position.y );
  	}

	bool operator!=(const Tile &other) const {
		return !(*this == other);
	}

	glm::vec2 Position;
	color4 Color;
};


bool tilesReleased = false;
GLfloat releaseTimer = 0;
// Global structure for storing current tiles
vector<Tile> tiles;
// Global structure for storing dropping tiles
vector< vector<Tile> > dropTiles;

glm::vec2 tilePos = glm::vec2(5, 19);
int tileType 		= TILE_TYPE_L;
int rotateType 		= 0;

// Velocity for each timer movement and step for keyboard movement
//----------------------------------------------------------------------------------------------------------
// The 'tile' array will always be some element [i][j] of this array (an array of glm::vec2)

int allRotationShapeSize[TILE_TYPE_NUM] = {4, 4, 4};

// Each vec2 array keeps the relative tile positions to the tile center for different tiles shapes

// All rotations for L shapes
glm::vec2 allRotationsLshape[4][4] = 
	{
		{glm::vec2(-1,-1), glm::vec2(-1, 0), glm::vec2( 0, 0), glm::vec2( 1,  0)},
		{glm::vec2( 1,-1), glm::vec2( 0,-1), glm::vec2( 0, 0), glm::vec2( 0,  1)},
		{glm::vec2( 1, 1), glm::vec2( 1, 0), glm::vec2( 0, 0), glm::vec2(-1,  0)},
		{glm::vec2(-1, 1), glm::vec2( 0, 1), glm::vec2( 0, 0), glm::vec2( 0, -1)}
	};

// All rotations for S shapes
glm::vec2 allRotationsSshape[4][4] = 
	{
		{glm::vec2(-1, -1), glm::vec2( 0,-1), glm::vec2(0, 0), glm::vec2( 1, 0)},
		{glm::vec2( 1, -1), glm::vec2( 1, 0), glm::vec2(0, 0), glm::vec2( 0, 1)},
		{glm::vec2( 1,  1), glm::vec2( 0, 1), glm::vec2(0, 0), glm::vec2(-1, 0)},
		{glm::vec2(-1,  1), glm::vec2(-1, 0), glm::vec2(0, 0), glm::vec2( 0,-1)}
	};

// All rotations for I shapes
glm::vec2 allRotationsIshape[4][4] = 
	{
		{glm::vec2(-2, 0), glm::vec2(-1, 0), glm::vec2(0, 0), glm::vec2( 1, 0)},
		{glm::vec2( 0,-2), glm::vec2( 0,-1), glm::vec2(0, 0), glm::vec2( 0, 1)},
		{glm::vec2( 2, 0), glm::vec2( 1, 0), glm::vec2(0, 0), glm::vec2(-1, 0)},
		{glm::vec2( 0, 2), glm::vec2( 0, 1), glm::vec2(0, 0), glm::vec2( 0,-1)}

	};

// Get Tile Bound 
//----------------------------------------------------------------------------------------------------------
struct TileBound{
	GLfloat left, right, up, down;
	TileBound(int _left, int _right, int _up, int _down):
		left(_left), right(_right), up(_up), down(_down){}
};


//----------------------------------------------------------------------------------------------------------
// To include camera class and robotArm class
#include "Camera.hpp"
#include "RobotArm.hpp"

// Global Camera Class
Camera myCamera(glm::vec3( EDGE_LEN * 12 / 2, EDGE_LEN * 22 / 2 , EDGE_LEN * 40),		// camera position
				glm::vec3( 0.0f, 1.0f, 0.0f),											// camera up direction
				glm::vec3( EDGE_LEN * 12 / 2, EDGE_LEN * 22 / 2, EDGE_LEN * .5f)			// camera looked center		
				);

RobotArm myRobotArm;

//----------------------------------------------------------------------------------------------------------
// To include text rendering class
#include "TextRender.hpp"

//----------------------------------------------------------------------------------------------------------
// Utility function for updating GLbuffer object
void genBoardVertexColorFromBoardColor(int x ,int y, color4 _color);
void genBoardVertexColorsFromBoardColors();
void genColorVertexFromTileColor(color4 * pPointColor, color4 _color);

//----------------------------------------------------------------------------------------------------------
// Utility function for misc sub-functionality
color4 genRandomColor();
const TileBound getTileBound( glm::vec2 * pTile);

//----------------------------------------------------------------------------------------------------------
// Utility function for detecting Collision 
bool checkInBound(glm::vec2 newPos);
bool checkTileGridCollision( int x, int y);
bool checkTilesGridsCollision();
bool testRotation(glm::vec2 currentTilePos);

//----------------------------------------------------------------------------------------------------------
// Function Declaration
void drawBoard();
void drawTiles();
void newTile();
bool rotateTile();
void shiftTileColor();
void unsetTiles();
void releaseTiles();

void moveDownRow(int startRow);
void moveDownRows(bool eliminatedRows[]);
void eliminateFullRow(int row);
bool checkFullRow(int row);
void setTile();
bool moveTile(glm::vec2 direction);
void moveDownTileToEnd();

void quad( point4 * pPoints, point4 p1, point4 p2, point4 p3, point4 p4);
void colorquad( color4 * pColors, color4 c1, color4 c2, color4 c3, color4 c4);
void initGrid();
void initBoard();
void initCurrentTile();
void init();


//----------------------------------------------------------------------------------------------------------
// Glut callback function
void processDisplay();
void processReshape(GLsizei width, GLsizei height);
void processSpecialKey(int key, int x, int y);
void processKeyboard(unsigned char key, int x, int y);
void processTimer(int val);

//----------------------------------------------------------------------------------------------------------
// Game control function
void newGame();
void pauseResumeGame();
bool tryStopGame();
void restartGame();
bool checkEndOfGame();

//----------------------------------------------------------------------------------------------------------
// Fruit match functions
bool matchFruitTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT]);
void printBoolBoardSizeArray(bool eliminatedFruitTiles[][BOARD_HEIGHT]);
void cleanUpEliminationTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT]);
void moveDownFruitTilesCols(bool eliminatedFruitTiles[][BOARD_HEIGHT]);
bool eliminateFruitTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT]);


//----------------------------------------------------------------------------------------------------------
// Game logic control function for support dropping tiles
void addUnsupportedTilesToDropTiles();
bool checkFruitMatchAndEliminate();
void checkFullRowsAndEliminate();
bool fallTiles();

//----------------------------------------------------------------------------------------------------------
// Function for dropping tiles

void drawDropTiles();
void setDropTiles(vector<Tile> &_tile);
bool searchConnectToBottom(glm::vec2 vertex);
void addTileToDropTiles(Tile _newDropTile);
void addTilesToDropTiles(vector<Tile> _newDropTiles);

//----------------------------------------------------------------------------------------------------------
// Functions for controlling Robot Arm 
void initRobotArm();
void drawBase(glm::mat4 model);
void drawUpperArm(glm::mat4 model);
void drawLowerArm(glm::mat4 model);
