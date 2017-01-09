#include "HKMaze.h"
#include <stdlib.h>
#include <cstdlib>
#include <iostream>

int HKMaze::ROWS = 200;
int HKMaze::COLUMNS = 200;
float HKMaze::ROOM_SIZE_X = 0;
float HKMaze::ROOM_SIZE_Y = 0;

std::string DirectionToString(Direction direction) {
	std::string str;

	// Select the right direction
	switch (direction) {
	case UP: // North
		str = "NORTH";
		break;
	case DOWN: // South
		str = "SOUTH";
		break;
	case LEFT: // West
		str = "WEST";
		break;
	case RIGHT: // East
		str = "EAST";
		break;
	default:
		str = "unkown";
		break;
	}

	return str;
}

Direction IntToDirection(int direction) {
	Direction dir;

	// Select the right direction
	switch (direction) {
	case 0:
		dir = UP;
		break;
	case 1:
		dir = DOWN;
		break;
	case 2:
		dir = LEFT;
		break;
	case 3:
		dir = RIGHT;
		break;
	default:
		dir = UP;
		break;
	}

	return dir;
}

// Default constructor
HKMaze::HKMaze() {
	InitializeMaze(RandomRoomID());
	return;
}

void HKMaze::Reset(int startRoom) {
	InitializeMaze(startRoom);
	return;
}

void HKMaze::ChangeRoomSize(int windowWidth, int windowHeight) {
	// calculate the correct size for the rooms
	ROOM_SIZE_X = windowWidth / float(COLUMNS);
	ROOM_SIZE_Y = windowHeight / float(ROWS);

	// Redo the maze
	InitializeMaze(currentRoomID);

	return;
}

void HKMaze::SetVerboseMode(bool enabled) {
	verbose = enabled;
	return;
}

bool HKMaze::VerboseMode() {
	return verbose;
}

bool HKMaze::WallBetweenRooms(int roomIDOne, int roomIDTwo) {
	bool wallFound = false;

	// for every wall while we havent found the wall
	for (int i = 0; !wallFound && i < walls.size(); ++i) {
		// If this wall connects the two rooms
		if (walls[i].conRoomOne == roomIDOne && walls[i].conRoomTwo == roomIDTwo) {
			wallFound = true;
		}

		// If this walls connects the two rooms
		if (walls[i].conRoomOne == roomIDTwo && walls[i].conRoomTwo == roomIDOne) {
			wallFound = true;
		}
	}

	return wallFound;
}

int HKMaze::StartRoomID() {
	return 1; // First room
}

int HKMaze::EndRoomID() {
	return ROWS * COLUMNS; // last room
}

Coord HKMaze::GetRoomSize() {
	Coord size;

	// Set the size
	size.x = ROOM_SIZE_X;
	size.y = ROOM_SIZE_Y;

	return size;
}


Coord HKMaze::GetRoomCenter(int roomID) {
	Coord center;

	// calculate the current row and column
	int row = (roomID - 1) / COLUMNS;
	int col = (roomID - 1) % COLUMNS;

	// calculate the distance of half a room
	float halfRoomX = ROOM_SIZE_X / 2;
	float halfRoomY = ROOM_SIZE_Y / 2;

	// calculate the center
	center.x = (col * ROOM_SIZE_X) + halfRoomX;
	center.y = (row * ROOM_SIZE_Y) + halfRoomY;

	return center;
}

int HKMaze::GetRoomID(int startRoom, Direction direction) {
	int id;

	// Select the right direction
	switch (direction) {
	case UP:
		id = startRoom + COLUMNS;

		// If the id is bigger than the last room
		if (id > ROWS * COLUMNS) {
			// set it to invalid
			id = -1;
		}

		break;
	case DOWN:
		id = startRoom - COLUMNS;

		// If the id is less than the first room
		if (id < 1) {
			// Set it to invalid
			id = -1;
		}

		break;

	case LEFT:
		// If the current room is on the left edge
		if (startRoom % COLUMNS == 1) {
			// it has no west neight
			id = -1;
		}
		else {
			// else it is the current room id - 1
			id = startRoom - 1;
		}
		break;
	case RIGHT:
		// If the current room is on the right edge
		if (startRoom % COLUMNS == 0) {
			// it has no east neighbor
			id = -1;
		}
		else {
			// Else it is the current room id + 1
			id = startRoom + 1;
		}

		break;
	default:
		// The neighbor is invalid
		id = -1;
		break;
	}

	return id;
}

