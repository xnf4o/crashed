#pragma once
#include <cmath>

struct Matrix3x3
{
	constexpr float* operator[](int index) noexcept
	{
		return data[index];
	}

	constexpr const float* operator[](int index) const noexcept
	{
		return data[index];
	}

	float data[3][3] = { };
};

struct ViewMatrix
{
	constexpr float* operator[](int index) noexcept
	{
		return data[index];
	}

	constexpr const float* operator[](int index) const noexcept
	{
		return data[index];
	}

	float data[4][4] = { };
};

struct Vector3
{
	constexpr Vector3(const float x = 0.f, const float y = 0.f, const float z = 0.f) noexcept :
		x(x), y(y), z(z) { }

	constexpr Vector3(const Vector3& other) noexcept
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

	constexpr Vector3 operator+(const Vector3& other) const noexcept
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	constexpr Vector3 operator-(const Vector3& other) const noexcept
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	constexpr Vector3 operator*(const Vector3& other) const noexcept
	{
		return Vector3(x * other.x, y * other.y, z * other.z);
	}

	constexpr Vector3 operator*(const float scale) const noexcept
	{
		return Vector3(x * scale, y * scale, z * scale);
	}

	float LengthTo(const Vector3& other) const noexcept
	{
		return std::sqrtf((x - other.x) * (x - other.x) +
			(y - other.y) * (y - other.y) +
			(z - other.z) * (z - other.z));
	}

	bool IsEqual(const Vector3& other, const float margin = std::numeric_limits<float>::epsilon()) const noexcept
	{
		return (std::fabsf(x - other.x) < margin &&
			std::fabsf(y - other.y) < margin &&
			std::fabsf(z - other.z) < margin);
	}

	constexpr Vector3 Scale(const float scale) const noexcept
	{
		return Vector3{ x * scale, y * scale, z * scale };
	}

	float x, y, z;
};
