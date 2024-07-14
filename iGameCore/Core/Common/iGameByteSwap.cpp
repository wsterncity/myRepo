#include "iGameByteSwap.h"

#include <cstdint>
#include <memory.h>

IGAME_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
ByteSwap::ByteSwap()
{

}
//------------------------------------------------------------------------------
ByteSwap::~ByteSwap()
{

}

//------------------------------------------------------------------------------
// Define swap functions for each type size.
template <size_t s>
struct ByteSwapper;
template <>
struct ByteSwapper<1>
{
	static inline void Swap(char*) {}
};
template <>
struct ByteSwapper<2>
{
	static inline void Swap(char* data)
	{
		const uint16_t& ref16 = *reinterpret_cast<uint16_t*>(data);
		*reinterpret_cast<uint16_t*>(data) = (ref16 >> 8) | (ref16 << 8);
	}
};
template <>
struct ByteSwapper<4>
{
	static inline void Swap(char* data)
	{
		const uint32_t& ref32 = *reinterpret_cast<uint32_t*>(data);
		*reinterpret_cast<uint32_t*>(data) =
			(ref32 >> 24) | (ref32 << 24) | ((ref32 & 0x00ff0000) >> 8) | ((ref32 & 0x0000ff00) << 8);
	}
};
template <>
struct ByteSwapper<8>
{
	static inline void Swap(char* data)
	{
		const uint64_t& ref64 = *reinterpret_cast<uint64_t*>(data);
		*reinterpret_cast<uint64_t*>(data) = (ref64 >> 56) | (ref64 << 56) |
			((ref64 & 0x00ff000000000000) >> 40) | ((ref64 & 0x000000000000ff00) << 40) |
			((ref64 & 0x0000ff0000000000) >> 24) | ((ref64 & 0x0000000000ff0000) << 24) |
			((ref64 & 0x000000ff00000000) >> 8) | ((ref64 & 0x00000000ff000000) << 8);
	}
};

//------------------------------------------------------------------------------
// Define range swap functions.
template <class T>
inline void ByteSwapRange(T* first, size_t num)
{
	// Swap one value at a time.
	T* last = first + num;
	for (T* p = first; p != last; ++p)
	{
		ByteSwapper<sizeof(T)>::Swap(reinterpret_cast<char*>(p));
	}
}
inline bool ByteSwapRangeWrite(const char* first, size_t num, FILE* f, int)
{
	// No need to swap segments of 1 byte.
	size_t status = fwrite(first, sizeof(char), static_cast<size_t>(num), f);
	return status == static_cast<size_t>(num);
}
inline bool ByteSwapRangeWrite(const signed char* first, size_t num, FILE* f, int)
{
	// No need to swap segments of 1 byte.
	size_t status = fwrite(first, sizeof(signed char), static_cast<size_t>(num), f);
	return status == static_cast<size_t>(num);
}
inline bool ByteSwapRangeWrite(const unsigned char* first, size_t num, FILE* f, int)
{
	// No need to swap segments of 1 byte.
	size_t status = fwrite(first, sizeof(unsigned char), static_cast<size_t>(num), f);
	return status == static_cast<size_t>(num);
}
template <class T>
inline bool ByteSwapRangeWrite(const T* first, size_t num, FILE* f, long)
{
	// Swap and write one value at a time.  We do not need to do this in
	// blocks because the file stream is already buffered.
	const T* last = first + num;
	bool result = true;
	for (const T* p = first; p != last && result; ++p)
	{
		// Use a union to avoid breaking C++ aliasing rules.
		union {
			T value;
			char data[sizeof(T)];
		} temp = { *p };
		ByteSwapper<sizeof(T)>::Swap(temp.data);
		size_t status = fwrite(temp.data, sizeof(T), 1, f);
		result = status == 1;
	}
	return result;
}
inline void ByteSwapRangeWrite(const char* first, size_t num, std::ostream* os, int)
{
	// No need to swap segments of 1 byte.
	os->write(first, num * static_cast<size_t>(sizeof(char)));
}
inline void ByteSwapRangeWrite(const signed char* first, size_t num, std::ostream* os, int)
{
	// No need to swap segments of 1 byte.
	os->write(reinterpret_cast<const char*>(first), num * static_cast<size_t>(sizeof(signed char)));
}
inline void ByteSwapRangeWrite(const unsigned char* first, size_t num, std::ostream* os, int)
{
	// No need to swap segments of 1 byte.
	os->write(reinterpret_cast<const char*>(first), num * static_cast<size_t>(sizeof(unsigned char)));
}
template <class T>
inline void ByteSwapRangeWrite(const T* first, size_t num, std::ostream* os, long)
{
	// Swap and write one value at a time.  We do not need to do this in
	// blocks because the file stream is already buffered.
	const T* last = first + num;
	for (const T* p = first; p != last; ++p)
	{
		// Use a union to avoid breaking C++ aliasing rules.
		union {
			T value;
			char data[sizeof(T)];
		} temp = { *p };
		ByteSwapper<sizeof(T)>::Swap(temp.data);
		os->write(temp.data, sizeof(T));
	}
}

