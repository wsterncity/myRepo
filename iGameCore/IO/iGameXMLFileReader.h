/**
 * @class   iGameXMLFileReader
 * @brief   iGameXMLFileReader's brief
 */

#pragma once
#include "iGameFilter.h"



class TiXmlElement;
IGAME_NAMESPACE_BEGIN

class iGameXMLFileReader : public Filter{
public:
    I_OBJECT(iGameXMLFileReader)
public:
    void SetFilePath(const std::string& filePath);

protected:
    std::string m_FilePath;
protected:
    TiXmlElement *FindTargetItem(TiXmlElement *root, const char *itemName);
    TiXmlElement* FindTargetAttributeItem(TiXmlElement* root, const char* itemName,
                                          const char* attributeName, const std::string& attributeData);

protected:
    iGameXMLFileReader() = default;
    ~iGameXMLFileReader() override = default;


};


IGAME_NAMESPACE_END