#pragma once

#include <iostream>

struct Parameters
{
    inline static float zoom;
    inline static int xOffset;
    inline static int yOffset;
    inline static float darkness;
    inline static float contrast;

    inline static int numNails;
    inline static int numLines;

    static void SetDefaultZoom() { zoom = 1.0f; }
    static void SetDefaultXOffset() { xOffset = 0; }
    static void SetDefaultYOffset() { yOffset = 0; }
    static void SetDefaultDarkness() { darkness = 0.0f; }
    static void SetDefaultContrast() { contrast = 0.0f; }

    static void SetDefaultNumNails() { numNails = 300; }
    static void SetDefaultNumLines() { numLines = 3000; }
    
    static void SetDefault()
    {
        SetDefaultZoom();
        SetDefaultXOffset();
        SetDefaultYOffset();
        SetDefaultDarkness();
        SetDefaultContrast();

        SetDefaultNumNails();
        SetDefaultNumLines();
    }
};