//------------------------------------------------------------------------------
// Define swap functions for each endian-ness.
#if defined(IGAME_WORDS_BIGENDIAN)
template <class T>
inline void ByteSwapBE(T*)
{
}
template <class T>
inline void ByteSwapBERange(T*, size_t)
{
}
template <class T>
inline bool ByteSwapBERangeWrite(const T* p, size_t num, FILE* f)
{
	size_t status = fwrite(p, sizeof(T), static_cast<size_t>(num), f);
	return status == static_cast<size_t>(num);
}
template <class T>
inline void ByteSwapBERangeWrite(const T* p, size_t num, std::ostream* os)
{
	os->write((char*)p, sizeof(T) * num);
}
template <class T>
inline void ByteSwapLE(T* p)
{
	ByteSwapper<sizeof(T)>::Swap(reinterpret_cast<char*>(p));
}
template <class T>
inline void ByteSwapLERange(T* p, size_t num)
{
	ByteSwapRange(p, num);
}
template <class T>
inline bool ByteSwapLERangeWrite(const T* p, size_t num, FILE* f)
{
	return ByteSwapRangeWrite(p, num, f, 1);
}
template <class T>
inline void ByteSwapLERangeWrite(const T* p, size_t num, std::ostream* os)
{
	ByteSwapRangeWrite(p, num, os, 1);
}
#else
template <class T>
inline void ByteSwapBE(T* p)
{
	ByteSwapper<sizeof(T)>::Swap(reinterpret_cast<char*>(p));
}
template <class T>
inline void ByteSwapBERange(T* p, size_t num)
{
	ByteSwapRange(p, num);
}
template <class T>
inline bool ByteSwapBERangeWrite(const T* p, size_t num, FILE* f)
{
	return ByteSwapRangeWrite(p, num, f, 1);
}
template <class T>
inline void ByteSwapBERangeWrite(const T* p, size_t num, std::ostream* os)
{
	ByteSwapRangeWrite(p, num, os, 1);
}
template <class T>
inline void ByteSwapLE(T*)
{
}
template <class T>
inline void ByteSwapLERange(T*, size_t)
{
}
template <class T>
inline bool ByteSwapLERangeWrite(const T* p, size_t num, FILE* f)
{
	size_t status = fwrite(p, sizeof(T), static_cast<size_t>(num), f);
	return status == static_cast<size_t>(num);
}
template <class T>
inline void ByteSwapLERangeWrite(const T* p, size_t num, std::ostream* os)
{
	os->write(reinterpret_cast<const char*>(p), static_cast<size_t>(sizeof(T)) * num);
}
#endif

//------------------------------------------------------------------------------
#define IGAME_BYTE_SWAP_IMPL(T)                                                                      \
  void ByteSwap::SwapLE(T* p) { ByteSwapLE(p); }                                             \
  void ByteSwap::SwapBE(T* p) { ByteSwapBE(p); }                                             \
  void ByteSwap::SwapLERange(T* p, size_t num) { ByteSwapLERange(p, num); }                  \
  void ByteSwap::SwapBERange(T* p, size_t num) { ByteSwapBERange(p, num); }                  \
  bool ByteSwap::SwapLERangeWrite(const T* p, size_t num, FILE* file)                           \
  {                                                                                                \
    return ByteSwapLERangeWrite(p, num, file);                                                  \
  }                                                                                                \
  bool ByteSwap::SwapBERangeWrite(const T* p, size_t num, FILE* file)                           \
  {                                                                                                \
    return ByteSwapBERangeWrite(p, num, file);                                                  \
  }                                                                                                \
  void ByteSwap::SwapLERangeWrite(const T* p, size_t num, std::ostream* os)                          \
  {                                                                                                \
    ByteSwapLERangeWrite(p, num, os);                                                           \
  }                                                                                                \
  void ByteSwap::SwapBERangeWrite(const T* p, size_t num, std::ostream* os)                          \
  {                                                                                                \
    ByteSwapBERangeWrite(p, num, os);                                                           \
  }
