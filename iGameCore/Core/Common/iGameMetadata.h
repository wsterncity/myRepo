#ifndef iGameMetadata_h
#define iGameMetadata_h

//#include <cstring>
//#include <map>
//#include <memory>
//#include <string>
//#include <vector>

#include <variant>

#include "iGameObject.h"
#include "iGameElementArray.h"

IGAME_NAMESPACE_BEGIN
using EntryValue = std::variant<unsigned char, char, unsigned short, short,
    unsigned int, int, unsigned long long, long long, float, double, bool, 
    std::string, IntArray2::Pointer, FloatArray2::Pointer, DoubleArray2::Pointer,
    StringArray::Pointer>;

class Metadata : public Object{
public:
    I_OBJECT(Metadata);
    static Pointer New() { return new Metadata; }

    void AddInt(const std::string& name, int value);
    bool GetInt(const std::string& name, int& value) const;
    int GetInt(const std::string& name) const;

    void AddIntArray(const std::string& name, IntArray2::Pointer value);
    bool GetIntArray(const std::string& name, IntArray2::Pointer& value) const;
    IntArray2::Pointer GetIntArray(const std::string& name) const;

    void AddFloat(const std::string& name, float value);
    bool GetFloat(const std::string& name, float& value) const;
    float GetFloat(const std::string& name) const;

    void AddFloatArray(const std::string& name, FloatArray2::Pointer value);
    bool GetFloatArray(const std::string& name, FloatArray2::Pointer& value) const;
    FloatArray2::Pointer GetFloatArray(const std::string& name) const;

    void AddDouble(const std::string& name, double value);
    bool GetDouble(const std::string& name, double& value) const;
    double GetDouble(const std::string& name) const;

    void AddDoubleArray(const std::string& name, DoubleArray2::Pointer value);
    bool GetDoubleArray(const std::string& name, DoubleArray2::Pointer& value) const;
    DoubleArray2::Pointer GetDoubleArray(const std::string& name) const;
 
    void AddString(const std::string& name, const std::string& value);
    bool GetString(const std::string& name, std::string& value) const;
    std::string GetString(const std::string& name) const;

    void AddStringArray(const std::string& name, StringArray::Pointer value);
    bool GetStringArray(const std::string& name, StringArray::Pointer& value) const;
    StringArray::Pointer GetStringArray(const std::string& name) const;

    bool AddSubMetadata(const std::string& name, Pointer sub_metadata);
    Pointer GetSubMetadata(const std::string& name);

    void RemoveEntry(const std::string& name);

    int GetNumberOfEntries() const { return static_cast<int>(m_Entries.size()); }
    const std::unordered_map<std::string, EntryValue>& entries() const { return m_Entries; }
    const std::unordered_map<std::string, Pointer>& sub_metadatas() const { return m_SubMetadatas; }

protected:
    Metadata() {}
    ~Metadata() override {}

    template <typename DataTypeT>
    void AddEntry(const std::string& entry_name, const DataTypeT& entry_value) {
        const auto it = m_Entries.find(entry_name);
        if (it != m_Entries.end()) {
            m_Entries.erase(it);
        }
        m_Entries.insert(std::make_pair(entry_name, EntryValue(entry_value)));
    }

    template <typename DataTypeT>
    bool GetEntry(const std::string& entry_name, DataTypeT& entry_value) const {
        const auto it = m_Entries.find(entry_name);
        if (it == m_Entries.end()) {
            return false;
        }
        entry_value = std::get<DataTypeT>(it->second);
        return true;
    }

    std::unordered_map<std::string, EntryValue> m_Entries;
    std::unordered_map<std::string, Pointer> m_SubMetadatas;
};
IGAME_NAMESPACE_END
#endif