#ifndef __BGRFILE_H__
#define __BGRFILE_H__

#include <vector>
#include "BaseFile.h"

#define BGR_SIGNATURE   "#BGR"

#ifdef _MSC_VER
#pragma pack(push,1)
#endif

typedef struct
{
    char signature[4]; // 0
    uint16_t endianess_check; // 4;
    uint16_t unk_06; // 6
    uint32_t num_entries; // 8
    uint32_t unk_0C; // 0xC
    uint32_t data_start; // 0x10
    uint32_t unk_14; // 0x14
} PACKED BGRHeader;

static_assert(sizeof(BGRHeader) == 0x18, "Incorrect structure size.");

typedef struct
{
    uint32_t level; // 0
    uint32_t unk_04; // 4
    uint32_t unk_08; // 8
    uint32_t cms_entry; // 0xC
    uint32_t cms_model_spec_idx; // 0x10
    uint32_t unk_14; // 0x14
    uint32_t unk_18; // 0x18
    uint32_t unk_1C; // 0x1C
    uint32_t unk_20; // 0x20
    uint32_t unk_24; // 0x24
    uint32_t unk_28; // 0x28
    uint32_t unk_2C; // 0x2C
    uint32_t unk_30; // 0x30
    uint32_t unk_34; // 0x34
    uint32_t unk_38; // 0x38
    uint32_t stage; // 0x3C
    uint32_t unk_40; // 0x40
    uint32_t unk_44; // 0x44
} PACKED BGREntry;

static_assert(sizeof(BGREntry) == 0x48, "Incorrect structure size.");

#ifdef _MSC_VER
#pragma pack(pop)
#endif

struct BgrEntry
{
    uint32_t level;
    uint32_t unk_04;
    uint32_t unk_08;
    uint32_t cms_entry;
    uint32_t cms_model_spec_idx;
    uint32_t unk_14;
    uint32_t unk_18;
    uint32_t unk_1C;
    uint32_t unk_20;
    uint32_t unk_24;
    uint32_t unk_28;
    uint32_t unk_2C;
    uint32_t unk_30;
    uint32_t unk_34;
    uint32_t unk_38;
    uint32_t stage;
    uint32_t unk_40;
    uint32_t unk_44;

    void Decompile(TiXmlNode *root) const;
    bool Compile(const TiXmlElement *root);
};

class BgrFile : public BaseFile
{
private:

    std::vector<BgrEntry> entries;

    void Reset();

public:

    BgrFile();
    virtual ~BgrFile();

    virtual bool Load(const uint8_t *buf, unsigned int size) override;
    virtual uint8_t *CreateFile(unsigned int *size) override;

    virtual TiXmlDocument *Decompile() const override;
    virtual bool Compile(TiXmlDocument *doc, bool big_endian=false) override;
};

#endif
