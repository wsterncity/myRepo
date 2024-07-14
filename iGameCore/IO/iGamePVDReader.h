/**
 * @class   iGamePVDReader
 * @brief   iGamePVDReader's brief
 */

#pragma once

#include "iGameXMLFileReader.h"
#include <iGameDataObject.h>

IGAME_NAMESPACE_BEGIN
class iGamePVDReader : public iGameXMLFileReader{
public:
    I_OBJECT(iGamePVDReader)

    static Pointer New(){return new iGamePVDReader;}

    bool Parsing() override;

    bool CreateDataObject() override;

protected:

    iGamePVDReader() = default;
    ~iGamePVDReader() = default;

protected:
    std::map<float, DataObject::Pointer> child_map;
};

IGAME_NAMESPACE_END