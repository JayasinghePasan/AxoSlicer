#pragma once
struct GuidHash
{
    std::size_t operator()(const GUID& g) const noexcept
    {
        const uint64_t* p = reinterpret_cast<const uint64_t*>(&g);
        return std::hash<uint64_t>()(p[0]) ^ std::hash<uint64_t>()(p[1]);
    }
};

struct GuidEqual
{
    bool operator()(const GUID& a, const GUID& b) const noexcept
    {
        return IsEqualGUID(a, b) != 0;
    }
};
