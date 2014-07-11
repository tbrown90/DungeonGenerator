#include "stdafx.h"
#include "dungeon.h"
#include "NameGenerator.h"
#include "utilities.h"
#include <iostream>
#include <time.h>


/*
	CTor
*/
Dungeon::Dungeon(int w, int h, int minRS, int maxRS, DungeonMask dm, RoomDensity rd, CorridorType ct, int dec)
{
	if (w % 2 == 0)
		w -= 1;
	width = w;

	if (h % 2 == 0)
		h -= 1;
	height = h;

	if (minRS % 2 == 0)
		minRS -= 1;
	minRoomSize = minRS;

	if (maxRS % 2 == 0)
		maxRS -= 1;
	maxRoomSize = maxRS;

	dungeonName.clear();
	NameGenerator ng = NameGenerator();
	dungeonName = ng.GenerateNewName();

	dungeonMask = dm;
	roomDensity = rd;
	corridorType = ct;
	deadEndChance = dec;

	//dungeon = new Cell*[height];
	dungeon = (Cell**)calloc(height, sizeof(Cell));
	for (int y = 0; y < height; ++y)
	{
		//dungeon[y] = new Cell[width];
		dungeon[y] = (Cell*)calloc(width, sizeof(Cell));
		for (int x = 0; x < width; ++x)
		{
			dungeon[y][x] = Cell(x, y, EMPTY);
		}
	}

	dungeonBuilt = false;
}

Dungeon::Dungeon() 
{
	dungeonBuilt = false;
	width = 1;
	height = 1;
	dungeonMask = SQUARE;
	roomDensity = SCATTERED;
	corridorType = MIXED;
	minRoomSize = 0;
	maxRoomSize = 0;
	deadEndChance = 0;
	dungeon = new Cell*[0];
	for (int y = 0; y < height; ++y)
	{
		dungeon[y] = new Cell[width];
		for (int x = 0; x < width; ++x)
		{
			dungeon[y][x] = Cell(x, y, EMPTY);
		}
	}

}
/*
	Delete the dungeon
*/
Dungeon::~Dungeon()
{
	if (!dungeonBuilt)
		return;

	for (int y = 0; y < height; ++y)
	{
		delete[] dungeon[y];
	}
	delete[] dungeon;
}

void Dungeon::LoadDungeon(Cell** dng, int w, int h, std::string n)
{
	dungeon = dng;
	width = w;
	height = h;
	dungeonName = n;

	dungeonBuilt = true;
}

/*
	Prints the dungeon
*/
void Dungeon::PrintDungeon()
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			switch (dungeon[y][x].GetType())
			{
			case EMPTY:
				printf(".");
				break;
			case WALL:
				printf("#");
				break;
			case ROOM:
				printf(" ");
				break;
			case DOOR:
				printf(" ");
				break;
			case CORRIDOR:
				printf(" ");
				break;
			}
		}
		printf("\n");
	}
}

/*
	Resets the dungeon
*/
void Dungeon::ResetDungeon()
{
	if (dungeonBuilt)
	{
		for (int y = 0; y < height; ++y)
		{
			delete[] dungeon[y];
		}
		delete[] dungeon;
	}

	dungeon = new Cell*[height];
	for (int y = 0; y < height; ++y)
	{
		dungeon[y] = new Cell[width];
		for (int x = 0; x < width; ++x)
		{
			dungeon[y][x] = Cell(x, y, EMPTY);
		}
	}

	rooms.clear();
}

/*
	Generates a dungeon
*/
void Dungeon::GenerateDungeon()
{
	ResetDungeon();

	ApplyMask();

	while (rooms.size() == 0)
	{
		switch (roomDensity)
		{
		case SPARSE:
			GenerateSparseRooms();
			break;
		case SCATTERED:
			GenerateScatteredRooms();
			break;
		case DENSE:
			GenerateDenseRooms();
			break;
		}	
	}

	GenerateCorridors();

	WallUpDungeon();
	CollapseTunnel();

	dungeonBuilt = true;
}

