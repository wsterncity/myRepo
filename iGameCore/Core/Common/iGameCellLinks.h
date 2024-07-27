#ifndef iGameCellLinks_h
#define iGameCellLinks_h

#include "iGameObject.h"
#include "iGameElementArray.h"

IGAME_NAMESPACE_BEGIN
class CellLinks : public Object {
public:
	I_OBJECT(CellLinks);
	static Pointer New() { return new CellLinks; }

	struct Link
	{
		uint8_t capcity{0};
		uint8_t size{0};
		igIndex* pointer{nullptr};
	};

	void Allocate(const IGsize numLinks, int ext = 0) {
		this->Initialize();
		this->Resize(numLinks);
	}

	void Initialize() {
		m_Buffer->Initialize();
	}

	void Reserve(const IGsize _Newsize)
	{
		m_Buffer->Reserve(_Newsize);
	}

	void Resize(const IGsize _Newsize)
	{
		m_Buffer->Resize(_Newsize);
	}

	void SetNumberOfLinks(const IGsize _Newsize) {
		this->Resize(_Newsize);
	}

	IGsize GetNumberOfLinks() const noexcept {
		return m_Buffer->GetNumberOfElements();
	}

	void Squeeze()
	{
		m_Buffer->Squeeze();
	}

	void Reset()
	{
		m_Buffer->Reset();
	}

	Link& GetLink(const IGsize linkId) { return m_Buffer->ElementAt(linkId); }

	int GetLinkSize(const IGsize linkId) { return m_Buffer->ElementAt(linkId).size; }

	igIndex* GetLinkPointer(const IGsize linkId) { return m_Buffer->ElementAt(linkId).pointer; }
	const igIndex* GetLinkPointer(const IGsize linkId) const { return m_Buffer->ElementAt(linkId).pointer; }

	IGsize AddLink(uint8_t size = 0) {
		m_Buffer->AddElement(Link{ 0,0,nullptr });
		if (size > 0)
		{
			this->ResizeLink(this->GetNumberOfLinks() - 1, size);
		}
		return this->GetNumberOfLinks() - 1;
	}

	void DeleteLink(const IGsize linkId) {
		Link& link = this->GetLink(linkId);
		link.size = link.capcity = 0;
		if (link.pointer)
		{
			delete[] link.pointer;
		}
		link.pointer = nullptr;
	}

	void RemoveReference(const IGsize cellId, const IGsize linkId)
	{
		Link& link = this->GetLink(linkId);

		for (int i = 0; i < link.size; i++)
		{
			if (link.pointer[i] == cellId)
			{
				for (int j = i; j < (link.size - 1); j++)
				{
					link.pointer[j] = link.pointer[j + 1];
				}
				link.size--;
				break;
			}
		}
	}

	void AddReference(const IGsize cellId, const IGsize linkId)
	{
		Link& link = this->GetLink(linkId);
		if (link.size >= link.capcity) {
			this->ResizeLink(linkId, 2 * link.size + 1);
		}
		link.pointer[link.size++] = cellId;
	}

	void ResizeLink(const IGsize linkId, uint8_t _Newsize)
	{
		Link& link = this->GetLink(linkId);
		igIndex* cells = nullptr;

		if ((cells = new igIndex[_Newsize]) == nullptr) {
			return;
		}

		memcpy(cells, link.pointer, static_cast<size_t>(
			_Newsize < link.size ? _Newsize : link.size) * sizeof(igIndex));

		if (link.pointer)
		{
			delete[] link.pointer;
		}
		
		link.pointer = cells;
		link.capcity = _Newsize;
		if (link.size > link.capcity) {
			link.size = link.capcity;
		}
	}

	void IncrementLinkSize(const IGsize linkId) { this->GetLink(linkId).size++; }

	void AllocateLinks(const IGsize n) {
		for (int i = 0; i < n; ++i)
		{
			Link& link = GetLink(i);
			link.pointer = new igIndex[link.size];
		}
	}

protected:
	CellLinks() {};
	~CellLinks() override {};

	ElementArray<Link>::Pointer m_Buffer{};
};
IGAME_NAMESPACE_END
#endif