#include "imgNIFTI.h"
#include <cstring>    // for std::memcpy
#include <iostream>   // for std::cout, std::endl

namespace i3d {

//
// ==================== NIFTIReader ====================
//

// Constructor: attempts to read a NIfTI file by name.
// - fname: path to the .nii or .nii.gz file.
// Uses nifti_image_read from the libnifti library.
NIFTIReader::NIFTIReader(const char *fname) {
    // Read header + data (second argument = 1 to also load image data).
    nim = nifti_image_read(fname, 1);
    if (!nim) {
        // If read fails, throw an exception with the filename.
        throw std::runtime_error("Failed to read NIfTI file: " + std::string(fname));
    }
}

// Destructor: frees the NIfTI image structure if it was allocated.
NIFTIReader::~NIFTIReader() {
    if (nim) {
        nifti_image_free(nim);
    }
}

// Print basic metadata of the loaded image to stdout.
void NIFTIReader::LoadImageInfo() {
    // nim->fname may be null, so we guard with a conditional.
    std::cout << "Reading NIfTI file: "
              << (nim->fname ? nim->fname : "[no name]") 
              << std::endl;
    std::cout << "Datatype: " << nim->datatype << std::endl;
    std::cout << "Dimensions: "
              << nim->nx << " x "
              << nim->ny << " x "
              << nim->nz << std::endl;
    std::cout << "Voxel Count: " << nim->nvox << std::endl;
}

// Template method to copy image data into a user-provided buffer.
// - T: pixel/voxel data type (e.g., float, int16_t, custom structs).
// - data: pointer to a pre-allocated array of length >= nvox.
//
// Use of reinterpret_cast:
//   reinterpret_cast<T*>(nim->data) treats the raw void* data pointer
//   as if it were T*. Then memcpy copies bytes accordingly.
template <class T>
void NIFTIReader::LoadData(T *data) {
    if (!nim || !nim->data) {
        throw std::runtime_error("Invalid NIfTI image or missing data");
    }

    T *raw = reinterpret_cast<T *>(nim->data);
    std::memcpy(data, raw, nim->nvox * sizeof(T));
}

// Explicit instantiation of the template for supported types.
// This ensures the compiler generates code for each T in the list.
#define INSTANTIATE_LOAD(T) template void NIFTIReader::LoadData(T *data);
INSTANTIATE_LOAD(bool)
INSTANTIATE_LOAD(GRAY8)
INSTANTIATE_LOAD(GRAY16)
INSTANTIATE_LOAD(RGB)
INSTANTIATE_LOAD(RGB16)
INSTANTIATE_LOAD(int8_t)
INSTANTIATE_LOAD(int16_t)
INSTANTIATE_LOAD(int32_t)
INSTANTIATE_LOAD(uint32_t)
INSTANTIATE_LOAD(float)
INSTANTIATE_LOAD(double)
INSTANTIATE_LOAD(std::complex<float>)
#undef INSTANTIATE_LOAD

//
// ==================== NIFTIWriter ====================
//

// Constructor: forwards filename to base ImageWriter and stores it.
// - fname: output filename (must end in .nii or .nii.gz).
NIFTIWriter::NIFTIWriter(const char *fname)
    : ImageWriter(fname), filename(fname) {}

// Prepares the nifti_image header from Image3dInfo.
// - info contains width/height/depth, datatype code, bytes per voxel, etc.
// Uses nifti_simple_init_nim to get a zero-initialized structure.
void NIFTIWriter::WriteImageInfo(const Image3dInfo &info) {
    // Allocate and initialize a fresh nifti_image struct
    nifti_image *nim = nifti_simple_init_nim();

    // Set dimensions
    nim->nx = info.width;
    nim->ny = info.height;
    nim->nz = info.depth;
    nim->nt = 1;   // time dimension = 1
    nim->nu = 1;   // vector dimension = 1

    // Data type and size per voxel
    nim->datatype = info.datatype;
    nim->nbyper   = info.bytesPerVoxel;
    nim->nvox     = nim->nx * nim->ny * nim->nz;

    // Populate the dim array: dim[0] holds rank (3D).
    nim->dim[0] = 3;
    nim->dim[1] = nim->nx;
    nim->dim[2] = nim->ny;
    nim->dim[3] = nim->nz;
    for (int i = 4; i < 8; ++i) nim->dim[i] = 1;

    // Associate the filename and compression flag (0 = no change, 1 = compress)
    nifti_set_filenames(nim, filename.c_str(), /*set_byte_order=*/0, /*compress=*/1);

    // Store to member so WriteImageData can find it.
    this->nim = nim;
}

// Template method to write image voxel data and then save to disk.
// - T: voxel data type.
// - data: pointer to voxel buffer.
// - info: metadata matching the data layout above.
//
// Logical flow:
// 1) If header not yet created, call WriteImageInfo(info).
// 2) Allocate raw buffer of size nvox * bytesPerVoxel.
// 3) Copy data into nim->data.
// 4) Write file with nifti_image_write.
// 5) Free memory and reset pointer.
template <typename T>
void NIFTIWriter::WriteImageData(const T *data, const Image3dInfo &info) {
    if (!nim) {
        WriteImageInfo(info);
    }

    // Allocate raw data buffer
    nim->data = malloc(nim->nvox * nim->nbyper);
    if (!nim->data) {
        throw std::runtime_error("Failed to allocate memory for NIfTI image data");
    }

    // Copy user data into the nifti_image struct
    std::memcpy(nim->data, data, nim->nvox * nim->nbyper);

    // Write the .nii/.nii.gz file
    nifti_image_write(nim);

    // Clean up the nifti_image struct and its data buffer
    nifti_image_free(nim);
    nim = nullptr;
}

// Explicit instantiation of WriteImageData for supported types.
#define INSTANTIATE_WRITE(T) \
    template void NIFTIWriter::WriteImageData(const T *data, const Image3dInfo &info);
INSTANTIATE_WRITE(bool)
INSTANTIATE_WRITE(GRAY8)
INSTANTIATE_WRITE(GRAY16)
INSTANTIATE_WRITE(RGB)
INSTANTIATE_WRITE(RGB16)
INSTANTIATE_WRITE(int8_t)
INSTANTIATE_WRITE(int16_t)
INSTANTIATE_WRITE(int32_t)
INSTANTIATE_WRITE(uint32_t)
INSTANTIATE_WRITE(float)
INSTANTIATE_WRITE(double)
INSTANTIATE_WRITE(std::complex<float>)
#undef INSTANTIATE_WRITE

} // namespace i3d
