#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

#include <string>
#include <sstream>
#include <vector>

#include "tinyxml/tinyxml.h"

#define UNUSED(x) (void)(x)

#ifdef _MSC_VER

#define FUNCNAME    __FUNCSIG__

// WARNING: not safe
#define snprintf    _snprintf
#include "vs/dirent.h"

#define strncasecmp _strnicmp
#define strcasecmp _stricmp

#else

#include <dirent.h>
#define FUNCNAME    __PRETTY_FUNCTION__

#endif



namespace Utils
{
	
    bool FileExists(const std::string & path);
    bool DirExists(const std::string & path);

    size_t GetFileSize(const std::string &path);

    uint8_t *ReadFile(const std::string &path, size_t *psize, bool show_error=true);
    uint8_t *ReadFileFrom(const std::string &path, size_t from, size_t size, bool show_error=true);
	
    size_t WriteFile(const std::string &path, const uint8_t *buf, size_t size, bool show_error=true, bool build_path=false);
    bool WriteFileBool(const std::string &path, const uint8_t *buf, size_t size, bool show_error=true, bool build_path=false);

    bool Mkdir(const std::string &str);

    bool RemoveFile(const std::string &str);
	
    bool CreatePath(const std::string &path, bool last_is_directory=false);
    FILE *fopen_create_path(const std::string &filename, const char *mode);
	
    inline uint32_t DifPointer(const void *ptr1, const void *ptr2) // ptr1-ptr2
	{
		return (uint32_t) ((uint64_t)ptr1 - (uint64_t)ptr2);
	}
	
    void TrimString(std::string &str, bool trim_left=true, bool trim_right=true);
    bool IsEmptyString(const std::string &str);
    
    size_t GetMultipleStrings(const std::string &str, std::vector<std::string> &list);
	std::string ToSingleString(const std::vector<std::string> &list);
	std::string ToSingleString(const std::vector<uint32_t> &list, bool hexadecimal=false);
	std::string ToSingleString(const std::vector<uint16_t> &list, bool hexadecimal=false);
	std::string ToSingleString(const std::vector<uint8_t> &list, bool hexadecimal=false);
	std::string ToSingleString(const std::vector<float> & list);
	
    std::string ToLowerCase(const std::string &str);
    bool BeginsWith(const std::string &str, const std::string &substr, bool case_sensitive=true);
	bool EndsWith(const std::string &str, const std::string &substr, bool case_sensitive=true);	
	
	std::string UnsignedToString(uint32_t value, bool hexadecimal);
	std::string FloatToString(float value);
	
    std::string GUID2String(const uint8_t *guid);
	
	// std::to_string doesn't work in mingw... lets make a replacer.
	template <typename T>
	std::string ToString(T val)
	{
		std::stringstream stream;
		stream << val;
		return stream.str();
	}

    TiXmlElement *FindRoot(TiXmlHandle *handle, const std::string &root_name);
	
    uint32_t GetUnsigned(const std::string &str, uint32_t default_value=0);
    bool GetBoolean(const std::string &str, bool default_value=false);
	
    size_t GetElemCount(const TiXmlElement *root, const char *name);

    void WriteParamString(TiXmlElement *root, const char *name, const std::string &value);
	void WriteParamMultipleStrings(TiXmlElement *root, const char *name, const std::vector<std::string> &values);
	
	void WriteParamUnsigned(TiXmlElement *root, const char *name, uint32_t value, bool hexadecimal=false); 
	void WriteParamMultipleUnsigned(TiXmlElement *root, const char *name, const std::vector<uint32_t> &values, bool hexadecimal=false);
	void WriteParamMultipleUnsigned(TiXmlElement *root, const char *name, const std::vector<uint16_t> &values, bool hexadecimal=false);
	void WriteParamMultipleUnsigned(TiXmlElement *root, const char *name, const std::vector<uint8_t> &values, bool hexadecimal=false);
	
	void WriteParamFloat(TiXmlElement *root, const char *name, float value); 
	void WriteParamMultipleFloats(TiXmlElement *root, const char *name, const std::vector<float> &values);
	
    void WriteParamGUID(TiXmlElement *root, const char *name, const uint8_t *value);
    void WriteParamBlob(TiXmlElement *root, const char *name, const uint8_t *value, size_t size);
	
    bool ReadAttrString(const TiXmlElement *root, const char *name, std::string & value);
    bool ReadAttrUnsigned(const TiXmlElement *root,  const char *name, uint32_t *value);
    bool ReadAttrFloat(const TiXmlElement *root,  const char *name, float *value);
	
    bool ReadParamString(const TiXmlElement *root, const char *name, std::string & value);
    bool ReadParamMultipleStrings(const TiXmlElement *root, const char *name, std::vector<std::string> & values);
	