bool HKMaze::ValidMove(int roomOne, int roomTwo) {
	bool contains = false, unblocked = false;
	std::vector<int> neighbors = GetRoomNeighbors(roomOne);

	// For every neighbors
	for (int i = 0; !contains && i < neighbors.size(); ++i) {
		// If this is the room we are looking for
		if (neighbors[i] == roomTwo) {
			// It is a neighor
			contains = true;
			// Check if there is a wall dividing the rooms
			unblocked = WallBetweenRooms(roomOne, roomTwo);
		}
	}

	// Return if the room is a neighbor and there is no wall blocking
	return (contains && unblocked);
}

void HKMaze::InitializeMaze(int startRoom) {
	constructed = false;
	needsHunt = false;
	verbose = false;
	rooms.clear();
	walls.clear();

	// Create all the rooms
	for (int row = 0; row < ROWS; ++row) {
		// Create all the columns
		for (int column = 0; column < COLUMNS; ++column) {
			// Initialize the current room
			Room room;
			room.roomID = CalculateRoomID(row, column);
			room.visited = false;
			rooms.push_back(room);
		}
	}

	if (verbose)
		std::cout << "Rooms created. " << rooms.size() << std::endl;

	// Create all the horizontal walls
	for (int row = 0; row < ROWS; ++row) {// For every row
		for (int column = 0; column < COLUMNS; ++column) {// For every column
														  // Create the bottom  and left walls
			walls.push_back(CreateHorizontalWall(row, column));
			walls.push_back(CreateVerticalWall(row, column));

			// If this is a top row room
			if (row + 1 == ROWS) {
				// Create the top edge walls
				walls.push_back(CreateHorizontalWall(row + 1, column));
			}

			// If this is a right column room
			if (column + 1 == COLUMNS) {
				// Create the right edge walls
				walls.push_back(CreateVerticalWall(row, column + 1));
			}
		}
	}

	// start the maze generation at the given room
	currentRoomID = startRoom;
	SetRoomVisited(currentRoomID);

	if (verbose)
		std::cout << "Walls created. " << walls.size() << std::endl;

	return;
}

int HKMaze::RandomRoomID() {
	// Pick a random room between 1 and ROWS * COLUMNS
	return (rand() % (ROWS * COLUMNS)) + 1;
}

Wall HKMaze::CreateHorizontalWall(int row, int column) {
	Wall newH;

	// calculate the start of the line
	newH.start.x = (column * ROOM_SIZE_X);
	newH.start.y = (row * ROOM_SIZE_Y);

	// calculate the end of the line
	newH.end.x = newH.start.x + ROOM_SIZE_X;
	newH.end.y = newH.start.y;

	// if this is the first row
	if (row == 0) {
		// this row is then an edge row
		newH.conRoomOne = -1;
		newH.conRoomTwo = CalculateRoomID(row, column);
	}
	else if (row == ROWS) { // If this is the last row
							// This is then an edge row
		newH.conRoomOne = CalculateRoomID(row - 1, column);
		newH.conRoomTwo = -1;
	}
	else {
		// This wall connects two rooms
		newH.conRoomOne = CalculateRoomID(row - 1, column);
		newH.conRoomTwo = CalculateRoomID(row, column);
	}

	// This wall is horizontal
	newH.horizontal = true;
	// If this is connected to the start room
	if (newH.conRoomOne == StartRoomID() || newH.conRoomTwo == StartRoomID()) {
		newH.conStart = true;
	}
	else { // if it is not connected to the start room
		newH.conStart = false;
	}

	// If this connected to the end room
	if (newH.conRoomOne == EndRoomID() || newH.conRoomTwo == EndRoomID()) {
		newH.conEnd = true;
	}
	else { // If it is not connected to the end room
		newH.conEnd = false;
	}

	return newH;
}

