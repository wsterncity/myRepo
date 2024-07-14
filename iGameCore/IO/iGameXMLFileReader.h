/**
 * @class   iGameXMLFileReader
 * @brief   iGameXMLFileReader's brief
 */

#pragma once
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include "iGameDataCollection.h"


class TiXmlElement;
class TiXmlDocument;
class TiXmlElement;
IGAME_NAMESPACE_BEGIN

class iGameXMLFileReader : public Filter{
public:
    I_OBJECT(iGameXMLFileReader)
public:
    void SetFilePath(const std::string& filePath);

    bool Open();
    virtual bool CreateDataObject();
    virtual bool Parsing() = 0;


protected:
    std::string m_FilePath;
    std::string m_FileName;

    DataObject::Pointer m_Output;
    // tinyxml stuff
    TiXmlElement* root{nullptr};
    TiXmlDocument* doc{nullptr};

protected:
    TiXmlElement *FindTargetItem(TiXmlElement *root, const char *itemName);

    TiXmlElement* FindTargetAttributeItem(TiXmlElement* root, const char* itemName,
                                          const char* attributeName, const std::string& attributeData);

    bool Execute() override;

    DataCollection m_Data;

protected:
    iGameXMLFileReader();
    ~iGameXMLFileReader() override = default;


};


IGAME_NAMESPACE_END