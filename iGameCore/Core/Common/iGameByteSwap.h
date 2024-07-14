#ifndef iGameByteSwap_h
#define iGameByteSwap_h

#include "iGameObject.h"
#include <ostream>
#include "iGameType.h"

IGAME_NAMESPACE_BEGIN
class ByteSwap : public Object {
public:
	I_OBJECT(ByteSwap);
	static Pointer New() {return new ByteSwap;}
	///@{
	/**
	 * Type-safe swap signatures to swap for storage in either Little
	 * Endian or Big Endian format.  Swapping is performed according to
	 * the true size of the type given.
	 */
#define IGAME_BYTE_SWAP_DECL(T)                                                                      \
  static void SwapLE(T* p);                                                                        \
  static void SwapBE(T* p);                                                                        \
  static void SwapLERange(T* p, size_t num);                                                       \
  static void SwapBERange(T* p, size_t num);                                                       \
  static bool SwapLERangeWrite(const T* p, size_t num, FILE* file);                                \
  static bool SwapBERangeWrite(const T* p, size_t num, FILE* file);                                \
  static void SwapLERangeWrite(const T* p, size_t num, std::ostream* os);                               \
  static void SwapBERangeWrite(const T* p, size_t num, std::ostream* os)
	IGAME_BYTE_SWAP_DECL(float);
	IGAME_BYTE_SWAP_DECL(double);
	IGAME_BYTE_SWAP_DECL(char);
	IGAME_BYTE_SWAP_DECL(short);
	IGAME_BYTE_SWAP_DECL(int);
	IGAME_BYTE_SWAP_DECL(long);
	IGAME_BYTE_SWAP_DECL(long long);
	IGAME_BYTE_SWAP_DECL(signed char);
	IGAME_BYTE_SWAP_DECL(unsigned char);
	IGAME_BYTE_SWAP_DECL(unsigned short);
	IGAME_BYTE_SWAP_DECL(unsigned int);
	IGAME_BYTE_SWAP_DECL(unsigned long);
	IGAME_BYTE_SWAP_DECL(unsigned long long);
#undef IGAME_BYTE_SWAP_DECL
	///@}

	///@{
	/**
	 * Swap 2, 4, or 8 bytes for storage as Little Endian.
	 */
	static void Swap2LE(void* p);
	static void Swap4LE(void* p);
	static void Swap8LE(void* p);
	///@}

	///@{
	/**
	 * Swap a block of 2-, 4-, or 8-byte segments for storage as Little Endian.
	 */
	static void Swap2LERange(void* p, size_t num);
	static void Swap4LERange(void* p, size_t num);
	static void Swap8LERange(void* p, size_t num);
	///@}

	///@{
	/**
	 * Swap a block of 2-, 4-, or 8-byte segments for storage as Little Endian.
	 * The results are written directly to a file to avoid temporary storage.
	 */
	static bool SwapWrite2LERange(void const* p, size_t num, FILE* f);
	static bool SwapWrite4LERange(void const* p, size_t num, FILE* f);
	static bool SwapWrite8LERange(void const* p, size_t num, FILE* f);
	static void SwapWrite2LERange(void const* p, size_t num, std::ostream* os);
	static void SwapWrite4LERange(void const* p, size_t num, std::ostream* os);
	static void SwapWrite8LERange(void const* p, size_t num, std::ostream* os);
	///@}

	///@{
	/**
	 * Swap 2, 4, or 8 bytes for storage as Big Endian.
	 */
	static void Swap2BE(void* p);
	static void Swap4BE(void* p);
	static void Swap8BE(void* p);
	///@}

	///@{
	/**
	 * Swap a block of 2-, 4-, or 8-byte segments for storage as Big Endian.
	 */
	static void Swap2BERange(void* p, size_t num);
	static void Swap4BERange(void* p, size_t num);
	static void Swap8BERange(void* p, size_t num);
	///@}

	///@{
	/**
	 * Swap a block of 2-, 4-, or 8-byte segments for storage as Big Endian.
	 * The results are written directly to a file to avoid temporary storage.
	 */
	static bool SwapWrite2BERange(void const* p, size_t num, FILE* f);
	static bool SwapWrite4BERange(void const* p, size_t num, FILE* f);
	static bool SwapWrite8BERange(void const* p, size_t num, FILE* f);
	static void SwapWrite2BERange(void const* p, size_t num, std::ostream* os);
	static void SwapWrite4BERange(void const* p, size_t num, std::ostream* os);
	static void SwapWrite8BERange(void const* p, size_t num, std::ostream* os);
	///@}

	/**
	 * Swaps the bytes of a buffer.  Uses an arbitrary word size, but
	 * assumes the word size is divisible by two.
	 */
	static void SwapVoidRange(void* buffer, size_t numWords, size_t wordSize);

protected:
	ByteSwap();
	~ByteSwap() override ;

};
IGAME_NAMESPACE_END
#endif
