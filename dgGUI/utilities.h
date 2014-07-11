#ifndef _UTILITIES
#define _UTILITIES

#include <stdlib.h>
#include <algorithm>

int RandomOddRange(int min, int max)
{
	int d = max - min + 1;
	int r = rand() % d + min;
	if (r % 2 == 0)
		r -= 1;
	return r;
}

std::vector<int> ShuffleDirs(std::vector<int> dirs)
{
	std::random_shuffle(dirs.begin(), dirs.end());

	return dirs;
}

std::vector<int> GetDirs(std::vector<int> dirs, int lastDir)
{
	dirs.clear();

	switch(lastDir)
	{
	case 0:
		dirs.push_back(0);
		dirs.push_back(1);
		dirs.push_back(2);
		dirs.push_back(3);
		break;
	case 1:
		dirs.push_back(1);
		dirs.push_back(2);
		dirs.push_back(3);
		dirs.push_back(0);
		break;
	case 2:
		dirs.push_back(2);
		dirs.push_back(3);
		dirs.push_back(0);
		dirs.push_back(1);
		break;
	case 3:
		dirs.push_back(3);
		dirs.push_back(0);
		dirs.push_back(1);
		dirs.push_back(2);
		break;
	}

	return dirs;
}
#endif