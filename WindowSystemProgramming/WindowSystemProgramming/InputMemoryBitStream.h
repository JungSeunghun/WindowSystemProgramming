#pragma once
#include "Define.h"
#include "Transform.h"

inline float ConvertFromFixed(uint32_t inNumber, float inMin, float inPrecision)
{
	return static_cast<float>(inNumber) * inPrecision + inMin;
}

class InputMemoryBitStream
{
private:
	char* buffer;
	uint32_t head;
	uint32_t capacity;
	bool isBufferOwner;

public:
	InputMemoryBitStream(char* buffer, uint32_t bitCount);
	InputMemoryBitStream(const InputMemoryBitStream& other);
	~InputMemoryBitStream();

	const char* GetBufferPtr() const { return buffer; }
	uint32_t GetRemainingBitCount() const { return capacity - head; }
	void ResetToCapacity(uint32_t capacity) { this->capacity = capacity << 3; head = 0; }

	void ReadBits(uint8_t& data, uint32_t bitCount);
	void ReadBits(void* data, uint32_t bitCount);
	void ReadBytes(void* data, uint32_t byteCount);

	template< typename T >
	void Read(T& data, uint32_t bitCount = sizeof(T) * 8);

	void Read(bool& data);

	void Read(Quaternion& quaternion);
	void Read(Vector3& vector);

	void Read(std::string& str);
};