Wall HKMaze::CreateVerticalWall(int row, int column) {
	Wall newV;

	// calculate the start of the line
	newV.start.x = (column * ROOM_SIZE_X);
	newV.start.y = (row * ROOM_SIZE_Y);

	// calculate the end of the line
	newV.end.x = newV.start.x;
	newV.end.y = newV.start.y + ROOM_SIZE_Y;

	// If the is the first column
	if (column == 0) {
		// This is then an edge wall
		newV.conRoomOne = -1;
		newV.conRoomTwo = CalculateRoomID(row, column);
	}
	else if (column == COLUMNS) { // If this is the last wall
								  // This is then an edge wall
		newV.conRoomOne = CalculateRoomID(row, column - 1);
		newV.conRoomTwo = -1;
	}
	else {
		// This connects two rooms
		newV.conRoomOne = CalculateRoomID(row, column - 1);
		newV.conRoomTwo = CalculateRoomID(row, column);
	}

	// This is not a horizontal wall
	newV.horizontal = false;

	// If this is connected to the start room
	if (newV.conRoomOne == StartRoomID() || newV.conRoomTwo == StartRoomID()) {
		newV.conStart = true;
	}
	else { // If this is not connected to the start room
		newV.conStart = false;
	}

	// If this is connected to the end room
	if (newV.conRoomOne == EndRoomID() || newV.conRoomTwo == EndRoomID()) {
		newV.conEnd = true;
	}
	else { // If it is not connected to the end room
		newV.conEnd = false;
	}

	return newV;
}

void HKMaze::RemoveConnectingWall(int roomIDOne, int roomIDTwo, Direction direction) {
	bool removed = false;

	if (verbose) {
		std::cout << " -- Removing the wall between room #" << roomIDOne << " and room #" << roomIDTwo << std::endl;
	}

	// For every wall while it has not been removed
	for (int i = 0; i < walls.size() && !removed; ++i) {
		switch (direction) {
		case UP: // North
				 // If the wall is horizontal
			if (walls[i].horizontal == true) {
				// If this wall connects the two rooms
				if (walls[i].conRoomOne == roomIDOne && walls[i].conRoomTwo == roomIDTwo) {
					if (verbose) {
						std::cout << " -- -- Wall found" << std::endl;
					}

					// remove the wall
					walls.erase(walls.begin() + i);
					removed = true;
				}
			}

			break;
		case DOWN: // South
				   // If the wall is horizontal
			if (walls[i].horizontal == true) {
				// If this wall connects the two rooms
				if (walls[i].conRoomOne == roomIDTwo && walls[i].conRoomTwo == roomIDOne) {
					if (verbose) {
						std::cout << " -- -- Wall found" << std::endl;
					}

					// Remove the wall
					walls.erase(walls.begin() + i);
					removed = true;
				}
			}

			break;
		case LEFT: // West
				   // If this is a vertical wall
			if (walls[i].horizontal == false) {
				// If this wall connects the two rooms
				if (walls[i].conRoomOne == roomIDTwo && walls[i].conRoomTwo == roomIDOne) {
					if (verbose) {
						std::cout << " -- -- Wall found" << std::endl;
					}


					// Remove the wall
					walls.erase(walls.begin() + i);
					removed = true;
				}
			}

			break;
		case RIGHT: // East
					// If this is a vertical wall
			if (walls[i].horizontal == false) {
				// If this wall connects the two rooms
				if (walls[i].conRoomOne == roomIDOne && walls[i].conRoomTwo == roomIDTwo) {
					if (verbose) {
						std::cout << " -- -- Wall found" << std::endl;
					}

					// Remove the wall
					walls.erase(walls.begin() + i);
					removed = true;
				}
			}

			break;
		default:
			removed = true;
		}
	}

	return;
}

bool HKMaze::WalkNext() {
	// Prepare to walk from this location
	std::vector<int> neigh = GetRoomNeighbors(currentRoomID), direction;
	needsHunt = CheckIfHuntNeeded(neigh);

	// If this location is not a dead end
	if (!needsHunt) {
		if (verbose) {
			std::cout << " -- The maze does not need a hunt" << std::endl;
		}

		// get a random direction from this room
		Direction dir = GetRandomDirection(neigh);

		if (verbose) {
			std::cout << " -- At room #" << currentRoomID << " moving " << DirectionToString(dir) << " to room #" << neigh[int(dir)] << std::endl;
		}

		// Remove the walls between these two walls
		RemoveConnectingWall(currentRoomID, neigh[int(dir)], dir);
		SetRoomVisited(neigh[int(dir)]);

		// set that room as the active room
		currentRoomID = neigh[int(dir)];

		if (verbose) {
			std::cout << " -- Walk completed" << std::endl;
		}
	}
	else {
		if (verbose) {
			std::cout << " -- The maze needs a hunt to continue" << std::endl;
		}
	}

	// Returns true if the maze needs a hunt performed
	return needsHunt;
}

