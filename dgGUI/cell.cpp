#include "stdafx.h"
#include "cell.h"

/*
	CTor
*/
Cell::Cell()
{

}

/*
	CTor
*/
Cell::Cell(int x, int y)
{
	X = x;
	Y = y;

	type = EMPTY;
}

/*
	CTor
*/
Cell::Cell(int x, int y, CellType t)
{
	X = x;
	Y = y;
	type = t;
}

/*
	DTor
*/
Cell::~Cell() {}