IGAME_BYTE_SWAP_IMPL(float)
IGAME_BYTE_SWAP_IMPL(double)
IGAME_BYTE_SWAP_IMPL(char)
IGAME_BYTE_SWAP_IMPL(short)
IGAME_BYTE_SWAP_IMPL(int)
IGAME_BYTE_SWAP_IMPL(long)
IGAME_BYTE_SWAP_IMPL(long long)
IGAME_BYTE_SWAP_IMPL(signed char)
IGAME_BYTE_SWAP_IMPL(unsigned char)
IGAME_BYTE_SWAP_IMPL(unsigned short)
IGAME_BYTE_SWAP_IMPL(unsigned int)
IGAME_BYTE_SWAP_IMPL(unsigned long)
IGAME_BYTE_SWAP_IMPL(unsigned long long)
#undef IGAME_BYTE_SWAP_IMPL

#if IGAME_SIZEOF_SHORT == 2
typedef short ByteSwapType2;
#else
#error "..."
#endif

#if IGAME_SIZEOF_INT == 4
typedef int ByteSwapType4;
#else
#error "..."
#endif

#if IGAME_SIZEOF_DOUBLE == 8
typedef double ByteSwapType8;
#else
#error "..."
#endif

//------------------------------------------------------------------------------
#define IGAME_BYTE_SWAP_SIZE(S)                                                                      \
  void ByteSwap::Swap##S##LE(void* p)                                                           \
  {                                                                                                \
    ByteSwap::SwapLE(static_cast<ByteSwapType##S*>(p));                                      \
  }                                                                                                \
  void ByteSwap::Swap##S##BE(void* p)                                                           \
  {                                                                                                \
    ByteSwap::SwapBE(static_cast<ByteSwapType##S*>(p));                                      \
  }                                                                                                \
  void ByteSwap::Swap##S##LERange(void* p, size_t n)                                            \
  {                                                                                                \
    ByteSwap::SwapLERange(static_cast<ByteSwapType##S*>(p), n);                              \
  }                                                                                                \
  void ByteSwap::Swap##S##BERange(void* p, size_t n)                                            \
  {                                                                                                \
    ByteSwap::SwapBERange(static_cast<ByteSwapType##S*>(p), n);                              \
  }                                                                                                \
  bool ByteSwap::SwapWrite##S##LERange(void const* p, size_t n, FILE* f)                        \
  {                                                                                                \
    return ByteSwap::SwapLERangeWrite(static_cast<const ByteSwapType##S*>(p), n, f);         \
  }                                                                                                \
  bool ByteSwap::SwapWrite##S##BERange(void const* p, size_t n, FILE* f)                        \
  {                                                                                                \
    return ByteSwap::SwapBERangeWrite(static_cast<const ByteSwapType##S*>(p), n, f);         \
  }                                                                                                \
  void ByteSwap::SwapWrite##S##LERange(void const* p, size_t n, std::ostream* os)                    \
  {                                                                                                \
    ByteSwap::SwapLERangeWrite(static_cast<const ByteSwapType##S*>(p), n, os);               \
  }                                                                                                \
  void ByteSwap::SwapWrite##S##BERange(void const* p, size_t n, std::ostream* os)                    \
  {                                                                                                \
    ByteSwap::SwapBERangeWrite(static_cast<const ByteSwapType##S*>(p), n, os);               \
  }
IGAME_BYTE_SWAP_SIZE(2)
IGAME_BYTE_SWAP_SIZE(4)
IGAME_BYTE_SWAP_SIZE(8)
#undef IGAME_BYTE_SWAP_SIZE

//------------------------------------------------------------------------------
// Swaps the bytes of a buffer.  Uses an arbitrary word size, but
// assumes the word size is divisible by two.
void ByteSwap::SwapVoidRange(void* buffer, size_t numWords, size_t wordSize)
{
	unsigned char temp, * out, * buf;
	size_t idx1, idx2, inc, half;

	half = wordSize / 2;
	inc = wordSize - 1;
	buf = static_cast<unsigned char*>(buffer);

	for (idx1 = 0; idx1 < numWords; ++idx1)
	{
		out = buf + inc;
		for (idx2 = 0; idx2 < half; ++idx2)
		{
			temp = *out;
			*out = *buf;
			*buf = temp;
			++buf;
			--out;
		}
		buf += half;
	}
}
IGAME_NAMESPACE_END