void HKMaze::Hunt() {
	// Pick a random value
	int r = rand() % 4;

	// Randomly hunt forward or backwards
	if (r < 2) {
		HuntForward();
	}
	else {
		HuntBackward();
	}

	return;
}

void HKMaze::HuntForward() {
	std::vector<int> neighbors;
	bool hasVisitedNeighbor = false, newCellFound = false;
	int visitedNeighbor = -1;
	Direction direction;

	// for every room
	for (int i = 0; needsHunt && i < rooms.size(); ++i) {
		// If the room is not visited
		if (rooms[i].visited == false) {
			// Get the rooms neighbors
			neighbors = GetRoomNeighbors(rooms[i].roomID);

			// For every neighbor
			for (int j = 0; !hasVisitedNeighbor && j < neighbors.size(); ++j) {
				Room room = GetRoom(neighbors[j]);

				// Check if any of the neighbors have been visited
				if (room.visited == true) {

					// get information about the neighbor
					visitedNeighbor = room.roomID;
					hasVisitedNeighbor = true;
					direction = IntToDirection(j);
				}
			}

			// If we have a visited neighbor
			if (hasVisitedNeighbor) {
				// Remove the wall connecting these two rooms
				RemoveConnectingWall(rooms[i].roomID, visitedNeighbor, direction);

				// Set the current room to this room
				currentRoomID = rooms[i].roomID;
				SetRoomVisited(currentRoomID);

				// We are done hunting
				newCellFound = true;
				needsHunt = false;
			}
		}
	}

	// If there are no new cells to walk from
	if (!newCellFound) {
		// The maze has been completely constructed
		constructed = true;
	}

	return;
}

void HKMaze::HuntBackward() {
	std::vector<int> neighbors;
	bool hasVisitedNeighbor = false, newCellFound = false;
	int visitedNeighbor = -1;
	Direction direction;

	// for every room
	for (int i = rooms.size() - 1; needsHunt && i >= 0; --i) {
		// if the room has not been visited yet
		if (rooms[i].visited == false) {
			// Get the rooms neighbors
			neighbors = GetRoomNeighbors(rooms[i].roomID);

			for (int j = 0; !hasVisitedNeighbor && j < neighbors.size(); ++j) {
				// Check if any of the neighbors have been visited
				Room room = GetRoom(neighbors[j]);

				if (room.visited == true) {
					// get information about the neighbor
					visitedNeighbor = room.roomID;
					hasVisitedNeighbor = true;
					direction = IntToDirection(j);
				}
			}

			// If we have a visited neighbor
			if (hasVisitedNeighbor) {
				// Remove the wall connecting these two rooms
				RemoveConnectingWall(rooms[i].roomID, visitedNeighbor, direction);

				// Set the current room to this room
				currentRoomID = rooms[i].roomID;
				SetRoomVisited(currentRoomID);

				// We are done hunting
				newCellFound = true;
				needsHunt = false;
			}
		}
	}

	// If there are no new cells to walk from
	if (!newCellFound) {
		// The maze has been completely constructed
		constructed = true;
	}

	return;
}

Direction HKMaze::GetRandomDirection(std::vector<int> neighbors) {
	// Get a random direction to walk in
	int dir = rand() % 4;

	// While we havent picked a neighbor that is valid and unvisited
	while (neighbors[dir] == -1 || RoomVisited(neighbors[dir])) {
		// pick a new direction
		dir = rand() % 4;
	}

	// Convert the into to Direction
	return IntToDirection(dir);
}

bool HKMaze::CheckIfHuntNeeded(std::vector<int> neighbors) {
	bool needsHunt = true;

	// For every neighbor
	for (int i = 0; needsHunt && i < 4; ++i) {
		// if the room is valid and unvisited
		if (neighbors[i] != -1 && !RoomVisited(neighbors[i])) {
			// we do not need to hunt
			needsHunt = false;
		}
	}

	return needsHunt;
}

Room HKMaze::GetRoom(int roomID) {
	Room room;
	bool found = false;

	room.roomID = -1;

	// For every room
	for (int i = 0; i < rooms.size() && !found; ++i) {
		// If this is the room we are looking for
		if (rooms[i].roomID == roomID) {
			// get the rooms information
			room = rooms[i];
			found = true;
		}
	}

	return room;
}