/*
	Generates Scattered Rooms
	Determines how many rooms to try and generate
	For each room
	Selects a random position in the dungeon (must be odd)
	Genereates the width and height of the room (must be odd);
	Make sure the room doesn't go off the edge
	Make sure the room doesn't over lap other rooms
	Place the room
	Encircle it with walls
	Generate a door
*/
void Dungeon::GenerateScatteredRooms()
{
	int numRooms = (width * height) / (maxRoomSize * maxRoomSize) / 2;
	int generatedRooms = 0;

	for (int i = 0; i < numRooms; ++i)
	{
		int numAttempts = 10;
		while (true && numAttempts > 0)
		{
			if (numRooms > 5 && generatedRooms < 2)
				numAttempts = 10;

			int xPos = RandomOddRange(1, width - 1);
			int yPos = RandomOddRange(1, height - 1);
			int rWidth = RandomOddRange(minRoomSize, maxRoomSize);
			int rHeight = RandomOddRange(minRoomSize, maxRoomSize);

			//Make sure we aren't at an edge
			if (xPos - 2 < 0 || xPos + rWidth + 1 > width - 1 || yPos - 2 < 0 || yPos + rHeight + 1 > height - 1)
			{
				numAttempts -= 1;
				continue;
			}

			bool canPlace = true;
			//Check to make sure the room doesn't overlap anything
			for (int y = yPos - 2; y < yPos + rHeight + 1; ++y)
			{
				for (int x = xPos - 2; x < xPos + rWidth + 1; ++x)
				{
					if (dungeon[y][x].GetType() != EMPTY)
					{
						canPlace = false;
						break;
					}
				}
				if (!canPlace)
					break;
			}

			if (!canPlace)
			{
				numAttempts -= 1;
				continue;
			}

			generatedRooms += 1;
			//Place the room
			for (int y = yPos; y < yPos + rHeight; ++y)
			{
				for (int x = xPos; x < xPos + rWidth; ++x)
				{
					dungeon[y][x].SetType(ROOM);
				}
			}
			Room r = Room(xPos, yPos, rWidth, rHeight);
			rooms.push_back(r);

			//Place Walls
			for (int y = yPos - 1; y < yPos + rHeight + 1; ++y)
			{
				dungeon[y][xPos - 1].SetType(WALL);
				dungeon[y][xPos + rWidth].SetType(WALL);
			}
			for (int x = xPos - 1; x < xPos + rWidth + 1; ++x)
			{
				dungeon[yPos - 1][x].SetType(WALL);
				dungeon[yPos + rHeight][x].SetType(WALL);
			}

			//Place doors
			int numDoors = (rWidth * rHeight) >= 12 ? 2 : 1;
			for (int j = 0; j < numDoors; ++j)
			{
				int prevDoorDir = -1;

				bool doorPlaced = false;
				while (!doorPlaced)
				{
					int dir = rand() % 4;
					int doorX = 0;
					int doorY = 0;

					if (dir == prevDoorDir)
							continue;

					switch (dir)
					{
					case 0:
						doorX = RandomOddRange(xPos, xPos + rWidth - 1);
						doorY = yPos - 1;
						if (doorY - 1 > 0 && dungeon[doorY - 1][doorX].GetType() == EMPTY)
						{
							prevDoorDir = dir;
							dungeon[doorY][doorX].SetType(DOOR);
							doorPlaced = true;
						}
						break;
					case 1:
						doorX = RandomOddRange(xPos, xPos + rWidth - 1);
						doorY = yPos + rHeight;
						if (doorY + 1 < height - 1 && dungeon[doorY + 1][doorX].GetType() == EMPTY)
						{
							prevDoorDir = dir;
							dungeon[doorY][doorX].SetType(DOOR);
							doorPlaced = true;
						}
						break;
					case 2:
						doorY = RandomOddRange(yPos, yPos + rHeight - 1);
						doorX = xPos - 1;
						if (doorX - 1 > 0 && dungeon[doorY][doorX - 1].GetType() == EMPTY)
						{
							prevDoorDir = dir;
							dungeon[doorY][doorX].SetType(DOOR);
							doorPlaced = true;
						}
						break;
					case 3:
						doorY = RandomOddRange(yPos, yPos + rHeight - 1);
						doorX = xPos + rWidth;
						if (doorX + 1 < width - 1 && dungeon[doorY][doorX + 1].GetType() == EMPTY)
						{
							prevDoorDir = dir;
							dungeon[doorY][doorX].SetType(DOOR);
							doorPlaced = true;
						}
						break;
					}
				}
			}
		} //END WHILE
	} //END FOR
}

