#pragma once

#include <string>
#include <GL/gl.h>

namespace StringArt
{
    // Global variables
    inline std::string g_filename;
    inline GLuint g_textures[2];

    // Main function
    void RenderUI();

    // Helper functions
    bool OutOfBounds(int i, int j);
    float DistancePointToLineSquared(int i, int j, float x1, float x2, float y1, float y2);
    bool CheckPNG(const std::string& filename);
    std::string ChooseFilename();
}