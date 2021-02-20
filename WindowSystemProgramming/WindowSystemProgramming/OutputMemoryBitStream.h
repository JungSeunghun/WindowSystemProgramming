#pragma once
#include "Define.h"
#include "Transform.h"

inline uint32_t ConvertToFixed(float inNumber, float inMin, float inPrecision)
{
	return static_cast<int> ((inNumber - inMin) / inPrecision);
}

class OutputMemoryBitStream
{
private:
	char* buffer;
	uint32_t head;
	uint32_t capacity;

	void ReallocBuffer(uint32_t bitLength);

public:
	OutputMemoryBitStream();
	~OutputMemoryBitStream();

	const char* GetBuffer() const { return buffer; }
	uint32_t GetBitLength() const { return head; }
	uint32_t GetByteLength() const { return (head + 7) >> 3; }

	void WriteBits(uint8_t data, uint32_t bitCount);
	void WriteBits(const void* data, uint32_t bitCount);
	void WriteBytes(const void* data, uint32_t byteCount);

	template< typename T >
	void Write(T data, uint32_t bitCount = sizeof(T) * 8);

	void Write(bool data);
	void Write(const std::string& str);

	void Write(const Vector3& vector);
	void Write(const Quaternion& quaternion);
};