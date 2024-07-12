#ifndef iGameDataArray_h
#define iGameDataArray_h

#include "iGameObject.h"
#include "iGameVectorD.h"
#include "iGameDataBuffer.h"

IGAME_NAMESPACE_BEGIN
class DataArray : public Object {
public:
	I_OBJECT(DataArray)
	static Pointer CreateArray(int type);

	/**
	 * Release memory and restore to unallocated state.
	 */
	void Initialize()
	{
		this->InitializeMemory();
		this->NumberOfValues = 0;
		this->Size = 0;
	}

	bool Allocate(igIndex numberOfValues, int strategy = 0)
	{
		return this->AllocateInternal(numberOfValues, 0);
	}

	int GetNumberOfComponents() const noexcept { return this->NumberOfComponents; }
	igIndex GetNumberOfTuples() const noexcept { return this->NumberOfValues / this->NumberOfComponents; }
	igIndex GetNumberOfValues() const noexcept { return this->NumberOfValues; }

	virtual double GetValue(igIndex valueId) = 0;
	virtual void GetTuple(igIndex tupleId, float* tuple) = 0;
	virtual void GetTuple(igIndex tupleId, double* tuple) = 0;

	virtual void SetValue(igIndex valueId, float value) = 0;
	virtual void SetValue(igIndex valueId, double value) = 0;
	virtual void SetTuple(igIndex tupleId, float* value) = 0;
	virtual void SetTuple(igIndex tupleId, double* value) = 0;

	virtual igIndex InsertNextValue(float value) = 0;
	virtual igIndex InsertNextValue(double value) = 0;
	virtual igIndex InsertNextTuple(float* tuple) = 0;
	virtual igIndex InsertNextTuple(double* tuple) = 0;

	igIndex InsertNextTuple2(double val0, double val1) 
	{
		double tuple[2]{ val0,val1 };
		return this->InsertNextTuple(tuple);
	}
	igIndex InsertNextTuple3(double val0, double val1, double val2)
	{
		double tuple[3]{ val0,val1,val2 };
		return this->InsertNextTuple(tuple);
	}
	igIndex InsertNextTuple4(double val0, double val1, double val2, double val3)
	{
		double tuple[4]{ val0,val1,val2,val3 };
		return this->InsertNextTuple(tuple);
	}

	void SetNumberOfValues(const igIndex numberOfValues)
	{
		this->AllocateInternal(numberOfValues, numberOfValues);
	}
	void SetNumberOfTuples(const igIndex numberOfTuples)
	{
		igIndex numberOfValues = numberOfTuples * this->NumberOfComponents;
		this->AllocateInternal(numberOfValues, numberOfValues);
	}
	void SetNumberOfComponents(const igIndex number)
	{
		if (number <= 0)
		{
			return;
		}
		this->NumberOfComponents = number;
	}

	virtual bool Resize(const igIndex numberOfTuples) = 0;

	/**
	 * Reset to an empty state but retain previously allocated memory.
	 */
	void Reset() { this->NumberOfValues = 0; }

	/**
	 * Free any unused memory.
	 */
	void Squeeze() { this->Resize(this->GetNumberOfTuples()); }

	virtual bool ShallowCopy(DataArray::Pointer other)
	{
		this->NumberOfComponents = other->NumberOfComponents;
		this->NumberOfValues = other->NumberOfValues;
		this->Size = other->Size;
		return true;
	}
	virtual bool DeepCopy(DataArray::Pointer other) 
	{
		this->NumberOfComponents = other->NumberOfComponents;
		this->NumberOfValues = other->NumberOfValues;
		this->Size = other->Size;
		return true;
	}

	// 获取数据类型的字节数
	static int GetDataTypeSize(int type);
	int GetDataTypeSize() const {
		return this->GetDataTypeSize(this->GetDataType());
	}
	virtual int GetDataType() const = 0;

protected:
	DataArray() {};
	~DataArray() override {};

	virtual bool AllocateInternal(igIndex sz, igIndex numberOfTuples) = 0;
	virtual void InitializeMemory() = 0;

	int NumberOfComponents{ 1 };
	igIndex NumberOfValues{ 0 };
	igIndex Size{ 0 };
};

