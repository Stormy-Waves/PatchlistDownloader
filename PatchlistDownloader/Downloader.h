#pragma once
#include <chrono>
#include <string>
#include <vector>
#include <curl/curl.h>

/* The Wuthering Waves patch list file structure, the values called id and number are still unknown. */
struct PatchListData
{
    std::string filename;
    std::string id;
    std::string size;
    std::string hash;
    std::string number;
};

struct DownloadSpeedInfo
{
    size_t totalBytesReceived;
    double downloadSpeed;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    DownloadSpeedInfo() : totalBytesReceived(0), downloadSpeed(0.0)
    {
        startTime = std::chrono::high_resolution_clock::now();
    }

    
    double GetElapsedTime()
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = currentTime - startTime;
        return elapsedTime.count();
    }
};

class Downloader
{
public:
    /* Reads the Wuthering Waves patch list on the given URL and returns it. */
    std::string DownloadPatchList(std::string Url);

    /* Process the given PatchList string, and generates .pak file download urls from the patch list and given BaseUrl. */
    std::vector<std::string> ProcessPatchList(const std::string& PatchList, const std::string& BaseUrl);

    /* Downloads .pak files from the URLs in the given PakUrls string vector to the given OutputPath */
    void DownloadPakFiles(std::vector<std::string> PakUrls, std::string OutputPath);

protected:
    CURL* curl = nullptr;

private:
    static size_t WriteCallback(void* Contents, size_t Size, size_t Nmemb, std::string* Output);
    static size_t WriteFileCallback(void* Contents, size_t Size, size_t Nmemb, FILE* Output);
    static int ProgressCallback(void* ClientTp, double DlTotal, double DlNow, double UlTotal, double UlNow);
};
