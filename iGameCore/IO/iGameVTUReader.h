/**
 * @class   iGameVTUReader
 * @brief   iGameVTUReader's brief
 */

#pragma once

#include "iGameXMLFileReader.h"

IGAME_NAMESPACE_BEGIN
class iGameVTUReader : public iGameXMLFileReader{

public:
    I_OBJECT(iGameVTUReader);

    static Pointer New(){return new iGameVTUReader;}

    bool Parsing() override;

protected:

    iGameVTUReader() = default;
    ~iGameVTUReader() = default;
};

IGAME_NAMESPACE_END
