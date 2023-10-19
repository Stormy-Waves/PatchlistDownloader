#include "Downloader.h"

#include <iostream>
#include <sstream>
#include <vector>

#include "MiscUtils.h"

static double startTime = 0;

size_t Downloader::WriteCallback(void* Contents, size_t Size, size_t Nmemb, std::string* Output)
{
    size_t total_size = Size * Nmemb;
    Output->append((char*)Contents, total_size);
    return total_size;
}

size_t Downloader::WriteFileCallback(void* Contents, size_t Size, size_t Nmemb, FILE* Output)
{
    size_t total_size = Size * Nmemb;
    
    return fwrite(Contents, 1, total_size, Output);
}

int Downloader::ProgressCallback(void* ClientTp, double DlTotal, double DlNow, double UlTotal, double UlNow)
{
    if (startTime == 0) {
        startTime = clock();
    }

    clock_t currentTime = clock();
    double elapsed = (currentTime - startTime) / CLOCKS_PER_SEC;

    // Calculate download speed in megabytes per second
    double downloadSpeed = (DlNow / elapsed) / 1e+6;
    
    if (DlTotal > 0)
    {
        double percentage = DlNow / DlTotal;
        MiscUtils::DisplayProgressBar(percentage, downloadSpeed);
    }
    return 0;
}

std::string Downloader::DownloadPatchList(std::string Url)
{

    curl = curl_easy_init();

    if (!curl)
    {
        std::cerr << "Failed to initialize libcurl!" << std::endl;
        curl_global_cleanup();
        exit(-1);
    }
    
    std::string response;

    // Set the URL to fetch
    curl_easy_setopt(curl, CURLOPT_URL, Url.c_str());

    // Set the callback functions for CURL
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Client/++UE4+Release-4.26-CL-0 Windows/10.0.22624.1.256.64bit");

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "Failed to download patch list: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        exit(-1);
    }

    curl_easy_cleanup(curl);
    
    return response;
}

std::vector<std::string> Downloader::ProcessPatchList(const std::string& PatchList, const std::string& BaseUrl)
{
    std::istringstream patchListStream(PatchList);
    std::string line;
    std::vector<std::string> pakUrls;

    while (std::getline(patchListStream, line))
    {
        std::istringstream lineStream(line);
        PatchListData data;

        if (std::getline(lineStream, data.filename, ',') && std::getline(lineStream, data.id, ',') &&
            std::getline(lineStream, data.size, ',') && std::getline(lineStream, data.hash, ',') &&
            std::getline(lineStream, data.number, ','))
        {
            // Create a new URL based on the filename inside the patch list
            std::string pakUrl = BaseUrl.substr(0, BaseUrl.find_last_of('/') + 1) + data.filename;
            pakUrls.push_back(pakUrl);
        }
    }

    return pakUrls;
}

void Downloader::DownloadPakFiles(std::vector<std::string> PakUrls, std::string OutputPath)
{
    curl = curl_easy_init();
    
    if (!curl)
    {
        std::cerr << "Failed to initialize libcurl!" << std::endl;
        curl_global_cleanup();
        exit(-1);
    }

    for (const std::string& pakUrl : PakUrls)
    {
        CURLcode res;

        // Construct the output file path based on the URL and the directory
        std::string filename = pakUrl.substr(pakUrl.find_last_of("/") + 1);
        std::string outputFilePath = OutputPath + filename;

        // Open the local file for writing
        FILE* outputFile = nullptr;
        if (fopen_s(&outputFile, outputFilePath.c_str(), "wb") != 0 || !outputFile)
        {
            std::cerr << "Failed to open output file: " << outputFilePath << std::endl;
            continue; // Skip to the next URL
        }
        
        curl_easy_setopt(curl, CURLOPT_URL, pakUrl.c_str());

        // Set the callback function to write data to the local file and user agent
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, outputFile);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Client/++UE4+Release-4.26-CL-0 Windows/10.0.22624.1.256.64bit");

        // Perform the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "Download failed: %s\n", curl_easy_strerror(res));
        }

        if (fclose(outputFile) != 0 || !outputFile)
        {
            std::cerr << "Failed to close output file: " << OutputPath << std::endl;
        } 
    }
    
    printf("Downloaded all files to %s", OutputPath.c_str());
}
