#include "unistd.h"
#include "FruitTetris.hpp"
// =================================================================================================================
// Tile Controller Function  


//-------------------------------------------------------------------------------------------------------------------
// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data

void updateBoard()
{
	// Update board colors

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_BOARD_COLOR]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(boardVertexColors), boardVertexColors);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);		
}

void updateTile()
{
	// For each of the 4 'cells' of the tile,
	for (int i = 0; i < 4; i++) 
	{
		// Calculate the grid coordinates of the cell
		GLfloat x = tilePos.x + tile[i].x; 
		GLfloat y = tilePos.y + tile[i].y;


		// Contraints that make the tile outside the UP_BOUND of board invisible
		// ==============================================================================
		vec4 p1 = vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1); 
		vec4 p2 = vec4(33.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);
		vec4 p3 = vec4(66.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
		vec4 p4 = vec4(66.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);

		// if ( y > UP_BOUND ) continue;

		// Two points are used by two triangles each
		vec4 newPoints[6] = {p1, p2, p3, p2, p3, p4}; 

		// Put new data in the VBO
		vec4 pointsColors[6];

		genColorVertexFromTileColor(pointsColors, tileColors[i]);

		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]);
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(vec4), 6*sizeof(vec4), pointsColors);


		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_POSITION]); 
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(vec4), 6*sizeof(vec4), newPoints);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------
// Called at the start of play and every time a tile is placed
void newTile()
{
	// First generate a random tile

	tileType = rand() % TILE_TYPE_NUM;
	vec2 (* pAllRotationShape)[4] = (tileType == TILE_TYPE_L) ?  allRotationsLshape :
			( (tileType == TILE_TYPE_S)? allRotationsSshape:allRotationsIshape );

	rotateType = rand() % ( allRotationShapeSize[tileType] );


#ifdef DEBUG
	cout << "newTile() - [Generate Randomness] TileType: " << tileType << ", RotateType: " << rotateType 
		<< ", RotationShape: " << allRotationShapeSize[tileType] << endl;
#endif

	// Update the geometry VBO of current tile
	for (int i = 0; i < 4; i++){
		tile[i] = pAllRotationShape[rotateType][i];
	}

	TileBound tileBound = getTileBound(tile);
	int coverage = ( RIGHT_BOUND - int(tileBound.right) + int(tileBound.left) + 1);

	int hpos = rand() % coverage - int(tileBound.left);
	int vpos = UP_BOUND - tileBound.up;
	tilePos = vec2(hpos , vpos);
	// Put the tile at the top of the board
	
#ifdef DEBUG
	cout << "newTile() - [Get Tile Boundary] Bound(" << tileBound.left << ", " << tileBound.right
		<<", " << tileBound.down << ", " << tileBound.up << ")" << endl;
	cout << "newTile() - [Boundaries] Right: " << (RIGHT_BOUND - int(tileBound.right)) 
		<< ", Left: " << (- int(tileBound.left)) << ", Coverage: " << coverage << endl;
	cout << "newTile() - [New Tile Position] x = " << hpos << ", y = " << vpos << endl;
#endif

	// You should randomlize the color in structure tileColors
	fillTileWithRandomColor();

	// Call update function to draw the tile with its position as well as its color
	updateTile(); 

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------
// Rotate the current tile 
bool rotateTile()
{
	vec2 (* pAllRotationShape)[4] = (tileType == TILE_TYPE_L) ?  allRotationsLshape :
			( (tileType == TILE_TYPE_S)? allRotationsSshape:allRotationsIshape );

	// First test if the rotated version are in the boundary
	if (!testRotation(tilePos))
	{
		cout << "Test rotation0 - False" << endl << endl;
		// Add logic to rescue a tile if it could be rotate with a unit movement.
		if ( testRotation(tilePos + vec2(1, 0)) )
		{
			cout << "Test rotation1 - True"<< endl;
			tilePos += vec2(1, 0);
		}
		else if ( testRotation(tilePos + vec2(-1, 0)) )
		{
			cout << "Test rotation2 - True" << endl;
			tilePos += vec2(-1, 0);
		}
		else if ( testRotation(tilePos + vec2( 0, 1)) )
		{
			cout << "Test rotation3 - True" << endl;
			tilePos += vec2(0, 1);
		}
		else if ( testRotation(tilePos + vec2( 2, 0)) )
		{
			cout << "Test rotation4 - True" << endl;
			tilePos += vec2(2, 0);			
		}
		else if ( testRotation(tilePos + vec2(-2, 0)))
		{
			cout << "Test rotation5 - True" << endl;
			tilePos += vec2(-2, 0);
		}
		else if ( testRotation(tilePos + vec2( 0, 2)) )
		{
			cout << "Test rotation5 - True" << endl;
			tilePos += vec2(0, 2);
		}
		else{

#ifdef DEBUG
		cout << "rotateTile() [Failure Rotation]"<< endl;
#endif		
			updateTile();
			return false;

		}
	}

	// Testing succeed, do actual rotation

	rotateType = (rotateType + 1) % allRotationShapeSize[tileType];
	// Update the geometry VBO of current tile
	for (int i = 0; i < 4; i++){
		tile[i] = pAllRotationShape[rotateType][i];

#ifdef DEBUG
		cout << "rotateTile() [Change Rotation] - Tile#" << i << " - " << tilePos.x + tile[i].x << "," << tilePos.y + tile[i].y << endl;
#endif
	}


#ifdef DEBUG
	cout << "rotateTile() [Change Rotation] - TileType: " << tileType << ", RotateType: " << rotateType 
		<< ", RotationShape: " << allRotationShapeSize[tileType] << endl;
#endif
	updateTile();
	return true;
}

//-------------------------------------------------------------------------------------------------------------------
// shift Teris color 

void shiftTileColor()
{
	vec4 oldColor = tileColors[3];

	tileColors[3] = tileColors[2];
	tileColors[2] = tileColors[1];
	tileColors[1] = tileColors[0];
	tileColors[0] = oldColor;
	updateTile();
}

//-------------------------------------------------------------------------------------------------------------------
// check the rows inside the range and see if they are full
void moveDownRow(int startRow)
{
	for (int row = startRow; row < BOARD_HEIGHT; ++row)
	{
		bool isBlackRow = false;

		// If the row to move down is the top row, add a black row
		if (row == UP_BOUND)
			isBlackRow = true;

		if (isBlackRow)
		{
			for (int col = 0; col < BOARD_WIDTH; ++col)
			{
				board[col][row] = false;
				boardColors[col][row] = black;
			}				
		}
		else
		{
			for (int col = 0; col < BOARD_WIDTH; ++col)
			{
				board[col][row] = board[col][row + 1];
				boardColors[col][row] = boardColors[col][row + 1];
			}			
		}
	}
}

void moveDownRows(bool eliminatedRows[])
{
	for (int i = UP_BOUND; i >= 0; --i)
	{
		if (eliminatedRows[i])
		{
			moveDownRow(i);
		}
	}
}

void eliminateFullRow(int row)
{
	for (int i = 0; i < BOARD_WIDTH; ++i)
	{
		board[i][row] = false;
		boardColors[i][row] = black;
	}
}

bool checkFullRow(int row)
{
	bool isRowFull = true;
	for (int i = 0; i < BOARD_WIDTH; ++i){
		isRowFull = isRowFull && board[i][row];
	}

#ifdef DEBUG
	cout << "checkFullRow() - [Row Status] : " << endl;
	for (int i = 0; i < BOARD_WIDTH; ++i)
	{
		cout << "[" << (board[i][row]? 'T' : 'F') << "],"; 
	}
	cout << endl;
	if ( isRowFull )
	{
		cout << "checkFullRow() - [A full row detected] : Row[" << row << "]" << endl;
	}
#endif


	return isRowFull;
}


//-------------------------------------------------------------------------------------------------------------------
// match fruits when a tile is set and see if there is a elimination 

// Basically for each falling fruit, the idea behind is to
// match the 3*3 grid and set all matching to true 
bool matchFruitTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT])
{
	// Basically a board-global matching
	bool flag = false;

	for (int y = 0; y < BOARD_HEIGHT; ++y)
	{
		for (int i = LEFT_BOUND; i < RIGHT_BOUND - 1; ++i)
		{
		
			if( _vec4_equal(boardColors[i + 0][y], boardColors[i + 1][y]) && 
				_vec4_equal(boardColors[i + 1][y], boardColors[i + 2][y]) &&
				!_vec4_equal(boardColors[i][y], black) 				  	  )
			{

#ifdef DEBUG 
				cout << "matchFruitTiles() - [row fruit tiles detected]" << endl;
				cout << "matchFruitTiles() - tile["<< i + 0 << "][" << y <<"] = " << _MATCH_COLOR(boardColors[i + 0][y]) << endl;
				cout << "matchFruitTiles() - tile["<< i + 1 << "][" << y <<"] = " << _MATCH_COLOR(boardColors[i + 1][y]) << endl;
				cout << "matchFruitTiles() - tile["<< i + 2 << "][" << y <<"] = " << _MATCH_COLOR(boardColors[i + 2][y]) << endl;
#endif
				eliminatedFruitTiles[(i + 0)][y] = true;
				eliminatedFruitTiles[(i + 1)][y] = true;
				eliminatedFruitTiles[(i + 2)][y] = true;

				flag = true;
			}
		}
	}

	for (int x = 0; x < BOARD_WIDTH; ++x)
	{	
		for (int i = DOWN_BOUND; i < UP_BOUND - 1; ++i)
		{		
			if( _vec4_equal(boardColors[x][i + 0], boardColors[x][i + 1]) && 
				_vec4_equal(boardColors[x][i + 1], boardColors[x][i + 2]) &&
				!_vec4_equal(boardColors[x][i], black) 				  )
			{

#ifdef DEBUG 
				cout << "matchFruitTiles() - [column fruit tiles detected]" << endl;
				cout << "matchFruitTiles() - tile["<< x << "][" << i + 0 <<"] = " << _MATCH_COLOR(boardColors[x][i + 0]) << endl;
				cout << "matchFruitTiles() - tile["<< x << "][" << i + 1 <<"] = " << _MATCH_COLOR(boardColors[x][i + 1]) << endl;
				cout << "matchFruitTiles() - tile["<< x << "][" << i + 2 <<"] = " << _MATCH_COLOR(boardColors[x][i + 2]) << endl;
#endif
				eliminatedFruitTiles[x][i + 0] = true;
				eliminatedFruitTiles[x][i + 1] = true;
				eliminatedFruitTiles[x][i + 2] = true;

				flag = true;
			}
		}

	}
	
	printEliminationTiles(eliminatedFruitTiles);
	return flag;
}

