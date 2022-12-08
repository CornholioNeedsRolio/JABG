#ifndef GAMEIMPORTANTSTUFF_HEADER
#define GAMEIMPORTANTSTUFF_HEADER
#include <cstdint>

using ColliderType = uint8_t;

enum
{
	BLOCKCOLLIDER_NONE,
	BLOCKCOLLIDER_FULL
};

enum
{
	CARD_N,
	CARD_S,
	CARD_W,
	CARD_E,
	CARD_UP,
	CARD_DOWN
};

#endif