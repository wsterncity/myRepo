#ifndef iGameType_h
#define iGameType_h

#include <stdint.h>

inline constexpr const char* FILE_NAME = "FILE_NAME";
inline constexpr const char* FILE_PATH = "FILE_PATH";
inline constexpr const char* FILE_SUFFIX = "FILE_SUFFIX";

// DataObjectType
inline constexpr int IG_NONE = -1;
inline constexpr int IG_DATA_OBJECT = 0;
inline constexpr int IG_POINT_SET = 1;
inline constexpr int IG_SURFACE_MESH = 2;
inline constexpr int IG_VOLUME_MESH = 3;
inline constexpr int IG_DATAOBJECTTYPE_COUNT = 4;

// DrawMode
inline constexpr int IG_POINTS = 0;
inline constexpr int IG_WIREFRAME = 1;
inline constexpr int IG_SURFACE = 2;
inline constexpr int IG_SURFACE_WITH_EDGE = 3;
inline constexpr int IG_VOLUME = 4;

// AttributeType
inline constexpr int IG_SCALAR = 0; // 标量
inline constexpr int IG_VECTOR = 1; // 矢量
inline constexpr int IG_NORMAL = 2; // 法线
inline constexpr int IG_TCOORD = 3; // 纹理坐标
inline constexpr int IG_TENSOR = 4; // 张量
inline constexpr int NUM_ATTRIBUTES = 5;

// AttributeWithElementType
inline constexpr int IG_POINT = 0;
inline constexpr int IG_CELL = 1;

inline constexpr int IG_VOID = 0;
inline constexpr int IG_CHAR = 1;
inline constexpr int IG_UNSIGNED_CHAR = 2;
inline constexpr int IG_SHORT = 3;
inline constexpr int IG_UNSIGNED_SHORT = 4;
inline constexpr int IG_INT = 5;
inline constexpr int IG_UNSIGNED_INT = 6;
inline constexpr int IG_LONG_LONG = 7;
inline constexpr int IG_UNSIGNED_LONG_LONG = 8;
inline constexpr int IG_FLOAT = 9;
inline constexpr int IG_DOUBLE = 10;
inline constexpr int IG_INDEX = 11;

inline constexpr int IG_INT8 = 1;
inline constexpr int IG_UINT8 = 2;
inline constexpr int IG_INT16 = 3;
inline constexpr int IG_UINT16 = 4;
inline constexpr int IG_INT32 = 5;
inline constexpr int IG_UINT32 = 6;
inline constexpr int IG_INT64 = 7;
inline constexpr int IG_UINT64 = 8;

typedef long IGenum;

// Index命名
typedef int igIndex;
typedef unsigned int iguIndex;
typedef long long igIndex64;
typedef unsigned long long iguIndex64;
typedef short igIndex16;
typedef unsigned short iguIndex16;

#endif