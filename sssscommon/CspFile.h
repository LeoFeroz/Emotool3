#ifndef __CSPFILE_H__
#define __CSPFILE_H__

#include <stdint.h>

#include <vector>

#include "BaseFile.h"

// "#CSP"
#define CSP_SIGNATURE	0x50534323

typedef struct
{
	uint32_t signature; // 0
	uint16_t endianess_check; // 4
	uint16_t unk_06; // 6
	uint32_t num_entries; // 8  number of (playable) models
	uint32_t unk_0C; // 0x0C 0, probably padding
	// 0x10
} __attribute__((packed)) CSPHeader;

typedef struct
{
	uint32_t cms_entry; // 0
	uint32_t cms_model_spec_idx; // 4
	float unk_08; // 8
	float unk_0C; // C
	float unk_10; // 0x10
	uint32_t unk_14; // 0x14
	float unk_18; // 0x18
	float unk_1C; // 0x1C
	float unk_20; // 0x20
	uint32_t unk_24; // 0x24   
	uint32_t unk_28; // 0x28 seems to be always 0
	uint32_t select_phrase_id; // 0x2C 
	uint32_t unk_30; // 0x30
	uint32_t unk_34; // 0x34;
	uint32_t unk_38; // 0x38  -1
	uint32_t unk_3C; // 0x3C  -1
	// 0x40	
} __attribute__((packed)) CSPEntry;

class CspEntry
{
public:

	uint32_t cms_entry;
	uint32_t cms_model_spec_idx; 
	float unk_08; 
	float unk_0C;
	float unk_10; 
	uint32_t unk_14;
	float unk_18; 
	float unk_1C; 
	float unk_20;
	uint32_t unk_24; 
	uint32_t unk_28; 
	uint32_t select_phrase_id; 
	uint32_t unk_30; 
	uint32_t unk_34; 
	uint32_t unk_38; 
	uint32_t unk_3C; 
};

class CspFile : public BaseFile
{
	
private:

	uint8_t *buf;
	unsigned int size;
	
	CSPEntry *entries; // not-allocated	(ptr in buf)	
	uint32_t num_entries;
	
	void Reset();	
	
	void ToNativeEntry(CSPEntry *foreign, CspEntry *native);
	void ToForeignEntry(CspEntry *native, CSPEntry *foreign);
	
    void WriteEntry(TiXmlElement *root, uint32_t entry) const;
	bool ReadEntry(TiXmlElement *root, CspEntry *entry);
	
	uint32_t AppendDefaultEntry();

public:

	CspFile();
	CspFile(uint8_t *buf, unsigned int size);
	virtual ~CspFile();
	
    virtual bool Load(const uint8_t *buf, unsigned int size) override;
	
	inline uint32_t GetNumEntries() { return num_entries; }
	
	uint32_t FindEntry(uint32_t cms_entry, uint32_t cms_model_spec_idx);
	
	CspEntry *GetEntry(uint32_t id);
	bool SetEntry(uint32_t id, CspEntry *entry);
	
	uint32_t AppendEntry(CspEntry *entry);
	bool DeleteEntry(uint32_t id);
	
    virtual TiXmlDocument *Decompile() const override;
	virtual bool Compile(TiXmlDocument *doc, bool big_endian=false) override;
	
	virtual uint8_t *CreateFile(unsigned int *size) override;
};

#endif

