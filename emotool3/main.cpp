#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <string.h>
#include <sys/stat.h>
#include <limits.h>

#ifdef _WIN32
#include <direct.h>
#endif

#include <iostream>
#include <fstream>
#include <algorithm>

#include "EmoFile.h"
#include "EmbFile.h"
#include "EmmFile.h"
#include "EmaFile.h"
#include "2ryFile.h"
#include "Utils.h"

#include "common.h"
#include "debug.h"
#include <direct.h>

// ...




enum class FileType
{
    EMO,
    EMB,
    EMM,
    EMA,
    _2RY,
    SKL,
    DDS,
    UNKNOWN,
};

struct FileTypeAssociation
{
    std::string extension;
    FileType file_type;
    bool is_xml;
};

#ifdef _MSC_VER
#pragma pack(push,1)
#endif

typedef struct
{
    char signature[4]; // 0
    uint32_t header_size; // 4  header size minus 4
    uint32_t flags; // 8
    uint32_t height; // C
    uint32_t width; // 0x10
    uint32_t pitch; // 0x14
    uint32_t depth; // 0x18
    uint32_t mip_map_count; // 0x1C
    uint32_t reserved1[11]; // 0x20
    uint32_t pixel_format_size; // 0x4C  always 0x20
    uint32_t pixel_format_flags; // 0x50
    char pixel_format_fourcc[4]; // 0x54
    uint32_t rgb_bit_count; // 0x58
    uint32_t rbit_mask; // 0x5C
    uint32_t gbit_mask; // 0x60
    uint32_t bbit_mask; // 0x64
    uint32_t abit_mask; // 0x68
    uint32_t caps; // 0x6C
    uint32_t caps2; // 0x70
    uint32_t caps3; // 0x74
    uint32_t caps4; // 0x78
    uint32_t reserved2; // 0x7C
    // size 0x80
} PACKED DDSHeader;

static_assert(sizeof(DDSHeader) == 0x80, "Incorrect structure size.");

#ifdef _MSC_VER
#pragma pack(pop)
#endif

const std::string VERSION_STRING = "0.042";

static const std::vector<FileTypeAssociation> file_associations =
{
    { ".emo", FileType::EMO, false },
    { ".emb", FileType::EMB, false },
    { ".emm", FileType::EMM, false },
    { ".ema", FileType::EMA, false },
    { ".2ry", FileType::_2RY, false },
    { ".skl", FileType::SKL, false },

    { ".emo.xml", FileType::EMO, true },
    { ".emb.xml", FileType::EMB, true },
    { ".emm.xml", FileType::EMM, true },
    { ".ema.xml", FileType::EMA, true },
    { ".2ry.xml", FileType::_2RY, true },
    { ".skl.xml", FileType::SKL, true },

    { ".dds", FileType::DDS, false },
};

FileType get_file_type(const std::string &file_path, bool *is_xml=nullptr)
{
    for (const FileTypeAssociation &fta : file_associations)
    {
        if (Utils::EndsWith(file_path, fta.extension, false))
        {
            if (is_xml)
                *is_xml = fta.is_xml;

            return fta.file_type;
        }
    }

    return FileType::UNKNOWN;
}

bool get_existing_file(const std::string &base_name, std::string &result)
{
    std::string file;

    for (const FileTypeAssociation &fta : file_associations)
    {
        file = base_name + fta.extension;

        if (Utils::FileExists(file))
        {
            result = file;
            return true;
        }
    }

    file = base_name + ".bin";
    if (Utils::FileExists(file))
    {
        result = file;
        return true;
    }

    return false;
}

void xmlize(const std::string &file_path)
{
    EmoFile emo;
    EmbFile emb;
    EmmFile emm;
    _2ryFile _2ry;
    EmaFile ema;
    BaseFile *file;

    bool is_xml;
    FileType type = get_file_type(file_path, &is_xml);

    if (type == FileType::EMO)
    {
        file = &emo;
    }
    else if (type == FileType::EMB)
    {
        file = &emb;
    }
    else if (type == FileType::EMM)
    {
        file = &emm;
    }
    else if (type == FileType::_2RY)
    {
        file = &_2ry;
    }
    else if (type == FileType::EMA)
    {
        file = &ema;
    }
    else
    {
        DPRINTF("Filetype of \"%s\" not supported.\n", file_path.c_str());
        return;
    }

    if (!file->SmartLoad(file_path))
        return;

    std::string output;

    if (is_xml)
    {
        output = file_path.substr(0, file_path.length()-4);
    }
    else
    {
        output = file_path + ".xml";
    }

    file->SmartSave(output);
}

void compare(const std::string &file1_path, const std::string &file2_path)
{
    EmoFile emo1, emo2;
    EmbFile emb1, emb2;
    EmmFile emm1, emm2;
    _2ryFile _2ry1, _2ry2;
    EmaFile ema1, ema2;

    FileType type1, type2;

    type1 = get_file_type(file1_path);
    type2 = get_file_type(file2_path);

    if (type1 != type2)
    {
        UPRINTF("File types are different, so I assume they are different.\n");
        return;
    }

    if (type1 == FileType::EMO)
    {
        if (!emo1.SmartLoad(file1_path))
            return;

        if (!emo2.SmartLoad(file2_path))
            return;

        UPRINTF("Files are %s.\n", (emo1 == emo2) ? "equal" : "different");
    }
    else if (type1 == FileType::EMB)
    {
        if (!emb1.SmartLoad(file1_path))
            return;

        if (!emb2.SmartLoad(file2_path))
            return;

        UPRINTF("Files are %s.\n", (emb1 == emb2) ? "equal" : "different");
    }
    else if (type1 == FileType::EMM)
    {
        if (!emm1.SmartLoad(file1_path))
            return;

        if (!emm2.SmartLoad(file2_path))
            return;

        UPRINTF("Files are %s.\n", (emm1 == emm2) ? "equal" : "different");
    }
    else if (type1 == FileType::_2RY)
    {
        if (!_2ry1.SmartLoad(file1_path))
            return;

        if (!_2ry2.SmartLoad(file2_path))
            return;

        UPRINTF("Files are %s.\n", (_2ry1 == _2ry2) ? "equal" : "different");
    }
    else if (type1 == FileType::EMA)
    {
        if (!ema1.SmartLoad(file1_path))
            return;

        if (!ema2.SmartLoad(file2_path))
            return;

        UPRINTF("Files are %s.\n", (ema1 == ema2) ? "equal" : "different");
    }
    else
    {
        UPRINTF("File types not supported for comparison.\n");
        return;
    }
}

void reverse_endianess(const std::string &file_path)
{
    EmoFile emo;
    EmbFile emb;
    EmmFile emm;
    _2ryFile _2ry;
    EmaFile ema;
    BaseFile *file;

    bool is_xml;
    FileType type = get_file_type(file_path, &is_xml);

    if (is_xml)
    {
        DPRINTF("This function doesn't make any sense for xml files!\n");
        return;
    }

    if (type == FileType::EMO)
    {
        file = &emo;
    }
    else if (type == FileType::EMB)
    {
        file = &emb;
    }
    else if (type == FileType::EMM)
    {
        file = &emm;
    }
    else if (type == FileType::_2RY)
    {
        file = &_2ry;
    }
    else if (type == FileType::EMA)
    {
        file = &ema;
    }
    else
    {
        DPRINTF("Filetype of \"%s\" not supported.\n", file_path.c_str());
        return;
    }

    if (!file->LoadFromFile(file_path))
        return;

    bool big_endian = file->IsBigEndian();
    file->SetEndianess(!big_endian);

    if (!file->SaveToFile(file_path))
        return;

    if (big_endian)
    {
        UPRINTF("File converted from big endian to little endian.\n");
    }
    else
    {
        UPRINTF("File converted from little endian to big endian.\n");
    }
}

void ps3_to_pc(const std::string &file_path)
{
    EmoFile emo;
    EmbFile emb;
    EmmFile emm;
    _2ryFile _2ry;
    EmaFile ema;
    BaseFile *file;

    bool is_xml;
    FileType type = get_file_type(file_path, &is_xml);

    if (is_xml)
    {
        DPRINTF("This function doesn't make any sense for xml files!\n");
        return;
    }

    if (type == FileType::EMO)
    {
        file = &emo;
    }
    else if (type == FileType::EMB)
    {
        file = &emb;
    }
    else if (type == FileType::EMM)
    {
        file = &emm;
    }
    else if (type == FileType::_2RY)
    {
        file = &_2ry;
    }
    else if (type == FileType::EMA)
    {
        file = &ema;
    }
    else
    {
        DPRINTF("Filetype of \"%s\" not supported.\n", file_path.c_str());
        return;
    }

    if (!file->LoadFromFile(file_path))
        return;

    if (!file->IsBigEndian())
    {
        DPRINTF("This file seems to be already in PC format.\n");
        return;
    }

    file->SetEndianess(false);

    if (type == FileType::EMB)
    {
        for (EmbContainedFile &f : emb)
        {
            uint8_t *buf = f.GetData();

            if (buf[0] == '#')
            {
                DPRINTF("This function cannot convert complex emb files, only files with textures.\n");
                return;
            }
            else if (memcmp(buf, "DDS", 3) == 0)
            {
                DPRINTF("Mmmm, this thing had PC textures...\n");
                return;
            }
            else if (f.GetSize() < 128)
                continue;

            uint16_t width = BE16(*(uint16_t *)(buf+0x20));
            uint16_t height = BE16(*(uint16_t *)(buf+0x22));
            uint8_t type = buf[0x18];
            uint8_t mip_maps = buf[0x19];
            const char *format;

            if (type == 0x88 || type == 0xA8)
            {
                format = "DXT5";
            }
            else if (type == 0x86 )
            {
                format = "DXT1";
            }
            else
            {
                DPRINTF("Unknown texture format %x, cannot convert.\n", type);
                return;
            }

            assert(sizeof(DDSHeader) == 0x80);

            DDSHeader *hdr = (DDSHeader *)buf;

            memset(hdr, 0, sizeof(DDSHeader));
            memcpy(hdr->signature, "DDS ", 4);
            hdr->header_size = sizeof(DDSHeader) - 4;

            if (mip_maps > 1)
            {
                hdr->flags = 0xA1007;
                hdr->mip_map_count = mip_maps;
                hdr->caps = 0x401008;
            }
            else
            {
                hdr->flags = 0x81007;
                hdr->caps = 0x1000;
            }

            hdr->height = height;
            hdr->width = width;
            hdr->pitch = ((width + 3)/4) * ((height + 3)/4) * ((strcmp(format, "DXT1") == 0) ? 8 : 16);
            hdr->pixel_format_size = 0x20;
            hdr->pixel_format_flags = 4;
            memcpy(hdr->pixel_format_fourcc, format, 4);

            uint8_t *last = buf + f.GetSize() - 0x10;
            bool all_EE = true;

            for (int i = 0; i < 0x10; i++)
            {
                if (last[i] != 0xEE)
                {
                    all_EE = false;
                    break;
                }
            }

            if (all_EE)
            {
                f.SetSize(f.GetSize()-0x10);
            }
        }
    }

    if (!file->SaveToFile(file_path))
        return;

    UPRINTF("File succesfully converted to PC format.\n");
}

void show_parts_info(const std::string &emo_path)
{
    EmoFile emo;

    if (!emo.SmartLoad(emo_path))
        return;

    for (size_t i = 0; i < emo.GetNumGroups(); i++)
    {
        UPRINTF("(%x) Absolute offset: %08x, name=%s\n", i, emo[i].GetOriginalOffset(), emo[i].GetName().c_str());
    }
}

