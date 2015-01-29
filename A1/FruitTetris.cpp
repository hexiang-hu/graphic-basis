#include "FruitTetris.hpp"

// =================================================================================================================
// Tile Controller Function  


//-------------------------------------------------------------------------------------------------------------------
// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data
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
		if (_IN_BOUND(x,y))
		{
			genColorVertexFromTileColor(pointsColors, tileColors[i]);
		}
		else
		{
			// If current tile is outside the boundary, set it as black
			genColorVertexFromTileColor(pointsColors, black);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]);
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(vec4), 6*sizeof(vec4), pointsColors);


		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_POSITION]); 
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(vec4), 6*sizeof(vec4), newPoints);

		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]);
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(vec4), 6*sizeof(vec4), pointsColors);
	}

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------
// Called at the start of play and every time a tile is placed
void newTile()
{
	// First generate a random tile

	tileType = rand() % TILE_TYPE_NUM;
	vec2 (* pAllRotationShape)[4] = (tileType == TILE_TYPE_L) ?  allRotationsLshape :
				( (tileType == TILE_TYPE_I)? allRotationsIshape : allRotationsSshape );
	rotateType = rand() % ( allRotationShapeSize[tileType] );


#ifdef DEBUG
	cout << "newTile() [Generate Randomness] - TileType: " << tileType << ", RotateType: " << rotateType 
		<< ", RotationShape: " << allRotationShapeSize[tileType] << endl;
#endif

	// Update the geometry VBO of current tile
	for (int i = 0; i < 4; i++){
		tile[i] = pAllRotationShape[rotateType][i];

#ifdef DEBUG
		cout << "newTile() [Tile init] - Tile#" << i << " - " << tilePos.x + tile[i].x << "," << tilePos.y + tile[i].y << endl;
#endif

	}

	Bound tileBound = getTileBound(tile);
	int coverage = ( RIGHT_BOUND - int(tileBound.right) + int(tileBound.left) + 1);

	int hpos = rand() % coverage - int(tileBound.left);
	int vpos = UP_BOUND - tileBound.up;
	tilePos = vec2(hpos , vpos);
	// Put the tile at the top of the board
	
#ifdef DEBUG
	cout << "newTile() [Get Tile Boundary] - Bound(" << tileBound.left << ", " << tileBound.right
		<<", " << tileBound.down << ", " << tileBound.up << ")" << endl;
	cout << "newTile() [Boundaries] - Right: " << (RIGHT_BOUND - int(tileBound.right)) 
		<< ", Left: " << (- int(tileBound.left)) << ", Coverage: " << coverage << endl;
	cout << "newTile() [New Tile Position] - x = " << hpos << ", y = " << vpos << endl;
#endif

	// Get the 4 pieces of the new tile
	updateTile(); 

	// Update the color VBO of current tile
	vec4 newColors[24];

	// You should randomlize the color
	fillTileWithRandomColor();
	genColorVertexsFromTileColors(newColors);
	
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]); 
	// Bind the VBO containing current tile vertex colours
	
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newColors), newColors); 
	// Put the colour data in the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------
// Rotate the current tile 
void rotateTile()
{
	vec2 (* pAllRotationShape)[4] = (tileType == TILE_TYPE_L) ?  allRotationsLshape :
				( (tileType == TILE_TYPE_I)? allRotationsIshape : allRotationsSshape );

	// First test if the rotated version are in the boundary
	if (!testRotation(tilePos))
	{
		// Add logic to rescue a tile if it could be rotate with a unit movement.
		if (testRotation(tilePos + vec2(1, 0)))
			tilePos += vec2(1, 0);
		else if (testRotation(tilePos + vec2(-1, 0)))
			tilePos += vec2(-1, 0);
		else if (testRotation(tilePos + vec2( 0, 1)))
			tilePos += vec2(0, 1);
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
void checkFullRow(int startRow = DOWN_BOUND, int endRow = UP_BOUND)
{

}

//-------------------------------------------------------------------------------------------------------------------
// match fruits when a tile is set and see if there is a elimination 
void matchFruit(int startRow = DOWN_BOUND, int endRow = UP_BOUND)
{

}

//-------------------------------------------------------------------------------------------------------------------
// Places the current tile - update the board vertex colour VBO and the array maintaining occupied cells
void setTile()
{
	checkFullRow();
	matchFruit();
}

//-------------------------------------------------------------------------------------------------------------------
// Given (x,y), tries to move the tile x squares to the right and y squares down
// Returns true if the tile was successfully moved, or false if there was some issue
void moveTile(vec2 direction)
{
	// Bound tileBound = getTileBound(tile);
	// vec2 futureTilePos = tilePos + direction;
	if( checkBound(tilePos + direction) )
		tilePos += direction;

#ifdef DEBUG
	cout << "moveTile() - Horizontal Movement: " << direction.x << ", Vertical Movement: " << direction.y << endl;
	cout << "moveTile() - Curret position - " << tilePos.x << ", " << tilePos.y << endl;
#endif

	updateTile();

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
	vec4 boardpoints[1200];
	for (int i = 0; i < 1200; i++)
		boardcolours[i] = black; // Let the empty cells on the board be black
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
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++)
			board[i][j] = false; 


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
	glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), boardcolours, GL_DYNAMIC_DRAW);
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

	// Initialize the grid, the board, and the current tile
	initGrid();
	initBoard();
	initCurrentTile();

	// The location of the uniform variables in the shader program
	locxsize = glGetUniformLocation(program, "xsize"); 
	locysize = glGetUniformLocation(program, "ysize");

	// Game initialization
	newTile(); 	
	// create new next tile

	// set to default
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);
}



//-------------------------------------------------------------------------------------------------------------------
// Starts the game over - empties the board, creates new tiles, resets line counters
void restartGame()
{
	cout << "Restart Game..." << endl;
	initGrid();
	initBoard();
	initCurrentTile();
	newTile(); 
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
		displacement -= vec2(0, step);
		break;
	case GLUT_KEY_LEFT :
		displacement -= vec2(step, 0);
		break;
	case GLUT_KEY_RIGHT:
		displacement += vec2(step, 0);
		break;
	}

	moveTile(displacement);

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
		case 'r': // 'r' key restarts the game
			restartGame();
			break;
		case 's':
			shiftTileColor();
			break;
	}
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------
void processTimer(int val)
{
	cout << "running timer" << endl;
	moveTile(vec2(0.0, velocity ));
	glutPostRedisplay();
	glutTimerFunc(1000, processTimer, 0);
}
//-------------------------------------------------------------------------------------------------------------------

void processIdle()
{
	// When the game agent is idle, check if the current tile is arrived at a position 
	// that it could never fail. If it is, then set the tile 
	if ( checkGridCollision() )
	{
		setTile();
	}

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
	glutTimerFunc(1000, processTimer, 0);

	glutMainLoop(); // Start main loop
	return 0;
}
