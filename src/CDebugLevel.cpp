#include "CDebugLevel.h"
#include "Engine/CTextureAtlas.h"
#include "Game/CBlock.h"
#include "Engine/CFileManager.h"
#include "glm/gtx/string_cast.hpp"
#include "Engine/CFrustumCollider.h"

CDebugLevel::CDebugLevel() : m_width(1920), m_height(1080)
{
	
}

void CDebugLevel::Run(CInputManager& _inputmanager, const float& deltaTime)
{
	m_pauseMenu.Tick(&_inputmanager);
	if(!m_pauseMenu.gamePaused())
	{
		m_player->Tick(_inputmanager, deltaTime);
		m_world->Tick(_inputmanager, deltaTime);
	}

	for (int i = m_fpscounter.size() - 2; i >= 0; --i)
		m_fpscounter[i + 1] = m_fpscounter[i];

	m_fpscounter[0] = 1.f / deltaTime;
	int sum = 0;
	std::string playerPos = glm::to_string(m_player->GetGlobalPosition());
	playerPos.erase(0, 4);
	playerPos = "Player Pos: " + playerPos;
	std::string chunkPos = glm::to_string(glm::ivec3(glm::floor(m_player->GetGlobalPosition() / 16.f)));
	chunkPos.erase(0, 5);
	chunkPos = "Chunk Pos:" + chunkPos;
	for (size_t i = 0; i != m_fpscounter.size(); ++i)
		sum += m_fpscounter[i];
	m_text.setString("FPS: "
	+ std::to_string(sum/ m_fpscounter.size()) + '\n'
	+ playerPos + "\n"
	+ chunkPos);
}

void CDebugLevel::Draw()
{
	CCamera& drawCamera = m_player->getCamera();
	SDrawInfo ortho = m_info;
	ortho.camera = &m_ortho;
	ortho.testlight = nullptr;

	glViewport(0, 0, m_width, m_height);
	
	m_renderer.setActiveCamera(&drawCamera);
	
	m_world->BulkDraw(&m_renderer);
	m_player->BulkDraw(&m_renderer);
	m_skybox.Draw(&m_renderer);
	m_renderer.RenderAll();

	m_renderer.setActiveCamera(&m_ortho);
	m_text.BulkDraw(&m_renderer);
	m_pauseMenu.BulkDraw(&m_renderer, m_width, m_height);
	if(m_holdingBlock.isInit())
		m_holdingBlock.BulkDraw(&m_renderer);
	m_crosshair.BulkDraw(&m_renderer);
	glm::vec3 cameraPos = drawCamera.GetGlobalPosition();
	if(m_world->getBlock(cameraPos.x, cameraPos.y, cameraPos.z) == BLOCK_WATER)
	{
		m_waterOverlay.setSize(glm::vec3(m_width, m_height, 1));

		m_waterOverlay.SetPriority(-1);
		m_waterOverlay.setColor({0.4, 0.4, 0.7, 5});
		m_waterOverlay.BulkDraw(&m_renderer);
	}
	m_renderer.RenderAll();

}

void CDebugLevel::Load(CGame* game)
{
	std::shared_ptr<CTextureAtlas> atlas = std::make_shared<CTextureAtlas>();
	atlas->Init("./res/atlas.png", 16);
	BLOCK_DATABASE::Init();
	game->setResolution(m_width, m_height);

	m_player = std::make_unique<CFPPlayer>(glm::vec3(0, 75, 0));
	m_world = std::make_unique<CWorld>(atlas, *m_player.get());

	m_player->setWorld(m_world.get());

	m_text.setFont("./res/Pixolletta8px.ttf", 25);
	m_text.SetPosition({ 0, 0, -5 });
	m_ortho.MakeOrtho(0, m_width, m_height, 0, -100, 100);

	m_info.camera = &m_player->getCamera();
	m_info.camera->makeMesh();
	m_info.mode = NORMAL_DRAW;
	m_info.defaultShader = CFileManager::getDefaultShader();

	m_depthView.InitRect(0,300 , 1, 600, 900, 1);

	m_pauseMenu.Init();
	m_holdingBlock.SetPosition({50, 120, -5});
	m_holdingBlock.setSize({45,45,45});
	m_holdingBlock.SetRotation({10, 45, 180});
	m_player->setHoldingBlockMesh(&m_holdingBlock);
	m_crosshair.Init2DRect(-10, -10, 10, 10);
	m_crosshair.SetTexture(CFileManager::getTexture("./res/crosshair.png"));
	m_crosshair.SetPosition({m_width*0.5f, m_height*0.5f, 2});
	m_skybox.Init(CFileManager::getTexture("./res/skybox.png"));
	m_waterOverlay.Init2DRect(0, 0, 1, 1);
	m_waterOverlay.SetShader(m_world->getWaterShader());
}

void CDebugLevel::UnLoad()
{

}
