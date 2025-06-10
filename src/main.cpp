#include "imgNIFTI.h"      // Header for NIFTIReader and NIFTIWriter
#include <iostream>        // For std::cout, std::cerr
#include <vector>          // For std::vector container

using namespace i3d;       // Bring i3d namespace into scope

int main() {
    try {
        // Paths to the input and output NIfTI files
        const char* inputFile  = "testFile/chris_MRA.nii";
        const char* outputFile = "testFile/output_chris_MRA.nii";

        //
        // === Reading the NIfTI file ===
        //

        // Create a reader and load the file header + data
        NIFTIReader reader(inputFile);
        reader.LoadImageInfo();           // Print header info to stdout

        // Retrieve the underlying nifti_image struct
        nifti_image* nim = reader.GetImage();

        // Populate an Image3dInfo struct from the header fields
        Image3dInfo info;
        info.width        = nim->nx;      // X dimension
        info.height       = nim->ny;      // Y dimension
        info.depth        = nim->nz;      // Z dimension
        info.datatype     = nim->datatype;// NIfTI datatype code
        info.bytesPerVoxel= nim->nbyper;  // Bytes per voxel

        // Allocate a buffer to hold all voxels (here using uint8_t)
        size_t totalVoxels = nim->nvox;   // Total number of voxels
        std::vector<uint8_t> buffer(totalVoxels);

        // Copy the voxel data into our buffer
        reader.LoadImageData(buffer.data());

        //
        // === Writing out a new NIfTI file ===
        //

        // Create a writer for the output filename
        NIFTIWriter writer(outputFile);

        // Provide the same metadata we read earlier
        writer.SetImageInfo(info);

        // Write the buffer back to disk as a NIfTI file
        writer.WriteImageData(buffer.data(), info);

        std::cout << "NIfTI file successfully read and re-written." << std::endl;

    } catch (const std::exception& e) {
        // Catch and report any errors (file not found, memory issues, etc.)
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;  // Success
}
