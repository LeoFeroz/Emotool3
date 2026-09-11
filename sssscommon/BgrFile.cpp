#include "BgrFile.h"
#include "SsssData.h"

#include "debug.h"

void BgrEntry::Decompile(TiXmlNode *root) const
{
    TiXmlElement *entry_root = new TiXmlElement("BgrEntry");

    SsssCharInfo *info = SsssData::FindInfo(cms_entry, cms_model_spec_idx);
    if (info)
    {
        Utils::WriteComment(entry_root, std::string(" ") + info->model_name + std::string(" / ") + info->char_name);
    }

    Utils::WriteParamUnsigned(entry_root, "CMS_ENTRY", cms_entry, true);
    Utils::WriteParamUnsigned(entry_root, "CMS_MODEL_SPEC_IDX", cms_model_spec_idx);
    Utils::WriteParamUnsigned(entry_root, "LEVEL", level, true);
    Utils::WriteParamUnsigned(entry_root, "U_04", unk_04, true);
    Utils::WriteParamUnsigned(entry_root, "U_08", unk_08, true);
    Utils::WriteParamUnsigned(entry_root, "U_14", unk_14, true);
    Utils::WriteParamUnsigned(entry_root, "U_18", unk_18, true);
    Utils::WriteParamUnsigned(entry_root, "U_1C", unk_1C, true);
    Utils::WriteParamUnsigned(entry_root, "U_20", unk_20, true);
    Utils::WriteParamUnsigned(entry_root, "U_24", unk_24, true);
    Utils::WriteParamUnsigned(entry_root, "U_28", unk_28, true);
    Utils::WriteParamUnsigned(entry_root, "U_2C", unk_2C, true);
    Utils::WriteParamUnsigned(entry_root, "U_30", unk_30, true);
    Utils::WriteParamUnsigned(entry_root, "U_34", unk_34, true);
    Utils::WriteParamUnsigned(entry_root, "U_38", unk_38, true);
    Utils::WriteParamUnsigned(entry_root, "STAGE", stage, true);
    Utils::WriteParamUnsigned(entry_root, "U_40", unk_40);
    Utils::WriteParamUnsigned(entry_root, "U_44", unk_44, true);

    root->LinkEndChild(entry_root);
}

bool BgrEntry::Compile(const TiXmlElement *root)
{
    if (!Utils::GetParamUnsigned(root, "CMS_ENTRY", &cms_entry))
        return false;

    if (!Utils::GetParamUnsigned(root, "CMS_MODEL_SPEC_IDX", &cms_model_spec_idx))
        return false;

    if (!Utils::GetParamUnsigned(root, "LEVEL", &level))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_04", &unk_04))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_08", &unk_08))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_14", &unk_14))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_18", &unk_18))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_1C", &unk_1C))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_20", &unk_20))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_24", &unk_24))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_28", &unk_28))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_2C", &unk_2C))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_30", &unk_30))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_34", &unk_34))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_38", &unk_38))
        return false;

    if (!Utils::GetParamUnsigned(root, "STAGE", &stage))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_40", &unk_40))
        return false;

    if (!Utils::GetParamUnsigned(root, "U_44", &unk_44))
        return false;

    return true;
}

BgrFile::BgrFile()
{
    this->big_endian = false;
}

BgrFile::~BgrFile()
{

}

void BgrFile::Reset()
{
    entries.clear();
}

