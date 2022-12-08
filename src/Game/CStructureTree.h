#ifndef CStructureTree_Header
#define CStructureTree_Header
/*^ thats right I changed the convention. Feeling cute, might not change everything to fit it OwO*/
/* future self here, wtf possesed me back then*/
#include "CStructure.h"

class CStructureTree : public CStructure
{
public:
	virtual unsigned char getStructureType() override;
	virtual bool shouldBeHere(int x, int y, int z) override;
	virtual void getInfo(int x, int y, int z, StructureInfo& info) override;
};

#endif