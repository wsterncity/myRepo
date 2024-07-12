#ifndef iGameMetadata_h
#define iGameMetadata_h

//#include <cstring>
//#include <map>
//#include <memory>
//#include <string>
//#include <vector>


#include "iGameObject.h"

IGAME_NAMESPACE_BEGIN

class EntryValue {
public:
    template <typename DataTypeT>
    explicit EntryValue(const DataTypeT& data) {
        const size_t data_type_size = sizeof(DataTypeT);
        m_Data.resize(data_type_size);
        memcpy(&m_Data[0], &data, data_type_size);
    }

    template <typename DataTypeT>
    explicit EntryValue(const std::vector<DataTypeT>& data) {
        const size_t total_size = sizeof(DataTypeT) * data.size();
        m_Data.resize(total_size);
        memcpy(&m_Data[0], &data[0], total_size);
    }

    EntryValue(const EntryValue& value);
    explicit EntryValue(const std::string& value);

    bool GetValue(std::string* value) const {
        if (value == nullptr) {
            value = new std::string(reinterpret_cast<const char*>(m_Data.data()), m_Data.size());
        }
        else {
            value->assign(reinterpret_cast<const char*>(m_Data.data()), m_Data.size());
        }
        
        return true;
    }

    template <typename DataTypeT>
    bool GetValue(DataTypeT* value) const {
        const size_t data_type_size = sizeof(DataTypeT);
        if (data_type_size != m_Data.size()) {
            return false;
        }
        memcpy(value, &m_Data[0], data_type_size);
        return true;
    }

    template <typename DataTypeT>
    bool GetValue(std::vector<DataTypeT>* value) const {
        if (m_Data.empty()) {
            return false;
        }
        const size_t data_type_size = sizeof(DataTypeT);
        if (m_Data.size() % data_type_size != 0) {
            return false;
        }
        value->resize(m_Data.size() / data_type_size);
        memcpy(&value->at(0), &m_Data[0], m_Data.size());
        return true;
    }

    const int GetSize() const { return static_cast<int>(m_Data.size()); };

    const std::vector<uint8_t>& GetData() const { return m_Data; }

private:
    std::vector<uint8_t> m_Data;
};


class Metadata : public Object{
public:
    I_OBJECT(Metadata);
    static Pointer New() { return new Metadata; }

    void AddInt(const std::string& name, int value);
    bool GetInt(const std::string& name, int* value) const;

    void AddIntArray(const std::string& name, const std::vector<int>& value);
    bool GetIntArray(const std::string& name, std::vector<int>* value) const;

    void AddDouble(const std::string& name, double value);
    bool GetDouble(const std::string& name, double* value) const;

    void AddDoubleArray(const std::string& name, const std::vector<double>& value);
    bool GetDoubleArray(const std::string& name, std::vector<double>* value) const;

    void AddFloat(const std::string& name, float value);
    bool GetFloat(const std::string& name, float* value) const;

    void AddFloatArray(const std::string& name, const std::vector<float>& value);
    bool GetFloatArray(const std::string& name, std::vector<float>* value) const;

    void AddString(const std::string& name, const std::string& value);
    bool GetString(const std::string& name, std::string* value) const;

    bool AddSubMetadata(const std::string& name, Pointer sub_metadata);
    Metadata* GetSubMetadata(const std::string& name);
    const Metadata* GetSubMetadata(const std::string& name) const;

    void RemoveEntry(const std::string& name);

    int GetNumberOfEntries() const { return static_cast<int>(m_Entries.size()); }
    const std::map<std::string, EntryValue>& entries() const { return m_Entries; }
    const std::map<std::string, Pointer>& sub_metadatas() const { return m_SubMetadatas; }

protected:
    Metadata() {}
    ~Metadata() override {}

    template <typename DataTypeT>
    void AddEntry(const std::string& entry_name, const DataTypeT& entry_value) {
        const auto itr = m_Entries.find(entry_name);
        if (itr != m_Entries.end()) {
            m_Entries.erase(itr);
        }
        m_Entries.insert(std::make_pair(entry_name, EntryValue(entry_value)));
    }

    template <typename DataTypeT>
    bool GetEntry(const std::string& entry_name, DataTypeT* entry_value) const {
        const auto itr = m_Entries.find(entry_name);
        if (itr == m_Entries.end()) {
            return false;
        }
        return itr->second.GetValue(entry_value);
    }

    std::map<std::string, EntryValue> m_Entries;
    std::map<std::string, Pointer> m_SubMetadatas;
};
IGAME_NAMESPACE_END
#endif