#include "StdAfx.h"
#include "NameGenerator.h"
#include <iostream>
#include <fstream>

NameGenerator::NameGenerator(void)
{
	ReadPlaces("places.txt");
	ReadDescriptions("descriptors.txt");
	name = new char[15];
	name = GenerateNewName();
	
}

NameGenerator::~NameGenerator(void)
{
}

std::string NameGenerator::GenerateNewName()
{
	std::string desc = new char[15];
	std::string place = new char[15];

	place = places[rand() % places.size()];
	desc = descriptors[rand() % descriptors.size()];

	name.clear();
	name.append("The ");
	name.append(desc);
	name.append(" ");
	name.append(place);

	return name;
}

void NameGenerator::ReadPlaces(char* placeFile)
{
	std::ifstream myFile;
	myFile.open(placeFile);

	if (myFile.is_open())
	{
		while (myFile.good())
		{
			char* temp = new char[15];
			myFile.getline(temp, 15);
			places.insert(places.end(), std::string(temp));
		}
		myFile.close();
	}
}

void NameGenerator::ReadDescriptions(char *descFile)
{
	std::ifstream myFile;
	myFile.open(descFile);

	if (myFile.is_open())
	{
		while (myFile.good())
		{
			char* temp = new char[15];
			myFile.getline(temp, 15);
			descriptors.insert(descriptors.end(), temp);
		}
		myFile.close();
	}
}
