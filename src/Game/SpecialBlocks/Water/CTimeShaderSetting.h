#ifndef CCTimeShaderSetting_Header
#define CTimeShaderSetting_Header
#include "CShader.h"

class CTimeShaderSetting : public CShaderSettings
{
    public:
        virtual void UpdateUniforms(class CCamera* camera) override;
};

#endif