bool eliminateFruitTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT])
{
	for (int col = 0; col < BOARD_WIDTH; ++col)
	{
		for (int row = 0; row < BOARD_HEIGHT; ++row)
		{
			if (eliminatedFruitTiles[col][row])
			{
				board[col][row] = false;
				boardColors[col][row] = black;
			}
		}
	}

	return false;
}

void moveDownFruitTileCol(int startRow, int col)
{
	for (int row = startRow; row < BOARD_HEIGHT; ++row)
	{
		bool isBlackRow = false;

		// If the row to move down is the top row, add a black row
		if (row == UP_BOUND)
			isBlackRow = true;

		if (isBlackRow)
		{
			board[col][row] = false;
			boardColors[col][row] = black;				
		}
		else
		{
			board[col][row] = board[col][row + 1];
			boardColors[col][row] = boardColors[col][row + 1];
		}
	}
}

void moveDownFruitTilesCols(bool eliminatedFruitTiles[][BOARD_HEIGHT])
{
	for (int row = UP_BOUND; row >= 0; --row)
	{
		for (int col = 0; col < BOARD_WIDTH; ++col)
		{
			if (eliminatedFruitTiles[col][row])
				moveDownFruitTileCol(row, col);
		}
	}	
}

void cleanUpEliminationTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT])
{
	for (int i = 0; i < BOARD_WIDTH; ++i)
	{	
		for (int j = 0; j < BOARD_HEIGHT; ++j)
		{
			eliminatedFruitTiles[i][j] = false;
		}
	}
}

void printEliminationTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT])
{
	cout << "printEliminationTiles() - [Fruit Elimination Matrix]" << endl;
	for (int j = UP_BOUND; j >= 0; --j)
	{	
		for (int i = 0; i < BOARD_WIDTH; ++i)
		{
			cout << "[" << (eliminatedFruitTiles[i][j] ? "@" : "_") << "]";
		}
		cout << endl;
	}
	cout << endl;
}

bool checkEndOfGame()
{
	bool flag = false;
	for (int i = 0; i < BOARD_WIDTH; ++i)
	{

#ifdef DEBUG
		if (board[i][UP_BOUND])
		{
			cout << "chcekEndOfGame() - board[" << i << "][" << UP_BOUND << "]"<< endl;
		}
#endif
		flag = ( flag || board[i][UP_BOUND]);
	}

	return flag;
}

//-------------------------------------------------------------------------------------------------------------------
// Places the current tile - update the board vertex colour VBO and the array maintaining occupied cells
bool setTile()
{
	// First check if the collision between tile and grid is detected

	// If a collision is detected
	for (int i = 0; i < 4; ++i)
	{
		int x = int(tilePos.x + tile[i].x);
		int y = int(tilePos.y + tile[i].y);

#ifdef DEBUG
		cout << "setTile() - [Setting Tile] x = " << x << ", y = " << y << endl;
#endif
		board[x][y] = true;
		boardColors[x][y] = tileColors[i];
		genBoardVertexColorFromBoardColor(x, y, boardColors[x][y]);

	}

	updateBoard();

	// The first priority is to eliminate row, since it will deminish more tiles
	// Perform row checking..
	TileBound tileBound = getTileBound(tile);
	int start = tileBound.down + tilePos.y;
	int end = tileBound.up + tilePos.y;
	bool eliminatedRows[BOARD_HEIGHT] = { false };

	for (int i = start; i < end + 1; ++i)
	{		
		if( checkFullRow(i) ){
			eliminatedRows[i] = true;
			eliminateFullRow(i);
		}
	}

	moveDownRows(eliminatedRows);
	genBoardVertexColorsFromBoardColors();
	updateBoard();


	// The second priority is to eliminate fruit tiles
	bool eliminatedFruitTiles[BOARD_WIDTH][BOARD_HEIGHT] = {false};
	// After reset the board, update the buffer object

	cleanUpEliminationTiles(eliminatedFruitTiles);
	if( matchFruitTiles(eliminatedFruitTiles))
	{
		cout << "eliminating fruit..." << endl;
		eliminateFruitTiles(eliminatedFruitTiles);
		moveDownFruitTilesCols(eliminatedFruitTiles);
		genBoardVertexColorsFromBoardColors();
		updateBoard();
	}

// 	while( matchFruitTiles(eliminatedFruitTiles))
// 	{

// #ifdef DEBUG
// 		cout << "setTile() -[eliminate fruit tiles]" << endl;
// #endif
// 		// if fruit tiles could continue matching, do it iteratively
// 		eliminatedFruitTiles(eliminatedFruitTiles);
// 		moveDownFruitTilesCols(eliminatedFruitTiles);

// 		// Update the tiles to status after elimination
// 		genBoardVertexColorsFromBoardColors();
// 		updateBoard();
// 		sleep(0.1);
// 		cleanUpEliminationTiles(eliminatedFruitTiles);
// 	}


	return true;

}

