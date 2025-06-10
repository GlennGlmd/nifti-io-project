#include "imgNIFTI.h"
#include <cstring>
#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;
using namespace i3d;

void TestAndCopy(const std::string& inputPath, const std::string& outputPath) {
    try {
        NIFTIReader reader(inputPath.c_str());
        reader.LoadImageInfo();
        auto* nim = reader.GetImage();

        Image3dInfo info;
        info.width = nim->nx;
        info.height = nim->ny;
        info.depth = nim->nz;
        info.datatype = nim->datatype;
        info.bytesPerVoxel = nim->nbyper;
        size_t voxelCount = nim->nvox;

        std::vector<uint8_t> buffer(voxelCount * nim->nbyper);
        reader.LoadImageData(reinterpret_cast<GRAY8*>(buffer.data()));

        NIFTIWriter writer(outputPath.c_str());
        writer.SetImageInfo(info);
        writer.WriteImageData(buffer.data(), info);

        std::cout << "✅ Success: " << inputPath << " -> " << outputPath << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Error with file: " << inputPath << " | " << e.what() << std::endl;
    }
}

int main() {
    std::string inputDir = "test_images";
    std::string outputDir = "test_output";
    fs::create_directory(outputDir);

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (entry.path().extension() == ".nii") {
            std::string input = entry.path().string();
            std::string output = outputDir + "/" + entry.path().filename().string();
            TestAndCopy(input, output);
        }
    }

    return 0;
}
