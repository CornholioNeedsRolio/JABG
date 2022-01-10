#ifndef CCAMERA_HEADER
#define CCAMERA_HEADER
#include <memory>
#include "CObject.h"
#include "CFrustumCollider.h"

class CCamera : public CObject
{
	SMat4 m_projection;

	class CMesh* m_mesh = nullptr; //fuck circular dependency and unique_ptr, they are too much hassle for some debug shit

	CFrustumCollider m_collider;
	uint8_t flags;
	enum
	{
		IS_ORTHO
	};
	uint8_t is_ortho : 1;
public:
	CCamera(SVector3 _positon = SVector3(0), SVector3 _rotation = SVector3(0));
	void MakePerspective(float fovy, float aspect, float near, float far);
	void MakeOrtho(float left, float right, float bottom, float top, float zNear, float zFar);

	void Draw(const SDrawInfo& info) override;

	SMat4 getProjection() const;
	SMat4 getView() const;
	glm::vec3 getMiddle() const;
	bool isOrtho() const;

	void makeMesh();

	CFrustumCollider& getCollider();

	virtual ~CCamera() override;
};

#endif