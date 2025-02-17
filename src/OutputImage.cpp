#include "OutputImage.h"

#include "InputImage.h"
#include "StringArt.h"
#include "Parameters.h"
#include "imgui.h"
#include "Image.h"

#include <thread>
#include <chrono>
#include <deque>
#include <algorithm>

namespace OutputImage
{
    void Init()
    {
        valid = SetupGrayscaleData();
        if (valid)
        {
            SetupDisplayData();
            SetupTexture();
        }
    }

    void Update()
    {
        if (running)
            UpdateDisplayData();
        UpdateTexture();
        DrawTexture();
    }

    void Reset()
    {
        size = 0;
        valid = false;
        running = false;
        iteration = 0;
        delete sequence;
        sequence = nullptr;
    }

    bool SetupGrayscaleData()
    {
        size = InputImage::size;
        if (size <= 0)
            return false;

        grayscaleData = new uint8_t[size * size];
        for (size_t i = 0; i < size; i++)
        {
            for (size_t j = 0; j < size; j++)
            {
                if (!StringArt::OutOfBounds(i, j))
                    grayscaleData[i * size + j] = 0xff;
            }
        }
        
        return true;
    }

    void SetupDisplayData()
    {
        displayData = new uint8_t[size * size * 4];
        UpdateDisplayData();
    }

    void SetupTexture()
    {
        glBindTexture(GL_TEXTURE_2D, StringArt::g_textures[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void GenerateStringArt()
    {
        running = true;
        Image targetImage(InputImage::displayData);
        Image currentImage;
        int currentNail = 0;
        int nextNail = 0;

        std::deque<int> lastNails;
        lastNails.push_back(currentNail);
        sequence = new int[Parameters::numLines];

        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (iteration = 0; iteration < Parameters::numLines; iteration++)
        {
            if (!running)
                return;
            
            sequence[iteration] = currentNail;
            float smallestMeanError = 255;
            for (size_t n = currentNail; n < currentNail + Parameters::numNails; n++)
            {
                int nail = n % Parameters::numNails;

                if (std::find(lastNails.begin(), lastNails.end(), nail) == lastNails.end())
                {
                    float meanError = targetImage.GetMeanError(currentNail, nail);
                    if (meanError < smallestMeanError)
                    {
                        smallestMeanError = meanError;
                        nextNail = nail;
                    }
                }
            }
            currentImage.AddLine(currentNail, nextNail);
            targetImage.SubtractLine(currentNail, nextNail);
            currentNail = nextNail;
            grayscaleData = currentImage.GetData();

            lastNails.push_back(currentNail);
            if (lastNails.size() > Parameters::numNails / 10)
                lastNails.pop_front();
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    }

    void Cancel()
    {
        running = false;
        iteration = 0;
        for (size_t i = 0; i < size; i++)
        {
            for (size_t j = 0; j < size; j++)
            {
                if (!StringArt::OutOfBounds(i, j))
                    grayscaleData[i * size + j] = 0xff;
            }
        }
        UpdateDisplayData();
    }

    void UpdateDisplayData()
    {
        for (size_t i = 0; i < size; i++)
        {
            for (size_t j = 0; j < size; j++)
            {
                if (StringArt::OutOfBounds(i, j))
                {
                    displayData[i * size * 4 + j * 4 + 3] = 0;
                    continue;
                }
                for (size_t k = 0; k < 4; k++)
                    displayData[i * size * 4 + j * 4 + k] = grayscaleData[i * size + j];
            }
        }
    }

    void UpdateTexture()
    {
        glBindTexture(GL_TEXTURE_2D, StringArt::g_textures[1]);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, displayData);
    }

    void DrawTexture()
    {
        ImVec2 windowSize = ImGui::GetContentRegionAvail();
        int smallest = std::min(windowSize.x, windowSize.y);
        ImGui::Image((void*)(intptr_t)StringArt::g_textures[1], ImVec2(smallest, smallest));
    }
}