//-------------------------------------------------------------------------------------------------------------------
// Given (x,y), tries to move the tile x squares to the right and y squares down
// Returns true if the tile was successfully moved, or false if there was some issue
bool moveTile(vec2 direction)
{
	if(ifGameStop) return false;

	bool flag = true;
	// Bound tileBound = getTileBound(tile);
	// vec2 futureTilePos = tilePos + direction;
	if( checkInBound(tilePos + direction) )
	{
		if ( checkTilesGridsCollision(tilePos + direction) )
		{
#ifdef DEBUG
	cout << "moveTile() - [Collision detected] " << endl;
#endif
			flag = false;
		}
		else
		{
			// If there is no collision in the future position
			tilePos += direction;
		}
	}
	else{
#ifdef DEBUG
	cout << "moveTile() - [Out of Bound] " << endl;
#endif		
		flag = false;
	}

#ifdef DEBUG
	cout << "moveTile() - Horizontal Movement: " << direction.x << ", Vertical Movement: " << direction.y << endl;
	cout << "moveTile() - Curret position - " << tilePos.x << ", " << tilePos.y << endl;
#endif

	if( flag ) updateTile();

	return flag;

}

void moveDownTileToEnd()
{
	if(ifGameStop) return;

#ifdef DEBUG
	cout << "moveDownTileToEnd() - [Straight down.] from ["<< tilePos.x <<"][" << tilePos.y << "]"<< endl;
#endif
	bool flag = false; 
	for (int i = 0; i <= BOARD_HEIGHT; ++i)
	{
		if( !moveTile(vec2(0.0, -1.0)) ){
			flag = true;
			break;
		}
	}

	if (flag == true)
	{
#ifdef DEBUG
	cout << "moveDownTileToEnd() - [Straight down.] to ["<< tilePos.x <<"][" << tilePos.y << "]"<< endl;
#endif
		setTile();
		updateTile();
		newTile();
	}
#ifdef DEBUG
	else
	{	
		cout << "moveDownTileToEnd() - [Error] " << endl;
		return;
	}
#endif

}