/*
	Generates Scattered Rooms
	Determines how many rooms to try and generate
	For each room
	Selects a random position in the dungeon (must be odd)
	Genereates the width and height of the room (must be odd);
	Make sure the room doesn't go off the edge
	Make sure the room doesn't over lap other rooms
	Place the room
	Encircle it with walls
	Generate a door
*/
void Dungeon::GenerateSparseRooms()
{
	int numRooms = (width * height) / (maxRoomSize * maxRoomSize) / 4;
	int generatedRooms = 0;

	for (int i = 0; i < numRooms; ++i)
	{
		int numAttempts = 5;
		while (true && numAttempts > 0)
		{
			if (numRooms > 5 && generatedRooms < 2)
				numAttempts = 5;

			int xPos = RandomOddRange(1, width - 1);
			int yPos = RandomOddRange(1, height - 1);
			int rWidth = RandomOddRange(minRoomSize, maxRoomSize);
			int rHeight = RandomOddRange(minRoomSize, maxRoomSize);

			//Make sure we aren't at an edge
			if (xPos - 2 < 0 || xPos + rWidth + 1 > width - 1 || yPos - 2 < 0 || yPos + rHeight + 1 > height - 1)
			{
				numAttempts -= 1;
				continue;
			}

			bool canPlace = true;
			//Check to make sure the room doesn't overlap anything
			for (int y = yPos - 2; y < yPos + rHeight + 1; ++y)
			{
				for (int x = xPos - 2; x < xPos + rWidth + 1; ++x)
				{
					if (dungeon[y][x].GetType() != EMPTY)
					{
						canPlace = false;
						break;
					}
				}
				if (!canPlace)
					break;
			}

			if (!canPlace)
			{
				numAttempts -= 1;
				continue;
			}

			generatedRooms += 1;
			//Place the room
			for (int y = yPos; y < yPos + rHeight; ++y)
			{
				for (int x = xPos; x < xPos + rWidth; ++x)
				{
					dungeon[y][x].SetType(ROOM);
				}
			}
			Room r = Room(xPos, yPos, rWidth, rHeight);
			rooms.push_back(r);

			//Place Walls
			for (int y = yPos - 1; y < yPos + rHeight + 1; ++y)
			{
				dungeon[y][xPos - 1].SetType(WALL);
				dungeon[y][xPos + rWidth].SetType(WALL);
			}
			for (int x = xPos - 1; x < xPos + rWidth + 1; ++x)
			{
				dungeon[yPos - 1][x].SetType(WALL);
				dungeon[yPos + rHeight][x].SetType(WALL);
			}

			//Place doors
			//int numDoors = (rWidth * rHeight) >= 12 ? 2 : 1;
			int dim = (rWidth * rHeight);
			int numDoors = 0;
			if (dim <= 12)
				numDoors = 1;
			else
				if (dim <= 25)
					numDoors = 2;
				else
					if (dim <= 50)
						numDoors = 3;
					else
						numDoors = 4;

			for (int j = 0; j < numDoors; ++j)
			{
				int prevDoorDir = -1;

				bool doorPlaced = false;
				while (!doorPlaced)
				{
					int dir = rand() % 4;
					int doorX = 0;
					int doorY = 0;

					if (dir == prevDoorDir)
							continue;

					switch (dir)
					{
					case 0:
						doorX = RandomOddRange(xPos, xPos + rWidth - 1);
						doorY = yPos - 1;
						if (doorY - 1 > 0 && dungeon[doorY - 1][doorX].GetType() == EMPTY)
						{
							prevDoorDir = dir;
							dungeon[doorY][doorX].SetType(DOOR);
							doorPlaced = true;
						}
						break;
					case 1:
						doorX = RandomOddRange(xPos, xPos + rWidth - 1);
						doorY = yPos + rHeight;
						if (doorY + 1 < height - 1 && dungeon[doorY + 1][doorX].GetType() == EMPTY)
						{
							prevDoorDir = dir;
							dungeon[doorY][doorX].SetType(DOOR);
							doorPlaced = true;
						}
						break;
					case 2:
						doorY = RandomOddRange(yPos, yPos + rHeight - 1);
						doorX = xPos - 1;
						if (doorX - 1 > 0 && dungeon[doorY][doorX - 1].GetType() == EMPTY)
						{
							prevDoorDir = dir;
							dungeon[doorY][doorX].SetType(DOOR);
							doorPlaced = true;
						}
						break;
					case 3:
						doorY = RandomOddRange(yPos, yPos + rHeight - 1);
						doorX = xPos + rWidth;
						if (doorX + 1 < width - 1 && dungeon[doorY][doorX + 1].GetType() == EMPTY)
						{
							prevDoorDir = dir;
							dungeon[doorY][doorX].SetType(DOOR);
							doorPlaced = true;
						}
						break;
					}
				}
			}
		} //END WHILE
	} //END FOR
}