void show_skeleton_info(const std::string &skl_path)
{
    EmoFile emo;
    EmaFile ema;
    SkeletonFile skl;

    const SkeletonFile *pskl;
    FileType type = get_file_type(skl_path);

    if (type == FileType::EMO)
    {
        if (!emo.SmartLoad(skl_path))
            return;

        pskl = &emo;
    }
    else if (type == FileType::EMA)
    {
        if (!ema.SmartLoad(skl_path))
            return;

        pskl = &ema;
    }
    else if (type == FileType::SKL)
    {
        if (!skl.SmartLoad(skl_path))
            return;

        pskl = &skl;
    }
    else
    {
        DPRINTF("File type not recognized: %s\n", skl_path.c_str());
        return;
    }

    for (size_t i = 0; i < pskl->GetNumBones(); i++)
    {
        const Bone &bone = (*pskl)[i];
        const Bone *parent = bone.GetParent();
        const Bone *child1 = bone.GetChild1();
        const Bone *child2 = bone.GetChild2();
        const Bone *child3 = bone.GetChild3();
        const Bone *child4 = bone.GetChild4();

        UPRINTF("(%x) name: %s, Absolute offset: %08x\n", i, bone.GetName().c_str(), bone.GetOriginalOffset());
        UPRINTF("parent: %s, child1: %s, child2: %s child3: %s, child4: %s\n\n", (parent) ? parent->GetName().c_str() : "NULL",
                (child1) ? child1->GetName().c_str() : "NULL", (child2) ? child2->GetName().c_str() : "NULL",
                (child3) ? child3->GetName().c_str() : "NULL", (child4) ? child4->GetName().c_str() : "NULL");
    }
}

bool remove_part(EmoFile &emo, const std::string &part_name)
{
    const char *type;

    if (emo.GroupExists(part_name))
    {
        emo.RemoveGroup(part_name);
        type = "Parts group";
    }
    else if (emo.PartExists(part_name))
    {
        emo.RemovePart(part_name);
        type = "Part";
    }
    else if (emo.SubPartExists(part_name))
    {
        emo.RemoveSubPart(part_name);
        type = "Subpart";
    }
    else
    {
        DPRINTF("Neither group, part, or subpart %s exists.\n", part_name.c_str());
        return false;
    }

    UPRINTF("%s \"%s\" was succesfully removed.\n", type, part_name.c_str());
    return true;
}

void remove_part(const std::string &emo_path, const std::string &parts_names)
{
    EmoFile emo;    
    std::vector<std::string> parts;

    if (!emo.SmartLoad(emo_path))
        return;

    Utils::GetMultipleStrings(parts_names, parts);

    for (const std::string &s : parts)
    {
        if (!remove_part(emo, s))
        {
            /*DPRINTF("File was not saved because a part couldn't be found.\n");
            return;*/
        }
    }

    if (!emo.SmartSave(emo_path))
        return;
}

static void export_obj_data(std::ofstream &f, const std::string &vertex, const std::string &uvmap, const std::string &normal, const std::string &topology)
{
    f << vertex << '\n';
    f << uvmap << '\n';
    f << normal << '\n';
    f << topology;
}

void export_obj(const std::string &emo_path)
{
    EmoFile emo;
    std::string directory;
    std::string full_obj_path;
    std::ofstream full_obj;

    std::string vertex, uvmap, normal, topology;

    if (!emo.SmartLoad(emo_path))
        return;

    if (Utils::EndsWith(emo_path, ".emo", false))
    {
        directory = emo_path.substr(0, emo_path.length()-4);
    }
    else if (Utils::EndsWith(emo_path, ".emo.xml", false))
    {
        directory = emo_path.substr(0, emo_path.length()-8);
    }
    else
    {
        DPRINTF("Emo file name should end with .emo or .emo.xml\n");
        return;
    }

    full_obj_path = directory + ".obj";
    directory += "_exported";

    full_obj.open(full_obj_path);
    if (!full_obj.is_open())
    {
        DPRINTF("Cannot open \"%s\" for writing.\n", full_obj_path.c_str());
        return;
    }

    full_obj << "# exported by emotool3 v" << VERSION_STRING << '\n';

    if (!Utils::CreatePath(directory, true))
    {
        DPRINTF("Error creatinng directory \"%s\"\n", directory.c_str());
        return;
    }

    _chdir(directory.c_str());

    size_t vertex_start = 0;

    for (size_t i = 0; i < emo.GetNumGroups(); i++)
    {
        const PartsGroup &pg = emo[i];

        for (size_t j = 0; j < pg.GetNumParts(); j++)
        {
            const EmgFile &p = pg[j];

            for (size_t k = 0; k < p.GetNumSubParts(); k++)
            {
                std::string vertex, uvmap, normal, topology;

                if(p[k].ExportObj(&vertex, &uvmap, &normal, &topology) > 0)
                {
                    std::ofstream file;
                    char filename[PATH_MAX];

                    snprintf(filename, sizeof(filename), "%s_%04x_%04x.obj", pg.GetName().c_str(), j, k);

                    file.open(filename);
                    if (!file.is_open())
                    {
                        DPRINTF("Error opening/creating file %s\n", filename);
                        return;
                    }

                    file << "# exported by emotool3 v" << VERSION_STRING << '\n';
                    export_obj_data(file, vertex, uvmap, normal, topology);
                    file.close();

                    size_t ret = p[k].ExportObj(&vertex, &uvmap, &normal, &topology, vertex_start, false);
                    if (ret > 0)
                    {
                        char sp_name_line[256];

                        snprintf(sp_name_line, sizeof(sp_name_line), "g %s_%04x_%04x\n", pg.GetName().c_str(), j, k);
                        full_obj << sp_name_line;

                        export_obj_data(full_obj, vertex, uvmap, normal, topology);
                        vertex_start += ret;
                    }
                }
            }
        }
    }

    full_obj.close();
}

void inject_obj_dir(const std::string &directory, bool do_uv, bool do_normal)
{
    EmoFile emo;
    std::string emo_path;

    if (!Utils::EndsWith(directory, "_work", false))
    {
        DPRINTF("Directory name must end with \"_work\"\n");
        return;
    }

    if (directory.length() == 5)
    {
        DPRINTF("Directory name must contain something before \"_work\"\n");
        return;
    }

    emo_path = directory.substr(0, directory.length()-5) + ".emo";
    if (!Utils::FileExists(emo_path))
    {
        std::string xml_path = emo_path + ".xml";
        if (!Utils::FileExists(xml_path))
        {
            DPRINTF("Neither \"%s\" or \"%s\" file exist.\n", emo_path.c_str(), xml_path.c_str());
            return;
        }

        emo_path = xml_path;
    }

    if (!emo.SmartLoad(emo_path))
        return;

    DIR *df = opendir(directory.c_str());
    struct dirent *entry;
    int count = 0;

    if (!df)
        goto clean;

    while ((entry = readdir(df)))
    {
        std::string path = directory + '/' + std::string(entry->d_name);

        if (!Utils::FileExists(path))
            continue;

        if (!Utils::EndsWith(path, ".obj", false))
            continue;

        std::string subpart_name = std::string(entry->d_name);
        subpart_name = subpart_name.substr(0, subpart_name.length()-4);

        SubPart *sp = emo.GetSubPart(subpart_name);
        if (!sp)
        {
            DPRINTF("Cannot find subpart \"%s\" in the emo.\n", subpart_name.c_str());
            goto clean;
        }

        size_t size;
        uint8_t *buf = Utils::ReadFile(path, &size);
        if (!buf)
            goto clean;

        if (sp->InjectObj(std::string((char *)buf), do_uv, do_normal))
        {
            count++;
        }
        else
        {
            DPRINTF("Error injecting subpart \"%s\"\n", subpart_name.c_str());
        }

        delete[] buf;
    }

    if (count > 0)
    {
        if (!emo.SmartSave(emo_path))
            goto clean;
    }

    UPRINTF("%d subparts injected.\n", count);

clean:

    if (df)
        closedir(df);
}

void inject_obj_file(const std::string &file, bool do_uv, bool do_normal)
{
    EmoFile emo;
    std::string emo_path;

    if (!Utils::EndsWith(file, ".obj", false))
    {
        DPRINTF("File must have extension \".obj\"\n");
        return;
    }

    emo_path = file.substr(0, file.length()-4) + ".emo";
    if (!Utils::FileExists(emo_path))
    {
        std::string xml_path = emo_path + ".xml";

        if (!Utils::FileExists(xml_path))
        {
            DPRINTF("Neither \"%s\" nor \"%s\" exist.\n", emo_path.c_str(), xml_path.c_str());
            return;
        }

        emo_path = xml_path;
    }

    if (!emo.SmartLoad(emo_path))
        return;

    size_t size;
    uint8_t *buf = Utils::ReadFile(file, &size);
    if (!buf)
    {
        DPRINTF("Cannot open \"%s\" for reading.\n", file.c_str());
        return;
    }

    std::string obj = (char *)buf;
    size_t count;

    if (obj.find("emotool") != std::string::npos || obj.find("3ds Max") != std::string::npos)
    {
        UPRINTF("Using algorithm for 3ds max or emotool2/3 .obj\n");
        count = emo.InjectObjBySubParts(obj, do_uv, do_normal);
    }
    else
    {
        UPRINTF("Using algorithm for blender/miscellaneous .obj\n");
        count = emo.InjectObj(obj, do_uv, do_normal);
    }

    if (count > 0)
    {
        if (!emo.SmartSave(emo_path))
            return;

        UPRINTF("%d subparts were injected.\n", count);
    }
    else
    {
        DPRINTF("Some error happened.\n");
    }
}

bool extract_emb(const EmbFile &emb, const std::string &directory, bool force_index_names, size_t *count)
{
    for (size_t i = 0; i < emb.GetNumFiles(); i++)
    {
        const EmbContainedFile &f = emb[i];
        std::string filepath = directory + '/';

        if (!force_index_names && emb.HasFileNames())
        {
            filepath += f.GetName();
        }
        else
        {
            filepath += EmbFile::CreateFileName(i, f.GetData(), f.GetSize());
        }

        if (f.IsEmb())
        {
            size_t pos = filepath.rfind('.');
            if (pos == std::string::npos)
            {
                DPRINTF("Recursivity error, we were expecting a filename with a dot: %s\n", filepath.c_str());
                return false;
            }

            filepath = filepath.substr(0, pos);
            if (!extract_emb(f, filepath, force_index_names, count))
                return false;
        }
        else
        {
            if (!Utils::WriteFileBool(filepath, f.GetData(), f.GetSize(), true, true))
                return false;

            UPRINTF("%s extracted.\n", filepath.c_str());
            *count = *count + 1;
        }
    }

    return true;
}

void extract_emb(const std::string &emb_path, bool recursive, bool force_index_names)
{
    EmbFile emb(recursive);
    std::string directory;

    if (!emb.SmartLoad(emb_path))
        return;

    if (Utils::EndsWith(emb_path, ".emb", false))
    {
        directory = emb_path.substr(0, emb_path.length()-4);
    }
    else if (Utils::EndsWith(emb_path, ".emb.xml", false))
    {
        directory = emb_path.substr(0, emb_path.length()-8);
    }
    else
    {
        DPRINTF("Emb file name should end with .emb or .emb.xml\n");
        return;
    }

    size_t count = 0;

    if (!extract_emb(emb, directory, force_index_names, &count))
        return;

    UPRINTF("%u files were extracted.\n", count);
}

