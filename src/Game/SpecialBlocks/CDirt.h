#ifndef CDirt_Header
#define CDirt_Header
#include "../CBlock.h"

class CDirt : public CBlock
{
    std::array<unsigned short, 4> m_abovegrassfaces;
public:
    CDirt();
    void SetGrassFace(uint8_t face, int index) {m_abovegrassfaces[index] = face;};
    virtual uint8_t getFaceAt(CBlock* neighbors[27], uint8_t face) const override;
};

#endif