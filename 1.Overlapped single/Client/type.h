#pragma once
typedef struct move_pakcet {
	short size;
	char type;
	int x;
	int y;
}MOVEPACKET;

enum class MOVETYPE
{
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT
};