template <typename TValueType>
class DataArrayTemplate : public DataArray {
public:
	I_TEMPLATED_OBJECT(DataArrayTemplate, TValueType);
	using ValueType = TValueType;

	double GetValue(igIndex valueId) override 
	{
		assert(0 <= valueId && valueId < this->GetNumberOfValues());
		return static_cast<double>(this->Buffer->Data[valueId]);
	}
	void GetTuple(igIndex tupleId, float* tuple) override
	{
		assert(0 <= tupleId && tupleId < this->GetNumberOfTuples());
		const igIndex valueId = tupleId * this->NumberOfComponents;
		ValueType* data = this->Buffer->Data + valueId;
		for (int i = 0; i < this->NumberOfComponents; ++i) {
			tuple[i] = static_cast<float>(data[i]);
		}
	}
	void GetTuple(igIndex tupleId, double* tuple) override
	{
		assert(0 <= tupleId && tupleId < this->GetNumberOfTuples());
		const igIndex valueId = tupleId * this->NumberOfComponents;
		ValueType* data = this->Buffer->Data + valueId;
		for (int i = 0; i < this->NumberOfComponents; ++i) {
			tuple[i] = static_cast<double>(data[i]);
		}
	}

	void SetValue(igIndex valueId, float value) override
	{
		assert(0 <= valueId && valueId < this->GetNumberOfValues());
		this->Buffer->Data[valueId] = static_cast<ValueType>(value);
	}
	void SetValue(igIndex valueId, double value) override
	{
		assert(0 <= valueId && valueId < this->GetNumberOfValues());
		this->Buffer->Data[valueId] = static_cast<ValueType>(value);
	}
	void SetTuple(igIndex tupleId, float* tuple) override
	{
		assert(0 <= tupleId && tupleId < this->GetNumberOfTuples());
		const igIndex valueId = tupleId * this->NumberOfComponents;
		ValueType* data = this->Buffer->Data + valueId;
		for (int i = 0; i < this->NumberOfComponents; ++i) {
			data[i] = static_cast<ValueType>(tuple[i]);
		}
	}
	void SetTuple(igIndex tupleId, double* tuple) override
	{
		assert(0 <= tupleId && tupleId < this->GetNumberOfTuples());
		const igIndex valueId = tupleId * this->NumberOfComponents;
		ValueType* data = this->Buffer->Data + valueId;
		for (int i = 0; i < this->NumberOfComponents; ++i) {
			data[i] = static_cast<ValueType>(tuple[i]);
		}
	}

	igIndex InsertNextValue(float value) override
	{
		if (this->NumberOfValues >= this->Size)
		{
			igIndex numTuples = this->GetNumberOfTuples();
			if (!this->Resize(2 * numTuples + 1)) // grow by factor of 2
			{
				return this->NumberOfValues - 1;
			}
		}

		this->Buffer->Data[this->NumberOfValues] = static_cast<ValueType>(value);
		return this->NumberOfValues++;
	}
	igIndex InsertNextValue(double value) override
	{
		if (this->NumberOfValues >= this->Size)
		{
			igIndex numTuples = this->GetNumberOfTuples();
			if (!this->Resize(2 * numTuples + 1)) // grow by factor of 2
			{
				return this->NumberOfValues - 1;
			}
		}

		this->Buffer->Data[this->NumberOfValues] = static_cast<ValueType>(value);
		return this->NumberOfValues++;
	}
	igIndex InsertNextTuple(float* tuple) override
	{
		igIndex tupleId = this->GetNumberOfTuples();
		if (tupleId * this->NumberOfComponents >= this->Size)
		{
			if (!this->Resize(2 * tupleId + 1)) // grow by factor of 2
			{
				return tupleId - 1;
			}
		}

		ValueType* data = this->Buffer->Data + tupleId * this->NumberOfComponents;
		for (int i = 0; i < this->NumberOfComponents; ++i) {
			data[i] = static_cast<ValueType>(tuple[i]);
			this->NumberOfValues++;
		}
		return tupleId;
	}
	igIndex InsertNextTuple(double* tuple) override
	{
		igIndex tupleId = this->GetNumberOfTuples();
		if (tupleId * this->NumberOfComponents >= this->Size)
		{
			if (!this->Resize(2 * tupleId + 1)) // grow by factor of 2
			{
				return tupleId - 1;
			}
		}

		ValueType* data = this->Buffer->Data + tupleId * this->NumberOfComponents;
		for (int i = 0; i < this->NumberOfComponents; ++i) {
			data[i] = static_cast<ValueType>(tuple[i]);
			this->NumberOfValues++;
		}
		return tupleId;
	}

