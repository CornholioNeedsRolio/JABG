#include "CTimeShaderSetting.h"
#include "Engine/CGame.h"
#include "Engine/CCamera.h"

void CTimeShaderSetting::UpdateUniforms(class CCamera* camera)
{
    GetParent()->SetUniform(CGame::getInstance()->getPassedTime(), "Time");
    GetParent()->SetUniform(camera->GetGlobalPosition(), "CameraPos");
}