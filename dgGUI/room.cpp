#include "stdafx.h"
#include "room.h"


Room::Room()
{
	roomID = nextRoomID;
	nextRoomID += 1;

	startX = startY = roomWidth = roomHeight = 0;
}

Room::Room(int x, int y, int w, int h)
{
	roomID = nextRoomID;
	nextRoomID += 1;

	startX = x;
	startY = y;
	roomWidth = w;
	roomHeight = h;
}