bool inject_emb(EmbFile &emb, const std::string &directory, bool force_index_names, size_t *num_injected, size_t *num_appended)
{
    size_t i;

    for (i = 0; i < emb.GetNumFiles(); i++)
    {
        std::string filepath = directory + '/';

        if (!force_index_names && emb.HasFileNames())
        {
            filepath += emb[i].GetName();

            if (emb[i].IsEmb())
            {
                size_t pos = filepath.rfind('.');
                if (pos == std::string::npos)
                {
                    DPRINTF("Recursivity error, we were expecting a filename with a dot: %s\n", filepath.c_str());
                    return false;
                }

                filepath = filepath.substr(0, pos);
            }
            else
            {
                if (!Utils::FileExists(filepath))
                    continue;
            }
        }
        else
        {
            filepath += Utils::ToString(i);

            if (!emb[i].IsEmb() && !get_existing_file(filepath, filepath))
                continue;
        }

        if (emb[i].IsEmb())
        {
            EmbFile &this_emb = emb[i];

            if (!inject_emb(this_emb, filepath, force_index_names, num_injected, num_appended))
                return false;

            unsigned int emb_size;
            uint8_t *emb_buf = this_emb.CreateFile(&emb_size);
            if (!emb_buf)
            {
                DPRINTF("Recursivity: CreateFile failed.\n");
                return false;
            }

            emb[i].SetData(emb_buf, emb_size, true);
        }
        else
        {
            size_t size;
            uint8_t *buf = Utils::ReadFile(filepath, &size);
            if (!buf)
            {
                DPRINTF("Error opening file \"%s\"\n", filepath.c_str());
                return false;
            }

            if (emb.ReplaceFile(i, buf, size))
            {
                UPRINTF("%s injected.\n", filepath.c_str());
                *num_injected = *num_injected + 1;
            }
            else
            {
                DPRINTF("%s: ReplaceFile failed.\n", FUNCNAME);
                return false;
            }
        }
    }

    if (!emb.HasFileNames() || force_index_names)
    {
        for (; ; i++)
        {
            std::string filepath = directory + '/';

            if (!get_existing_file(filepath + Utils::ToString(i), filepath))
                break;

            size_t size;
            uint8_t *buf = Utils::ReadFile(filepath, &size);
            if (!buf)
            {
                DPRINTF("Error opening file \"%s\"\n", filepath.c_str());
                return false;
            }

            if (emb.AppendFile(buf, size, "") == (uint16_t)-1)
                return false;

            UPRINTF("%s appended.\n", filepath.c_str());
            *num_appended = *num_appended + 1;
        }
    }

    return true;
}

void inject_emb(const std::string &directory, bool recursive, bool force_index_names)
{
    EmbFile emb(recursive);
    std::string emb_path;

    emb_path = directory + ".emb";
    if (!Utils::FileExists(emb_path))
    {
        std::string xml_path = emb_path + ".xml";
        if (!Utils::FileExists(xml_path))
        {
            DPRINTF("Neither \"%s\" or \"%s\" file exist.\n", emb_path.c_str(), xml_path.c_str());
            return;
        }

        emb_path = xml_path;
    }

    if (!emb.SmartLoad(emb_path))
        return;

    size_t injected = 0, appended = 0;

    if (!inject_emb(emb, directory, force_index_names, &injected, &appended))
        return;

    if (injected > 0 || appended > 0)
    {
        if (!emb.SmartSave(emb_path))
            return;
    }

    UPRINTF("%d files were injected.\n", injected);
    UPRINTF("%d files were appended.\n", appended);
}

static bool load_any(BaseFile &file, const std::string &base, const std::string &extension)
{
    std::string bin = base + extension;
    std::string xml = base + extension + ".xml";

    if (Utils::FileExists(bin))
    {
        if (!file.SmartLoad(bin))
            return false;
    }
    else if (Utils::FileExists(xml))
    {
        if (!file.SmartLoad(xml))
            return false;
    }
    else
    {
        DPRINTF("Cannot find either \"%s\" or \"%s\"\n", bin.c_str(), xml.c_str());
        return false;
    }

    return true;
}

static bool ga_try_to_add(const EmoFile &emo_other, Bone *bone, std::vector<Bone *> &linked_bones, const std::vector<std::string> &bones_to_not_add)
{
    if (!bone)
        return false;

    if (std::find(linked_bones.begin(), linked_bones.end(), bone) != linked_bones.end())
        return false;

    if (std::find(bones_to_not_add.begin(), bones_to_not_add.end(), bone->GetName()) == bones_to_not_add.end())
    {
        UPRINTF("GUESS algorithm added bone \"%s\"\n", bone->GetName().c_str());

        linked_bones.push_back(bone);
        std::sort(linked_bones.begin(), linked_bones.end(), BoneSorter(&emo_other));
    }

    return false;
}

static uint8_t get_edge_texture(const EmoFile &emo)
{
    for (const PartsGroup &pg : emo)
    {
        const std::string name = pg.GetName();
        size_t pos;

        pos = name.find("EDGE_");
        if (pos == std::string::npos)
        {
            pos = name.find("edge");
            if (pos == std::string::npos)
                pos = name.find("modeledge");
        }

        if (pos != std::string::npos)
        {
            std::vector<uint8_t> list;

            if (pg.GetEmbIndexes(list, true, true, false) == 1)
            {
                return list[0];
            }
        }
    }

    return 0xFF;
}

