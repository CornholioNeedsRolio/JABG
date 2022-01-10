#ifndef CFPPLAYER_HEADER
#define CFPPLAYER_HEADER
#include "../Game/CGameEntityCollider.h"
#include "../CObject.h"
#include "../CCamera.h"
#include "../CMesh.h"
#include "../CAABB.h"

class CFPPlayer : public CObject
{
	CCamera m_camera;
	
	void RotateCamera(glm::ivec2 pos, float sensivity, float delta);
	void Move(bool forward, bool back, bool top, bool bottom, bool left, bool right, float delta, float gravity);
	float m_camerayrot;
	float m_sensivity = 0.4;
	float m_speed = 5;
	float m_gravity = -9.81;
	float m_aceel = 0;

	float m_jumpProgress = 0;
	float m_jumpHeight = 1.25;
	float m_jumpDuration = 0.55;
	float m_jumpCoolDown = 0;
	float m_jumpCoolDownMax = 0.15;
	float m_rampUp = 0;

	glm::vec3 m_velocity;
	glm::vec3 m_accel;
	glm::vec3 m_pointing;

	class CWorld* m_world = 0x0;
	CGameEntityCollider m_collider;

	float jumpHyperbola(float value);
	CMesh m_targetBlock;
	CMesh m_debugTarget;

	//FLAGS
	uint8_t m_flags;
	enum
	{
		IS_HOVERING = 1 << 0,
		IS_ONGROUND = 1 << 1,
		IS_JUMPING  = 1 << 2,
		IS_NOCLIPPING = 1 << 3
	};
	bool checkFlag(const uint8_t& flag) const;
	void setFlags(const uint8_t& flag, const bool& set);
public:
	CFPPlayer(SVector3 _pos = SVector3(0), SVector3 _rot = SVector3(0), class CWorld* world = 0x0);

	CCamera& getCamera();
	
	void Tick(CInputManager& _manager, float deltaTime) override;
	void Draw(const SDrawInfo& info) override;
	void BulkDraw(class CBulkRenderer* renderer) override;
	void setWorld(class CWorld* world);

	void Hover(bool value);

	bool isJumping();
	bool canJump();
};

#endif