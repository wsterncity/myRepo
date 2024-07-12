#ifndef iGameCellArray_h
#define iGameCellArray_h

#include "iGameObject.h"
#include "iGameIdList.h"
#include "iGameDataBuffer.h"
#include "iGameCell.h"

IGAME_NAMESPACE_BEGIN
class Tag : public Object{
public:
	I_OBJECT(Tag);
	static Pointer New() { return new Tag; }
	std::vector<uint8_t> DeletedTag;
	igIndex Size{ 0 };

	void Init(int taggedSize) {
		if (taggedSize > 0) {
			this->Reset();
			this->Size = (static_cast<size_t>(taggedSize) + 7) / 8;
			this->DeletedTag.resize(this->Size, 0);
		}
	}

	void Reset() {
		this->DeletedTag.clear();
		this->Size = 0;
	}

	void AddTag() {
		if (this->Size % 8 == 0)
		{
			this->DeletedTag.emplace_back(0);
		}
		this->Size++;
	}

	void MarkDeleted(igIndex id) noexcept {
		this->DeletedTag[id / 8] |= (1 << (id % 8));
	}

	bool IsDeleted(igIndex id) const noexcept {
		return (this->DeletedTag[id / 8] >> (id % 8)) & 1;
	}

protected:
	Tag() {}
	~Tag() override {}
};

class CellArray : public Object {
public:
	I_OBJECT(CellArray);
	static Pointer New() { return new CellArray; }

	void Initialize()
	{
		this->InitializeMemory();
		this->NumberOfCells = 0;
		this->DeletedMask->Reset();
	}

	bool Allocate(igIndex sz, int strategy = 0)
	{
		return this->AllocateInternal(sz, 0);
	}

	Cell* GetCell(igIndex cellId)
	{
		assert(!this->IsDeleted(cellId));
		igIndex begin = this->GetBeginOffset(cellId);
		int size = this->GetCellSize(cellId);
		igIndex* ptr = this->Buffer->Data + begin;
		this->Cell->Initialize(size, ptr);
		return this->Cell;
	}

	void GetCellAtId(igIndex cellId, igIndex& ncells, igIndex const*& cell)
	{
		assert(!this->IsDeleted(cellId));
		ncells = this->GetCellSize(cellId);
		cell = this->Buffer->Data + this->GetBeginOffset(cellId);
	}
	igIndex GetCellAtId(igIndex cellId, igIndex const*& cell)
	{
		assert(!this->IsDeleted(cellId));
		cell = this->Buffer->Data + this->GetBeginOffset(cellId);
		return this->GetCellSize(cellId);;
	}

	void GetCellAtId(igIndex cellId, igIndex& ncells, igIndex* cell)
	{
		assert(!this->IsDeleted(cellId));
		ncells = this->GetCellSize(cellId);
		igIndex* ptr = this->Buffer->Data + this->GetBeginOffset(cellId);
		for (int i = 0; i < ncells; i++)
		{
			cell[i] = ptr[i];
		}
	}
	igIndex GetCellAtId(igIndex cellId, igIndex* cell)
	{
		assert(!this->IsDeleted(cellId));
		igIndex ncells = this->GetCellSize(cellId);
		igIndex* ptr = this->Buffer->Data + this->GetBeginOffset(cellId);
		for (int i = 0; i < ncells; i++)
		{
			cell[i] = ptr[i];
		}
		return ncells;
	}

	void GetCellAtId(igIndex cellId, IdList::Pointer cell)
	{
		assert(!this->IsDeleted(cellId));
		cell->Reset();
		igIndex begin = this->GetBeginOffset(cellId);
		int size = this->GetCellSize(cellId);
		igIndex* pos = this->Buffer->Data + begin;
		for (int i = 0; i < size; i++) {
			cell->InsertNextId(pos[i]);
		}
	}

	void SetCell(igIndex cellId, igIndex* cell, int ncell) 
	{
		assert(!this->IsDeleted(cellId));
		assert(this->GetCellSize(cellId) == ncell);
		igIndex begin = this->GetBeginOffset(cellId);
		igIndex* ptr = this->Buffer->Data + begin;
		for (int i = 0; i < ncell; i++) {
			ptr[i] = cell[i];
		}
	}

