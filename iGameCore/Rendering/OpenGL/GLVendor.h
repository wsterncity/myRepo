#pragma once

#ifdef __APPLE__
#define __gl3_h_
#define __glext_h_
#define __glext3_h_
#include "GL410/GLVendor.h"
#else
#include "GL460/GLVendor.h"
#endif