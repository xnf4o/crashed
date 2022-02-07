#include "memory.h"

#include <Windows.h>
#include <vector>
#include <stdexcept>

// game_sq_call -> 56 57 53 48 83 EC 40 4C 89 C3 48 89 D7 48 89 CE C7 44 24 ? ? ? ? ?
// game_sq_pushstring -> 56 57 48 83 EC 28 48 89 CF 48 85 D2 75 48 4D 85 C0 74 43 48 8B 47 30

std::uint8_t* memory::Scan(const char* signature) noexcept
{
    static const auto patternToByte = [](const char* pattern) noexcept -> std::vector<int>
    {
        auto bytes = std::vector<int>{ };
        auto start = const_cast<char*>(pattern);
        auto end = const_cast<char*>(pattern) + std::strlen(pattern);

        for (auto current = start; current < end; ++current)
        {
            if (*current == '?')
            {
                ++current;

                if (*current == '?')
                    ++current;

                bytes.push_back(-1);
            }
            else
                bytes.push_back(std::strtoul(current, &current, 16));

        }

        return bytes;
    };

    static const auto handle = ::GetModuleHandleA("aces.exe");

    if (!handle)
        return nullptr;

    auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(handle);
    auto ntHeaders =
        reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(handle) + dosHeader->e_lfanew);

    auto size = ntHeaders->OptionalHeader.SizeOfImage;
    auto bytes = patternToByte(signature);
    auto scanBytes = reinterpret_cast<std::uint8_t*>(handle);

    auto s = bytes.size();
    auto d = bytes.data();

    for (auto i = 0ul; i < size - s; ++i)
    {
        bool found = true;

        for (auto j = 0ul; j < s; ++j)
        {
            if (scanBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }

        if (found)
            return &scanBytes[i];
    }

    return nullptr;
}
