#include "InputImage.h"
#include "StringArt.h"
#include "imgui.h"
#include "Parameters.h"

#include <algorithm>

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace InputImage
{
    void Init()
    {
        valid = LoadInputData() && SetupDisplayData();
        if (valid)
            SetupTexture();
    }

    void Update()
    {
        UpdateDisplayData();
        UpdateTexture();
        DrawTexture();
    }

    void Reset()
    {
        width = 0;
        height = 0;
        size = 0;
        valid = false;
        stbi_image_free(inputData);
        inputData = nullptr;
        delete[] displayData;
        displayData = nullptr;
    }

    bool LoadInputData()
    {
        int comp;
        inputData = stbi_load(StringArt::g_filename.c_str(), &width, &height, &comp, STBI_rgb_alpha);
        return (bool)inputData;
    }

    bool SetupDisplayData()
    {
        size = std::min(width, height);
        if (size <= 0)
            return false;
            
        displayData = new uint8_t[size * size * 4];
        return true;
    }

    void SetupTexture()
    {
        glBindTexture(GL_TEXTURE_2D, StringArt::g_textures[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void UpdateDisplayData()
    {
        size = (int)(std::min(width, height) / Parameters::zoom);
        for (size_t i = 0; i < size; i++)
        {
            for (size_t j = 0; j < size; j++)
            {
                if (StringArt::OutOfBounds(i, j))
                {
                    displayData[i * size * 4 + j * 4 + 3] = 0;
                    continue;
                }

                int ii = (height - size) / 2 - Parameters::yOffset + i;
                int jj = (width - size) / 2 + Parameters::xOffset + j;

                int r = inputData[ii * width * 4 + jj * 4];
                int g = inputData[ii * width * 4 + jj * 4 + 1];
                int b = inputData[ii * width * 4 + jj * 4 + 2];

                int grayscale = (.299 * r + .587 * g + .114 * b) * (Parameters::darkness + 1);
                grayscale = std::clamp((int)((Parameters::contrast + 1) * (grayscale - 128)) + 128, 0, 255);

                displayData[i * size * 4 + j * 4] = (char)grayscale;
                displayData[i * size * 4 + j * 4 + 1] = (char)grayscale;
                displayData[i * size * 4 + j * 4 + 2] = (char)grayscale;
                displayData[i * size * 4 + j * 4 + 3] = 0xff;
            }
        }
    }

    void UpdateTexture()
    {
        glBindTexture(GL_TEXTURE_2D, StringArt::g_textures[0]);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, displayData);
    }

    void DrawTexture()
    {
        ImVec2 windowSize = ImGui::GetContentRegionAvail();
        int smallest = std::min(windowSize.x, windowSize.y);
        ImGui::Image((void*)(intptr_t)StringArt::g_textures[0], ImVec2(smallest, smallest));
    }
}
