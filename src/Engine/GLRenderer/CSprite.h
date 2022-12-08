#ifndef CSPRITE_HEADER
#define CSPRITE_HEADER
#include "Engine/GLRenderer/CMesh.h"
#include "Engine/Objects/CObject.h"

class CSprite : public CMesh
{
public:
	void Init(std::string texture);
	void Init(std::shared_ptr<CTexture> texture);
	void Draw(const SDrawInfo& info) override;
	void setRect(float left, float top, float right, float bottom);
};

#endif
