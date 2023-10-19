#include "MiscUtils.h"

std::string MiscUtils::NormalizeOutputPath(const std::string& OutputPath)
{
    if (OutputPath.empty() || OutputPath.back() == '/' || OutputPath.back() == '\\')
    {
        return OutputPath;
    }
    else if (OutputPath.find_first_of('\\'))
    {
        return OutputPath + "\\";
    }
    else
    {
        return OutputPath + "/";
    }
}

void MiscUtils::DisplayProgressBar(double Percentage, double DownloadSpeed)
{
    int barWidth = 50;
    int progress = Percentage * barWidth;

    printf("[");
    for (int i = 0; i < barWidth; i++)
    {
        if (i < progress)
        {
            printf("=");
        }
        else if (i == Percentage)
        {
            printf(">");
        }
        else
        {
            printf(" ");
        }
    }
    printf("] %.1f%%  Speed: %.2f MB/s\r", Percentage * 100, DownloadSpeed);
    fflush(stdout);
}