void mix_parts(	const std::string &base,
                const std::string &other,
                const std::string &result,
                const std::vector<std::string> &parts_to_remove,
                const std::vector<std::string> &parts_to_add,
                const std::vector<std::string> &parts_to_rename_from,
                const std::vector<std::string> &parts_to_rename_to,
                std::vector<std::string> &bones_to_not_add,
                const std::vector<std::string> &exclude_bones_parts,
                const std::vector<std::string> &additional_bones,
                std::vector<std::string> &bones_to_not_clone)
{
    EmoFile emo_base, emo_other;
    EmbFile emb_base, emb_other;
    EmmFile emm_base, emm_other;

    SkeletonFile &skl_base = emo_base;
    //const SkeletonFile &skl_other = skl_other;

    const EmoFile &emo_other_ro = emo_other;
    const EmbFile &emb_other_ro = emb_other;
    const EmmFile &emm_other_ro = emm_other;

    std::vector<Bone *> linked_bones, cloned_bones, renamed_bones; // Index are from "other_emo"
    std::vector<std::string> materials_list, materials_to_rename;
    std::vector<uint8_t> textures_from, textures_to;

    uint8_t edge_texture_base = 0xFF, edge_texture_other = 0xFF;

    bool use_guess_algorithm = false;
    bool not_clone_is_same = false;

    if (additional_bones.size() == 1)
    {
        if (additional_bones[0] == "GUESS")
        {
            use_guess_algorithm = true;
        }
    }

    if (bones_to_not_clone.size() == 1)
    {
        if (bones_to_not_clone[0] == "SAME")
        {
            not_clone_is_same = true;
        }
    }

    if (!load_any(emo_base, base, ".emo"))
        return;

    if (!load_any(emo_other, other, ".emo"))
        return;

    if (!load_any(emb_base, base, ".emb"))
        return;

    if (!load_any(emb_other, other, ".emb"))
        return;

    if (!load_any(emm_base, base, ".emm"))
        return;

    if (!load_any(emm_other, other, ".emm"))
        return;

    // Do this now!
    edge_texture_base = get_edge_texture(emo_base);
    edge_texture_other = get_edge_texture(emo_other);

    // Parameters check
    for (const std::string &group : parts_to_remove)
    {
        if (!emo_base.GroupExists(group))
        {
            DPRINTF("Cannot find parts group \"%s\" in \"%s\", which was specified as parameter to remove.\n"
                    "Aborting\n", group.c_str(), base.c_str());
            return;
        }
    }

    for (const std::string &group : parts_to_add)
    {
        if (!emo_other_ro.GroupExists(group))
        {
            DPRINTF("Cannot find parts group \"%s\" in \"%s\", which was specified as parameter to add.\n"
                    "Aborting\n", group.c_str(), other.c_str());
            return;
        }
    }

    for (const std::string &group : parts_to_rename_from)
    {
        if (!emo_base.GroupExists(group))
        {
            DPRINTF("Cannot find parts group \"%s\" in \"%s\" which was specified as parameter to rename.\n",
                    group.c_str(), base.c_str());

            return;
        }
    }

    for (const std::string &group : parts_to_rename_to)
    {
        if (emo_base.GroupExists(group))
        {
            DPRINTF("WARNING: parts group \"%s\" in \"%s\" specified as parameter in \"rename_to\" already exists in the .emo\n"
                    "This will create a .emo with two similar group names, which can have undefined behaviour.\n",
                    group.c_str(), base.c_str());

        }
    }

    if (parts_to_rename_from.size() != parts_to_rename_to.size())
    {
        DPRINTF("Number of parts to rename from must be the same as number of parts to rename to.\n");
        return;
    }

    UPRINTF("\n------ PHASE 1 BEGINS ------\n");

    for (size_t i = 0; i < parts_to_rename_from.size(); i++)
    {
        PartsGroup &pg = emo_base[parts_to_rename_from[i]];

        pg.SetName(parts_to_rename_to[i]);
        UPRINTF("Parts group \"%s\" renamed to \"%s\" (in \"%s\")\n",
                parts_to_rename_from[i].c_str(), parts_to_rename_to[i].c_str(), base.c_str());
    }

    for (const std::string &group : parts_to_remove)
    {
        if (!emo_base.RemoveGroup(group))
        {
            DPRINTF("Failed in removing parts group \"%s\"\n"
                    "Aborting.\n", group.c_str());
            return;
        }

        UPRINTF("Parts group \"%s\" in \"%s\" removed\n", group.c_str(), base.c_str());
    }

    for (const std::string &group : parts_to_add)
    {
        emo_base += emo_other_ro[group];

        if (std::find(exclude_bones_parts.begin(), exclude_bones_parts.end(), group) == exclude_bones_parts.end())
        {
            emo_other.GetLinkedBones(emo_other_ro[group], linked_bones, false, true, true);
        }
        else if (use_guess_algorithm || not_clone_is_same)
        {
            // We need to add these for the algorithm later
            std::vector<Bone *> bones_to_not_add2;

            emo_other.GetLinkedBones(emo_other_ro[group], bones_to_not_add2, true, true, true);

            for (Bone *b : bones_to_not_add2)
            {
                if (std::find(bones_to_not_add.begin(), bones_to_not_add.end(), b->GetName()) == bones_to_not_add.end())
                {
                    if (use_guess_algorithm)
                        UPRINTF("GUESS algorithm, excluding bone \"%s\" from parts group \"%s\"\n",
                                b->GetName().c_str(), group.c_str());

                    bones_to_not_add.push_back(b->GetName());
                }
            }
        }

        UPRINTF("Parts group \"%s\" copied from \"%s\" to \"%s\"\n", group.c_str(), other.c_str(), base.c_str());
    }

    if (not_clone_is_same)
    {
        bones_to_not_clone = bones_to_not_add;
    }

    if (!use_guess_algorithm)
    {
        for (const std::string &bone : additional_bones)
        {
            Bone *b = emo_other.GetBone(bone);

            if (b)
            {
                if (std::find(linked_bones.begin(), linked_bones.end(), b) == linked_bones.end())
                {
                    linked_bones.push_back(b);
                }
            }
        }
    }
    else
    {
        // GUESS algorithm

        bool break_the_loop = false;

        while(!break_the_loop)
        {
            bool repeat = false;

            for (Bone *b : linked_bones)
            {
                if (ga_try_to_add(emo_other_ro, b->GetParent(), linked_bones, bones_to_not_add))
                {
                    repeat = true;
                    break; // break for
                }

                if (ga_try_to_add(emo_other_ro, b->GetChild1(), linked_bones, bones_to_not_add))
                {
                    repeat = true;
                    break; // break for
                }

                if (ga_try_to_add(emo_other_ro, b->GetChild2(), linked_bones, bones_to_not_add))
                {
                    repeat = true;
                    break; // break for
                }

                if (ga_try_to_add(emo_other_ro, b->GetChild3(), linked_bones, bones_to_not_add))
                {
                    repeat = true;
                    break; // break for
                }

                if (ga_try_to_add(emo_other_ro, b->GetChild4(), linked_bones, bones_to_not_add))
                {
                    repeat = true;
                    break; // break for
                }
            }

            if (!repeat)
                break_the_loop = true;
        }
    }

    if (additional_bones.size() > 0)
    {
        std::sort(linked_bones.begin(), linked_bones.end(), BoneSorter(&emo_other));
    }

    while (cloned_bones.size() != linked_bones.size())
    {
        for (Bone *b : linked_bones)
        {
            const std::string bone_name = b->GetName();

            if (std::find(bones_to_not_add.begin(), bones_to_not_add.end(), bone_name) != bones_to_not_add.end())
            {
                renamed_bones.push_back(b);
                continue;
            }

            if (std::find(renamed_bones.begin(), renamed_bones.end(), b) == renamed_bones.end())
            {
                Bone *bb = emo_base.GetBone(bone_name);

                if (bb)
                {
                    std::string new_name = Utils::GetRandomString(bone_name.length());
                    bb->SetName(new_name);

                    UPRINTF("Bone \"%s\" in \"%s\" has been renamed as \"%s\"\n", bone_name.c_str(), base.c_str(), new_name.c_str());
                    renamed_bones.push_back(b);
                }
            }
        }

        for (Bone *b : linked_bones)
        {
            const std::string bone_name = b->GetName();

            if (!emo_base.BoneExists(bone_name))
            {
                if (std::find(bones_to_not_add.begin(), bones_to_not_add.end(), bone_name) == bones_to_not_add.end())
                {
                    skl_base += *b;
                }

                // Must be added here
                if (std::find(renamed_bones.begin(), renamed_bones.end(), b) == renamed_bones.end())
                {
                    renamed_bones.push_back(b);
                }

                UPRINTF("Bone \"%s\" was copied from \"%s\" to \"%s\".\n", bone_name.c_str(), other.c_str(), base.c_str());
            }
        }

        for (Bone *b : linked_bones)
        {
            if (std::find(cloned_bones.begin(), cloned_bones.end(), b) != cloned_bones.end())
                continue;

            const std::string bone_name = b->GetName();
            Bone *not_found;

            // pirpipir
            if (std::find(bones_to_not_clone.begin(), bones_to_not_clone.end(), bone_name) != bones_to_not_clone.end())
            {
                UPRINTF("Not cloning bones of \"%s\"\n", bone_name.c_str());
                cloned_bones.push_back(b);
                continue;
            }

            if (!emo_base.CloneBoneParentChild(emo_other, bone_name, &not_found))
            {
                if (!not_found)
                {
                    DPRINTF("CloneBoneParentChild totally failed, weird (bone \"%s\").\n"
                            "Aborting.\n", bone_name.c_str());
                    return;
                }

                linked_bones.push_back(not_found);
                std::sort(linked_bones.begin(), linked_bones.end(), BoneSorter(&emo_other));
                goto continue_while;
            }
            else
            {
                UPRINTF("Bone \"%s\" parent/child cloned.\n", bone_name.c_str());
                cloned_bones.push_back(b);
            }
        }

continue_while:
        // Stupid language needs a sentence
        int dummy = 0;
        dummy = dummy; // And this to avoid the non used warning -_-'
    }

    for (const std::string &group : parts_to_add)
    {
        Bone *not_found;

        if (emo_base.CloneLinkedBones(emo_other_ro, emo_base[group], &not_found) == (size_t)-1)
        {
            DPRINTF("CloneLinkedBones failed (cannot find bone \"%s\"). On parts group \"%s\"\n"
                    "Aborting.\n", not_found->GetName().c_str(), group.c_str());
            return;
        }

        UPRINTF("CloneLinkedBones of \"%s\" OK.\n", group.c_str());
    }

    UPRINTF("------ PHASE 1 ENDS ------\n");

    // That's was the critical part
    // Now move into materials

    UPRINTF("\n------ PHASE 2 BEGINS ------\n");

    for (const std::string &group : parts_to_add)
    {
        emo_base[group].GetEmmMaterials(materials_list, false, true);
    }

    for (const std::string &material : materials_list)
    {
        const EmmMaterial *bm = emm_base.GetMaterial(material);

        if (bm)
        {
            const EmmMaterial *om = emm_other_ro.GetMaterial(material);
            assert(om != nullptr);

            if (*bm != *om)
            {
                printf("Material params of \"%s\" are different between the two emm, so we will add it with other name.\n",
                       material.c_str());
                materials_to_rename.push_back(material);
            }
        }
    }

    for (const std::string &material : materials_to_rename)
    {
        std::string new_name;

        new_name = material + '_' + other;
        if (new_name.length() > 31)
        {
            new_name = new_name.substr(0, 31);
        }

        EmmMaterial &om = emm_other[material];

        om.SetName(new_name);
        emm_base += om;

        UPRINTF("Added \"%s\" from \"%s\" to \"%s\" (emm), which had been previoulsy renamed.\n",
                new_name.c_str(), other.c_str(), base.c_str());
    }

    for (const std::string &material : materials_list)
    {
        // Only add here the ones that don't exist, as the other have already been processed (by either ignoring because identical or by adding after renaming)
        if (!emm_base.MaterialExists(material))
        {
            emm_base += emm_other_ro[material];

            UPRINTF("Added \"%s\" from \"%s\" to \"%s\" (emm).\n",
                    material.c_str(), other.c_str(), base.c_str());
        }
    }

    for (const std::string &material: materials_to_rename)
    {
        std::string new_name;

        // This must be identical to what we did before!!!!
        new_name = material + '_' + other;
        if (new_name.length() > 31)
        {
            new_name = new_name.substr(0, 31);
        }

        UPRINTF("Renaming \"%s\" to \"%s\" in \"%s\" (in the emo, for newly added parts).\n",
                material.c_str(), new_name.c_str(), base.c_str());

        for (const std::string &group : parts_to_add)
        {
            size_t num = emo_base[group].ReplaceEmmMaterial(material, new_name);
            if (num > 0)
            {
                UPRINTF("(Renamed: %d times for parts group \"%s\")\n", num, group.c_str());
            }
        }
    }

    uint16_t before = emo_base.GetMaterialCount();
    emo_base.SetMaterialCount(emm_base);
    UPRINTF("Material count modded by %d\n", emo_base.GetMaterialCount()-before);

    UPRINTF("------ PHASE 2 ENDS ------\n");

    // Textures times
    UPRINTF("\n------ PHASE 3 BEGINS ------\n");

    if (edge_texture_base == 0xFF)
    {
        /*DPRINTF("Unable to estimate edge texture for \"%s\"\n"
               "Aborting.\n", base.c_str());
        return;*/
        DPRINTF("WARNING: Unable to estimate edge texture for \"%s\"\n"
                "If the .emo didn't have edge, this can be safely ignored.\n", base.c_str());
    }
    else
    {
        UPRINTF("Edge texture of \"%s\" is estimated to be #%d.\n", base.c_str(), edge_texture_base);
    }

    if (edge_texture_other == 0xFF)
    {
        DPRINTF("WARNING: Unable to estimate edge texture for \"%s\"\n"
                "If the .emo didn't have edge, this can be safely ignored.\n", other.c_str());
    }
    else
    {
        UPRINTF("Edge texture of \"%s\" is estimated to be #%d.\n", other.c_str(), edge_texture_other);
    }

    for (const std::string &group : parts_to_add)
    {
        emo_base[group].GetEmbIndexes(textures_from, false, true, true);
    }

    for (size_t i = 0, j = emb_base.GetNumFiles(); i < textures_from.size(); i++)
    {
        if (textures_from[i] == edge_texture_other && edge_texture_base != 0xFF)
        {
            textures_to.push_back(edge_texture_base);
        }
        else
        {
            textures_to.push_back(j++);
        }
    }

    assert (textures_from.size() == textures_to.size());

    for (const std::string &group : parts_to_add)
    {
        for (size_t i = 0; i < textures_from.size(); i++)
        {
            if (textures_from[i] != textures_to[i])
            {
                size_t count = emo_base[group].ReplaceEmbIndex(textures_from[i], textures_to[i]);
                if (count > 0)
                {
                    UPRINTF("Texture #%d mapped to #%d in part \"%s\"\n", textures_from[i], textures_to[i], group.c_str());
                }
            }
        }
    }

    for (uint8_t t : textures_from)
    {
        if (t == edge_texture_other)
            continue;

        uint16_t nt = emb_base.AppendFile(emb_other_ro, t);

        if (nt == 0xFFFF)
        {
            DPRINTF("AppendTex failed, while attempting to add texture #%d from \"%s\" to \"%s\" (emb)\n"
                    "Aborting.\n", t, other.c_str(), base.c_str());
            return;
        }

        UPRINTF("Texture #%d from \"%s\" added as #%d to \"%s\"\n", t, other.c_str(), nt, base.c_str());
    }

    UPRINTF("------ PHASE 3 ENDS ------\n");

    UPRINTF("\nAll operations in RAM on \"%s\" have finished. Now, we'll save the results in \"%s\"\n",
            base.c_str(), result.c_str());

    if (!emo_base.SmartSave(result + ".emo"))
        return;

    if (!emb_base.SmartSave(result + ".emb"))
        return;

    if (!emm_base.SmartSave(result + ".emm"))
        return;

    size_t size;
    uint8_t *buf = Utils::ReadFile(base + ".2ry", &size);
    if (!buf)
        return;

    if (!Utils::WriteFileBool(result + ".2ry", buf, size))
        return;

    UPRINTF("\n***Laralalala, finished, let's hope the model doesn't crash or is a monster.****\n");
}

void mix_parts( const std::string &base_param,
                const std::string &other_param,
                const std::string &result_param,
                const std::string &parts_to_remove_param,
                const std::string &parts_to_add_param,
                const std::string &parts_to_rename_from_param,
                const std::string &parts_to_rename_to_param,
                const std::string &bones_to_not_add_param,
                const std::string &exclude_bones_parts_param,
                const std::string &additional_bones_param,
                const std::string &bones_to_not_clone_param)
{
    std::vector<std::string> parts_to_remove, parts_to_add, parts_to_rename_from, parts_to_rename_to, bones_to_not_add, exclude_bones_parts, additional_bones, bones_to_not_clone;
    std::string temp;

    Utils::GetMultipleStrings(parts_to_remove_param, parts_to_remove);
    Utils::GetMultipleStrings(parts_to_add_param, parts_to_add);
    Utils::GetMultipleStrings(parts_to_rename_from_param, parts_to_rename_from);
    Utils::GetMultipleStrings(parts_to_rename_to_param, parts_to_rename_to);
    Utils::GetMultipleStrings(bones_to_not_add_param, bones_to_not_add);
    Utils::GetMultipleStrings(exclude_bones_parts_param, exclude_bones_parts);
    Utils::GetMultipleStrings(additional_bones_param, additional_bones);
    Utils::GetMultipleStrings(bones_to_not_clone_param, bones_to_not_clone);

    mix_parts(base_param, other_param, result_param, parts_to_remove, parts_to_add, parts_to_rename_from, parts_to_rename_to, bones_to_not_add, exclude_bones_parts, additional_bones, bones_to_not_clone);
}

void replace_face(const std::string &emo_path, uint16_t index_from, uint16_t index_to)
{
    EmoFile emo;

    if (!emo.SmartLoad(emo_path))
        return;

    PartsGroup *face = emo.GetGroup("face");
    PartsGroup *edge = emo.GetGroup("edge");

    if (!face)
    {
        DPRINTF("This emo doesn't have any parts group called \"face\"\n");
        return;
    }

    if (index_from >= face->GetNumParts() || (*face)[index_from].IsEmpty())
    {
        DPRINTF("index_from is bigger than the number of parts in group \"face\"\n");
        return;
    }

    if (index_to >= face->GetNumParts() || (*face)[index_to].IsEmpty())
    {
        DPRINTF("index_to is bigger than the number of parts in group \"face\"\n");
        return;
    }

    (*face)[index_to] = (*face)[index_from];

    if (edge)
    {
        if (edge->GetNumParts() != face->GetNumParts())
        {
            DPRINTF("Uh oh, edge doesn't contain same number of parts than face, I won't continue.\n");
            return;
        }

        (*edge)[index_to] = (*edge)[index_from];
    }

    if (!emo.SmartSave(emo_path))
        return;

    UPRINTF("All went OK!\n");
}