    bool ReadParamUnsigned(const TiXmlElement *root, const char *name, uint32_t *value);

    bool ReadParamMultipleUnsigned(const TiXmlElement *root, const char *name, std::vector<uint32_t> &values);
    bool ReadParamMultipleUnsigned(const TiXmlElement *root, const char *name, std::vector<uint16_t> &values);
    bool ReadParamMultipleUnsigned(const TiXmlElement *root, const char *name, std::vector<uint8_t> &values);
	
    bool ReadParamMultipleUnsigned(const TiXmlElement *root, const char *name, uint32_t *values, size_t count);
    bool ReadParamMultipleUnsigned(const TiXmlElement *root, const char *name, uint16_t *values, size_t count);
    bool ReadParamMultipleUnsigned(const TiXmlElement *root, const char *name, uint8_t *values, size_t count);

    bool ReadParamFloat(const TiXmlElement *root, const char *name, float *value);
    bool ReadParamMultipleFloats(const TiXmlElement *root, const char *name, std::vector<float> &values);
    bool ReadParamMultipleFloats(const TiXmlElement *root, const char *name, float *values, size_t count);
	
    bool ReadParamGUID(const TiXmlElement *root, const char *name, uint8_t *value);
    uint8_t *ReadParamBlob(const TiXmlElement *root, const char *name, size_t *psize);
	
    bool ReadParamUnsignedWithMultipleNames(const TiXmlElement *root, uint32_t *value, const char *name1, const char *name2, const char *name3=nullptr, const char *name4=nullptr, const char *name5=nullptr);
    bool ReadParamFloatWithMultipleNames(const TiXmlElement *root, float *value, const char *name1, const char *name2, const char *name3=nullptr, const char *name4 = nullptr, const char *name5=nullptr);

    bool GetParamString(const TiXmlElement *root, const char *name, std::string & value);
    bool GetParamMultipleStrings(const TiXmlElement *root, const char *name, std::vector<std::string> & values);
	
    bool GetParamUnsigned(const TiXmlElement *root, const char *name, uint32_t *value);

    bool GetParamMultipleUnsigned(const TiXmlElement *root, const char *name, std::vector<uint32_t> &values);
    bool GetParamMultipleUnsigned(const TiXmlElement *root, const char *name, std::vector<uint16_t> &values);
    bool GetParamMultipleUnsigned(const TiXmlElement *root, const char *name, std::vector<uint8_t> &values);
	
    bool GetParamMultipleUnsigned(const TiXmlElement *root, const char *name, uint32_t *values, size_t count);
    bool GetParamMultipleUnsigned(const TiXmlElement *root, const char *name, uint16_t *values, size_t count);
    bool GetParamMultipleUnsigned(const TiXmlElement *root, const char *name, uint8_t *values, size_t count);

    bool GetParamFloat(const TiXmlElement *root, const char *name, float *value);
    bool GetParamMultipleFloats(const TiXmlElement *root, const char *name, std::vector<float> &values);
    bool GetParamMultipleFloats(const TiXmlElement *root, const char *name, float *values, size_t count);
	
    bool GetParamGUID(const TiXmlElement *root, const char *name, uint8_t *value);
    uint8_t *GetParamBlob(const TiXmlElement *root, const char *name, size_t *psize);

    bool GetParamUnsignedWithMultipleNames(const TiXmlElement *root, uint32_t *value, const char *name1, const char *name2, const char *name3=nullptr, const char *name4=nullptr, const char *name5=nullptr);
    bool GetParamFloatWithMultipleNames(const TiXmlElement *root, float *value, const char *name1, const char *name2, const char *name3=nullptr, const char *name4=nullptr, const char *name5=nullptr);

    void WriteComment(TiXmlElement *root, const std::string & comment);
	
    std::string ModelFileName(uint32_t cms_entry, uint32_t cms_model_spec_idx, const char *ext);

    int RandomInt(int min, int max);
    std::string GetRandomString(size_t len);

    std::u16string Utf8ToUcs2(const std::string & utf8);
	std::string Ucs2ToUtf8(const std::u16string & ucs2);
	
    std::string Base64Encode(const uint8_t *buf, size_t size, bool add_new_line);
    uint8_t *Base64Decode(const std::string &data, size_t *ret_size);
	
	std::string GetAppData();
    std::string GetTempFile(const std::string &prefix, const std::string &extension);

    void RunProgram(const std::string &program, const std::vector<std::string> &args);

    class StringFinder
    {
    private:

        std::string str1;

    public:

        StringFinder(const std::string &str) { str1 = str; }
        bool operator()(const std::string &str2) const
        {
            return (strcasecmp(str1.c_str(), str2.c_str()) == 0);
        }
    };
}

#endif