void HKMaze::SetRoomVisited(int roomID) {
	bool found = false;

	// for every room
	for (int i = 0; i < rooms.size() && !found; ++i) {
		// If this is the room we are looking for
		if (rooms[i].roomID == roomID) {
			if (verbose) {
				std::cout << " -- -- room #" << roomID << " has now been visited." << std::endl;
			}

			// Set the room to have been visited
			rooms[i].visited = true;
			found = true;
		}
	}

	return;
}

int HKMaze::GetRoomIndex(int roomID) {
	bool found = false;
	int cur = 0;

	// While we have not found the room
	while (!found && cur < rooms.size()) {
		// If this is the room we are looking for
		if (rooms[cur].roomID == roomID) {
			// We have found it
			found = true;
		}
		else {
			// else move to the next room
			++cur;
		}
	}

	return cur;
}

bool HKMaze::RoomVisited(int roomID) {
	int index = GetRoomIndex(roomID);
	bool visited = true;

	if (verbose) {
		std::cout << " -- -- room #" << roomID << " visited?";
	}

	// If the index is valid
	if (ValidateRoomIndex(index)) {
		// Get if the room is visited
		visited = rooms[index].visited;
	}

	if (verbose) {
		std::cout << "  " << visited << std::endl;
	}

	return visited;
}

int HKMaze::CalculateRoomID(int row, int column) {
	return (row * COLUMNS) + (column + 1);
}

std::vector<int> HKMaze::GetRoomNeighbors(int roomID) {
	if (verbose) {
		std::cout << " -- Room #" << roomID << " : ";
	}

	// Get the id and validate it
	int north = ValidateRoomID(roomID + COLUMNS);
	int south = ValidateRoomID(roomID - COLUMNS);
	int west, east;

	// Get the correct west id
	if (roomID % COLUMNS == 1) {
		west = -1;
	}
	else {
		west = roomID - 1;
	}

	// get the correct east id
	if (roomID % COLUMNS == 0) {
		east = -1;
	}
	else {
		east = roomID + 1;
	}

	if (verbose) {
		std::cout << "N" << north;
		std::cout << "  S" << south;
		std::cout << "  W" << west;
		std::cout << "  E" << east << std::endl;
	}

	// Create a vector of the neighbors
	std::vector<int> neighbors;
	neighbors.clear();

	neighbors.push_back(north);
	neighbors.push_back(south);
	neighbors.push_back(west);
	neighbors.push_back(east);

	return neighbors;
}

std::vector<Wall> HKMaze::GetRoomWalls(int roomID) {
	std::vector<Wall> newWalls;
	newWalls.clear();

	// for every wall
	for (int i = 0; i < walls.size(); ++i) {
		// if the wall connects to this room
		if (walls[i].conRoomOne == roomID || walls[i].conRoomTwo == roomID) {
			// add it to the vector
			newWalls.push_back(walls[i]);
		}
	}

	return newWalls;
}

int HKMaze::ValidateRoomID(int roomID) {
	// If the id is not in bounds
	if (roomID <= 0 || roomID > ROWS*COLUMNS) {
		// return invalid
		return -1;
	}

	return roomID;
}

bool HKMaze::ValidateRoomIndex(int index) {
	// if the index is not in the bounds
	if (index < 0 || index >= rooms.size()) {
		// return invalid
		return false;
	}

	return true;
}

std::vector<Wall> HKMaze::GetWalls() {
	return walls;
}

void HKMaze::Step() {
	// If the maze is not constructed
	if (!constructed) {
		if (verbose) {
			std::cout << "Performing a ";
		}

		// If we need to hunt
		if (needsHunt) {
			if (verbose) {
				std::cout << "Hunt" << std::endl;
			}

			// Hunt for a new room
			Hunt();
		}
		else { //if we do not need to perform a hunt
			if (verbose) {
				std::cout << "Walk" << std::endl;
			}

			// Walk to the next room
			needsHunt = WalkNext();
		}
	}

	return;
}

void HKMaze::Generate() {
	// While the maze is not constructed
	while (!constructed) {
		// Step again
		Step();
	}

	return;
}

bool HKMaze::IsConstructed() const {
	return constructed;
}