void unstrip_triangles(const std::string &emo_path)
{
    EmoFile emo;

    if (!emo.SmartLoad(emo_path))
        return;

    for (PartsGroup &pg: emo)
    {
        for (EmgFile &p : pg)
        {
            for (SubPart &sp : p)
            {
                if (sp.GetStrips())
                {
                    for (size_t i = 0; i < sp.GetNumSubMeshes(); i++)
                    {
                        sp[i].SetFaces(sp.GetTriangles(i));
                    }

                    sp.SetStrips(false);
                }
            }
        }
    }

    if (!emo.SmartSave(emo_path))
        return;

    UPRINTF("Done.\n");
}

void extract_skeleton(const std::string &skl_path, bool as_xml)
{
    EmoFile emo;
    EmaFile ema;
    SkeletonFile *skl;
    std::string name;

    FileType type = get_file_type(skl_path);

    if (type == FileType::EMO)
    {
        if (!emo.SmartLoad(skl_path))
            return;

        skl = &emo;
        name = skl_path.substr(0, skl_path.rfind(".emo"));
    }
    else if (type == FileType::EMA)
    {
        if (!ema.SmartLoad(skl_path))
            return;

        skl = &ema;
        name = skl_path.substr(0, skl_path.rfind(".ema"));
    }
    else if (type == FileType::SKL)
    {
        DPRINTF("It doesn't make sense to extract skeleton of askeleton file!\n");
        return;
    }
    else
    {
        DPRINTF("File type of \"%s\" not supported for this function.\n", skl_path.c_str());
        return;
    }

    name += ".skl";

    if (as_xml)
    {
        name += ".xml";

        if (!skl->DecompileSkeletonToFile(name))
            return;
    }
    else
    {
        if (!skl->SaveSkeletonToFile(name))
            return;
    }

    UPRINTF("Done.\n");
}

#ifdef FBX_SUPPORT

static FbxManager *create_fbx_manager()
{
    FbxManager *sdk_manager = FbxManager::Create();
    FbxIOSettings *ios = FbxIOSettings::Create(sdk_manager, IOSROOT);
    //ios->SetBoolProp(EXP_FBX_EMBEDDED, true);
    sdk_manager->SetIOSettings(ios);

    return sdk_manager;
}

static bool export_fbx_scene(FbxManager *sdk_manager, FbxScene *scene, const std::string &output, const FbxString &version, bool ascii, bool y_up, bool inches)
{
    int format = sdk_manager->GetIOPluginRegistry()->GetNativeWriterFormat();

    if (ascii)
    {
        int count = sdk_manager->GetIOPluginRegistry()->GetWriterFormatCount();

        for (int i = 0; i < count; i++)
        {
            if (sdk_manager->GetIOPluginRegistry()->WriterIsFBX(i))
            {
                FbxString desc = sdk_manager->GetIOPluginRegistry()->GetWriterFormatDescription(i);
                if (desc.Find("ascii") >= 0)
                {
                    format = i;
                    break;
                }
            }
        }
    }

    FbxExporter* exporter = FbxExporter::Create(sdk_manager, "");
    bool ret = exporter->Initialize(output.c_str(), format, sdk_manager->GetIOSettings());

    if (!ret)
    {
        DPRINTF("FbxExporter::Initialize error:: %s\n\n", exporter->GetStatus().GetErrorString());
        return false;
    }

    if (!y_up)
    {
        scene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::eMax);
    }

    if (inches)
    {
        scene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::Inch);
    }
    else
    {
        scene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::m);
    }

    exporter->SetFileExportVersion(version);
    exporter->Export(scene);
    exporter->Destroy();

    return true;
}

static FbxScene *import_fbx_scene(FbxManager *sdk_manager, const std::string &fbx_path)
{
    FbxImporter* importer = FbxImporter::Create(sdk_manager, "");

    if (!importer->Initialize(fbx_path.c_str(), -1, sdk_manager->GetIOSettings()))
    {
        DPRINTF("FBX: import Initialize failed.\n");
        return nullptr;
    }

    FbxScene* scene = FbxScene::Create(sdk_manager, "EMOTOOL3");
    importer->Import(scene);
    importer->Destroy();

    return scene;
}

static bool get_mode(const std::string &mode, bool &normal_parts, bool &edge_parts)
{
    if (mode.length() == 0)
    {
        normal_parts = edge_parts = true;
    }
    else if (Utils::ToLowerCase(mode) == "all")
    {
        normal_parts = edge_parts = true;
    }
    else if (Utils::ToLowerCase(mode) == "normal")
    {
        normal_parts = true;
        edge_parts = false;
    }
    else if (Utils::ToLowerCase(mode) == "edge")
    {
        normal_parts = false;
        edge_parts = true;
    }
    else
    {
        DPRINTF("Wrong value for mode. I was expecting ALL, NORMAL or EDGES, not \"%s\"\n", mode.c_str());
        return false;
    }

    return true;
}

void export_fbx(const std::string &emo_path, const std::string &param_version, bool ascii, const std::string &up_axis, const std::string &unit, const std::string &mode)
{
    EmoFile emo;
    std::string output, output_dir;
    FbxString version;

    bool y_up, inches;
    bool normal_parts, edge_parts;

    if (param_version == "2010")
    {
        version = FBX_2010_00_COMPATIBLE;
    }
    else if (param_version == "2011")
    {
        version = FBX_2011_00_COMPATIBLE;
    }
    else if (param_version == "2012")
    {
        version = FBX_2012_00_COMPATIBLE;
    }
    else if (param_version == "2013")
    {
        version = FBX_2013_00_COMPATIBLE;
    }
    else if (param_version == "2014")
    {
        version = FBX_2014_00_COMPATIBLE;
    }
    else if (param_version == "2016")
    {
        version = FBX_2016_00_COMPATIBLE;
    }
    else
    {
        DPRINTF("Invalid version \"%s\". Valid values are 2010, 2011, 2012, 2013, 2014 and 2016.\n", param_version.c_str());
        return;
    }

    if (up_axis.length() == 0)
    {
        y_up = false;
    }
    else if (Utils::ToLowerCase(up_axis) == "y")
    {
        y_up = true;
    }
    else if (Utils::ToLowerCase(up_axis) == "z")
    {
        y_up = false;
    }
    else
    {
        DPRINTF("Wrong value for up_axis. I was expecting Y or Z, not \"%s\"\n", up_axis.c_str());
        return;
    }

    if (unit.length() == 0)
    {
        inches = false;
    }
    else if (Utils::ToLowerCase(unit) == "m")
    {
        inches = false;
    }
    else if (Utils::ToLowerCase(unit) == "i")
    {
        inches = true;
    }
    else
    {
        DPRINTF("Wrong value for unit. I was expecting m or i, not \"%s\"\n", unit.c_str());
        return;
    }

    if (!get_mode(mode, normal_parts, edge_parts))
        return;

    if (!emo.SmartLoad(emo_path))
        return;

    if (Utils::EndsWith(emo_path, ".emo", false))
    {
        output = emo_path.substr(0, emo_path.length()-4);
    }
    else if (Utils::EndsWith(emo_path, ".emo.xml", false))
    {
        output = emo_path.substr(0, emo_path.length()-8);
    }
    else
    {
        DPRINTF("Emo file name should end with .emo or .emo.xml\n");
        return;
    }

    output += ".fbx";

    DPRINTF("Exporting full model...\n");

    FbxManager *sdk_manager = create_fbx_manager();
    FbxScene *scene = FbxScene::Create(sdk_manager, "EMOTOOL3");

    if (!emo.ExportFbx(scene, normal_parts, edge_parts))
    {
        DPRINTF("ExportFbx failed.\n");
        return;
    }

    if (!export_fbx_scene(sdk_manager, scene, output, version, ascii, y_up, inches))
        return;

    output_dir = output.substr(0, output.length()-4) + "_exported";
    Utils::CreatePath(output_dir, true);

    for (const PartsGroup &pg: emo)
    {
        for (const EmgFile &p : pg)
        {
            for (const SubPart &sp : p)
            {
                if (sp.IsEdge() && !edge_parts)
                    continue;

                if (!sp.IsEdge() && !normal_parts)
                    continue;

                DPRINTF("Exporting subpart \"%s\"\n", sp.GetMetaName().c_str());

                scene = FbxScene::Create(sdk_manager, "EMOTOOL3");
                output = output_dir + "/" + sp.GetMetaName();

                if (!emo.ExportFbx(sp.GetMetaName(), scene))
                {
                    DPRINTF("ExportFbx failed on subpart \"%s\"\n", sp.GetMetaName().c_str());
                    return;
                }

                if (!export_fbx_scene(sdk_manager, scene, output, version, ascii, y_up, inches))
                    return;
            }
        }
    }

    sdk_manager->Destroy();
}

void inject_fbx_file(const std::string &fbx_path, bool use_fbx_tangent, const std::string &mode)
{
    EmoFile emo;
    std::string emo_path;

    bool normal_parts, edge_parts;

    if (!get_mode(mode, normal_parts, edge_parts))
        return;

    if (!Utils::EndsWith(fbx_path, ".fbx", false))
    {
        DPRINTF("Fbx file must end with \".fbx\"\n");
        return;
    }

    emo_path = fbx_path.substr(0, fbx_path.length()-4) + ".emo";
    if (!Utils::FileExists(emo_path))
    {
        std::string emo_xml_path = emo_path + ".xml";
        if (!Utils::FileExists(emo_xml_path))
        {
            DPRINTF("Neither \"%s\" nor \"%s\" exist.\n", emo_path.c_str(), emo_xml_path.c_str());
            return;
        }

        emo_path = emo_xml_path;
    }

    if (!emo.SmartLoad(emo_path))
        return;

    FbxManager *sdk_manager = create_fbx_manager();
    FbxScene *scene = import_fbx_scene(sdk_manager, fbx_path);

    if (!scene)
        return;

    if (!emo.InjectFbx(scene, normal_parts, edge_parts, use_fbx_tangent))
    {
        DPRINTF("InjectFbx failed.\n");
        return;
    }

   if (!emo.SmartSave(emo_path))
       return;

   sdk_manager->Destroy();

   UPRINTF("Done.\n");
}

