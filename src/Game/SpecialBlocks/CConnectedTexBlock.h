#ifndef CConnectedTexBlock_Header_
#define CConnectedTexBlock_Header_
#include <array>
#include <cstdint>
#include "../CBlock.h"

class CConnectedTexBlock : public CBlock
{
    std::array<uint16_t, 16> ConnectedTextures;
    uint16_t GetFaceIndex(bool N, bool S, bool E, bool W) const;
public:
    CConnectedTexBlock();
    void setFaceAt(int pos, int face);
    virtual uint8_t getFaceAt(CBlock* neighbors[27], uint8_t face) const override;
};

#endif