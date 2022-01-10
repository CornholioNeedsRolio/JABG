#ifndef CGAME_HEADER
#define CGAME_HEADER
#include "CDisplay.h"
/*
Handles engine specifics, and levels
*/
class CGame
{
	float m_deltaTime = 0;
	unsigned int m_tickNum = 0;
	class CLevel* m_currentLevel = nullptr; //DO NOT USE CREATE A LEVEL HERE WITH NEW
	CDisplay m_display;
	CInputManager m_inputmanager;

	static CGame* m_instance;
public:
	void Init(std::string title = "noname");
	void setResolution(int width, int height);
	void Run();
	void setLevel(class CLevel* level);
	unsigned int getTickNum() const;

	static CGame* getInstance();
};

#endif