void inject_fbx_dir(const std::string &directory, bool use_fbx_tangent, const std::string &mode)
{
    EmoFile emo;
    std::string emo_path;

    bool normal_parts, edge_parts;

    if (!get_mode(mode, normal_parts, edge_parts))
        return;

    if (!Utils::EndsWith(directory, "_work", false))
    {
        DPRINTF("Directory name must end with \"_work\"\n");
        return;
    }

    if (directory.length() == 5)
    {
        DPRINTF("Directory name must contain something before \"_work\"\n");
        return;
    }

    emo_path = directory.substr(0, directory.length()-5) + ".emo";
    if (!Utils::FileExists(emo_path))
    {
        std::string xml_path = emo_path + ".xml";
        if (!Utils::FileExists(xml_path))
        {
            DPRINTF("Neither \"%s\" or \"%s\" file exist.\n", emo_path.c_str(), xml_path.c_str());
            return;
        }

        emo_path = xml_path;
    }

    if (!emo.SmartLoad(emo_path))
        return;

    FbxManager *sdk_manager = create_fbx_manager();

    DIR *df = opendir(directory.c_str());
    struct dirent *entry;
    int count = 0;

    if (!df)
        goto clean;

    while ((entry = readdir(df)))
    {
        std::string path = directory + '/' + std::string(entry->d_name);

        if (!Utils::FileExists(path))
            continue;

        if (!Utils::EndsWith(path, ".fbx", false))
            continue;

        std::string subpart_name = std::string(entry->d_name);
        subpart_name = subpart_name.substr(0, subpart_name.length()-4);

        SubPart *sp = emo.GetSubPart(subpart_name);
        if (!sp)
        {
            DPRINTF("Cannot find subpart \"%s\" in the emo.\n", subpart_name.c_str());
            goto clean;
        }

        if (!edge_parts && sp->IsEdge())
        {
            DPRINTF("Skipping \"%s\" because it is edge and mode is NORMAL.\n", sp->GetMetaName());
            continue;
        }

        if (!normal_parts && !sp->IsEdge())
        {
            DPRINTF("Skipping \"%s\" because it is normal part and mode is EDGE.\n", sp->GetMetaName());
            continue;
        }

        FbxScene *scene = import_fbx_scene(sdk_manager, path);
        if (!scene)
        {
            DPRINTF("Failed to import this fbx: %s\n", path.c_str());
            return;
        }

        if (!emo.InjectFbx(subpart_name, scene, use_fbx_tangent))
        {
            DPRINTF("InjectFbx failed on subpart.\n", subpart_name.c_str());
            return;
        }

        count++;
    }

    if (count > 0)
    {
        if (!emo.SmartSave(emo_path))
            goto clean;
    }

    UPRINTF("%d subparts injected.\n", count);

clean:

    if (df)
        closedir(df);

    sdk_manager->Destroy();
}

#endif

void reorder_vertex(const std::string &emo_path_dst, const std::string &emo_path_src)
{
    EmoFile emo_dst, emo_src;

    DPRINTF("This function destroys emo. It is a debug function only!\n");

    if (!emo_dst.SmartLoad(emo_path_dst))
        return;

    if (!emo_src.SmartLoad(emo_path_src))
        return;

    if (emo_dst.GetNumGroups() != emo_src.GetNumGroups())
    {
        DPRINTF("No identical number of group. Aborting.\n");
        return ;
    }

    for (size_t i = 0; i < emo_dst.GetNumGroups(); i++)
    {
        PartsGroup &pg_dst = emo_dst[i];
        const PartsGroup &pg_src = emo_src[i];

        if (pg_dst.GetName() != pg_src.GetName())
        {
            DPRINTF("Not same name in group %x. Aborting.\n", i);
            return;
        }

        if (pg_dst.GetNumParts() != pg_src.GetNumParts())
        {
            DPRINTF("No identical number of parts in group %s. Aborting.\n", pg_dst.GetName().c_str());
            return;
        }

        for (size_t j = 0; j < pg_dst.GetNumParts(); j++)
        {
            EmgFile &p_dst = pg_dst[j];
            const EmgFile &p_src = pg_src[j];

            if (p_dst.GetNumSubParts() != p_src.GetNumSubParts())
            {
                DPRINTF("No identical number of subparts in part %x in group %s.\n", j, pg_dst.GetName().c_str());
                return;
            }

            for (size_t k = 0; k < p_dst.GetNumSubParts(); k++)
            {
                SubPart &sp_dst = p_dst[k];
                const SubPart &sp_src = p_src[k];

                if (sp_dst != sp_src)
                {
                    std::vector<VertexData> vertex_dst, vertex_src;
                    std::vector<VertexData> new_vertex_dst;

                    vertex_dst = sp_dst.GetVertex();
                    vertex_src = sp_src.GetVertex();

                    if (vertex_dst.size() != vertex_src.size())
                    {
                        DPRINTF("Skipping %s because no same number of vertex.\n", sp_dst.GetMetaName().c_str());
                        continue;
                    }

                    for (size_t l = 0; l < vertex_src.size(); l++)
                    {
                        VertexData &vs = vertex_src[l];
                        const VertexCommon &vsc = vs.VertexUnion.vertex64.common;
                        bool found = false;

                         assert(vs.size == sizeof(Vertex64) || vs.size == sizeof(Vertex52));

                        for (size_t m = 0; m < vertex_dst.size(); m++)
                        {
                            VertexData &vd = vertex_dst[m];
                            const VertexCommon &vdc = vd.VertexUnion.vertex64.common;

                            assert(vd.size == sizeof(Vertex64) || vd.size == sizeof(Vertex52));

                            if (vs.size != vd.size)
                                break;

                            if (vsc.pos.x == vdc.pos.x && vsc.pos.y == vdc.pos.y && vsc.pos.z == vdc.pos.z)
                            {
                                if (vsc.tex.u == vdc.tex.u && vsc.tex.v == vdc.tex.v)
                                {
                                    float *bw_s, *bw_d;

                                    if (vs.size == sizeof(Vertex64))
                                    {
                                        bw_s = vs.VertexUnion.vertex64.blend_weight;
                                        bw_d = vd.VertexUnion.vertex64.blend_weight;
                                    }
                                    else
                                    {
                                        bw_s = vs.VertexUnion.vertex52.blend_weight;
                                        bw_d = vd.VertexUnion.vertex52.blend_weight;
                                    }

                                    for (int bi = 0; bi < 3; bi++)
                                    {
                                        if (bw_s[bi] != bw_d[bi])
                                        {
                                            DPRINTF("blend_weight not same for: %.9g, %.9g, %.9g\n", vsc.pos.x, vsc.pos.y, vsc.pos.z);
                                            break;
                                        }
                                    }

                                    new_vertex_dst.push_back(vd);
                                    found = true;
                                    break;
                                }
                            }
                        }

                        if (!found)
                        {
                            DPRINTF("Vertex not found. Skipping subpart %s.\n", sp_dst.GetMetaName().c_str());
                            break;
                        }
                    }

                    if (new_vertex_dst.size() == vertex_dst.size())
                    {
                        DPRINTF("Reordered vertex of %s\n", sp_dst.GetMetaName().c_str());
                        sp_dst.SetVertex(new_vertex_dst);
                        //sp_dst.SetVertex(vertex_dst);
                    }
                }
            }
        }
    }

    if (!emo_dst.SmartSave(emo_path_dst))
        return;

    UPRINTF("Done.\n");
}

void show_anims_info(const std::string &ema_path)
{
    EmaFile ema;

    if (!ema.SmartLoad(ema_path))
        return;

    for (size_t i = 0; i < ema.GetNumAnimations(); i++)
    {
        const EmaAnimation &anim = ema[i];
        float duration = anim.GetDuration() / 100.0f;
        UPRINTF("Id=0x%x, name=%s. Num of commands = %d, duration = %.2fs\n", i, anim.GetName().c_str(), anim.GetNumCommands(), duration);
    }
}

void mix_anims(const std::string &ema_base, const std::string &ema_other, const std::string &ema_result, const std::vector<std::string> &anims_dst, const std::vector<std::string> &anims_src)
{
    EmaFile base, other;
    std::vector<std::string> removed_bones;

    if (!base.SmartLoad(ema_base))
        return;

    if (!other.SmartLoad(ema_other))
        return;

    // Check parameters

    if (anims_dst.size() != anims_src.size())
    {
        DPRINTF("Number of DST_ANIMS and SRC_ANIMS must be same!\n");
        return;
    }

    for (size_t i = 0; i < anims_dst.size(); i++)
    {
        const std::string &ad = anims_dst[i];
        const std::string &as = anims_src[i];

        if (Utils::BeginsWith(ad, "0x", false))
        {
            uint32_t id = Utils::GetUnsigned(ad, 0xFFFFFFFF);

            if (id >= base.GetNumAnimations())
            {
                DPRINTF("Animation with id=\"0x%x\" doesn't exist in the base.\n", id);
                return;
            }
        }

        else if (!base.AnimationExists(ad))
        {
            DPRINTF("Animation with name \"%s\" doesn't exist in the base.\n", ad.c_str());
            return;
        }

        if (Utils::BeginsWith(as, "0x", false))
        {
            uint32_t id = Utils::GetUnsigned(as, 0xFFFFFFFF);

            if (id >= other.GetNumAnimations())
            {
                DPRINTF("Animation with id=\"0x%x\" doesn't exist in other.\n", id);
                return;
            }
        }

        else if (!other.AnimationExists(as))
        {
            DPRINTF("Animation with name \"%s\" doesn't exist in other.\n", as.c_str());
            return;
        }
    }

    for (size_t i = 0; i < anims_dst.size(); i++)
    {
        const EmaAnimation *asrc;

        if (Utils::BeginsWith(anims_src[i], "0x"))
        {
            asrc = &other[Utils::GetUnsigned(anims_src[i])];
        }
        else
        {
            asrc = &other[anims_src[i]];
        }

        uint32_t id;

        if (Utils::BeginsWith(anims_dst[i], "0x"))
        {
            id = Utils::GetUnsigned(anims_dst[i]);

        }
        else
        {
            id = base.FindAnimation(anims_dst[i]);
        }

        std::string orig_name = base[id].GetName();
        base[id] = *asrc;

        EmaAnimation &anim = base[id];
        anim.SetName(orig_name);

        if (base.HasSkeleton() && other.HasSkeleton())
        {
            for (size_t j = 0; j < anim.GetNumCommands(); j++)
            {
                EmaCommand &command = anim[j];
                const std::string &bn = command.GetBone()->GetName();

                if (!base.BoneExists(bn))
                {
                    if (std::find(removed_bones.begin(), removed_bones.end(), bn) == removed_bones.end())
                    {
                        removed_bones.push_back(bn);
                    }

                    anim.RemoveCommand(j);
                    j--;
                }
            }
        }

        if (!base.LinkAnimation(anim))
        {
            DPRINTF("LinkAnimation failed, on animation=%s\n", anims_dst[i].c_str());
            return;
        }

        UPRINTF("--Animation %s from other copied to %s from base.\n", anims_src[i].c_str(), anims_dst[i].c_str());
    }

    UPRINTF("Operation in ram finished. Now will save result to \"%s\"\n", ema_result.c_str());

    if (!base.SmartSave(ema_result))
        return;

    UPRINTF("*****Operation completed succesfully.\n");

    if (removed_bones.size() > 0)
    {
        UPRINTF("Now I will list the bones that had to be discarded because they didn't exist in base.\n");

        for (const std::string &bn : removed_bones)
        {
            UPRINTF("%s\n", bn.c_str());
        }
    }
}

void mix_anims(const std::string &ema_base, const std::string &ema_other, const std::string &ema_result, const std::string &anims_dst_param, const std::string &anims_src_param)
{
    std::vector<std::string> anims_dst, anims_src;

    Utils::GetMultipleStrings(anims_dst_param, anims_dst);
    Utils::GetMultipleStrings(anims_src_param, anims_src);

    mix_anims(ema_base, ema_other, ema_result, anims_dst, anims_src);
}

void test_equal()
{
    EmoFile emo, emo2, emo3;
    const std::string INPUT = "TESTS/HLD_00.emo";
    const std::string OUTPUT = "TESTS/HLD_00.emo.xml";

    if (!emo.SmartLoad(INPUT))
        return;

    if (!emo.SmartSave(OUTPUT))
        return;

    if (!emo2.SmartLoad(OUTPUT))
        return;

    DPRINTF("Comparison: %d\n", (emo == emo2));

    emo3 = emo2;

    DPRINTF("Comparison: %d\n", (emo3 == emo));

    EmoFile emo4 = emo;

    DPRINTF("Comparison: %d\n", (emo4 == emo2));
}

void test_create()
{
    EmoFile emo;

    if (!emo.SmartLoad("HLD_00.ps3.emo"))
        return;

    emo.SmartSave("HLD_00_other.ps3.emo");
}

