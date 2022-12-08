#include "CSprite.h"
#include "Engine/CFileManager.h"


void CSprite::Init(std::string texture)
{
	auto temp = CFileManager::getTexture(texture);

	
	Init(temp);
}

void CSprite::Init(std::shared_ptr<CTexture> texture)
{
	SetTexture(texture);
	SetShader(CFileManager::getShader("./res/DefaultShader/DefaultShader"));
	setRect(0, 0, texture->getWidth(), texture->getHeight());
}

void CSprite::Draw(const SDrawInfo& info)
{
	CMesh::Draw(info);
}

void CSprite::setRect(float left, float top, float right, float bottom)
{
	auto temp = GetTexture();
	Clear();

	float width = temp->getWidth();
	float height = temp->getHeight();
	
	float umin = left / width;
	float vmin = top / height;

	float umax = right / width;
	float vmax = bottom / height;
	
	right -= left;
	bottom -= top;
	CMesh::Init(
		{
				{ {0			, bottom , 0},{umin, vmax} },
				{ {right		, bottom	, 0},{umax, vmax} },
				{ {right		, 0		, 0},{umax, vmin} },

				{ {right		, 0		, 0},{umax, vmin } },
				{ {0			, 0		, 0},{umin, vmin} },
				{ {0			, bottom	, 0},{umin, vmax} }
		});

}