bool BgrFile::Load(const uint8_t *buf, unsigned int size)
{
    Reset();

    BGRHeader *hdr = (BGRHeader *)buf;

    if (size < sizeof(BGRHeader) || memcmp(hdr->signature, BGR_SIGNATURE, 4) != 0)
        return false;

    entries.resize(val32(hdr->num_entries));

    BGREntry *fentries = (BGREntry *)GetOffsetPtr(hdr, hdr->data_start);

    for (size_t i = 0; i < hdr->num_entries; i++)
    {
        BgrEntry &entry = entries[i];

        entry.cms_entry = val32(fentries[i].cms_entry);
        entry.cms_model_spec_idx = val32(fentries[i].cms_model_spec_idx);
        entry.level = val32(fentries[i].level);
        entry.unk_04 = val32(fentries[i].unk_04);
        entry.unk_08 = val32(fentries[i].unk_08);
        entry.unk_14 = val32(fentries[i].unk_14);
        entry.unk_18 = val32(fentries[i].unk_18);
        entry.unk_1C = val32(fentries[i].unk_1C);
        entry.unk_20 = val32(fentries[i].unk_20);
        entry.unk_24 = val32(fentries[i].unk_24);
        entry.unk_28 = val32(fentries[i].unk_28);
        entry.unk_2C = val32(fentries[i].unk_2C);
        entry.unk_30 = val32(fentries[i].unk_30);
        entry.unk_34 = val32(fentries[i].unk_34);
        entry.unk_38 = val32(fentries[i].unk_38);
        entry.stage = val32(fentries[i].stage);
        entry.unk_40 = val32(fentries[i].unk_40);
        entry.unk_44 = val32(fentries[i].unk_44);
    }

    return true;
}

uint8_t *BgrFile::CreateFile(unsigned int *size)
{
    unsigned int file_size = sizeof(BGRHeader) + entries.size() * sizeof(BGREntry);
    uint8_t *buf;

    buf = new uint8_t[file_size];
    if (!buf)
    {
        DPRINTF("%s: Memory allocation error (0x%x)\n", FUNCNAME, file_size);
        return nullptr;
    }

    memset(buf, 0, file_size);

    BGRHeader *hdr = (BGRHeader *)buf;

    memcpy(hdr->signature, BGR_SIGNATURE, 4);
    hdr->endianess_check = val16(0xFFFE);
    hdr->num_entries = val32(entries.size());
    hdr->data_start = val32(sizeof(BGRHeader));

    BGREntry *fentries = (BGREntry *)GetOffsetPtr(hdr, hdr->data_start);

    for (size_t i = 0; i < entries.size(); i++)
    {
        const BgrEntry &entry = entries[i];

        fentries[i].cms_entry = val32(entry.cms_entry);
        fentries[i].cms_model_spec_idx = val32(entry.cms_model_spec_idx);
        fentries[i].level = val32(entry.level);
        fentries[i].unk_04 = val32(entry.unk_04);
        fentries[i].unk_08 = val32(entry.unk_08);
        fentries[i].unk_14 = val32(entry.unk_14);
        fentries[i].unk_18 = val32(entry.unk_18);
        fentries[i].unk_1C = val32(entry.unk_1C);
        fentries[i].unk_20 = val32(entry.unk_20);
        fentries[i].unk_24 = val32(entry.unk_24);
        fentries[i].unk_28 = val32(entry.unk_28);
        fentries[i].unk_2C = val32(entry.unk_2C);
        fentries[i].unk_30 = val32(entry.unk_30);
        fentries[i].unk_34 = val32(entry.unk_34);
        fentries[i].unk_38 = val32(entry.unk_38);
        fentries[i].stage = val32(entry.stage);
        fentries[i].unk_40 = val32(entry.unk_40);
        fentries[i].unk_44 = val32(entry.unk_44);
    }

    *size = file_size;
    return buf;
}

TiXmlDocument *BgrFile::Decompile() const
{
    TiXmlDocument *doc = new TiXmlDocument();

    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "utf-8", "" );
    doc->LinkEndChild(decl);

    TiXmlElement *root = new TiXmlElement("BGR");

    for (const BgrEntry &entry : entries)
    {
        entry.Decompile(root);
    }

    doc->LinkEndChild(root);

    return doc;
}

bool BgrFile::Compile(TiXmlDocument *doc, bool big_endian)
{
    Reset();
    this->big_endian = big_endian;

    TiXmlHandle handle(doc);
    const TiXmlElement *root = Utils::FindRoot(&handle, "BGR");

    if (!root)
    {
        DPRINTF("Cannot find\"BGR\" in xml.\n");
        return false;
    }

    for (const TiXmlElement *elem = root->FirstChildElement(); elem; elem = elem->NextSiblingElement())
    {
       if (elem->ValueStr() == "BgrEntry")
       {
           BgrEntry entry;

           if (!entry.Compile(elem))
           {
               DPRINTF("%s: BGREntry compilation failed.\n", FUNCNAME);
               return false;
           }

           entries.push_back(entry);
       }
    }

    return true;
}



