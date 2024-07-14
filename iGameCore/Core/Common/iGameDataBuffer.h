#ifndef iGameDataBuffer_h
#define iGameDataBuffer_h

#include "iGameObject.h"

IGAME_NAMESPACE_BEGIN
template<typename TValueType>
class DataBuffer : public Object{
public:
	I_TEMPLATED_OBJECT(DataBuffer, TValueType);
	static Pointer New() { return new DataBuffer; }
	using ValueType = TValueType;

	void Initialize()
	{
		//if (this->Data)
		//{
		//	delete[] this->Data;
		//}
		this->Data = nullptr;
		this->Size = 0;
	}

	bool Allocate(size_t sz, int strategy = 0)
	{
		return this->AllocateInternal(sz);
	}

	bool Resize(const size_t sz)
	{
		ValueType* newData;
		size_t newSize;

		if (sz == this->Size)
		{
			return true;
		}
		else
		{
			newSize = sz;
		}

		if (newSize <= 0)
		{
			this->Initialize();
			return true;
		}

		if ((newData = new ValueType[newSize]) == nullptr)
		{
			return false;
		}

		if (this->Data)
		{
			memcpy(newData, this->Data,
				static_cast<size_t>(sz < this->Size ? sz : this->Size) * sizeof(ValueType));
			delete[] this->Data;
		}

		this->Size = newSize;
		this->Data = newData;
		return true;
	}

	ValueType* GetPointer() { return this->Data; }

	void Copy(ValueType* ps, size_t s) {
		delete[] this->Data;
		this->Data = ps;
		this->Size = s;
	}

	size_t GetSize() const { return this->Size; }
	size_t GetBytes() const { return this->Size * sizeof(ValueType); }

	ValueType* Data{ nullptr };
	size_t Size{ 0 };

protected:
	DataBuffer() {}
	~DataBuffer() 
	{
		this->Initialize();
	}

	bool AllocateInternal(igIndex sz)
	{
		if (sz > this->Size)
		{
			this->Initialize();
			this->Size = (sz > 0 ? sz : 1);
			this->Data = new ValueType[this->Size];
			if (this->Data == nullptr)
			{
				this->Size = 0;
				return false;
			}
		}
		return true;
	}


};
IGAME_NAMESPACE_END
#endif