// ================================================================================================================= 
// Initialization controller

//-------------------------------------------------------------------------------------------------------------------
// Init back grid of the board
void initGrid()
{
	// ***Generate geometry data
	vec4 gridpoints[64]; 
	// Array containing the 64 points of the 32 total lines ( 21 horizontal line + 11 vertical line ) to be later put in the VBO
	vec4 gridcolours[64]; 
	// One colour per vertex
	
	// Vertical lines 
	for (int i = 0; i < 11; i++){
		gridpoints[2*i] 	= vec4((33.0 + (33.0 * i)), 33.0, 0, 1);
		gridpoints[2*i + 1] = vec4((33.0 + (33.0 * i)), 693.0, 0, 1);
	}
	// Horizontal lines
	for (int i = 0; i < 21; i++){
		gridpoints[22 + 2*i] 	 = vec4(33.0, (33.0 + (33.0 * i)), 0, 1);
		gridpoints[22 + 2*i + 1] = vec4(363.0, (33.0 + (33.0 * i)), 0, 1);
	}
	// Make all grid lines white
	for (int i = 0; i < 64; i++)
		gridcolours[i] = white;


	// *** set up buffer objects
	// Set up first VAO (representing grid lines)
	glBindVertexArray(vaoIDs[VAO_GRID]);
	// Bind the first VAO
	glGenBuffers(2, vboIDs); 
	// Create two Vertex Buffer Objects for this VAO (positions, colours)

	// Grid vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_GRID_POSITION]); 
	// Bind the first grid VBO (vertex positions)
	glBufferData(GL_ARRAY_BUFFER, sizeof(gridpoints), gridpoints, GL_STATIC_DRAW); 
	// Put the grid points in the VBO
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); 
	glEnableVertexAttribArray(vPosition); 
	// Enable the attribute
	
	// Grid vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_GRID_COLOR]); 
	// Bind the second grid VBO (vertex colours)
	glBufferData(GL_ARRAY_BUFFER, sizeof(gridcolours), gridcolours, GL_STATIC_DRAW); 
	// Put the grid colours in the VBO
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vColor); 
	// Enable the attribute
}

