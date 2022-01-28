#ifndef COBJECT_HEADER
#define COBJECT_HEADER
#include <glm/glm.hpp>
#include <memory>
#include "CInputManager.h"

using SVector3 = glm::vec3;
using SMat4 = glm::mat4;

enum
{
	NORMAL_DRAW,
	DEPTHMAP_DRAW
};

struct SDrawInfo
{
	class CCamera* camera;
	short mode;
	std::shared_ptr<class CShader> defaultShader = nullptr;
	class CDirectionalLight* testlight = nullptr;
};

class CObject
{
	SVector3 m_positon;
	SVector3 m_rotation;
	SVector3 m_size;
	CObject* m_attachment = nullptr;
	bool m_ignoreCamera = false;
public:
	CObject(SVector3 _position = SVector3(0), SVector3 _rotation = SVector3(0));
	virtual ~CObject(){};

	virtual void Draw(const SDrawInfo& info) {};//deprecated
	virtual void BulkDraw(class CBulkRenderer* renderer) {};
	virtual void Tick(CInputManager& _manager, float deltaTime) {};

	virtual bool fustumCulling(class CCamera* camera) const { return true; };

	void Attach(CObject* _attachment);
	void Detach();

	const SVector3& GetPosition() const;
	void SetPosition(const SVector3& a);

	SVector3 GetRotation() const;
	void SetRotation(const SVector3& a);

	SVector3 GetGlobalPosition() const;
	void SetGlobalPosition(const SVector3& a);

	SVector3 GetGlobalRotation() const;
	void SetGlobalRotation(const SVector3& a);

	const SVector3& getSize() const;
	void setSize(const SVector3& size);

	void Move(glm::vec3 velocity);
	void setIgnoreCamera(bool value);
	SMat4 getModelMatrix(class CCamera* camera);
};

#endif