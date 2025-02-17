#pragma once

#include "InputImage.h"

class Image
{
private:
    uint8_t* m_imageData;

public:
    Image();
    Image(const uint8_t* data);
    ~Image();

    uint8_t GetValue(int i, int j) const { return m_imageData[i * InputImage::size + j]; }
    uint8_t* GetData() const { return m_imageData; }

    void AddLine(int nail1, int nail2);
    void SubtractLine(int nail1, int nail2);
    float GetMeanError(int nail1, int nail2) const;

private:
    int PlotLineLow(int x1, int y1, int x2, int y2, int*& xValues, int*& yValues) const;
    int PlotLineHigh(int x1, int y1, int x2, int y2, int*& xValues, int*& yValues) const;
    int PlotVerticalLine(int x, int y1, int y2, int*& xValues, int*& yValues) const;
    int PlotHorizontalLine(int x1, int x2, int y, int*& xValues, int*& yValues) const;
    int PlotLine(int x1, int y1, int x2, int y2, int*& xValues, int*& yValues) const;
    int DrawLine(int nail1, int nail2, int*& xValues, int*& yValues) const;
};