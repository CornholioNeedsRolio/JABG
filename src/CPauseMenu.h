#ifndef CPauseMenu_Header
#define CPauseMenu_Header
#include "BulkRenderer/CBulkRenderer.h"
#include "CDrawableText.h"
#include "CInputManager.h"
#include "CSprite.h"

class CPauseMenu
{
    bool m_paused = false;
    CMesh m_rect;
    CDrawableText m_text;

    void DrawRect(int x1, int y1, int x2, int y2, float depth, glm::vec4 color, CBulkRenderer* renderer);
public:
    void Init();
    void Tick(CInputManager* input_manager);
    void BulkDraw(CBulkRenderer* renderer, int width, int height);
    bool gamePaused();
};

#endif