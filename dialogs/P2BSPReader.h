#pragma once

static constexpr const int LUMP_AMOUNT = 64;

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
    lumpData_t lumps[LUMP_AMOUNT];
};