void test_downvertex()
{
    EmoFile emo;

    if (!emo.SmartLoad("MST_00.emo"))
        return;

    for (PartsGroup &pg : emo)
    {
        if (pg.GetName() == "face")
        {
            for (EmgFile &p : pg)
            {
                for (SubPart &sp : p)
                {
                    if (sp.GetVertexSize() == sizeof(Vertex64))
                    {
                        std::vector<VertexData> vertex = sp.GetVertex();

                        for (VertexData &v : vertex)
                        {
                            VertexData temp = v;

                            v.size = sizeof(Vertex52);
                            v.VertexUnion.vertex52.color = temp.VertexUnion.vertex64.color;
                            v.VertexUnion.vertex52.blend[0] = temp.VertexUnion.vertex64.blend[0];
                            v.VertexUnion.vertex52.blend[1] = temp.VertexUnion.vertex64.blend[1];
                            v.VertexUnion.vertex52.blend[2] = temp.VertexUnion.vertex64.blend[2];
                            v.VertexUnion.vertex52.blend[3] = temp.VertexUnion.vertex64.blend[3];
                            v.VertexUnion.vertex52.blend_weight[0] = temp.VertexUnion.vertex64.blend_weight[0];
                            v.VertexUnion.vertex52.blend_weight[1] = temp.VertexUnion.vertex64.blend_weight[1];
                            v.VertexUnion.vertex52.blend_weight[2] = temp.VertexUnion.vertex64.blend_weight[2];
                        }

                        sp.SetVertex(vertex);
                        DPRINTF("Vertex converted: %x!\n", sp.GetVertexSize());
                    }
                }
            }
        }
    }

    emo.SmartSave("MST_00_test.emo");
}

void test_vertex_color()
{
    EmoFile emo;

    if (!emo.SmartLoad("HLD_00.emo"))
        return;

    for (PartsGroup &pg : emo)
    {
        for (EmgFile &p : pg)
        {
            for (SubPart &sp : p)
            {
                std::vector<VertexData> vertex = sp.GetVertex();

                for (VertexData &v : vertex)
                {
                    if (v.size == sizeof(Vertex64))
                    {
                        v.VertexUnion.vertex64.color = 0xFF0000FF;
                    }
                    else if (v.size == sizeof(Vertex52))
                    {
                        v.VertexUnion.vertex52.color = 0xFF0000FF;
                    }
                }

                sp.SetVertex(vertex);
            }
        }
    }

    emo.SmartSave("HLD_00_test.emo");
}

void matrix_test()
{
    EmoFile emo;

    if (!emo.SmartLoad("HLD_00.emo"))
        return;

    SkeletonFile &skl = emo;

    for (Bone &b : skl)
    {
        if (b.HasMatrix2())
        {
            b.SetHasMatrix2(false);
        }
    }

    emo.SmartSave("HLD_00_test.emo");
}

void test_remove_blend_weight()
{
    EmoFile emo;

    if (!emo.SmartLoad("HLD_00.emo"))
        return;

    for (PartsGroup &pg : emo)
    {
        if (true)
        {
            for (EmgFile &p : pg)
            {
                for (SubPart &sp : p)
                {
                    if (true)
                    {
                        std::vector<VertexData> vertex = sp.GetVertex();

                        for (VertexData &v : vertex)
                        {
                            float *weight;
                            uint8_t *indices;

                            if (v.size == sizeof(Vertex52))
                            {
                                weight = v.VertexUnion.vertex52.blend_weight;
                                indices = v.VertexUnion.vertex52.blend;
                            }
                            else
                            {
                                weight = v.VertexUnion.vertex64.blend_weight;
                                indices = v.VertexUnion.vertex64.blend;
                            }

                            indices[0] = indices[1] = indices[2] = indices[3] = 0;
                            weight[0] = weight[1] = weight[2] = 0.0f;
                        }

                        sp.SetVertex(vertex);
                        DPRINTF("Vertex modified: %x!\n", sp.GetVertexSize());
                    }
                }
            }
        }
    }

    emo.SmartSave("HLD_00_test.emo");
}

void fix_evil_freya_skirt()
{
    unstrip_triangles("FREYA_HILDA.emo");

    EmoFile emo;

    if (!emo.SmartLoad("FREYA_HILDA.emo"))
        return;

    SubPart *plower_body = emo.GetSubPart("SHARE_lower_body_0000_0000");
    assert(plower_body);

    SubPart *pedge_lower_body = emo.GetSubPart("EDGE_SHARE_lower_body_0000_0000");
    assert(pedge_lower_body);

    SubPart &lower_body = *plower_body;
    SubPart &edge_lower_body = *pedge_lower_body;

    emo += *emo.GetGroup("SHARE_lower_body");
    emo += *emo.GetGroup("EDGE_SHARE_lower_body");

    emo.GetGroup(emo.GetNumGroups()-2)->SetName("Show_0");
    emo.GetGroup(emo.GetNumGroups()-1)->SetName("Showmodeledge_0");

    SubPart *pshow_0 = emo.GetSubPart("Show_0_0000_0000");
    assert(pshow_0);

    SubPart *pedge_show_0 = emo.GetSubPart("Showmodeledge_0_0000_0000");
    assert(pedge_show_0);

    SubPart &show_0 = *pshow_0;
    SubPart &edge_show_0 = *pedge_show_0;

    float min_y = 10000.0;
    float max_y = -10000.0;

    std::vector<VertexData> vertex;
    std::vector<uint16_t> faces, new_faces, show_0_faces, edge_show_0_faces;

    vertex = lower_body.GetVertex();
    faces = lower_body[0].GetFaces();

    for (const VertexData &v : vertex)
    {
        if (v.VertexUnion.vertex64.common.pos.y > max_y)
        {
            max_y = v.VertexUnion.vertex64.common.pos.y;
        }

        if (v.VertexUnion.vertex64.common.pos.y < min_y)
        {
            min_y = v.VertexUnion.vertex64.common.pos.y;
        }
    }

    UPRINTF("max_y = %f, min_y = %f\n", max_y, min_y);

    const float split_y = -0.50f;//-0.414865f;

    for (size_t i = 0; i < faces.size(); i += 3)
    {
        bool add_it = true;

        for (int j = 0; j < 3; j++)
        {
            const VertexData &v = vertex[faces[i+j]];

            if (v.VertexUnion.vertex64.common.pos.y > split_y)
            {
                add_it = false;
                break;
            }
        }

        if (add_it)
        {
            new_faces.push_back(faces[i]);
            new_faces.push_back(faces[i+1]);
            new_faces.push_back(faces[i+2]);
        }
        else
        {
            show_0_faces.push_back(faces[i]);
            show_0_faces.push_back(faces[i+1]);
            show_0_faces.push_back(faces[i+2]);
        }
    }

    lower_body[0].SetFaces(new_faces);

    vertex = edge_lower_body.GetVertex();
    faces = edge_lower_body[0].GetFaces();
    new_faces.clear();

    for (size_t i = 0; i < faces.size(); i += 3)
    {
        bool add_it = true;

        for (int j = 0; j < 3; j++)
        {
            const VertexData &v = vertex[faces[i+j]];

            if (v.VertexUnion.vertex64.common.pos.y > split_y)
            {
                add_it = false;
                break;
            }
        }

        if (add_it)
        {
            new_faces.push_back(faces[i]);
            new_faces.push_back(faces[i+1]);
            new_faces.push_back(faces[i+2]);
        }
        else
        {
            edge_show_0_faces.push_back(faces[i]);
            edge_show_0_faces.push_back(faces[i+1]);
            edge_show_0_faces.push_back(faces[i+2]);
        }
    }

    edge_lower_body[0].SetFaces(new_faces);

    show_0[0].SetFaces(show_0_faces);
    show_0.RemoveSubMesh(1);

    edge_show_0[0].SetFaces(edge_show_0_faces);
    edge_show_0.RemoveSubMesh(1);

    if (!emo.SmartSave("FREYA_HILDA_FIX.emo"))
        return;

    UPRINTF("Done.\n");
}

void fix_pandora_skirt_test()
{
    EmoFile emo;

    unstrip_triangles("PND_HLD.emo");

    if (!emo.SmartLoad("PND_HLD.emo"))
        return;

    SubPart *plower_body = emo.GetSubPart("SHARE_legs_0000_0000");
    assert(plower_body);

    SubPart *pedge_lower_body = emo.GetSubPart("EDGE_SHARE_legs_0000_0000");
    assert(pedge_lower_body);

    SubPart &lower_body = *plower_body;
    SubPart &edge_lower_body = *pedge_lower_body;

    emo += emo["SHARE_legs"];
    emo += emo["EDGE_SHARE_legs"];

    /*emo[emo.GetNumGroups()-2].SetName("Show_0");
    emo[emo.GetNumGroups()-1].SetName("Showmodeledge_0");

    SubPart *pshow_0 = emo.GetSubPart("Show_0_0000_0000");
    assert(pshow_0);

    SubPart *pedge_show_0 = emo.GetSubPart("Showmodeledge_0_0000_0000");
    assert(pedge_show_0);

    SubPart &show_0 = *pshow_0;
    SubPart &edge_show_0 = *pedge_show_0;*/

    float min_y = 10000.0;
    float max_y = -10000.0;

    std::vector<VertexData> vertex;
    std::vector<uint16_t> faces, new_faces, show_0_faces, edge_show_0_faces;

    vertex = lower_body.GetVertex();
    faces = lower_body[0].GetFaces();

    for (const VertexData &v : vertex)
    {
        if (v.VertexUnion.vertex64.common.pos.y > max_y)
        {
            max_y = v.VertexUnion.vertex64.common.pos.y;
        }

        if (v.VertexUnion.vertex64.common.pos.y < min_y)
        {
            min_y = v.VertexUnion.vertex64.common.pos.y;
        }
    }

    UPRINTF("max_y = %f, min_y = %f\n", max_y, min_y);

    const float split_y = -0.60f;

    for (size_t i = 0; i < faces.size(); i += 3)
    {
        bool add_it = true;

        for (int j = 0; j < 3; j++)
        {
            const VertexData &v = vertex[faces[i+j]];

            if (v.VertexUnion.vertex64.common.pos.y > split_y)
            {
                add_it = false;
                break;
            }
        }

        if (add_it)
        {
            new_faces.push_back(faces[i]);
            new_faces.push_back(faces[i+1]);
            new_faces.push_back(faces[i+2]);
        }
        else
        {
            show_0_faces.push_back(faces[i]);
            show_0_faces.push_back(faces[i+1]);
            show_0_faces.push_back(faces[i+2]);
        }
    }

    lower_body[0].SetFaces(new_faces);

    vertex = edge_lower_body.GetVertex();
    faces = edge_lower_body[0].GetFaces();
    new_faces.clear();

    for (size_t i = 0; i < faces.size(); i += 3)
    {
        bool add_it = true;

        for (int j = 0; j < 3; j++)
        {
            const VertexData &v = vertex[faces[i+j]];

            if (v.VertexUnion.vertex64.common.pos.y > split_y)
            {
                add_it = false;
                break;
            }
        }

        if (add_it)
        {
            new_faces.push_back(faces[i]);
            new_faces.push_back(faces[i+1]);
            new_faces.push_back(faces[i+2]);
        }
        else
        {
            edge_show_0_faces.push_back(faces[i]);
            edge_show_0_faces.push_back(faces[i+1]);
            edge_show_0_faces.push_back(faces[i+2]);
        }
    }

    edge_lower_body[0].SetFaces(new_faces);

    //show_0[0].SetFaces(show_0_faces);
    //edge_show_0[0].SetFaces(edge_show_0_faces);

    if (!emo.SmartSave("PND_HLD_FIX.emo"))
        return;

    UPRINTF("Done.\n");
}

