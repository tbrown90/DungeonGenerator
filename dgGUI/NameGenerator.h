#pragma once

#include <vector>

class NameGenerator
{
public:
	NameGenerator(void);
	~NameGenerator(void);

	std::vector<std::string> places;
	std::vector<std::string> descriptors;
	std::string  name;

	int numPlaces;
	int numDescriptors;

	std::string GenerateNewName();
	void ReadPlaces(char* placeFile);
	void ReadDescriptions(char* descFile);
};
