#ifndef CSTRUCTURE_HEADER
#define CSTRUCTURE_HEADER
#include <vector>
#include <tuple>

enum
{
	Structure_Tree,
};

using StructureInfo = std::vector<std::pair<std::tuple<int, int, int>, std::vector<std::pair<std::tuple<int, int, int>, int>>>>;
class CStructure
{
public:
	virtual unsigned char getStructureType() = 0;
	virtual bool shouldBeHere(int x, int y, int z) = 0; /*checks if there should be a structure, global position*/
	virtual void getInfo(int x, int y, int z, StructureInfo& info) = 0;/*makes structure at the certain point*/
};

#endif