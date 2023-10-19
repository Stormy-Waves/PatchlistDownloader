#include "cmdparser.hpp"
#include "Downloader.h"
#include "MiscUtils.h"

int main(int argc, char* argv[])
{
    // Initialize CLI parser and downloader
    cli::Parser parser(argc, argv);
    Downloader downloader;
    
    parser.set_required<std::string>("url", "url", "URL to patch list");
    parser.set_optional<std::string>("o", "output-path", "./", ".pak file output path");
    parser.run_and_exit_if_error();

    // Save arguments to variables
    std::string url = parser.get<std::string>("url");
    std::string output_path = MiscUtils::NormalizeOutputPath(parser.get<std::string>("o"));

    // Download the patch list
    std::string patchlist = downloader.DownloadPatchList(url);

    // Process the patch patch list
    std::vector<std::string> pak_urls = downloader.ProcessPatchList(patchlist, url);

    // Delete the patch list to free up memory
    patchlist.clear();

    // Download the .pak files
    printf("The progress bar shows the download progress of the current .pak file.\n");
    downloader.DownloadPakFiles(pak_urls, output_path);

    // Cleanup CURL
    curl_global_cleanup();

    return 0;
}
