#include "CPauseMenu.h"

void CPauseMenu::Init()
{
    m_rect.Init2DRect(0, 0, 1, 1);
    m_text.setFont("./res/Pixolletta8px.ttf", 35);
    m_text.setString("Paused");
    m_text.SetPosition({10, 10, 7});
}

void CPauseMenu::DrawRect(int x1, int y1, int x2, int y2, float depth, glm::vec4 color, CBulkRenderer* renderer)
{
    m_rect.setSize(glm::vec3(x2-x1, y2-y1, 1));
    m_rect.SetPosition(glm::vec3(x1, y1, depth));
    m_rect.setColor(color);
    m_rect.BulkDraw(renderer);
}

void CPauseMenu::Tick(CInputManager* input_manager)
{
    if(input_manager->keyPressed(SDL_SCANCODE_ESCAPE))
    {
        m_paused = !m_paused;
        if(m_paused)
            SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}

void CPauseMenu::BulkDraw(CBulkRenderer* renderer, int width, int height)
{
    if(gamePaused())
    {
        //DrawRect(0, 0, width*0.25f, height, 5, glm::vec4(0.3, 0.55, 0.7, 0.75), renderer);
        DrawRect(0, 0, width*0.25f+5, height*0.15, 6, glm::vec4(0.5, 0.5, 0.5, 1), renderer);
        m_text.BulkDraw(renderer);
        //DrawRect(width*0.3f, 0, width, height, 5, glm::vec4(0.3, 0.55, 0.7, 0.75), renderer);
    }
}

bool CPauseMenu::gamePaused()
{
    return m_paused;
}