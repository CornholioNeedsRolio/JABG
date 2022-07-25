#ifndef CLEVEL_HEADER
#define CLEVEL_HEADER
#include "CInputManager.h"
#include "Engine/CGame.h"

class CLevel
{
public:
	virtual void Run(CInputManager& _inputmanager, const float& deltaTime) = 0;
	virtual void Draw() = 0;
	virtual void Load(CGame* game) = 0;
	virtual void UnLoad() = 0;
};

#endif