#ifndef CDEBUGLEVEL_HEADER
#define CDEBUGLEVEL_HEADER
#include "CLevel.h"
#include "Misc/CFPPlayer.h"
#include "Game/CWorld.h"
#include "CDrawableText.h"
#include "CRenderManager.h"
#include "COctree.h"
#include "BulkRenderer/CBulkRenderer.h"
#include "CSkyBox.h"
#include "CDirectionalLight.h"
#include "CPauseMenu.h"
#include <array>

class CDebugLevel : public CLevel
{
	std::unique_ptr<CFPPlayer> m_player;
	std::array<float, 100> m_fpscounter;
	CDrawableText m_text;
	SDrawInfo m_info;
	CPauseMenu m_pauseMenu;

	CCamera m_ortho;
	CBulkRenderer m_renderer;
	CMesh m_depthView;
	CMesh m_holdingBlock;
	CMesh m_crosshair;
	CSkyBox m_skybox;

	std::shared_ptr<CTexture> m_depthTexture = nullptr;

	std::unique_ptr<CWorld> m_world;
	//std::vector<CObject*> m_renderlist;
	CDirectionalLight m_testLight;
	CRenderManager m_renderManager;
	const uint16_t m_width, m_height;
public:
	CDebugLevel();
	void Run(CInputManager& _inputmanager, const float& deltaTime) override;
	void Draw() override;
	void Load(CGame* game) override;
	void UnLoad() override;
};

#endif