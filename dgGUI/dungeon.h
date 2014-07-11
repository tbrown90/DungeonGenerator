#ifndef _DUNGEON
#define _DUNGEON

#include "cell.h"
#include "room.h"
#include <vector>

enum DungeonMask
{
	SQUARE = 0,
	CIRCLE = 1,
	CASTLE = 2,
	TRIANGLE = 3,
	CROSS = 4,
};

enum RoomDensity
{
	SPARSE = 0,
	SCATTERED = 1,
	DENSE = 2
};

enum CorridorType
{
	STRAIGHT = 0,
	MIXED = 1,
	LABYRINTH = 2
};

class Dungeon
{
private:
	Cell**		dungeon;
	std::vector<Room>	rooms;

	std::string	dungeonName;

	int			width;
	int			height;
	int			minRoomSize;
	int			maxRoomSize;

	DungeonMask	dungeonMask;
	RoomDensity	roomDensity;
	CorridorType	corridorType;
	int			deadEndChance;

	void		ResetDungeon();
	void		GenerateScatteredRooms();
	void		GenerateSparseRooms();
	void		GenerateDenseRooms();

	void		GenerateCorridors();
	bool		Tunnel(Cell**, int, int, int);
	void		CollapseTunnel();
	void		Collapse(int, int);
	bool		CheckDeadEnd(int, int, int&);
	void		WallUpDungeon();

	void		ApplyMask();
	void		ApplyCircleMask();
	void		ApplyCastleMask();
	void		ApplyTriangleMask();
	void		ApplyCrossMask();

public:
	Cell**		GetDungeon() { return dungeon; }
	void		LoadDungeon(Cell** dng, int w, int h, std::string n);
	std::vector<Room>	GetRooms() { return rooms; }

	int			GetWidth() { return width; }
	void		SetWidth(int w) { width = w; }
	int			GetHeight() { return height; }
	void		SetHeight(int h) { height = h; }
	int			GetMinRoomSize() { return minRoomSize; }
	int			GetMaxRoomSize() { return maxRoomSize; }
	DungeonMask	GetMask() { return dungeonMask; }
	RoomDensity	GetRoomDensity() { return roomDensity; }
	CorridorType GetCorridorType() { return corridorType; }

	std::string GetName() { return dungeonName; }
	void SetName(std::string n) { dungeonName = n; }
	
	void		GenerateDungeon();
	

	void		PrintDungeon();

	Dungeon(int, int, int, int, DungeonMask, RoomDensity, CorridorType, int);
	Dungeon();
	~Dungeon();

	bool dungeonBuilt;
};

#endif