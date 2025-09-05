#pragma once

struct BoundingBox
{
    float minX, minY, minZ;
    float maxX, maxY, maxZ;

    BoundingBox()
        : minX(0), minY(0), minZ(0), maxX(0), maxY(0), maxZ(0) 
    {}

    BoundingBox(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z)
        : minX(min_x), minY(min_y), minZ(min_z), maxX(max_x), maxY(max_y), maxZ(max_z) 
    {}

    void expandToInclude(float x, float y, float z)
    {
        if (x < minX) minX = x;
        if (y < minY) minY = y;
        if (z < minZ) minZ = z;
        if (x > maxX) maxX = x;
        if (y > maxY) maxY = y;
        if (z > maxZ) maxZ = z;
    }

    void expandToInclude(const BoundingBox& other)
    {
        if (other.minX < minX) minX = other.minX;
        if (other.minY < minY) minY = other.minY;
        if (other.minZ < minZ) minZ = other.minZ;

        if (other.maxX > maxX) maxX = other.maxX;
        if (other.maxY > maxY) maxY = other.maxY;
        if (other.maxZ > maxZ) maxZ = other.maxZ;
    }

    Vector3 getCenter() const
    {
        return Vector3((minX + maxX) * 0.5f, (minY + maxY) * 0.5f, (minZ + maxZ) * 0.5f);
    }

    BoundingBox& operator=(const BoundingBox& other) noexcept
    {
        if (this != &other)
        {
            minX = other.minX; minY = other.minY; minZ = other.minZ;
            maxX = other.maxX; maxY = other.maxY; maxZ = other.maxZ;
        }
        return *this;
    }

    bool operator==(const BoundingBox& rhs) const noexcept
    {
        return minX == rhs.minX && minY == rhs.minY && minZ == rhs.minZ && maxX == rhs.maxX && maxY == rhs.maxY && maxZ == rhs.maxZ;
    }

    bool operator!=(const BoundingBox& rhs) const noexcept
    {
        return !(minX == rhs.minX && minY == rhs.minY && minZ == rhs.minZ && maxX == rhs.maxX && maxY == rhs.maxY && maxZ == rhs.maxZ);
    }


};