	igIndex InsertNextCell(igIndex* cell, int ncell) 
	{
		igIndex beginOffset = this->GetBeginOffset(this->NumberOfCells);
		igIndex endOffset = beginOffset + ncell;
		if (endOffset > this->Buffer->Size) {
			igIndex newSize = std::max(endOffset, igIndex(beginOffset * 2));
			this->Buffer->Resize(newSize);
		}

		igIndex* Data = this->Buffer->Data + beginOffset;
		for (int i = 0; i < ncell; i++) {
			Data[i] = cell[i];
		}
		this->DeletedMask->AddTag();
		if (NumberOfCells == 0)
		{
			NumberOfComponent = ncell;
		}
		else if (NumberOfCells == -1)
		{
			this->Offset->AddId(endOffset);
		}
		else if (NumberOfComponent != ncell)
		{
			for (int i = 0; i < NumberOfCells; i++)
			{
				this->Offset->AddId(NumberOfComponent);
			}
			this->Offset->AddId(endOffset);
		}
		
		return this->NumberOfCells++;
	}

	void DeleteCell(igIndex cellId) {
		this->DeletedMask->MarkDeleted(cellId);
	}

	bool IsDeleted(igIndex cellId) {
		return this->DeletedMask->IsDeleted(cellId);
	}

	void GarbageCollection() {
		igIndex i, j, k = 0, begin = 0;
		igIndex* pos = this->Buffer->Data;
		for (i = 0; i < this->NumberOfCells; i++) {
			if (!IsDeleted(i)) {
				igIndex size = this->GetCellSize(i);
				igIndex* ptr = this->Buffer->Data + this->GetBeginOffset(i);
				for (j = 0; j < size; j++) {
					pos[j] = ptr[j];
				}
				pos += size;
				begin += size;
				if (NumberOfCells == -1)
				{
					this->Offset->SetId(k + 1, begin);
				}
				k++;
			}
		}
		this->Buffer->Resize(begin);
		if (NumberOfCells == -1 || k == 0)
		{
			this->Offset->Resize(k + 1);
		}
		this->DeletedMask->Init(k);
	}

	IndexArray::Pointer ConvertToDataArray()
	{
		if (NumberOfComponent == -1)
		{
			return nullptr;
		}
		IndexArray::Pointer ConvertedArray = IndexArray::New();
		ConvertedArray->SetArray(this->Buffer, NumberOfComponent, NumberOfCells * NumberOfComponent, this->Buffer->Size);
		return ConvertedArray;
	}

	igIndex* GetRawPointer() { return this->Buffer->Data; }
	igIndex* GetOffsetRawPointer() { return this->Offset->GetRawPointer(); }
	DataBuffer<igIndex>::Pointer GetBuffer() { return this->Buffer; }
	IdList::Pointer GetOffset() { return this->Offset; }
	void SetNumberOfCells(igIndex number) { this->AllocateInternal(number, number); }
	igIndex GetNumberOfCells() { return this->NumberOfCells; }

protected:
	CellArray() {
		this->Buffer = DataBuffer<igIndex>::New();
		this->DeletedMask = Tag::New();
		this->Offset = IdList::New();
		this->Offset->AddId(0);
	};
	~CellArray() override { }

	int GetBeginOffset(igIndex cellId) const {
		return NumberOfComponent == -1 ? this->Offset->GetId(cellId) : NumberOfComponent * cellId;
	}

	int GetEndOffset(igIndex cellId) const {
		return NumberOfComponent == -1 ? this->Offset->GetId(cellId + 1) : NumberOfComponent * (cellId + 1);
	}

	int GetCellSize(igIndex cellId) const {
		return NumberOfComponent == -1 ? this->Offset->GetId(cellId + 1) - this->Offset->GetId(cellId) : NumberOfComponent;
	}

	void InitializeMemory()
	{
		this->Buffer->Initialize();
		this->Offset->Initialize();
	}

	bool AllocateInternal(igIndex sz, igIndex numberOfCells)
	{
		if (sz > this->Buffer->Size)
		{
			this->InitializeMemory();
			this->Offset->Allocate(1);
			this->Offset->SetId(0, 0);
			this->Buffer->Resize(sz);
		}
		this->NumberOfCells = numberOfCells;
		return true;
	}

	DataBuffer<igIndex>::Pointer Buffer{};
	igIndex NumberOfCells{ 0 };
	igIndex NumberOfComponent{ -1 };
	IdList::Pointer Offset{};
	Tag::Pointer DeletedMask{};

	Cell::Pointer Cell{};
};



IGAME_NAMESPACE_END
#endif