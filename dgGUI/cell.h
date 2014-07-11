#ifndef _CELL
#define _CELL

enum CellType
{
	EMPTY = 0x00000000,
	IMPASSABLE = 0x00000001,
	ROOM = 0x00000002,
	CORRIDOR = 0x00000004,
	WALL = 0x00000010,
	DOOR = 0x00000020,

	STAIRS_UP = 0x00400000,
	STAIRS_DN = 0x00800000,

	OPEN_SPACE = ROOM | CORRIDOR,
	BLOCK_ROOM = IMPASSABLE | ROOM,
	BLOCK_CORR = IMPASSABLE | WALL | CORRIDOR,
	BLOCK_DOOR = IMPASSABLE | DOOR,
	STAIRS = STAIRS_UP | STAIRS_DN,

};

class Cell
{
private:
	int X;
	int Y;

	CellType type;

public:
	int GetX() { return X; }
	int GetY() { return Y; }

	CellType GetType() { return type; }
	void	 SetType(CellType t) { type = t; }

	Cell();
	Cell(int, int);
	Cell(int, int, CellType);
	~Cell();

};

#endif