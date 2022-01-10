#include "CDebugLevel.h"
#include "CTextureAtlas.h"
#include "Game/CBlock.h"
#include "CFileManager.h"
#include "glm/gtx/string_cast.hpp"
#include "CFrustumCollider.h"
#include <iostream>

CDebugLevel::CDebugLevel() : m_width(640), m_height(480)
{
	
}

void CDebugLevel::Run(CInputManager& _inputmanager, const float& deltaTime)
{
	m_player->Tick(_inputmanager, deltaTime);
	m_world->Tick(_inputmanager, deltaTime);

	for (int i = m_fpscounter.size() - 2; i >= 0; --i)
		m_fpscounter[i + 1] = m_fpscounter[i];

	m_fpscounter[0] = 1.f / deltaTime;
	int sum = 0;
	for (size_t i = 0; i != m_fpscounter.size(); ++i)
	{
		sum += m_fpscounter[i];
		//std::cout << m_fpscounter[i] << ' ';
	}
	//std::cout << '\n';
	m_text.setString("FPS: " + std::to_string(sum/ m_fpscounter.size()) + '\n' + glm::to_string(m_player->GetGlobalPosition()));
}

void CDebugLevel::Draw()
{
	CCamera& drawCamera = m_player->getCamera();
	SDrawInfo ortho = m_info;
	ortho.camera = &m_ortho;
	ortho.testlight = nullptr;
	

	//m_light.PrepareShadowMaps(m_renderlist);

	//glViewport(0, 0, 1920, 1080);
	/*drawCamera.getCollider()->updatePlanes();
	for (auto& object : m_renderlist)
		object->Draw(m_info);
	m_text.Draw(ortho);*/

	
	//m_testLight.renderToMe(m_renderManager, m_info, drawCamera);
	//drawCamera.getCollider().updatePlanes();
	//m_renderManager.Draw(m_info, m_width, m_height);

	//m_testLight.Draw(m_info);
    //std::cout << 69 << std::endl;
	//std::cout << "cc8" << std::endl;
	m_renderer.setActiveCamera(&drawCamera);
	m_world->BulkDraw(&m_renderer);
	m_player->BulkDraw(&m_renderer);
	m_renderer.RenderAll();
	//m_world->Draw(m_info);
	//m_player->Draw(m_info);

	//drawCamera.Draw(m_info);
	glViewport(0, 0, m_width, m_height);
	m_text.Draw(ortho);
	//ortho.defaultShader = CFileManager::getShader("./res/DefaultShader/DepthDebugView");
	//m_depthView.Draw(ortho);
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

	//m_renderlist.push_back(&*m_world);
	//m_renderlist.push_back(m_player.get());
	m_renderManager.addObjectToRender(m_world.get());
	m_renderManager.addObjectToRender(m_player.get());

	m_text.setFont("./res/EmotionEngine.ttf", 30);
	m_text.SetPosition({ 0, 0, 0 });
	m_ortho.MakeOrtho(0, m_width, m_height, 0, -10, 10);

	m_info.camera = &m_player->getCamera();
	m_info.camera->makeMesh();
	m_info.mode = NORMAL_DRAW;
	m_info.defaultShader = CFileManager::getDefaultShader();

	m_testLight.Init(1024*2, 1024*2);
	m_testLight.setAngle(-45, 0, 0);
	m_info.testlight = &m_testLight;


	m_depthView.InitRect(0,300 , 1, 600, 900, 1);

	//m_depthTexture = std::shared_ptr<CTexture>(&m_testLight.getTexture());
	//m_depthView.SetTexture(m_depthTexture);
}

void CDebugLevel::UnLoad()
{

}
