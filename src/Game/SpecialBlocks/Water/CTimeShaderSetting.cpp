#include "CTimeShaderSetting.h"
#include "CGame.h"
#include "CCamera.h"

void CTimeShaderSetting::UpdateUniforms(class CCamera* camera)
{
    GetParent()->SetUniform(CGame::getInstance()->getPassedTime(), "Time");
    GetParent()->SetUniform(camera->GetGlobalPosition(), "CameraPos");
}