#ifndef iGameCellArray_h
#define iGameCellArray_h

#include "iGameObject.h"
#include "iGameElementArray.h"
#include "iGameCell.h"
#include "iGameFlatArray.h"
#include "iGameMarker.h"

IGAME_NAMESPACE_BEGIN
class CellArray : public Object {
public:
	I_OBJECT(CellArray);
	static Pointer New() { return new CellArray; }

	void Initialize() 
	{
		m_Buffer->Initialize();
		m_Offsets->Initialize();
		m_Offsets->AddValue(0);
	}

	void Reserve(const IGsize _Newsize)
	{
		m_Buffer->Reserve(_Newsize);
	}

	void Resize(const IGsize _Newsize)
	{
		m_Buffer->Resize(_Newsize);
	}

	void Reset()
	{
		m_Buffer->Reset();
		m_Offsets->Reset();
		m_DeleteMasker->Reset();
	}

	void Squeeze()
	{
		m_Buffer->Squeeze();
		m_Offsets->Squeeze();
	}

	void SetNumberOfCells(const IGsize _Newsize)
	{
		m_NumberOfCells = _Newsize;
	}

	IGsize GetNumberOfCells() const noexcept
	{
		return m_NumberOfCells;
	}
	
	int GetCellIds(const IGsize cellId, igIndex* cell)
	{
		assert(!this->IsDeleted(cellId));
		int ncells = this->GetCellSize(cellId);
		igIndex* ptr = m_Buffer->RawPointer() + this->GetBeginOffset(cellId);
		for (int i = 0; i < ncells; i++)
		{
			cell[i] = ptr[i];
		}
		return ncells;
	}
	int GetCellIds(const IGsize cellId, const igIndex*& cell)
	{
		assert(!this->IsDeleted(cellId));
		cell = m_Buffer->RawPointer() + this->GetBeginOffset(cellId);
		return this->GetCellSize(cellId);
	}

	void GetCellIds(const IGsize cellId, IdArray::Pointer cell)
	{
		assert(!this->IsDeleted(cellId));
		cell->Reset();
		IGuint begin = this->GetBeginOffset(cellId);
		int size = this->GetCellSize(cellId);
		igIndex* pos = m_Buffer->RawPointer() + begin;
		for (int i = 0; i < size; i++) {
			cell->AddId(pos[i]);
		}
	}

	void SetCellIds(const IGsize cellId, igIndex* cell, int ncell)
	{
		assert(!this->IsDeleted(cellId));
		assert(this->GetCellSize(cellId) == ncell);
		IGuint begin = this->GetBeginOffset(cellId);
		igIndex* ptr = m_Buffer->RawPointer() + begin;
		for (int i = 0; i < ncell; i++) {
			ptr[i] = cell[i];
		}
	}

	IGsize AddCellIds(igIndex* cell, int ncell) 
	{
		IGuint beginOffset = this->GetBeginOffset(m_NumberOfCells);
		IGuint endOffset = beginOffset + ncell;
		if (endOffset > m_Buffer->GetNumberOfIds()) {
			const IGsize newSize = std::max(endOffset, (beginOffset * 2));
			m_Buffer->Resize(newSize);
		}

		igIndex* ptr = m_Buffer->RawPointer() + beginOffset;
		for (int i = 0; i < ncell; i++) {
			ptr[i] = cell[i];
		}
		m_DeleteMasker->AddTag();
		if (m_UseOffsets)
		{
			m_Offsets->AddValue(endOffset);
		}
		else
		{
			if (m_NumberOfCells == 0)
			{
				m_FixedCellSize = ncell;
			}
			else if (m_FixedCellSize != ncell)
			{
				for (int i = 0; i < m_NumberOfCells; i++)
				{
					m_Offsets->AddValue((i + 1) * m_FixedCellSize);
				}
				m_Offsets->AddValue(endOffset);
				m_UseOffsets = true;
			}	
		}

		return m_NumberOfCells++;
	}
	IGsize AddCellId2(igIndex val0, igIndex val1) {
		igIndex cell[2]{ val0,val1 };
		return this->AddCellIds(cell, 2);
	}
	IGsize AddCellId3(igIndex val0, igIndex val1, igIndex val2) {
		igIndex cell[3]{ val0,val1,val2 };
		return this->AddCellIds(cell, 3);
	}

	void ReplaceCellReference(const IGsize cellId, igIndex from, igIndex to) 
	{
		IGuint begin = this->GetBeginOffset(cellId);
		int size = this->GetCellSize(cellId);
		igIndex* pos = m_Buffer->RawPointer() + begin;
		for (int i = 0; i < size; i++) {
			if (pos[i] == from) {
				pos[i] = to;
			}
		}
	}

	void DeleteCell(const IGsize cellId)
	{
		m_DeleteMasker->MarkDeleted(cellId);
	}

	bool IsDeleted(const IGsize cellId) {
		return m_DeleteMasker->IsDeleted(cellId);
	}

	void GarbageCollection() {
		igIndex i, j, k = 0, begin = 0;
		igIndex* pos = m_Buffer->RawPointer();
		for (i = 0; i < m_NumberOfCells; i++) {
			if (!IsDeleted(i)) {
				int size = this->GetCellSize(i);
				igIndex* ptr = m_Buffer->RawPointer() + this->GetBeginOffset(i);
				for (j = 0; j < size; j++) {
					pos[j] = ptr[j];
				}
				pos += size;
				begin += size;
				if (m_UseOffsets)
				{
					m_Offsets->SetValue(k + 1, begin);
				}
				k++;
			}
		}
		m_Buffer->Resize(begin);
		if (m_UseOffsets || k == 0)
		{
			m_Offsets->Resize(k + 1);
		}
		m_DeleteMasker->Resize(k);
	}

	IdArray::Pointer GetCellIdArray() { return m_Buffer; }
	UnsignedIntArray::Pointer GetOffset() { return m_Offsets; }
	void SetData(IdArray* ids,UnsignedIntArray* offsets) { 
		m_Buffer = ids;
        m_Offsets = offsets;
	}

protected:
	CellArray() {
		m_Buffer = IdArray::New();
		m_DeleteMasker = DeleteMarker::New();
		m_Offsets = UnsignedIntArray::New();
		m_Offsets->AddValue(0);
	};
	~CellArray() override { }

	IGuint GetBeginOffset(const IGsize cellId) const {
		return m_UseOffsets ? 
			m_Offsets->GetValue(cellId) : 
			m_FixedCellSize * cellId;
	}

	IGuint GetEndOffset(const IGsize  cellId) const {
		return m_UseOffsets ? 
			m_Offsets->GetValue(cellId + 1) : 
			m_FixedCellSize * (cellId + 1);
	}

	IGuint GetCellSize(const IGsize  cellId) const {
		return m_UseOffsets ? 
			m_Offsets->GetValue(cellId + 1) - m_Offsets->GetValue(cellId) : 
			m_FixedCellSize;
	}

	IdArray::Pointer m_Buffer{};
	IGsize m_NumberOfCells{ 0 };

	UnsignedIntArray::Pointer m_Offsets{};

	int m_FixedCellSize{ -1 };
	bool m_UseOffsets{ false };
	DeleteMarker::Pointer m_DeleteMasker{};
};
IGAME_NAMESPACE_END
#endif