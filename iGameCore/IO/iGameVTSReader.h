﻿/**
 * @class   iGameVTSReader
 * @brief   iGameVTSReader's brief
 */
#ifndef iGameVTSReader_h
#define iGameVTSReader_h


#include "iGameXMLFileReader.h"

IGAME_NAMESPACE_BEGIN
class iGameVTSReader : public iGameXMLFileReader{
public:
    I_OBJECT(iGameVTSReader)

    static Pointer New(){return new iGameVTSReader;}

    bool Parsing() override;

protected:
    iGameVTSReader() = default;
    ~iGameVTSReader() = default;
};

IGAME_NAMESPACE_END
#endif