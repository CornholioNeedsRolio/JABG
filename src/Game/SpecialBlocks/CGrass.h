#ifndef CGrass_Header
#define CGrass_Header
#include "../CBlock.h"

class CGrass : public CBlock
{
public:
    CGrass();
    virtual uint8_t getFaceAt(CBlock* neighbors[27], uint8_t face) const override;
};

#endif