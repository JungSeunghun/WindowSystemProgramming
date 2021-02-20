#include "InputMemoryBitStream.h"

InputMemoryBitStream::InputMemoryBitStream(char* buffer, uint32_t bitCount)
	: buffer(buffer), capacity(bitCount), head(0), isBufferOwner(false)
{}

InputMemoryBitStream::InputMemoryBitStream(const InputMemoryBitStream& other) 
	: capacity(other.capacity), head(other.head), isBufferOwner(true)
{
	//allocate buffer of right size
	int byteCount = capacity / 8;
	buffer = static_cast<char*>(malloc(byteCount));
	//copy
	memcpy(buffer, other.buffer, byteCount);
}

InputMemoryBitStream::~InputMemoryBitStream() {
	if (isBufferOwner)
		free(buffer);
}

void InputMemoryBitStream::ReadBits(uint8_t& data, uint32_t bitCount)
{
	uint32_t byteOffset = head >> 3;
	uint32_t bitOffset = head & 0x7;

	data = static_cast<uint8_t>(buffer[byteOffset]) >> bitOffset;

	uint32_t bitsFreeThisByte = 8 - bitOffset;
	if (bitsFreeThisByte < bitCount) {
		//we need another byte
		data |= static_cast<uint8_t>(buffer[byteOffset + 1]) << bitsFreeThisByte;
	}

	//don't forget a mask so that we only read the bit we wanted...
	data &= (~(0x00ff << bitCount));

	head += bitCount;
}

void InputMemoryBitStream::ReadBits(void* data, uint32_t bitCount) {
	uint8_t* destByte = reinterpret_cast<uint8_t*>(data);
	//write all the bytes
	while (bitCount > 8) {
		ReadBits(*destByte, 8);
		++destByte;
		bitCount -= 8;
	}
	//write anything left
	if (bitCount > 0)
		ReadBits(*destByte, bitCount);
}

void InputMemoryBitStream::ReadBytes(void* data, uint32_t byteCount) {
	ReadBits(data, byteCount << 3); 
}

template< typename T >
void InputMemoryBitStream::Read(T& data, uint32_t bitCount) {
	static_assert(std::is_arithmetic< T >::value ||	std::is_enum< T >::value,
		"Generic Read only supports primitive data types");
	ReadBits(&data, bitCount);
}

void InputMemoryBitStream::Read(bool& data) {
	ReadBits(&data, 1); 
}

void InputMemoryBitStream::Read(Quaternion& quaternion)
{
	float precision = (2.f / 65535.f);

	uint32_t f = 0;

	Read(f, 16);
	quaternion.x = ConvertFromFixed(f, -1.f, precision);
	Read(f, 16);
	quaternion.y = ConvertFromFixed(f, -1.f, precision);
	Read(f, 16);
	quaternion.z = ConvertFromFixed(f, -1.f, precision);

	quaternion.w = sqrtf(1.f -	
		(quaternion.x * quaternion.x +
			quaternion.y * quaternion.y +
			quaternion.z * quaternion.z));
	bool isNegative;
	Read(isNegative);

	if (isNegative)
		quaternion.w *= -1;
}

void InputMemoryBitStream::Read(Vector3& vector)
{
	Read(vector.x);
	Read(vector.z);

	bool isZero;
	Read(isZero);
	if (!isZero)
		Read(vector.y);
}

void InputMemoryBitStream::Read(std::string& str)
{
	uint32_t elementCount;
	Read(elementCount);
	str.resize(elementCount);
	for (auto& element : str) {
		Read(element);
	}
}