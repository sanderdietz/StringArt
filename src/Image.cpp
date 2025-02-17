#include "Image.h"

#include "InputImage.h"
#include "StringArt.h"
#include "Parameters.h"

#include <math.h>
#include <algorithm>

Image::Image()
{
    m_imageData = new uint8_t[InputImage::size * InputImage::size];

    for (size_t i = 0; i < InputImage::size; i++)
    {
        for (size_t j = 0; j < InputImage::size; j++)
        {
            if (!StringArt::OutOfBounds(i, j))
                m_imageData[i * InputImage::size + j] = 255;
        }
    }
}

Image::Image(const uint8_t* data)
{
    m_imageData = new uint8_t[InputImage::size * InputImage::size];
    for (size_t i = 0; i < InputImage::size; i++)
    {
        for (size_t j = 0; j < InputImage::size; j++)
        {
            if (!StringArt::OutOfBounds(i, j))
                m_imageData[i * InputImage::size + j] = data[i * InputImage::size * 4 + j * 4];
        }
    }
}

Image::~Image()
{
    delete[] m_imageData;
    m_imageData = nullptr;
}

void Image::AddLine(int nail1, int nail2)
{
    int* xValues;
    int* yValues;
    int length = DrawLine(nail1, nail2, xValues, yValues);

    for (size_t i = 0; i < length; i++)
    {
        int y = yValues[i];
        int x = xValues[i];
        int currentValue = m_imageData[y * InputImage::size + x];
        m_imageData[y * InputImage::size + x] = (uint8_t)std::clamp(currentValue - 32, 0, 255);
    }
    delete[] xValues;
    delete[] yValues;
}

void Image::SubtractLine(int nail1, int nail2)
{
    int* xValues;
    int* yValues;
    int length = DrawLine(nail1, nail2, xValues, yValues);

    for (size_t i = 0; i < length; i++)
    {
        int y = yValues[i];
        int x = xValues[i];
        int currentValue = m_imageData[y * InputImage::size + x];
        m_imageData[y * InputImage::size + x] = (uint8_t)std::clamp(currentValue + 32, 0, 255);
    }
    delete[] xValues;
    delete[] yValues;
}

float Image::GetMeanError(int nail1, int nail2) const
{
    unsigned long long error = 0;

    int* xValues;
    int* yValues;
    int length = DrawLine(nail1, nail2, xValues, yValues);

    for (size_t i = 0; i < length; i++)
    {
        int x = xValues[i];
        int y = yValues[i];
        int currentValue = m_imageData[y * InputImage::size + x];
        error += std::max(0, currentValue);
    }
    delete[] xValues;
    delete[] yValues;

    return error / length; 
}

int Image::PlotLineLow(int x1, int y1, int x2, int y2, int*& xValues, int*& yValues) const // x1 < x2
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int yi = 1;
    if (dy < 0)
    {
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;
    int y = y1;

    xValues = new int[dx];
    yValues = new int[dx];

    for (size_t i = 0; i < dx; i++)
    {
        int x = x1 + i;
        xValues[i] = x;
        yValues[i] = y;
        if (D > 0)
        {
            y += yi;
            D += 2 * (dy - dx);
        }
        else
            D += 2 * dy;
    }
    return dx;
}

int Image::PlotLineHigh(int x1, int y1, int x2, int y2, int*& xValues, int*& yValues) const // y1 < y2
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int xi = 1;
    if (dx < 0)
    {
        xi = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = x1;

    xValues = new int[dy];
    yValues = new int[dy];

    for (size_t i = 0; i < dy; i++)
    {
        int y = y1 + i;
        xValues[i] = x;
        yValues[i] = y;
        if (D > 0)
        {
            x += xi;
            D += 2 * (dx - dy);
        }
        else
            D += 2 * dx;
    }
    return dy;
}

int Image::PlotVerticalLine(int x, int y1, int y2, int*& xValues, int*& yValues) const // y1 < y2
{
    int dy = y2 - y1;
    
    xValues = new int[dy];
    yValues = new int[dy];

    for (size_t i = 0; i < dy; i++)
    {
        xValues[i] = x;
        yValues[i] = y1 + i;
    }
    return dy;
}

int Image::PlotHorizontalLine(int x1, int x2, int y, int*& xValues, int*& yValues) const // x1 < x2
{
    int dx = x2 - x1;
    
    xValues = new int[dx];
    yValues = new int[dx];

    for (size_t i = 0; i < dx; i++)
    {
        xValues[i] = x1 + i;
        yValues[i] = y;
    }
    return dx;
}

int Image::PlotLine(int x1, int y1, int x2, int y2, int*& xValues, int*& yValues) const
{
    if (x1 == x2)
    {
        if (y1 > y2)
            return PlotVerticalLine(x1, y2, y1, xValues, yValues);
        else
            return PlotVerticalLine(x1, y1, y2, xValues, yValues);
    }
    else if (y1 == y2)
    {
        if (x1 > x2)
            return PlotHorizontalLine(x2, x1, y1, xValues, yValues);
        else
            return PlotHorizontalLine(x1, x2, y1, xValues, yValues);
    }
    else if (abs(y2 - y1) < abs(x2 - x1))
    {
        if (x1 > x2)
            return PlotLineLow(x2, y2, x1, y1, xValues, yValues);
        else
            return PlotLineLow(x1, y1, x2, y2, xValues, yValues);
    }
    else
    {
        if (y1 > y2)
            return PlotLineHigh(x2, y2, x1, y1, xValues, yValues);
        else
            return PlotLineHigh(x1, y1, x2, y2, xValues, yValues);
    }
}

int Image::DrawLine(int nail1, int nail2, int*& xValues, int*& yValues) const
{
    int size = InputImage::size;
    int numNails = Parameters::numNails;

    float angle1 = (float)(nail1 % numNails) / numNails * 2.0f * M_PIf;
    int x1 = .5 * size * (1.0f + cos(angle1));
    int y1 = .5 * size * (1.0f - sin(angle1));

    float angle2 = (float)(nail2 % numNails) / numNails * 2 * M_PIf;
    int x2 = .5 * size * (1.0f + cos(angle2));
    int y2 = .5 * size * (1.0f - sin(angle2));

    return PlotLine(x1, y1, x2, y2, xValues, yValues);
}
