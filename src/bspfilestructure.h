
#pragma once

static constexpr const int BSP_LUMP_AMOUNT = 64;

#pragma pack( push, 1 )
struct lumpData_t
{
	int fileOffset;
	int fileLength;
	int lumpVersion;
	char lump_IV_CC[4];
};

struct BSPHeaderStruct_t
{
	int id;
	int m_version;
	lumpData_t lumps[BSP_LUMP_AMOUNT];
};
#pragma pack( pop )
