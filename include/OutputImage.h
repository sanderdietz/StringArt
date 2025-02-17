#pragma once

#include "Image.h"

namespace OutputImage
{
    inline int size;
    inline bool valid;
    inline bool running;
    inline int iteration;
    inline int* sequence;
    inline uint8_t* grayscaleData;
    inline uint8_t* displayData;

    void Init();
    void Update();
    void Reset();

    bool SetupGrayscaleData();
    void SetupDisplayData();
    void SetupTexture();

    void GenerateStringArt();
    void Cancel();

    void UpdateDisplayData();
    void UpdateTexture();
    void DrawTexture();
}