//-------------------------------------------------------------------------------------------------------------------
// Init playing board
void initBoard()
{
	// *** Generate the geometric data
	vec4 boardpoints[BOARD_WIDTH*BOARD_HEIGHT*3*2];
	// for (int i = 0; i < BOARD_WIDTH*BOARD_HEIGHT*3*2; i++)
	// 	boardVertexColors[i] = black; 
	// Let the empty cells on the board be black

	// Each cell is a square (2 triangles with 6 vertices)
	for (int i = 0; i < 20; i++){
		for (int j = 0; j < 10; j++)
		{		
			vec4 p1 = vec4(33.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p2 = vec4(33.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			vec4 p3 = vec4(66.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p4 = vec4(66.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			
			// Two points are reused
			boardpoints[6*(10*i + j)    ] = p1;
			boardpoints[6*(10*i + j) + 1] = p2;
			boardpoints[6*(10*i + j) + 2] = p3;
			boardpoints[6*(10*i + j) + 3] = p2;
			boardpoints[6*(10*i + j) + 4] = p3;
			boardpoints[6*(10*i + j) + 5] = p4;
		}
	}

	// Initially no cell is occupied
	for (int i = 0; i < BOARD_WIDTH; i++)
		for (int j = 0; j < BOARD_HEIGHT; j++)
		{
			board[i][j]			= false;
			boardColors[i][j] 	= black; 
		}

	// Generate vertex color structure from board
	genBoardVertexColorsFromBoardColors();

	// *** set up buffer objects
	glBindVertexArray(vaoIDs[VAO_BOARD]);
	glGenBuffers(2, &vboIDs[2]);

	// Grid cell vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_BOARD_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	// Grid cell vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_BOARD_COLOR]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), boardVertexColors, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vColor);
}

//-------------------------------------------------------------------------------------------------------------------
// No geometry for current tile initially
void initCurrentTile()
{
	// Bind the Current vao to Tils's array
	glBindVertexArray(vaoIDs[VAO_TILE]);
	glGenBuffers(2, &vboIDs[4]);

	// Current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	// Current tile vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vColor);
}

//-------------------------------------------------------------------------------------------------------------------
// init openGL configure
void init()
{
	// initialize random generator with seed 
	srand(time(NULL));

	// Load shaders and use the shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// Get the location of the attributes (for glVertexAttribPointer() calls)
	vPosition = glGetAttribLocation(program, "vPosition");
	vColor 	  = glGetAttribLocation(program, "vColor");

	// Create 3 Vertex Array Objects, each representing one 'object'. Store the names in array vaoIDs
	glGenVertexArrays(3, &vaoIDs[0]);

	// The location of the uniform variables in the shader program
	locxsize = glGetUniformLocation(program, "xsize"); 
	locysize = glGetUniformLocation(program, "ysize");

	// Game initialization
	newGame();
	// create new next tile

	// set to default
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);
}

void newGame()
{
	cout << "New Game" << endl;
	cout << "============================================================" << endl;
	
	// Global variable for controlling game logic
	ifGameStop = false;
	ifAllowTimer = true;

	initGrid();
	initBoard();
	initCurrentTile();
	newTile();	
}

void pauseResumeGame()
{
	ifAllowTimer = ! ifAllowTimer;

	if( false == ifAllowTimer )
	{
		cout << "pauseGame() - [Do something]" << endl;
	}
}

//-------------------------------------------------------------------------------------------------------------------
// Starts the game over - empties the board, creates new tiles, resets line counters
void restartGame()
{
	newGame();
}


// ================================================================================================================= 
// CallBack Functions

// Draws the game
void processDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1i(locxsize, xsize); // x and y sizes are passed to the shader program to maintain shape of the vertices on screen
	glUniform1i(locysize, ysize);

	glBindVertexArray(vaoIDs[1]); // Bind the VAO representing the grid cells (to be drawn first)
	glDrawArrays(GL_TRIANGLES, 0, 1200); // Draw the board (10*20*2 = 400 triangles)

	glBindVertexArray(vaoIDs[2]); // Bind the VAO representing the current tile (to be drawn on top of the board)
	glDrawArrays(GL_TRIANGLES, 0, 24); // Draw the current tile (8 triangles)

	glBindVertexArray(vaoIDs[0]); // Bind the VAO representing the grid lines (to be drawn on top of everything else)
	glDrawArrays(GL_LINES, 0, 64); // Draw the grid lines (21+11 = 32 lines)


	glutSwapBuffers();
}

//-------------------------------------------------------------------------------------------------------------------

// Reshape callback will simply change xsize and ysize variables, which are passed to the vertex shader
// to keep the game the same from stretching if the window is stretched
void processReshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

//-------------------------------------------------------------------------------------------------------------------

// Handle arrow key keypresses
void processSpecialKey(int key, int x, int y)
{
	// Initial attempt for special key event
	vec2 displacement = vec2(0,0);

	switch(key) {
	case GLUT_KEY_UP :
		rotateTile();
		break;
	case GLUT_KEY_DOWN :
		moveDownTileToEnd();
		break;
	case GLUT_KEY_LEFT :
		displacement -= vec2(step, 0);
		break;
	case GLUT_KEY_RIGHT:
		displacement += vec2(step, 0);
		break;
	}

	if( displacement != vec2(0, 0) && false == moveTile(displacement) )
	{
		if (displacement.y < 0)
		{
			setTile();
			updateTile();
			newTile();
		}
		else
			updateTile();

	}

}

//-------------------------------------------------------------------------------------------------------------------

// Handles standard keypresses
void processKeyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
		case 033: // Both escape key and 'q' cause the game to exit
		    exit(EXIT_SUCCESS);
		    break;
		case 'q':
			exit (EXIT_SUCCESS);
			break;
		case 'p':
			pauseResumeGame();
			break;
		case 'r': // 'r' key restarts the game
			restartGame();
			break;
		case ' ':
			shiftTileColor();
			break;
	}
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------
void processTimer(int val)
{
	if (ifAllowTimer)
	{
		cout << "running timer" << endl;

		if ( false == moveTile(vec2(0.0, velocity )) )
		{
			if (velocity < 0)
			{
				setTile();
				updateTile();
				newTile();
			}
			else
				updateTile();
		}
	}
	else
	{
		cout << "timer stopped" << endl;

	}

	glutPostRedisplay();
	glutTimerFunc(1000, processTimer, 1);
}
//-------------------------------------------------------------------------------------------------------------------

void tryStopGame()
{
	if(checkEndOfGame())
	{
		ifGameStop = true;
		cout << "Game Over..." << endl;
		exit(EXIT_SUCCESS);
	}
}

void processIdle()
{
	tryStopGame();
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	// initialize glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(xsize, ysize);
	glutInitWindowPosition(680, 178); // Center the game window (well, on a 1920x1080 display)
	glutCreateWindow("Fruit Tetris");
	// initizlize grew
	glewInit();

	// init grid, broad, tile
	init();

	// Callback functions
	glutDisplayFunc(processDisplay);
	glutReshapeFunc(processReshape);
	glutSpecialFunc(processSpecialKey);
	glutKeyboardFunc(processKeyboard);
	glutIdleFunc(processIdle);
	// Callback function for timer 
	glutTimerFunc(1000, processTimer, 1);

	glutMainLoop(); // Start main loop
	return 0;
}