/*
	GenerateDenseRooms
	Steps the the dungeon attempting to build a room every step
	Selects a random position in the dungeon (must be odd)
	Genereates the width and height of the room (must be odd);
	Make sure the room doesn't go off the edge
	Make sure the room doesn't over lap other rooms
	Place the room
	Encircle it with walls
	Generate a door
*/
void Dungeon::GenerateDenseRooms()
{
	for (int y = 1; y < height - 1; y += 2)
	{
		for (int x = 1; x < width - 1; x += 2)
		{
			int numAttempts = 3;
			while (true && numAttempts > 0)
			{
				int xPos = x;
				int yPos = y;
				int rWidth = RandomOddRange(minRoomSize, maxRoomSize);
				int rHeight = RandomOddRange(minRoomSize, maxRoomSize);

				//Make sure we aren't at an edge
				if (xPos - 2 < 0 || xPos + rWidth + 1 > width - 1 || yPos - 2 < 0 || yPos + rHeight + 1 > height - 1)
				{
					numAttempts -= 1;
					continue;
				}

				bool canPlace = true;
				//Check to make sure the room doesn't overlap anything
				for (int yy = yPos - 2; yy < yPos + rHeight + 1; ++yy)
				{
					for (int xx = xPos - 2; xx < xPos + rWidth + 1; ++xx)
					{
						if (dungeon[yy][xx].GetType() != EMPTY)
						{
							canPlace = false;
							break;
						}
					}
					if (!canPlace)
						break;
				}

				if (!canPlace)
				{
					numAttempts -= 1;
					continue;
				}

				//Place the room
				for (int yy = yPos; yy < yPos + rHeight; ++yy)
				{
					for (int xx = xPos; xx < xPos + rWidth; ++xx)
					{
						dungeon[yy][xx].SetType(ROOM);
					}
				}
				Room r = Room(xPos, yPos, rWidth, rHeight);
				rooms.push_back(r);

				//Place Walls
				for (int y = yPos - 1; y < yPos + rHeight + 1; ++y)
				{
					dungeon[y][xPos - 1].SetType(WALL);
					dungeon[y][xPos + rWidth].SetType(WALL);
				}
				for (int x = xPos - 1; x < xPos + rWidth + 1; ++x)
				{
					dungeon[yPos - 1][x].SetType(WALL);
					dungeon[yPos + rHeight][x].SetType(WALL);
				}

				//Place doors
				int numDoors = (rWidth * rHeight) >= 12 ? 2 : 1;
				for (int j = 0; j < numDoors; ++j)
				{
					int prevDoorDir = -1;

					bool doorPlaced = false;
					while (!doorPlaced)
					{
						int dir = rand() % 4;
						int doorX = 0;
						int doorY = 0;

						if (dir == prevDoorDir)
								continue;

						switch (dir)
						{
						case 0:
							doorX = RandomOddRange(xPos, xPos + rWidth - 1);
							doorY = yPos - 1;
							if (doorY - 1 > 0 && dungeon[doorY - 1][doorX].GetType() == EMPTY)
							{
								prevDoorDir = dir;
								dungeon[doorY][doorX].SetType(DOOR);
								doorPlaced = true;
							}
							break;
						case 1:
							doorX = RandomOddRange(xPos, xPos + rWidth - 1);
							doorY = yPos + rHeight;
							if (doorY + 1 < height - 1 && dungeon[doorY + 1][doorX].GetType() == EMPTY)
							{
								prevDoorDir = dir;
								dungeon[doorY][doorX].SetType(DOOR);
								doorPlaced = true;
							}
							break;
						case 2:
							doorY = RandomOddRange(yPos, yPos + rHeight - 1);
							doorX = xPos - 1;
							if (doorX - 1 > 0 && dungeon[doorY][doorX - 1].GetType() == EMPTY)
							{
								prevDoorDir = dir;
								dungeon[doorY][doorX].SetType(DOOR);
								doorPlaced = true;
							}
							break;
						case 3:
							doorY = RandomOddRange(yPos, yPos + rHeight - 1);
							doorX = xPos + rWidth;
							if (doorX + 1 < width - 1 && dungeon[doorY][doorX + 1].GetType() == EMPTY)
							{
								prevDoorDir = dir;
								dungeon[doorY][doorX].SetType(DOOR);
								doorPlaced = true;
							}
							break;
						}
					}
				}
			}
		} //END X FOR
	} //END Y FOR
}

