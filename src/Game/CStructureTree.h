#ifndef CStructureTree_Header
#define CStructureTree_Header
/*^ thats right I changed the convention. Feeling cute, might not change everything to fit it OwO*/

#include "CStructure.h"

class CStructureTree : public CStructure
{
public:
	bool shouldBeHere(int x, int z) override;
	void getInfo(int x, int y, int z, StructureInfo& info) override;
};

#endif