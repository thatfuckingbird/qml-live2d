/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppPal.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <GL/glew.h>
#include <Model/CubismMoc.hpp>
#include "LAppDefine.hpp"
#include <Live2DItem.h>

using std::endl;
using namespace Csm;
using namespace std;
using namespace LAppDefine;

csmByte* LAppPal::LoadFileAsBytes(const string filePath, csmSizeInt* outSize)
{
    const char* path = filePath.c_str();

    int size = std::filesystem::file_size(filePath);
    if (DebugLogEnable)
    {
        PrintLog(path);
    }

    std::fstream file;

    file.open(path, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        if (DebugLogEnable)
        {
            PrintLog("file open error");
        }
        return NULL;
    }
    char* buf = new char[size];
    file.read(buf, size);
    file.close();

    *outSize = size;
    return reinterpret_cast<csmByte*>(buf);
}

void LAppPal::PrintLog(const csmChar* format, ...)
{
    va_list args;
    csmChar buf[512] = {0};
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args); // 標準出力でレンダリング
    Live2DItem::logFunction(buf);
    va_end(args);
}
