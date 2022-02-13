#include <pe.hpp>

// PE header manager
// Thanks for IPower for letting me use his code:
// https://github.com/iPower/KasperskyHook/blob/master/KasperskyHookDrv/pe.cpp

namespace pe
{
  PIMAGE_SECTION_HEADER section_header_getter(const uint64_t image_base, const char* section_name)
  {
    if (!image_base || !section_name)
      return nullptr;

    const auto pimage_dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(image_base);

    if (pimage_dos_header->e_magic != IMAGE_DOS_SIGNATURE)
      return nullptr;

    const auto pimage_nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(image_base + pimage_dos_header->e_lfanew);

    if (pimage_nt_headers->Signature != IMAGE_NT_SIGNATURE)
      return nullptr;

    auto psection = IMAGE_FIRST_SECTION(pimage_nt_headers);
    const auto NumberOfSections = pimage_nt_headers->FileHeader.NumberOfSections;

    PIMAGE_SECTION_HEADER psection_hdr = nullptr;

    for (USHORT i = 0; i < NumberOfSections; ++i)
    {
      if (!strncmp(reinterpret_cast<const char*>(&psection->Name[0]), section_name, IMAGE_SIZEOF_SHORT_NAME))
      {
        psection_hdr = psection;
        break;
      }

      ++psection;
    }

    return psection_hdr;
  }
};
