#include "StringArt.h"
#include "InputImage.h"
#include "OutputImage.h"
#include "imgui.h"
#include "Parameters.h"

#include <iostream>
#include <algorithm>
#include <thread>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

bool StringArt::OutOfBounds(int i, int j)
{
    int size = InputImage::size;
    return (.5 * size - i) * (.5 * size - i) + (.5 * size - j) * (.5 * size - j) > .25 * size * size;
}

float StringArt::DistancePointToLineSquared(int i, int j, float x1, float x2, float y1, float y2)
{
    float deltaX = (x2 - x1);
    float deltaY = (y2 - y1);
    float rootNumerator = deltaY * j - deltaX * i + x2 * y1 - y2 * x1;
    float denominator = deltaY * deltaY + deltaX * deltaX;
    return rootNumerator * rootNumerator / denominator;
}

bool StringArt::CheckPNG(const std::string& filename)
{
    int length = filename.length();
    return filename.substr(length - 4, 4) == ".png";
}

std::string StringArt::ChooseFilename()
{
    std::string filename;
    if (ImGui::Button("Choose PNG file"))
    {
        FILE* file = popen("zenity \
            --file-selection \
            --title='Select a PNG image'", "r");
        
        if (file)
        {
            char buffer[256];
            if (fgets(buffer, sizeof(buffer), file))
            {
                std::string bufferString = buffer;
                filename = bufferString.substr(0, bufferString.length() - 1);
            }
            pclose(file);
        }
    }
    return filename;
}