	bool Resize(const igIndex numberOfTuples) override
	{
		igIndex newSize = numberOfTuples * this->NumberOfComponents;

		if (newSize == this->Size)
		{
			return true;
		}

		if (newSize <= 0)
		{
			this->Initialize();
			return true;
		}

		this->Buffer->Resize(newSize);

		if (this->NumberOfValues > newSize)
		{
			this->NumberOfValues = newSize;
		}

		this->Size = newSize;
		return true;
	}

	bool ShallowCopy(DataArray::Pointer other) override
	{
		Pointer o{};
		if ((o = DynamicCast<Self>(other)) != nullptr)
		{
			this->Buffer = o->Buffer;
			return DataArray::ShallowCopy(other);
		}
		return false;
	}

	bool DeepCopy(DataArray::Pointer other) override
	{
		Pointer o{};
		if ((o = DynamicCast<Self>(other)) != nullptr)
		{
			this->Buffer->Resize(o->Buffer->Size);
			for (int i = 0; i < o->Buffer->Size; i++)
			{
				this->Buffer->Data[i] = o->Buffer->Data[i];
			}
			return DataArray::DeepCopy(other);
		}
		return false;
	}

	bool SetArray(SmartPointer<DataBuffer<TValueType>> dataBuffer, int numberOfcomponent, igIndex numberOfValues, igIndex size)
	{
		if (dataBuffer == nullptr || numberOfcomponent < 1 || numberOfValues < 0 || size < 0)
		{
			return false;
		}
		this->Buffer = dataBuffer;
		this->NumberOfComponents = numberOfcomponent;
		this->NumberOfValues = numberOfValues;
		this->Size = size;
		return true;
	}

	TValueType* GetRawPointer() {
		return this->Buffer->GetPointer();
	}

protected:
	DataArrayTemplate() 
	{
		this->Buffer = DataBuffer<TValueType>::New();
	}
	~DataArrayTemplate() override {}

	bool AllocateInternal(igIndex sz, igIndex numberOfValues) override
	{
		if (sz > this->Size)
		{
			this->InitializeMemory();
			this->Size = (sz > 0 ? sz : 1);
			this->Buffer->Allocate(this->Size);
		}
		this->NumberOfValues = numberOfValues;
		return true;
	}
	void InitializeMemory() override
	{
		this->Buffer->Initialize();
	}

	SmartPointer<DataBuffer<TValueType>> Buffer{};
};

#define iGameNewDataArrayMacro(TypeName, ValueType, DataType)    \
class TypeName : public DataArrayTemplate<ValueType> {           \
public:                                                          \
	I_OBJECT(TypeName);                                          \
	static Pointer New() { return new TypeName; }                \
	int GetDataType() const override { return DataType; }        \
protected:														 \
	TypeName() = default;                                        \
	~TypeName() override = default;                              \
};

iGameNewDataArrayMacro(FloatArray, float, IG_FLOAT)
iGameNewDataArrayMacro(DoubleArray, double, IG_DOUBLE)

iGameNewDataArrayMacro(IntArray, int, IG_INT)
iGameNewDataArrayMacro(UnsignedIntArray, unsigned int, IG_UNSIGNED_INT)

iGameNewDataArrayMacro(CharArray, char, IG_CHAR)
iGameNewDataArrayMacro(UnsignedCharArray, unsigned char, IG_UNSIGNED_CHAR)

iGameNewDataArrayMacro(ShortArray, short, IG_SHORT)
iGameNewDataArrayMacro(UnsignedShortArray, unsigned short, IG_UNSIGNED_SHORT)

iGameNewDataArrayMacro(LongLongArray, long long, IG_LONG_LONG)
iGameNewDataArrayMacro(UnsignedLongLongArray, unsigned long long, IG_UNSIGNED_LONG_LONG)

iGameNewDataArrayMacro(IndexArray, igIndex, IG_INDEX)

IGAME_NAMESPACE_END
#endif