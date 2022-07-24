#ifndef CConnectedTexBlock_Header_
#define CConnectedTexBlock_Header_
#include <array>
#include <cstdint>

class CConnectedTexBlock : public CBlock
{
    std::array<uint16_t, 14> ConnectedTextures;
public:
    CConnectedTexBlock();
    //virtual uint8_t getFaceAt(CBlock* neighbors[27], uint8_t face) const override;
};

#endif