void StringArt::RenderUI()
{
    // Turn on docking
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    // Input Parameters panel
    {
        ImGui::Begin("Input Parameters", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::PushItemWidth(100);

        ImGui::BeginDisabled(OutputImage::running);

        std::string newFilename = ChooseFilename();
        bool updateFilename = false;
        if (!newFilename.empty())
        {
            updateFilename = true;
            g_filename = newFilename;
            Parameters::SetDefault();
        }
        if (!g_filename.empty())
        {
            ImGui::SameLine();
            if (!CheckPNG(g_filename))
                ImGui::Text("Wrong file format, this program only supports .png images.");
            else
            {
                ImGui::Text(g_filename.c_str());
                if (updateFilename || !InputImage::valid)
                {
                    updateFilename = false;
                    InputImage::Init();
                }
            }
        }

        ImGui::BeginDisabled(!InputImage::valid);

        int xMid = Parameters::xOffset + InputImage::width / 2;
        int yMid = Parameters::yOffset + InputImage::height / 2;
        int distLeft = xMid - InputImage::size / 2;
        int distRight = InputImage::width - xMid - InputImage::size / 2;
        int distTop = yMid - InputImage::size / 2;
        int distBottom = InputImage::height - yMid - InputImage::size / 2;
        int minDist = std::min({ distLeft, distRight, distTop, distBottom });

        float minZoom = .5f * (float)InputImage::size * Parameters::zoom / (.5f * (float)InputImage::size + (float)minDist);
        float maxZoom = 5.0f;
        ImGui::DragFloat("Zoom", &Parameters::zoom, .01f, minZoom, maxZoom, "%.2f");
        Parameters::zoom = std::clamp(Parameters::zoom, minZoom, maxZoom);
        ImGui::SameLine();
        ImGui::PushID(0);
        if (ImGui::Button("Reset"))
            Parameters::SetDefaultZoom();
        ImGui::PopID();

        int xMaxOffset = (InputImage::width - InputImage::size) / 2;
        int yMaxOffset = (InputImage::height - InputImage::size) / 2;

        ImGui::BeginDisabled(xMaxOffset <= 0);
        ImGui::DragInt("X offset", &Parameters::xOffset, 1.0f, -xMaxOffset, xMaxOffset);
        Parameters::xOffset = std::clamp(Parameters::xOffset, -xMaxOffset, xMaxOffset);
        ImGui::SameLine();
        ImGui::PushID(1);
        if (ImGui::Button("Reset"))
        Parameters::SetDefaultXOffset();
        ImGui::PopID();
        ImGui::EndDisabled();
        
        ImGui::BeginDisabled(yMaxOffset <= 0);
        ImGui::DragInt("Y offset", &Parameters::yOffset, 1.0f, -yMaxOffset, yMaxOffset);
        Parameters::yOffset = std::clamp(Parameters::yOffset, -yMaxOffset, yMaxOffset);
        ImGui::SameLine();
        ImGui::PushID(2);
        if (ImGui::Button("Reset"))
            Parameters::SetDefaultYOffset();
        ImGui::PopID();
        ImGui::EndDisabled();

        ImGui::DragFloat("Darkness", &Parameters::darkness, .01f, -1.0f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
        Parameters::darkness = std::clamp(Parameters::darkness, -1.0f, 1.0f);
        ImGui::SameLine();
        ImGui::PushID(3);
        if (ImGui::Button("Reset"))
        Parameters::SetDefaultDarkness();
        ImGui::PopID();
        
        ImGui::DragFloat("Constrast", &Parameters::contrast, .01f, -1.0f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
        Parameters::contrast = std::clamp(Parameters::contrast, -1.0f, 1.0f);
        ImGui::SameLine();
        ImGui::PushID(4);
        if (ImGui::Button("Reset"))
            Parameters::SetDefaultContrast();
        ImGui::PopID();

        ImGui::EndDisabled();
        ImGui::EndDisabled();

        ImGui::End();
    }

    // Output Parameters panel
    {
        ImGui::Begin("Output Parameters", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::PushItemWidth(100);

        ImGui::BeginDisabled(!InputImage::valid);
        ImGui::BeginDisabled(OutputImage::running);

        ImGui::InputInt("Number of nails", &Parameters::numNails);
        Parameters::numNails = std::clamp(Parameters::numNails, 2, 500);
        ImGui::SameLine();
        ImGui::PushID(7);
        if (ImGui::Button("Reset"))
        Parameters::SetDefaultNumNails();
        ImGui::PopID();
        
        ImGui::InputInt("Number of lines", &Parameters::numLines);
        Parameters::numLines = std::clamp(Parameters::numLines, 1, 10000);
        ImGui::SameLine();
        ImGui::PushID(8);
        if (ImGui::Button("Reset"))
            Parameters::SetDefaultNumNails();
        ImGui::PopID();

        if (ImGui::Button("Reset parameters"))
            Parameters::SetDefault();
        if (ImGui::Button("Generate"))
        {
            OutputImage::Init();
            std::thread thread(OutputImage::GenerateStringArt);
            thread.detach();
        }

        ImGui::EndDisabled();

        ImGui::BeginDisabled(!OutputImage::running);
        if (ImGui::Button("Cancel"))
            OutputImage::Reset();
        ImGui::EndDisabled();
        
        ImGui::EndDisabled();

        ImGui::End();
    }

    // Output Files panel
    {
        ImGui::Begin("Output Files", nullptr, ImGuiWindowFlags_NoMove);
        ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_NoTooltip;

        ImGui::Text("%d/%d lines drawn", OutputImage::iteration, Parameters::numLines);
        
        ImGui::BeginDisabled(OutputImage::iteration != Parameters::numLines);
        if (ImGui::Button("Save String Art image"))
            stbi_write_png("StringArtImage.png", OutputImage::size, OutputImage::size, 4, OutputImage::displayData, OutputImage::size * 4);
        
        if (ImGui::Button("Save instructions"))
        {
            FILE* file = fopen("StringArtInstructions.txt", "w");
            if (file)
            {
                fprintf(file, "String Art Instructions\n\nParameters:\n");
                fprintf(file, "Zoom: %.2f\n", Parameters::zoom);
                fprintf(file, "X offset: %d\n", Parameters::xOffset);
                fprintf(file, "Y offset: %d\n", Parameters::yOffset);
                fprintf(file, "Darkness: %.2f\n", Parameters::darkness);
                fprintf(file, "Contrast: %.2f\n", Parameters::contrast);
                fprintf(file, "Number of nails: %d\n", Parameters::numNails);
                fprintf(file, "Number of lines: %d\n", Parameters::numLines);
                
                for (size_t i = 0; i < Parameters::numLines; i++)
                {
                    if (i % 100 == 0)
                        fprintf(file, "\n\nNails %d-%d:\n", i + 1, i + 100);
                    else if (i % 20 == 0)
                        fprintf(file, "\n");

                    fprintf(file, "%3d", OutputImage::sequence[i]);
                    if (i == Parameters::numLines - 1)
                        fprintf(file, "\n");
                    else
                        fprintf(file, ", ");
                }
                fclose(file);
            }
        }
        ImGui::EndDisabled();

        ImGui::End();
    }

    // Input Image viewport panel
    {
        ImGui::Begin("Input Image", nullptr, ImGuiWindowFlags_NoMove);

        if (InputImage::valid)
            InputImage::Update();
        
        ImGui::End();
    }

    // Output Image viewport panel
    {
        ImGui::Begin("Output Image", nullptr, ImGuiWindowFlags_NoMove);

        if (OutputImage::valid)
            OutputImage::Update();

        ImGui::End();
    }
}