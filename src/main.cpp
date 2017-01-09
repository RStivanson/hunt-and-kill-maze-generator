#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include "HKMaze.h"

using namespace std;

struct Color {
	float r, g, b;
};

struct Player {
	int room;
	bool enabled;
};

Color globalColor;
HKMaze maze;
Player player;

int DELAY = 10;
const int DELAY_INC = 20, MAX_DELAY = 180, MIN_DELAY = 1;
int numberOfSteps, numberOfMoves;
const int VIEW_WIDTH = 500, VIEW_HEIGHT = 500;

// Steps along the maze generation
void ExecuteMazeStep(int x) {
	// if maze is not constructed
	if (!maze.IsConstructed()) {
		// print info if we have to
		if (maze.VerboseMode()) {
			cout << endl << "Performing a maze step" << endl;
		}

		// Step the maze forward
		maze.Step();

		// force a redisplay
		glutPostRedisplay();
		// reset the timer
		glutTimerFunc(DELAY, ExecuteMazeStep, 0);

		// Increase the steps
		++numberOfSteps;
	}
	else {
		cout << "Maze generation complete in " << numberOfSteps << " steps!" << endl;
	}
	return;
}

// Drawing routine.
void DrawScene() {
	vector<Wall> walls = maze.GetWalls();

	// Clear the color on the screen
	glClear(GL_COLOR_BUFFER_BIT);

	// For every wall
	for (int i = 0; i < walls.size(); ++i) {
		// if it is connected to the start or end, change the color
		if (walls[i].conStart) {
			glColor3f(0.65, 0.4, 1.0);
		}
		else if (walls[i].conEnd) {
			glColor3f(1.0, 0.2, 1.0);
		}
		else {
			glColor3f(globalColor.r, globalColor.g, globalColor.b);
		}

		// Start the openGL machine for drawing lines
		glBegin(GL_LINES);
		// set the vertexs for the line
		glVertex3f(walls[i].start.x + 0.5, walls[i].start.y, 0);
		glVertex3f(walls[i].end.x + 0.5, walls[i].end.y, 0);
		glEnd();
	}

	// If the player is enabled
	if (player.enabled) {
		// Get the center coords for the players location
		Coord coords = maze.GetRoomCenter(player.room);
		Coord size = maze.GetRoomSize();

		// Draw the player in all black
		glColor3f(0, 0, 0);

		// Start the openGL machine for drawing the player
		glBegin(GL_POLYGON);
		// Set the vertices for the player
		glVertex3f(coords.x - (size.x / 2) + (size.x / 5), coords.y - (size.y / 2) + (size.y / 5), 0);
		glVertex3f(coords.x - (size.x / 2) + (size.x / 5), coords.y + (size.y / 2) - (size.y / 5), 0);
		glVertex3f(coords.x + (size.x / 2) - (size.x / 5), coords.y + (size.y / 2) - (size.y / 5), 0);
		glVertex3f(coords.x + (size.x / 2) - (size.x / 5), coords.y - (size.y / 2) + (size.y / 5), 0);
		glEnd();
	}

	glFlush();

	return;
}

// Resets the Player to the beginning of the maze
void ResetPlayer() {
	// Reset the player
	cout << "Resetting the player..." << endl;
	player.enabled = false;
	player.room = maze.StartRoomID();

	// Restart the number of moves
	numberOfMoves = 0;

	return;
}

// Resets the Player and Maze
void Reset() {
	// Reset the entire maze
	cout << "Resetting..." << endl;
	maze.ChangeRoomSize(VIEW_WIDTH, VIEW_HEIGHT);
	maze.Reset(maze.RandomRoomID());
	ResetPlayer();
	numberOfSteps = 0;

	return;

}

// Initialization routine.
void Setup() {
	Reset();
	glClearColor(1.0, 1.0, 1.0, 0.0);
}

// OpenGL window reshape routine.
void Resize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, VIEW_WIDTH + 2, 0.0, VIEW_HEIGHT + 2, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return;
}