void test_cammy()
{
    EmoFile cammy, shaina;
    std::vector<const Bone *> added_bones;

    if (!cammy.SmartLoad("CMY_01.obj.emo"))
        return;

    if (!shaina.SmartLoad("DSN_02.emo"))
        return;

    const SkeletonFile &shaina_skl = shaina;
    SkeletonFile &cammy_skl = cammy;

    for (const Bone &b : shaina_skl)
    {
        if (!cammy_skl.BoneExists(b.GetName()))
        {
            cammy_skl += b;
            added_bones.push_back(&b);
            UPRINTF("Added bone %s\n", b.GetName().c_str());
        }
    }

    for (const Bone *&b : added_bones)
    {
        Bone *not_found;
        cammy_skl.CloneBoneParentChild(shaina_skl, b->GetName(), &not_found);
    }

    if (!cammy.SmartSave("test.emo"))
        return;
}

#define CUT_TIME    110

void test_athena_ema()
{
    EmaFile ema;

    if (!ema.SmartLoad("ATN_EVE.ema"))
        return;

    EmaAnimation &animation = ema["ATG_DEM_WIN"];
    for (size_t i = 0; i < animation.GetNumCommands(); i++)
    {
        EmaCommand &command = animation[i];

        for (size_t j = 0; j < command.GetNumSteps(); j++)
        {
            EmaStep &step = command[j];

            if (step.time < CUT_TIME)
            {
                if (step.time != 0)
                    step.time = 0;
                else
                    step.time = 0;
            }
            else
            {
                step.time -= CUT_TIME;
            }
        }
    }

    /*for (size_t i = 0; i < animation.GetNumCommands(); i++)
    {
        EmaCommand &command = animation[i];

        for (size_t j = 0; j < command.GetNumSteps(); j++)
        {
            EmaStep &step = command[j];

            if (step.time < CUT_TIME)
            {
                command.RemoveStep(j);
                j--;
            }
        }

        if (command.GetNumSteps() == 0)
        {
            animation.RemoveCommand(i);
            i--;
        }
    }*/

    animation.SetDuration(animation.GetDuration()-CUT_TIME);

    if (!ema.SmartSave("ATN_EVE_test.ema"))
        return;

    UPRINTF("Done.\n");
}

void atg_with_hld_guard()
{
    EmaFile atg, hld;

    if (!atg.SmartLoad("ATG.ema"))
        return;

    if (!hld.SmartLoad("HLD.ema"))
        return;

    std::vector<std::string> anims =
    {
        "GRD_KAMAE_A",
        "GRD_KAMAE_B",
        "GRD_KAMAE_C",
        "GRD_L",
        "GRD_H",
        "GRD_FUTOBI",
        "GRD_CLASH",
        //"GRD_S",
    };

    for (const std::string &s : anims)
    {
        Bone *not_found;

        std::string atg_anim = "ATG_" + s;
        std::string hld_anim = "HLD_" + s;

        if (!atg.AnimationExists(atg_anim))
        {
            DPRINTF("Animation %s doesn't exist.\n", atg_anim.c_str());
            return;
        }

        if (!hld.AnimationExists(hld_anim))
        {
            DPRINTF("Animation %s doesn't exist.\n", hld_anim.c_str());
            return;
        }

        DPRINTF("%s\n", atg_anim.c_str());

        atg[atg_anim] = hld[hld_anim];
        atg[hld_anim].SetName(atg_anim);

        for (size_t i = 0; i < atg[atg_anim].GetNumCommands(); i++)
        {
            EmaCommand &command = atg[atg_anim][i];

            if (!atg.BoneExists(command.GetBone()->GetName()))
            {
                atg[atg_anim].RemoveCommand(i);
                i--;
            }
        }

        if (!atg.LinkAnimation(atg[atg_anim], &not_found))
        {
            DPRINTF("Bone %s not found.\n", not_found->GetName().c_str());
            return;
        }
    }

    if (!atg.SmartSave("ATG_TEST.ema"))
        return;

    UPRINTF("Done.\n");
}

void pandora_mouth_test()
{
    EmaFile bba;

    if (!bba.SmartLoad("HLD.bba.ema"))
        return;

    std::vector<std::string> blacklist =
    {
        //"mouth",
        "jawtop",
        //"cheek"
    };

    for (EmaAnimation &animation : bba)
    {
        for (size_t i = 0; i < animation.GetNumCommands(); i++)
        {
            EmaCommand &c = animation[i];
            std::string bone_name = c.GetBone()->GetName();

            for (const std::string &s: blacklist)
            {
                if (Utils::ToLowerCase(bone_name).find(s) != std::string::npos)
                {
                    DPRINTF("Removing command because of \"%s\"\n", bone_name.c_str());
                    animation.RemoveCommand(i);
                    i--;
                    break;
                }
            }
        }
    }

    if (!bba.SmartSave("HLD_test.bba.ema"))
        return;

    UPRINTF("Done.\n");
}

int main(int argc, char *argv[])
{
    set_debug_level(2);

    if (argc == 1)
    {
        // Tests go here
        //test_vertex_color();
        //export_fbx("HLD_00.emo");
        //matrix_test();
        //test_remove_blend_weight();
        //inject_fbx_file("DSN_02.fbx");
        //inject_fbx("DMR_02.fbx");
        //inject_fbx("HLD_00.fbx");
        //fix_evil_freya_skirt();
        //fix_pandora_skirt_test();
        //test_cammy();
        //atg_with_hld_guard();
        //pandora_mouth_test();
    }

    else if (argc >= 2 && strcasecmp(argv[1], "MixParts") == 0) // Special case
    {
        if (argc != 28)
        {
            DPRINTF("Number of parameters, ->excluding<- program name and MixParts, must be 26, but I received %d."
                    "Yep, not one more, not one less.\nHave you ever seen a program taking all those params?\n", argc-2);
            return -1;
        }

        mix_parts(argv[2],
                  argv[3],
                  argv[4],
                  argv[5],
                  argv[6],
                  argv[7],
                  argv[8],
                  argv[9],
                  argv[10],
                  argv[11],
                  argv[12]);

    }
    else if (argc >= 2 && strcasecmp(argv[1], "MixAnims") == 0)
    {
        if (argc != 7)
        {
            DPRINTF("Invalid number of parameters.\n");
            return -1;
        }

        mix_anims(argv[2], argv[3], argv[4], argv[5], argv[6]);
    }
    else if (argc >= 3)
    {
        const char *function = argv[1];
        const std::string file = argv[2]; // Directory in some functins
        bool is_dir;
        std::string param1, param2, param3, param4, param5, param6;


        if (argc >= 4)
            param1 = argv[3];

        if (argc >= 5)
            param2 = argv[4];

        if (argc >= 6)
            param3 = argv[5];

        if (argc >= 7)
            param4 = argv[6];

        if (argc >= 8)
            param5 = argv[7];

        if (argc >= 9)
            param6 = argv[8];

        if (!Utils::FileExists(file) && !Utils::DirExists(file))
        {
            DPRINTF("File or directory \"%s\" doesn't exist.\n", file.c_str());
            return -1;
        }

        is_dir = Utils::DirExists(file);

        if (strcasecmp(function, "Xmlize") == 0)
        {
            xmlize(file);
        }
        else if (strcasecmp(function, "Compare") == 0)
        {
            if (param1.length() == 0)
            {
                DPRINTF("Second file parameter is missing.\n");
                return -1;
            }
            else if (!Utils::FileExists(param1))
            {
                DPRINTF("File %s doesn't exist.\n", param1.c_str());
                return -1;
            }

            compare(file, param1);
        }
        else if (strcasecmp(function, "ReverseEndianess") == 0)
        {
            reverse_endianess(file);
        }
        else if (strcasecmp(function, "Ps3ToPC") == 0)
        {
            ps3_to_pc(file);
        }
        else if (strcasecmp(function, "ShowPartsInfo") == 0)
        {
            show_parts_info(file);
        }
        else if (strcasecmp(function, "ShowSkeletonInfo") == 0)
        {
            show_skeleton_info(file);
        }
        else if (strcasecmp(function, "RemovePart") == 0)
        {
            if (param1.length() == 0)
            {
                DPRINTF("Parameter part name wasn't specified.\n");
                return -1;
            }

            remove_part(file, param1);
        }
        else if (strcasecmp(function, "ExportObj") == 0)
        {
            export_obj(file);
        }
        else if (strcasecmp(function, "InjectObj") == 0)
        {
            bool do_uv, do_normal;

            do_uv = Utils::GetBoolean(param1, true);
            do_normal = Utils::GetBoolean(param2, true);

            if (is_dir)
                inject_obj_dir(file, do_uv, do_normal);
            else
                inject_obj_file(file, do_uv, do_normal);
        }
        else if (strcasecmp(function, "ExtractEmb") == 0)
        {
            bool recursive = Utils::GetBoolean(param1, true);
            bool force_index_names = Utils::GetBoolean(param2, false);

            extract_emb(file, recursive, force_index_names);
        }
        else if (strcasecmp(function, "InjectEmb") == 0)
        {
            bool recursive = Utils::GetBoolean(param1, true);
            bool force_index_names = Utils::GetBoolean(param2, false);

            if (!is_dir)
            {
                DPRINTF("This function takes a directory as parameter, not a file!\n");
                return -1;
            }

            inject_emb(file, recursive, force_index_names);
        }
        else if (strcasecmp(function, "ReplaceFace") == 0)
        {
            uint32_t index_from, index_to;

            if (param1 == "" || param2 == "")
            {
                DPRINTF("Invalid number of params!\n");
                return -1;
            }

            index_from = Utils::GetUnsigned(param1, 0xFFFFFFFF);
            index_to = Utils::GetUnsigned(param2, 0xFFFFFFFF);

            if (index_from == 0xFFFFFFFF)
            {
                DPRINTF("Parameter REPLACE_SRC is invalid. Make sure to use decimal number without zeroes in the left,"
                        "or hexadecimal numbers with 0x prefix.\n");
                return -1;
            }

            if (index_to == 0xFFFFFFFF)
            {
                DPRINTF("Parameter REPLACE_DST is invalid. Make sure to use decimal number without zeroes in the left,"
                        "or hexadecimal numbers with 0x prefix.\n");
                return -1;
            }

            replace_face(file, index_from, index_to);
        }
        else if (strcasecmp(function, "UnstripTriangles") == 0)
        {
            unstrip_triangles(file);
        }
        else if (strcasecmp(function, "ExtractSkeleton") == 0)
        {
            bool as_xml = Utils::GetBoolean(param1, true);

            extract_skeleton(file, as_xml);
        }
        else if (strcasecmp(function, "ExportFbx") == 0)
        {
            if (param1 == "")
            {
                DPRINTF("Parameter VERSION must be specified.\n");
                return -1;
            }

            bool ascii = Utils::GetBoolean(param2);

#ifdef FBX_SUPPORT

            export_fbx(file, param1, ascii, param3, param4, param5);
#else

            DPRINTF("This version of emotool3 doesn't have fbx support.\n");
            return -1;
#endif
        }        
        else if (strcasecmp(function, "InjectFbx") == 0)
        {
#ifdef FBX_SUPPORT

            bool use_fbx_tangent = Utils::GetBoolean(param1);

            if (is_dir)
                inject_fbx_dir(file, use_fbx_tangent, param2);
            else
                inject_fbx_file(file, use_fbx_tangent, param2);
#else
            DPRINTF("This version of emotool3 doesn't have fbx support.\n");
            return -1;
#endif
        }
        else if (strcasecmp(function, "ReorderVertex") == 0)
        {
            if (param1 == "")
            {
                DPRINTF("Parameter emo_src must be specified.\n");
                return -1;
            }

            reorder_vertex(file, param1);
        }
        else if (strcasecmp(function, "ShowAnimsInfo") == 0)
        {
            show_anims_info(file);
        }
        else
        {
            DPRINTF("Unrecognized function \"%s\"\n", function);
            return -1;
        }
    }

    return 0;
}
