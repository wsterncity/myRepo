#include "iGameDataArray.h"

IGAME_NAMESPACE_BEGIN
DataArray::Pointer DataArray::CreateArray(int type)
{
	switch (type)
	{
	case IG_CHAR:
		return CharArray::New();

	case IG_UNSIGNED_CHAR:
		return UnsignedCharArray::New();

	case IG_SHORT:
		return ShortArray::New();

	case IG_UNSIGNED_SHORT:
		return UnsignedShortArray::New();

	case IG_INT:
		return IntArray::New();

	case IG_UNSIGNED_INT:
		return UnsignedIntArray::New();

	case IG_LONG_LONG:
		return LongLongArray::New();

	case IG_UNSIGNED_LONG_LONG:
		return UnsignedLongLongArray::New();

	case IG_FLOAT:
		return FloatArray::New();

	case IG_DOUBLE:
		return DoubleArray::New();

	default:
		break;
	}
	return nullptr;
}

int DataArray::GetDataTypeSize(int type)
{
	switch (type)
	{
	case IG_CHAR:
	case IG_UNSIGNED_CHAR:
		return 1;
	case IG_SHORT:
	case IG_UNSIGNED_SHORT:
		return 2;
	case IG_INT:
	case IG_UNSIGNED_INT:
	case IG_FLOAT:
		return 4;
	case IG_LONG_LONG:
	case IG_UNSIGNED_LONG_LONG:
	case IG_DOUBLE:
		return 8;
	default:
		return 0;
	}
}

IGAME_NAMESPACE_END