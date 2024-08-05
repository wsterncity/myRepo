/**
 * @class   iGameXMLFileReader
 * @brief   Currently, iGameXMLFileReader only support UTF-8 encoded files,
 *          the files mixed with machine code and UTF-8 are not supported.
 */

#pragma once
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include "iGameDataCollection.h"
#include "iGameBase64Util.h"


namespace tinyxml2{
    class XMLElement;
    class XMLDocument;
}

IGAME_NAMESPACE_BEGIN

class iGameXMLFileReader : public Filter {
public:
    I_OBJECT(iGameXMLFileReader)
public:
    void SetFilePath(const std::string& filePath);

    bool Open();
    virtual bool CreateDataObject();
    virtual bool Parsing() = 0;

    bool Execute() override;

protected:
    std::string m_FilePath;
    std::string m_FileName;

    DataObject::Pointer m_Output;
    // tinyxml stuff
    tinyxml2::XMLElement* root{nullptr};
    tinyxml2::XMLDocument* doc{nullptr};

protected:
    tinyxml2::XMLElement *FindTargetItem(tinyxml2::XMLElement *root, const char *itemName);

    tinyxml2::XMLElement* FindTargetAttributeItem(tinyxml2::XMLElement* root, const char* itemName,
                                          const char* attributeName, const char* attributeData);

    void ReadBase64EncodedPoints(const char* p, const Points::Pointer& pointSet);

    template<typename T>
    void ReadBase64EncodedArray(const char* p, typename FlatArray<T>::Pointer arr){
        if(is_header_type_uint64){
            auto byte_size =  Base64_Convert_TargetValue<uint64_t>(p);
            Base64_ConvertTo_FlatArray<T>(p, 8, byte_size, arr);
        } else {
            auto byte_size =  Base64_Convert_TargetValue<uint32_t>(p);
            Base64_ConvertTo_FlatArray<T>(p, 4, byte_size, arr);
        }
    }


    DataCollection m_Data;

    bool is_header_type_uint64 {false};
protected:
    iGameXMLFileReader();
    ~iGameXMLFileReader() override = default;


};


IGAME_NAMESPACE_END