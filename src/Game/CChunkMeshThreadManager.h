#ifndef CCHUNKMESHTHREADMANAGER_HEADER
#define CCHUNKMESHTHREADMANAGER_HEADER
#include <thread>
#include <atomic>
#include <array>
#include <mutex>

class CChunkMeshThreadManager
{
	std::thread m_thread;
	std::atomic_bool m_running;
	class CWorld* m_world = nullptr;
	
	std::tuple<int, int, int> m_playerPos;
	int m_renderDistance;
	class CChunk* m_selectedChunk = nullptr;
	std::array<class CChunk*, 27> m_neighbors = { nullptr };
	std::atomic_char m_flags = 0;
	int m_current_x, m_current_y;
	enum
	{
		SELECTING_SHIT = 0x1,
		BUILDING_MESH = 0x2
	};
public:
	CChunkMeshThreadManager(class CWorld* world);
	~CChunkMeshThreadManager();
	void threadMain();


	void ChooseChunk();
	void setPlayerPos(std::tuple<int, int, int> playerPos, int renderDistance);
};

#endif