/*
	GenerateCorridors
	Steps through the dungeon on odd steps
	Attemps to open a tunnel at each step
*/
void Dungeon::GenerateCorridors()
{
	for (int y = 1; y < height; y += 2)
	{
		for (int x = 1; x < width; x += 2)
		{
			Tunnel(dungeon, x, y, rand() % 4);
		}
	}
}

/*
	Tunnel()
	Make sure we aren't outside of the dungeon
	Based on the direction, determine we are inside the dungeon and not
	overlapping anything.
	Set the cells the corridor covers to corridors
	Generate a list of directions
	Determine if we should turn (shuffle the list of directions) or go straight
	for each direction
	Attempt to tunnel again
*/
bool Dungeon::Tunnel(Cell** dng, int x, int y, int lastDir)
{
	if (x < 0 || x >= width || y < 0 || y>= height)
		return false;

	switch (lastDir)
	{
	case 0:
		{
			if (y - 2 < 0 || y - 1 < 0)
				return false;
			if (dng[y - 1][x].GetType() != EMPTY && dng[y - 1][x].GetType() != DOOR)
				return false;
			if(dng[y - 2][x].GetType() != EMPTY)
				return false;

			dng[y][x].SetType(CORRIDOR);
			if (dng[y - 1][x].GetType() != DOOR)
				dng[y - 1][x].SetType(CORRIDOR);
			else
				return true;
			dng[y - 2][x].SetType(CORRIDOR);

			std::vector<int> dirs = std::vector<int>();
			dirs = GetDirs(dirs, lastDir);
			int shuffle = rand() % 100;
			if (corridorType == STRAIGHT)
			{
				if (shuffle < 5)
					dirs = ShuffleDirs(dirs);
			}
			else
				if (corridorType == MIXED)
				{
					if (shuffle < 50)
						dirs = ShuffleDirs(dirs);
				}
				else
				{
					dirs = ShuffleDirs(dirs);
				}

			std::vector<int>::iterator itr;
			for (itr = dirs.begin(); itr != dirs.end(); ++itr)
			{
				if (!Tunnel(dng, x, y - 2, *itr))
					continue;
			}
		}
		break;
	case 1:
		{
			if (y + 2 >= height || y + 1 >= height)
				return false;
			if (dng[y + 1][x].GetType() != EMPTY && dng[y + 1][x].GetType() != DOOR)
				return false;
			if(dng[y + 2][x].GetType() != EMPTY)
				return false;

			dng[y][x].SetType(CORRIDOR);
			if (dng[y + 1][x].GetType() != DOOR)
				dng[y + 1][x].SetType(CORRIDOR);
			else
				return true;
			dng[y + 2][x].SetType(CORRIDOR);

			std::vector<int> dirs = std::vector<int>();
			dirs = GetDirs(dirs, lastDir);
			int shuffle = rand() % 100;
			if (corridorType == STRAIGHT)
			{
				if (shuffle < 5)
					dirs = ShuffleDirs(dirs);
			}
			else
				if (corridorType == MIXED)
				{
					if (shuffle < 50)
						dirs = ShuffleDirs(dirs);
				}
				else
				{
					dirs = ShuffleDirs(dirs);
				}

			std::vector<int>::iterator itr;
			for (itr = dirs.begin(); itr != dirs.end(); ++itr)
			{
				if (!Tunnel(dng, x, y + 2, *itr))
					continue;
			}
		}
		break;
	case 2:
		{
			if (x - 2 < 0 || x - 1 < 0)
				return false;
			if (dng[y][x - 1].GetType() != EMPTY && dng[y][x - 1].GetType() != DOOR)
				return false;
			if(dng[y][x - 2].GetType() != EMPTY)
				return false;

			dng[y][x].SetType(CORRIDOR);
			if (dng[y][x - 1] .GetType() != DOOR)
				dng[y][x - 1].SetType(CORRIDOR);
			else
				return true;
			dng[y][x - 2].SetType(CORRIDOR);

			std::vector<int> dirs = std::vector<int>();
			dirs = GetDirs(dirs, lastDir);
			int shuffle = rand() % 100;
			if (corridorType == STRAIGHT)
			{
				if (shuffle < 5)
					dirs = ShuffleDirs(dirs);
			}
			else
				if (corridorType == MIXED)
				{
					if (shuffle < 50)
						dirs = ShuffleDirs(dirs);
				}
				else
				{
					dirs = ShuffleDirs(dirs);
				}

			std::vector<int>::iterator itr;
			for (itr = dirs.begin(); itr != dirs.end(); ++itr)
			{
				if (!Tunnel(dng, x - 2, y, *itr))
					continue;
			}
		}
		break;
	case 3:
		{
			if (x + 2 >= width || x + 1 >= width)
				return false;
			if (dng[y][x + 1].GetType() != EMPTY && dng[y][x + 1].GetType() != DOOR)
				return false;
			if(dng[y][x + 2].GetType() != EMPTY)
				return false;

			dng[y][x].SetType(CORRIDOR);
			if (dng[y][x + 1] .GetType() != DOOR)
				dng[y][x + 1].SetType(CORRIDOR);
			else
				return true;
			dng[y][x + 2].SetType(CORRIDOR);

			std::vector<int> dirs = std::vector<int>();
			dirs = GetDirs(dirs, lastDir);
			int shuffle = rand() % 100;
			if (corridorType == STRAIGHT)
			{
				if (shuffle < 5)
					dirs = ShuffleDirs(dirs);
			}
			else
				if (corridorType == MIXED)
				{
					if (shuffle < 50)
						dirs = ShuffleDirs(dirs);
				}
				else
				{
					dirs = ShuffleDirs(dirs);
				}

			std::vector<int>::iterator itr;
			for (itr = dirs.begin(); itr != dirs.end(); ++itr)
			{
				if (!Tunnel(dng, x + 2, y, *itr))
					continue;
			}
		}
		break;
	}

	return true;
}

