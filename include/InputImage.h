#pragma once

#include <stdint.h>

namespace InputImage
{
    inline int width;
    inline int height;
    inline int size;
    inline bool valid;
    inline uint8_t* inputData;
    inline uint8_t* displayData;

    void Init();
    void Update();
    void Reset();

    bool LoadInputData();
    bool SetupDisplayData();
    void SetupTexture();

    void UpdateDisplayData();
    void UpdateTexture();
    void DrawTexture();
}
