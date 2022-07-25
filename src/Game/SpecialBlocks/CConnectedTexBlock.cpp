#include "CConnectedTexBlock.h"

CConnectedTexBlock::CConnectedTexBlock() : CBlock()
{

}

uint8_t CConnectedTexBlock::getFaceAt(CBlock* neighbors[27], uint8_t face) const
{
    CBlock* self = (CBlock*)this;
    bool top = neighbors[16] == self, bot = neighbors[10] == self, left = neighbors[12] == self, right = neighbors[14] == self, front = neighbors[22] == self, back = neighbors[4] == self;
    bool N = false, W  = false, S  = false, E = false;
    switch(face) {
        case CBlock::BLOCK_FRONT:
            S = bot; N = top;
            W = right; E = left;
            break;
        case CBlock::BLOCK_BACK:
            S = bot; N = top;
            W = left; E = right;
            break;
        case CBlock::BLOCK_LEFT:
            S = bot; N = top;
            W = front; E = back;
            break;
        case CBlock::BLOCK_RIGHT:
            S = bot; N = top;
            W = back; E = front;
            break;
        case CBlock::BLOCK_TOP:
            S = front; N = back;
            W = right; E = left;
            break;
        case CBlock::BLOCK_BOTTOM:
            S = front; N = back;
            W = left; E = right;
            break;
    }

    return GetFaceIndex(N, W, S, E);
}

uint16_t CConnectedTexBlock::GetFaceIndex(bool N, bool W, bool S, bool E) const
{
    return ConnectedTextures[N  |  (W << 1) | (S<<2) | (E<<3)];
}

void CConnectedTexBlock::setFaceAt(int pos, int face) {
    ConnectedTextures[pos] = face;
}