/*
	Iterates through the dungeon On odd steps
	Attempt to collaps a corridor if we are in one.
*/
void Dungeon::CollapseTunnel()
{
	int numPasses = (deadEndChance < 25 || deadEndChance > 75) ? 1 : 2;
	for (int p = 0; p < numPasses; p++)
	{
		for (int i = 0; i < height / 2; ++i)
		{
			int y = (i * 2) + 1;
			for (int j = 0; j < width / 2; ++j)
			{
				int x = (j * 2) + 1;

				Collapse(y, x);
			}
		}
	}
}

/*
	Collapse()
	Determine if we should try and collapse this tunnel
	Check to see if we can collapse this cell we are at.
	Collapse the cell if we can
	Step to the next open cell.
*/
void Dungeon::Collapse(int y, int x)
{
	int yy = y;
	int xx = x;
	int dir = -1;

	while(true)
	{
		if (rand() % 100 >= deadEndChance)
			break;

		if (CheckDeadEnd(yy, xx, dir))
			dungeon[yy][xx].SetType(WALL);
		else
			break;

		switch (dir)
		{
		case 0:
			yy -= 1;
			break;
		case 1:
			yy += 1;
			break;
		case 2:
			xx -= 1;
			break;
		case 3:
			xx += 1;
			break;
		}

		dir = -1;
	}
}

