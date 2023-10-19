#pragma once
#include <string>

namespace MiscUtils
{
    std::string NormalizeOutputPath(const std::string& OutputPath);
    void DisplayProgressBar(double Percentage, double DownloadSpeed);
}
