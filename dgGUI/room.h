#ifndef _ROOM
#define _ROOM

static int nextRoomID = 0;

class Room
{
private:
	int roomID;
	int startX;
	int startY;
	int roomWidth;
	int roomHeight;

public:
	int GetID() { return roomID; }
	int StartX() { return startX; }
	int StartY() { return startY; }
	int RoomWidth() { return roomWidth; }
	int RoomHeight() { return roomHeight; }

	int SetID(int val) { roomID = val; }
	int SetX(int val) { startX = val; }
	int SetY(int val) { startY = val; }
	int SetWidth(int val) { roomWidth = val; }
	int SetHeight(int val) { roomHeight = val; }

	Room();
	Room(int, int, int, int);
	~Room() {}
};

#endif