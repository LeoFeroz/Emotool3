#ifndef __BASEFILE_H__
#define __BASEFILE_H__

#include <stdint.h>
#include <string>
#include <vector>

#include "Utils.h"
#include "tinyxml/tinyxml.h"

#undef CreateFile

#ifndef PACKED

#ifdef _MSC_VER
#define PACKED
#else
#define PACKED  __attribute__((packed))
#endif

#endif // PACKED

class BaseFile
{
protected:
	
	bool big_endian;
	
    uint64_t val64(uint64_t val) const;
    uint32_t val32(uint32_t val) const;
    uint16_t val16(uint16_t val) const;
    float val_float(float val) const;

    void copy_float(void *x, float val) const;
	
    uint8_t *GetOffsetPtr(const void *base, uint32_t offset, bool native=false) const;
    uint8_t *GetOffsetPtr(const void *base, uint32_t *offsets_table, uint32_t idx, bool native=false) const;
	
	void ModTable(uint32_t *offsets, unsigned int count, int mod_by);	
	unsigned int GetHighestValueIdx(uint32_t *values, unsigned int count, bool native=false);		
	const char *FindString(const char *list, const char *str, unsigned int num_str);	
	
public:
	// Don't make any function abstract, instead let's provide an empty default implementation

	virtual ~BaseFile() { }

    inline bool IsBigEndian() { return big_endian; }
    inline void SetEndianess(bool big_endian) { this->big_endian = big_endian; }
	
    virtual bool Load(const uint8_t *buf, unsigned int size) { UNUSED(buf); UNUSED(size); return false; }
	virtual bool LoadFromFile(const std::string &path, bool show_error=true);
   	
    virtual TiXmlDocument *Decompile() const { return nullptr; }
    virtual bool Compile(TiXmlDocument *doc, bool big_endian=false) { UNUSED(doc); UNUSED(big_endian); return NULL; }
	
	virtual bool DecompileToFile(const std::string &path, bool show_error=true, bool build_path=false);		
	virtual bool CompileFromFile(const std::string &path, bool show_error=true, bool big_endian=false);	
	
    virtual uint8_t *CreateFile(unsigned int *psize) { UNUSED(psize); return NULL; }
	virtual bool SaveToFile(const std::string &path, bool show_error=true, bool build_path=false);
	
	bool SmartLoad(const std::string &path, bool show_error=true, bool xml_big_endian=false);
	bool SmartSave(const std::string &path, bool show_error=true, bool build_path=false);
	
};

#endif
