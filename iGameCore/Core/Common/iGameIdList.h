#ifndef iGameIdArray_h
#define iGameIdArray_h

#include "iGameObject.h"

IGAME_NAMESPACE_BEGIN
class IdList : public Object {
public:
    I_OBJECT(IdList);
    static Pointer New() { return new IdList; }

    /**
     * Release memory and restore to unallocated state.
     */
    void Initialize()
    {
        this->InitializeMemory();
        this->NumberOfIds = 0;
        this->Size = 0;
    }

    bool Allocate(igIndex sz, int strategy = 0)
    {
        return this->AllocateInternal(sz, 0);
    }

    igIndex GetNumberOfIds() const noexcept { return this->NumberOfIds; }

    igIndex GetId(igIndex i) { assert(0 <= i && i < GetNumberOfIds()); return this->Ids[i]; }

    void SetId(igIndex i, igIndex idx) { assert(0 <= i && i < GetNumberOfIds()); this->Ids[i] = idx; }

    /**
     * Add the id specified to the end of the list. Range checking is performed.
     */
    igIndex InsertNextId(igIndex id)
    {
        if (this->NumberOfIds >= this->Size)
        {
            if (!this->Resize(2 * this->NumberOfIds + 1)) // grow by factor of 2
            {
                return this->NumberOfIds - 1;
            }
        }
        this->Ids[this->NumberOfIds++] = id;
        return this->NumberOfIds - 1;
    }
    igIndex AddId(igIndex id)
    {
        return this->InsertNextId(id);
    }
    /**
     * If id is not already in list, insert it and return location in
     * list. Otherwise return just location in list.
     */
    igIndex InsertUniqueId(const igIndex id)
    {
        for (igIndex i = 0; i < this->NumberOfIds; i++)
        {
            if (id == this->Ids[i])
            {
                return i;
            }
        }

        return this->InsertNextId(id);
    }

    /**
     * Find the location i of the provided id.
     */
    igIndex FindIdLocation(const igIndex id)
    {
        for (int i = 0; i < this->NumberOfIds; i++)
            if (this->Ids[i] == id)
                return i;
        return -1;
    }

    /**
     * Specify the number of ids for this object to hold. Does an
     * allocation as well as setting the number of ids.
     */
    void SetNumberOfIds(const igIndex number)
    {
        this->AllocateInternal(number, number);
    }

    bool Resize(const igIndex sz)
    {
        igIndex* newIds;
        igIndex newSize;

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

        if ((newIds = new igIndex[newSize]) == nullptr)
        {
            return false;
        }

        if (this->NumberOfIds > newSize)
        {
            this->NumberOfIds = newSize;
        }

        if (this->Ids)
        {
            memcpy(newIds, this->Ids,
                static_cast<size_t>(sz < this->Size ? sz : this->Size) * sizeof(igIndex));
            if (this->ManageMemory)
            {
                delete[] this->Ids;
            }
        }
        this->ManageMemory = true;

        this->Size = newSize;
        this->Ids = newIds;
        return true;
    }

    igIndex SearchId(igIndex id) const
    {
        igIndex i;
        for (i = 0; i < this->GetNumberOfIds(); i++)
        {
            if (this->Ids[i] == id)
            {
                return i;
            }
        }
        return (-1);
    }

    igIndex* GetPointer(igIndex i) { return this->Ids + i; }
    igIndex* GetPointer() { return this->Ids; }
    igIndex* GetRawPointer() { return this->Ids; }

    void SetArray(igIndex* array, igIndex size)
    {
        if (this->ManageMemory)
        {
            delete[] this->Ids;
        }
        if (array) 
        {
            this->ManageMemory = true;
        }
        this->Ids = array;
        this->NumberOfIds = size;
        this->Size = size;
    }

    /**
     * Reset to an empty state but retain previously allocated memory.
     */
    void Reset() { this->NumberOfIds = 0; }

    /**
     * Free any unused memory.
     */
    void Squeeze() { this->Resize(this->NumberOfIds); }

    igIndex* begin() { return this->Ids; }
    igIndex* end() { return this->Ids + this->NumberOfIds; }
    const igIndex* begin() const { return this->Ids; }
    const igIndex* end() const { return this->Ids + this->NumberOfIds; }

protected:
    IdList() { }
    ~IdList() override
    {
        if (this->ManageMemory)
        {
            delete[] this->Ids;
        }
    }

    bool AllocateInternal(igIndex sz, igIndex numberOfIds)
    {
        if (sz > this->Size)
        {
            this->InitializeMemory();
            this->Size = (sz > 0 ? sz : 1);
            this->Ids = new igIndex[this->Size];
            if (this->Ids == nullptr)
            {
                this->NumberOfIds = 0;
                return false;
            }
        }
        this->NumberOfIds = numberOfIds;
        return true;
    }

    /**
     * Release memory.
     */
    void InitializeMemory()
    {
        if (this->ManageMemory)
        {
            delete[] this->Ids;
        }
        this->ManageMemory = true;
        this->Ids = nullptr;
    }

    igIndex* Ids{ nullptr };
    igIndex NumberOfIds{ 0 };
    igIndex Size{ 0 };
    bool ManageMemory{ false };
};

IGAME_NAMESPACE_END
#endif