// Move the player in the given direction if possible
void MovePlayer(Direction direction) {
	int newRoom = maze.GetRoomID(player.room, direction);

	// If there is no wall between the two rooms
	if (!maze.WallBetweenRooms(player.room, newRoom)) {
		// Update player info and move the player
		++numberOfMoves;
		cout << "Number of moves so far: " << numberOfMoves << endl;
		player.room = newRoom;
	}

	return;
}

// Handles keyboard input
void HandleKeyboard(unsigned char key, int x, int y) {
	bool redisplay = true;

	switch (key) {
	case 'w': // Move up
		MovePlayer(UP);
		break;
	case 's': // Move down
		MovePlayer(DOWN);
		break;
	case 'a': // Move left
		MovePlayer(LEFT);
		break;
	case 'd': // Move right
		MovePlayer(RIGHT);
		break;
	case 'r': // Enter play mode
		if (maze.IsConstructed()) {
			cout << "Enabling play mode" << endl;
			ResetPlayer();
			player.enabled = true;
		}
		else {
			cout << "The maze needs to be constructed before it can be played" << endl;
		}
		break;
	case 'R': // Draws everything in Red
		globalColor.r = 1.0;
		globalColor.g = 0.0;
		globalColor.b = 0.0;
		break;
	case 'g': // Interactively display the generation of the maze (Generates new maze)
		Reset();

		if (maze.VerboseMode()) {
			cout << "Maze generation verbose mode is ON" << endl;
		}
		else {
			cout << "Maze generation verbose mode is OFF" << endl;
		}

		glutTimerFunc(DELAY, ExecuteMazeStep, 0);

		break;
	case 'i': // "Instant" maze regeneration
		Reset();
		maze.Generate();
		break;
	case 'G': // Draws everything in Green
		globalColor.r = 0.0;
		globalColor.g = 1.0;
		globalColor.b = 0.0;
		break;
	case 'b': // Draws everything in Black
		globalColor.r = 0.0;
		globalColor.g = 0.0;
		globalColor.b = 0.0;
		break;
	case 'B': // Draws everything in Blue
		globalColor.r = 0.0;
		globalColor.g = 0.0;
		globalColor.b = 1.0;
		break;
	case 'q': // Exit the program
		redisplay = false;
		exit(0);
		break;
	case 'Q': // Exit the program
		redisplay = false;
		exit(0);
		break;
	case 'V': // Set the verbosity mode
		maze.SetVerboseMode(!maze.VerboseMode());
		break;
	case '+': // Increase the delay
		if (DELAY == MIN_DELAY) {
			DELAY = DELAY_INC;
			cout << "DELAY has been raised to " << DELAY << endl;
		}
		else if (DELAY < MAX_DELAY) {
			DELAY += DELAY_INC;

			if (DELAY + DELAY_INC > MAX_DELAY) {
				DELAY = MAX_DELAY;
			}

			cout << "DELAY has been raised to " << DELAY << endl;
		}


		break;
	case '-': // decrease the delay
		if (DELAY > MIN_DELAY) {
			DELAY -= DELAY_INC;

			if (DELAY < MIN_DELAY) {
				DELAY = MIN_DELAY;
			}

			cout << "DELAY has been lowered to " << DELAY << endl;
		}

		break;
	}

	// if we need to redisplay
	if (redisplay) {
		glutPostRedisplay();
	}
	return;
}

// Initialization function
void Start(int argc, char **argv) {
	// Initialize glut
	glutInit(&argc, argv);

	glutInitContextVersion(3, 0);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Robert's Hunt and Kill Maze");
	glutDisplayFunc(DrawScene);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(HandleKeyboard);

	glewExperimental = GL_TRUE;
	glewInit();

	// Initialize the maze
	Setup();

	glutMainLoop();

	return;
}

// Main routine.
int main(int argc, char **argv) {
	srand(time(NULL));

	int rows, col;
	cout << "How many rows? ";
	cin >> rows;
	cout << endl;

	cout << "How many columns? ";
	cin >> col;
	cout << endl;

	maze.ROWS = rows;
	maze.COLUMNS = col;

	Start(argc, argv);

	return 0;
}
