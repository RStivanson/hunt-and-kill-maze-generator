#ifndef HKMAZE_H
#define HKMAZE_H

#include <vector>
#include <string>

struct Coord {
	float x, y;
};

struct Wall {
	bool horizontal;
	bool conStart, conEnd;
	Coord start, end;
	int conRoomOne, conRoomTwo;
};

struct Room {
	int roomID;
	bool visited;
};

enum Direction {
	UP = 0, 
	DOWN = 1, 
	LEFT = 2, 
	RIGHT = 3
};

std::string DirectionToString(Direction direction);
Direction IntToDirection(int direction);

class HKMaze {
	private:
		// variables
		static float ROOM_SIZE_X, ROOM_SIZE_Y;
		
		bool constructed, needsHunt, verbose;
		int currentRoomID;
		std::vector<Room> rooms;
		std::vector<Wall> walls;
		
		// Initializes the maze
		void InitializeMaze(int startRoom);
		
		// return the room that matches the room id
		Room GetRoom(int roomID);
		
		// Get the index of the room with the room id
		int GetRoomIndex(int roomID);
		
		// return if the given room has already been visited
		bool RoomVisited(int roomID);
		
		// sets the room to have been visited
		void SetRoomVisited(int roomID);
		
		// Calculate the room id in a 2d space
		int CalculateRoomID(int row, int column);
		
		// Get the neighors to this room
		std::vector<int> GetRoomNeighbors(int roomID);
		
		// validate that the room id is within range
		int ValidateRoomID(int roomID);
		
		// Validate the index of the room
		bool ValidateRoomIndex(int index);
		
		// Get the walls that surround this room
		std::vector<Wall> GetRoomWalls(int roomID);
		
		// Remove the walls connecting the two rooms
		void RemoveConnectingWall(int roomIDOne, int roomIDTwo, Direction direction);
		
		// Create a horizontal wall
		Wall CreateHorizontalWall(int row, int column);
		
		// Create a vertical wall
		Wall CreateVerticalWall(int row, int column);
		
		// Pick a random direction to an unvisited room
		Direction GetRandomDirection(std::vector<int> neighbors);
		
		// Walk to the next room
		bool WalkNext();
		
		// Hunt for a room next to a visited room
		void Hunt();
		
		// Hunt for a room next to a visited room, bottom to top
		void HuntForward();
		
		// Hunt for a room next to a visited room, top to bottom
		void HuntBackward();
		
		// Check if the room is at a dead end
		bool CheckIfHuntNeeded(std::vector<int> neighbors);
	public:
		static int ROWS, COLUMNS;
		
		// Default constructor
		HKMaze();
		
		// Generate the maze in one go
		void Generate();
		
		// Perform the next step needed towards construction of the maze
		void Step();
		
		// Change the size of the rooms so they fit the window
		void ChangeRoomSize(int windowWidth, int windowHeight);
		
		// Reset the maze, and set the maze gen start room
		void Reset(int startRoom = 1);
		
		// Pick a random room id
		int RandomRoomID();
		
		// Set the verbosity mode for the maze generation
		void SetVerboseMode(bool enabled);
		
		// Get the state of the maze verbosity mode
		bool VerboseMode();
		
		// Check if a wall exists between the two rooms
		bool WallBetweenRooms(int roomIDOne, int roomIDTwo);
		
		// Gets the id of the room in the direction from the given room
		int GetRoomID(int startRoom, Direction direction);
		
		// Returns the x and y sizes of the displayed rooms
		Coord GetRoomSize();
		
		// Returns the center coords for the given room
		Coord GetRoomCenter(int roomID);
		
		// Returns the id of the room where the player will start
		int StartRoomID();
		
		// returns the id of the room where the player will end
		int EndRoomID();
		
		// Returns true if it valid to move from roomOne to roomTwo
		bool ValidMove(int roomOne, int roomTwo);
		
		// Returns the construction status of the maze
		bool IsConstructed() const;
		
		// Returns all the walls in the maze
		std::vector<Wall> GetWalls();
};

#endif