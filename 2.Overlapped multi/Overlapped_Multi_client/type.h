#pragma once
typedef struct _tagResolution
{
	unsigned int iW;
	unsigned int iH;
}RESOLUTION,*PRESOLUTION;
enum class MOVETYPE
{
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

enum class PACKET_TYPE
{
	START,
	MOVE,
	END,

};
#pragma pack (push,1)
typedef struct move_packet {
	char size;
	char packet_type=static_cast<char>(PACKET_TYPE::MOVE);
	char id;
	char move_type;
	
	int x;
	int y;
}MOVEPACKET;

typedef struct start_packet {
	char size;
	char packet_type = static_cast<char>(PACKET_TYPE::START);
	char id;
}STARTPACKET;

typedef struct end_packet {
	char size=sizeof(end_packet);
	char packet_type = static_cast<char>(PACKET_TYPE::END);
	char id;
}ENDPACKET;

#pragma pack(pop)

