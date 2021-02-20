#include "OutputMemoryBitStream.h"

OutputMemoryBitStream::OutputMemoryBitStream()
	:head(0), buffer(nullptr)
{
	ReallocBuffer(1500 * 8);
}

OutputMemoryBitStream::~OutputMemoryBitStream() {
	std::free(buffer); 
}


void OutputMemoryBitStream::ReallocBuffer(uint32_t bitLength) {
	if (buffer == nullptr) {
		//just need to memset on first allocation
		buffer = static_cast<char*>(std::malloc(bitLength >> 3));
		memset(buffer, 0, bitLength >> 3);
	}
	else {
		//need to memset, then copy the buffer
		char* tempBuffer = static_cast<char*>(std::malloc(bitLength >> 3));
		memset(tempBuffer, 0, bitLength >> 3);
		memcpy(tempBuffer, buffer, capacity >> 3);
		std::free(buffer);
		buffer = tempBuffer;
	}

	//handle realloc failure
	//...
	capacity = bitLength;
}

void OutputMemoryBitStream::WriteBits(uint8_t data, uint32_t bitCount) {
	uint32_t nextBitHead = head + static_cast<uint32_t>(bitCount);

	if (nextBitHead > capacity) {
		ReallocBuffer(std::max(capacity * 2, nextBitHead));
	}

	uint32_t byteOffset = head >> 3;
	uint32_t bitOffset = head & 0x7;

	uint8_t currentMask = ~(0xff << bitOffset);
	buffer[byteOffset] = (buffer[byteOffset] & currentMask) | (data << bitOffset);

	uint32_t bitsFreeThisByte = 8 - bitOffset;

	if (bitsFreeThisByte < bitCount)
		buffer[byteOffset + 1] = data >> bitsFreeThisByte;

	head = nextBitHead;
}

void OutputMemoryBitStream::WriteBits(const void* data, uint32_t bitCount) {
	const char* srcByte = static_cast<const char*>(data);
	while (bitCount > 8) {
		WriteBits(*srcByte, 8);
		++srcByte;
		bitCount -= 8;
	}

	if (bitCount > 0)
		WriteBits(*srcByte, bitCount);
}

void OutputMemoryBitStream::WriteBytes(const void* data, uint32_t byteCount) {
	WriteBits(data, byteCount << 3);
}

void OutputMemoryBitStream::Write(bool data) {
	WriteBits(&data, 1); 
}


template< typename T >
void OutputMemoryBitStream::Write(T data, uint32_t bitCount){
	static_assert(std::is_arithmetic< T >::value || std::is_enum< T >::value,
		"Generic Write only supports primitive data types");
	WriteBits(&data, bitCount);
}

void OutputMemoryBitStream::Write(const std::string& str) {
	uint32_t elementCount = static_cast<uint32_t>(str.size());
	Write(elementCount);
	for (const auto& element : str)
		Write(element);
}

void OutputMemoryBitStream::Write(const Vector3& vector) {
	Write(vector.x);
	Write(vector.z);
	if (vector.y == 0)
		Write(true);
	else {
		Write(false);
		Write(vector.y);
	}
	
}

void OutputMemoryBitStream::Write(const Quaternion& quaternion) {
	float precision = (2.f / 65535.f);
	Write(ConvertToFixed(quaternion.x, -1.f, precision), 16);
	Write(ConvertToFixed(quaternion.y, -1.f, precision), 16);
	Write(ConvertToFixed(quaternion.z, -1.f, precision), 16);
	Write(quaternion.w < 0);
}


