#ifndef iGameCellLinks_h
#define iGameCellLinks_h

#include "iGameObject.h"

IGAME_NAMESPACE_BEGIN
class CellLinks : public Object {
public:
	I_OBJECT(CellLinks);
	static Pointer New() { return new CellLinks; }

	struct Link
	{
		uint8_t capcity;
		uint8_t ncells;
		igIndex* cells;
	};

	void Allocate(igIndex numLinks, int ext = 0) {
		Link linkInit = { 0, 0, nullptr };

		this->Initialize();
		this->Size = numLinks;
		this->Array = new Link[numLinks];

		for (igIndex i = 0; i < numLinks; i++)
		{
			this->Array[i] = linkInit;
		}
	}

	void Initialize() {
		if (this->Array)
		{
			for (igIndex i = 0; i < this->Size; i++)
			{
				delete[] this->Array[i].cells;
			}

			delete[] this->Array;
			this->Array = nullptr;
		}
		this->Size = 0;
		this->MaxId = -1;
		this->NumberOfLinks = 0;
		this->NumberOfCells = 0;
	}

	Link& GetLink(igIndex linkId) { return this->Array[linkId]; }

	igIndex GetNcells(igIndex linkId) { return this->Array[linkId].ncells; }

	igIndex* GetCells(igIndex linkId) { return this->Array[linkId].cells; }

	igIndex InsertNextLink(int ncells = 0) {
		if (++this->MaxId >= this->Size)
		{
			this->Resize(this->Size + 1);
		}
		if (ncells > 0) 
		{
			this->ResizeLink(this->MaxId, ncells);
		}
		return this->MaxId;
	}

	void DeleteLink(igIndex linkId) {
		this->Array[linkId].ncells = this->Array[linkId].capcity = 0;
		delete[] this->Array[linkId].cells;
		this->Array[linkId].cells = nullptr;
	}

	void RemoveCellReference(igIndex cellId, igIndex linkId)
	{
		igIndex* cells = this->Array[linkId].cells;
		igIndex ncells = this->Array[linkId].ncells;

		for (igIndex i = 0; i < ncells; i++)
		{
			if (cells[i] == cellId)
			{
				for (igIndex j = i; j < (ncells - 1); j++)
				{
					cells[j] = cells[j + 1];
				}
				this->Array[linkId].ncells--;
				break;
			}
		}
	}

	void AddCellReference(igIndex cellId, igIndex linkId)
	{
		if (this->Array[linkId].ncells >= this->Array[linkId].capcity) {
			this->ResizeLink(linkId, this->Array[linkId].ncells);
		}
		this->Array[linkId].cells[this->Array[linkId].ncells++] = cellId;
	}

	void ResizeLink(igIndex linkId, int size)
	{
		if (size < 1) size = 1;
		igIndex newSize = this->Array[linkId].ncells + size;
		igIndex* cells = new igIndex[newSize];
		memcpy(cells, this->Array[linkId].cells,
			static_cast<size_t>(this->Array[linkId].ncells) * sizeof(igIndex));
		delete[] this->Array[linkId].cells;
		this->Array[linkId].cells = cells;
		this->Array[linkId].capcity = newSize;
	}

	void Squeeze()
	{
		this->Resize(this->MaxId + 1);
	}

	void Reset() 
	{
		this->MaxId = -1;
	}

	void IncrementLinkCount(igIndex linkId) { this->Array[linkId].ncells++; }

	void AllocateLinks(igIndex n) {
		for (igIndex i = 0; i < n; ++i)
		{
			this->Array[i].cells = new igIndex[this->Array[i].ncells];
		}
	}

protected:
	CellLinks() {};
	~CellLinks() override {};

	Link* Array;              // pointer to data
	igIndex Size;             // allocated size of data
	igIndex MaxId;            // maximum index inserted thus far

	igIndex NumberOfLinks;
	igIndex NumberOfCells;

	Link* Resize(igIndex sz) 
	{
		igIndex i;
		Link* newArray;
		igIndex newSize;
		Link linkInit = { 0, 0, nullptr };

		if (sz > this->Size)
		{
			newSize = this->Size + sz;
		}
		else if (sz == this->Size)
		{
			return this->Array;
		}
		else {
			newSize = sz;
		}

		newArray = new Link[newSize];

		for (i = 0; i < sz && i < this->Size; i++)
		{
			newArray[i] = this->Array[i];
		}

		for (i = this->Size; i < newSize; i++)
		{
			newArray[i] = linkInit;
		}

		this->Size = newSize;
		delete[] this->Array;
		this->Array = newArray;

		return this->Array;
	}
};
IGAME_NAMESPACE_END
#endif