/*
	CheckDeadEnd
	Determine how many walls this cell has
	If the cell has 3 walls it is a dead end so return true
*/
bool Dungeon::CheckDeadEnd(int y, int x, int &dir)
{
	int numWalls = 0;

	if (y - 1 >= 0)
	{
		if (dungeon[y - 1][x].GetType() == WALL)
			numWalls += 1;
		else
		{
			if (dir == -1)
				dir = 0;
		}
	}
	if (y + 1 < height)
	{
		if (dungeon[y + 1][x].GetType() == WALL)
			numWalls += 1;
		else
		{
			if (dir == -1)
				dir = 1;
		}
	}
	if (x - 1 >= 0)
	{
		if (dungeon[y][x - 1].GetType() == WALL)
			numWalls += 1;
		else
		{
			if (dir == -1)
				dir = 2;
		}
	}
	if (x + 1 < width)
	{
		if (dungeon[y][x + 1].GetType() == WALL)
			numWalls += 1;
		else
		{
			if (dir == -1)
				dir = 3;
		}
	}

	if (numWalls == 3)
		return true;

	return false;

}

/*
	WallUpDungeon()

	Iterate through the dungeon
	Change the cell to a wall if the cell is empty
*/
void Dungeon::WallUpDungeon()
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (dungeon[i][j].GetType() == EMPTY)
				dungeon[i][j].SetType(WALL);
		}
	}
}


/*
*	ApplyMask
*
*	Masks out cells, marking them impassible based on the mask shape.
*/
void Dungeon::ApplyMask()
{
	switch (dungeonMask)
	{
	case SQUARE:
		break;
	case CIRCLE:
		ApplyCircleMask();
		break;
	case CASTLE:
		ApplyCastleMask();
		break;
	case TRIANGLE:
		ApplyTriangleMask();
		break;
	case CROSS:
		ApplyCrossMask();
		break;
	}
}

