#ifndef CChunkSaveComponent_Header
#define CChunkSaveComponent_Header
#include <fstream>
#include <memory>
#include <array>
#include <vector>

class CChunkSaveComponent : public IChunkComponent
{
			class CChunk* m_parent = nullptr;
			void old_save();
			bool old_load();

			void new_save();
			bool new_load();

			std::tuple<int, int, int> m_megaChunkPos, m_localChunkPos;
			std::string getChunkPath(const std::string& filePath) const;
			bool m_dirty = false;
			void InitPositions();
			bool m_isInit = false;
public:
			std::vector<char> SerializeChunk() const;
			void DeserializeChunk(const std::vector<char>& serializedChunk);

			CChunkSaveComponent(class CChunk* parent);
			const std::tuple<int, int, int>& GetMegaChunkPos() const;
			const std::tuple<int, int, int>& GetLocalChunkPos() const;
			int GetLocalChunkIndex() const;
			CChunk *getParent() const;

			void MakeDirty() override;
			bool IsDirty() const override;

			void save();
			bool load();
};

#endif