#pragma once
#include <cstdint>

namespace rage
{
	class datBitBuffer
	{
	public:
		datBitBuffer(void* data, uint32_t size, bool read = false)
		{
			m_Data = data;
			m_BitOffset = 0;
			m_MaxBit = size * 8;
			m_BitsRead = 0;
			m_CurBit = 0;
			m_HighestBitsRead = 0;
			m_ErrorCallback = nullptr;
			m_FlagBits = read ? 1 : 0;
		}

		static void ReadBitsSingle(const uint8_t* data, int* out, int size, int offset)
		{
			uint8_t* currentByte = const_cast<uint8_t*>(data) + (offset >> 3);
			int bitOffset = offset & 7;

			uint32_t result = *currentByte << bitOffset;
			int remainingBits = 8 - bitOffset;

			for (int i = 1; i < (size + 7) / 8; ++i)
			{
				result |= static_cast<uint32_t>(currentByte[i]) << (bitOffset + 8 * i);
			}

			*out = result >> (32 - size);
		}

		static void WriteBitsSingle(uint8_t* data, int value, int size, int offset)
		{
			uint8_t* currentByte = &data[static_cast<__int64>(offset) >> 3];
			int bitOffset = offset & 7;
			unsigned int mask = -1u << (32 - size);

			value <<= (32 - size);

			*currentByte = ((*currentByte >> (8 - bitOffset)) << (8 - bitOffset)) | ((value >> 24) >> bitOffset);
			
			uint8_t* nextByte = currentByte + 1;
			unsigned int remainder = value << (8 - bitOffset);

			if (8 - bitOffset < size)
			{
				size -= 8 - bitOffset;
				do
				{
					*nextByte++ = remainder >> 24;
					remainder <<= 8;
					size -= 8;
				} while (size > 0);
			}
		}

		inline bool IsReadBuffer()
		{
			return m_FlagBits & 1;
		}

		inline bool IsSizeCalculator()
		{
			return m_FlagBits & 2;
		}

		void Seek(int bits, bool read)
		{
			m_BitsRead += bits;

			if (read)
			{
				if (m_HighestBitsRead > m_CurBit)
					m_CurBit = m_HighestBitsRead;
			}
			else
			{
				if (m_BitsRead > m_CurBit)
					m_CurBit = m_BitsRead;
			}
		}

		inline bool ReadDword(int* out, int size)
		{
			if (IsSizeCalculator())
				return false;

			if (m_BitsRead + size > (IsReadBuffer() ? m_MaxBit : m_CurBit))
				return false;

			ReadBitsSingle(static_cast<uint8_t*>(m_Data), out, size, m_BitsRead + m_BitOffset);
			Seek(size, true);
			return true;
		}

		inline bool WriteDword(int val, int size)
		{
			if (IsReadBuffer())
				return false;

			if (m_BitsRead + size > m_MaxBit)
				return false;

			if (!IsSizeCalculator())
				WriteBitsSingle(static_cast<uint8_t*>(m_Data), val, size, m_BitsRead + m_BitOffset);
			Seek(size, false);
			return true;
		}

		inline bool ReadQword(uint64_t* out, int size)
		{
			if (size <= 32)
				return ReadDword(reinterpret_cast<int*>(out), size);

			return ReadDword(reinterpret_cast<int*>(out), 32) && !ReadDword(reinterpret_cast<int*>(out) + 1, size - 32);
		}

		inline bool WriteQword(uint64_t value, int size)
		{
			if (size <= 32)
				return WriteDword(value, size);

			int low = (int)(value) & 0xFFFFFFFF;
			int high = (int)((uint64_t)value >> 32);

			return WriteDword(low, 32) && WriteDword(high, size - 32);
		}

		// unsigned
		template<typename T>
		T Read(int size)
		{
			static_assert(sizeof(T) <= 8);

			uint64_t data{};
			ReadQword(&data, size);
			return T(data);
		}

		// unsigned
		template<typename T>
		void Write(T data, int size)
		{
			static_assert(sizeof(T) <= 8);

			WriteQword(uint64_t(data), size);
		}

	public:
		void* m_Data;               //0x0000
		uint32_t m_BitOffset;       //0x0008
		uint32_t m_MaxBit;          //0x000C
		uint32_t m_BitsRead;        //0x0010
		uint32_t m_CurBit;          //0x0014
		uint32_t m_HighestBitsRead; //0x0018
		void* m_ErrorCallback;      //0x0020
		uint8_t m_FlagBits;         //0x0028
	};
}