void Dungeon::ApplyCircleMask()
{
	int r = (width < height ? width : height) / 2;
	int rSq = r * r;
	int cX = width / 2;
	int cY = height / 2;

	for (int y = 0; y < width; ++y)
	{
		for (int x = 0; x < height; ++x)
		{
			if ((x - cX) * (x - cX) + (y - cY) * (y - cY) > rSq)
			{
				dungeon[y][x].SetType(IMPASSABLE);
			}
		}
	}
}

void Dungeon::ApplyCastleMask()
{
	//Fill with all impassables
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
			dungeon[y][x].SetType(IMPASSABLE);

	int cornerW = (width - 2) / 3;
	int midW = (width - 2) - (2 * cornerW);

	int cornerH = (height - 2) / 3;
	int midH = (height - 2) - (2 * cornerH);

	//Top Left Corner
	for (int y = 1; y < 1 + cornerH; ++y)
		for (int x = 1; x < 1 + cornerW; ++x)
			dungeon[y][x].SetType(EMPTY);

	//Top Right Corner
	for (int y = 1; y < 1 + cornerH; ++y)
		for (int x = (width - 1) - cornerW; x < (width - 1); ++x)
			dungeon[y][x].SetType(EMPTY);

	//Bottom Left Corner
	for (int y = (height - 1) - cornerH; y < (height - 1); ++y)
		for (int x = 1; x < 1 + cornerW; ++x)
			dungeon[y][x].SetType(EMPTY);

	//Bottom Right Corner
	for (int y = (height - 1) - cornerH; y < (height - 1); ++y)
		for (int x = (width - 1) - cornerW; x < (width - 1); ++x)
			dungeon[y][x].SetType(EMPTY);

	//Vert Line
	for (int y = 1 + cornerH / 2; y < height - 1 - cornerH / 2; ++y)
		for (int x = 1 + cornerW; x < 1 + cornerW + midW; ++x)
			dungeon[y][x].SetType(EMPTY);

	//Hor Line
	for (int y = 1 + cornerH; y < 1 + cornerH + midH; ++y)
		for (int x = 1 + cornerW / 2; x < width - 1 - cornerW / 2; ++x)
			dungeon[y][x].SetType(EMPTY);
}

void Dungeon::ApplyTriangleMask()
{
	int aX = width / 2;
	int aY = 0;
	int bX = width - 1;
	int bY = height - 1;
	int cX = 0;
	int cY = height - 1;

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int asX = x - aX;
			int asY = y - aY;

			bool s_ab = (bX - aX) * asY - (bY - aY) * asX > 0;

			if ((cX - aX) * asY - (cY - aY) * asX > 0 == s_ab)
			{
				dungeon[y][x].SetType(IMPASSABLE);
				continue;
			}

			if ((cX - bX) * (y - bX) - (cY - bY) * (x - bX) > 0 != s_ab)
			{
				dungeon[y][x].SetType(IMPASSABLE);
				continue;
			}
		}
	}
}

void Dungeon::ApplyCrossMask()
{
	int thirdWidth = width / 3;
	int thirdHeight = thirdHeight / 3;

	//Top Left
	for (int y = 0; y < thirdHeight; ++y)
	{
		for (int x = 0; x < thirdWidth; ++x)
		{
			dungeon[y][x].SetType(IMPASSABLE);
		}
	}

	//TopRight
	for (int y = 0; y < thirdHeight; ++y)
	{
		for (int x = 2 * thirdWidth; x < width; ++x)
		{
			dungeon[y][x].SetType(IMPASSABLE);
		}
	}

	//BottomLeft
	for (int y = 2 * thirdHeight; y < height; ++y)
	{
		for (int x = 0; x < thirdWidth; ++x)
		{
			dungeon[y][x].SetType(IMPASSABLE);
		}
	}
	
	//Bottom Right
	for (int y = 2 * thirdHeight; y < height; ++y)
	{
		for (int x = 2 * thirdWidth; x < width; ++x)
		{
			dungeon[y][x].SetType(IMPASSABLE);
		}
	}
}