#ifndef _NIFTI_TOOLS_
#define _NIFTI_TOOLS_

// Standard headers
#include <string>           // for std::string
#include <complex>          // for std::complex
#include <cstdint>          // for fixed-width integer types (e.g., int8_t)
#include <stdexcept>        // for std::runtime_error
#include <nifti/nifti1_io.h> // for libnifti types and I/O functions

// Project-specific headers
#include "i3d/imgfiles.h"   // base classes ImageReader/ImageWriter
#include <i3d/image3d.h>    // definitions of GRAY8, RGB, etc.

namespace i3d {

//
// ==================== Image3dInfo Struct ====================
//
// Holds basic metadata about a 3D image.
//
struct Image3dInfo {
    int width;           // Number of voxels in X
    int height;          // Number of voxels in Y
    int depth;           // Number of voxels in Z
    int datatype;        // NIfTI datatype code (e.g., DT_FLOAT)
    int bytesPerVoxel;   // Size of each voxel in bytes
};

//
// ==================== NIFTIReader Class ====================
//
// Reads NIfTI (.nii/.nii.gz) files into memory by leveraging libnifti.
// Inherits from the generic ImageReader interface.
//
class I3D_CORE_EXPORT NIFTIReader : public ImageReader {
protected:
    nifti_image *nim;    // Pointer to the underlying NIfTI structure

    // Templated helper to copy the raw voxel buffer into a user array.
    // T must match the stored datatype.
    template <class T>
    void LoadData(T *data);

public:
    // Constructor: opens and reads the NIfTI file (header + data).
    // Throws std::runtime_error on failure.
    NIFTIReader(const char *fname);

    // Destructor: frees the nifti_image struct if allocated.
    virtual ~NIFTIReader();

    // Prints basic header info (dimensions, datatype, etc.).
    virtual void LoadImageInfo();

    // Gives direct access to the nifti_image pointer.
    nifti_image* GetImage() const { return nim; }

    // Overrides for each supported voxel type – forwards to LoadData<T>().
    virtual void LoadImageData(bool             *data) { LoadData(data); }
    virtual void LoadImageData(GRAY8            *data) { LoadData(data); }
    virtual void LoadImageData(GRAY16           *data) { LoadData(data); }
    virtual void LoadImageData(RGB              *data) { LoadData(data); }
    virtual void LoadImageData(RGB16            *data) { LoadData(data); }
    virtual void LoadImageData(int8_t           *data) { LoadData(data); }
    virtual void LoadImageData(int16_t          *data) { LoadData(data); }
    virtual void LoadImageData(int32_t          *data) { LoadData(data); }
    virtual void LoadImageData(uint32_t         *data) { LoadData(data); }
    virtual void LoadImageData(float            *data) { LoadData(data); }
    virtual void LoadImageData(double           *data) { LoadData(data); }
    virtual void LoadImageData(std::complex<float> *data) { LoadData(data); }
};

//
// ==================== NIFTIWriter Class ====================
//
// Writes 3D image data into NIfTI format. Inherits from ImageWriter.
//
class I3D_CORE_EXPORT NIFTIWriter : public ImageWriter {
protected:
    std::string filename;   // Output filename (including path)
    nifti_image *nim = nullptr; // NIfTI header + data (built on WriteImageInfo)
    Image3dInfo imageInfo;  // Stored metadata to be used during SaveImageData

public:
    // Constructor: initializes filename and base ImageWriter.
    NIFTIWriter(const char *fname);

    // Prepares and fills a libnifti header structure from Image3dInfo.
    // Call before writing data.
    virtual void WriteImageInfo(const Image3dInfo &info);

    // Templated helper to copy user data into the nifti_image struct and write.
    // T must match datatype and bytesPerVoxel in the header.
    template <typename T>
    void WriteImageData(const T *data, const Image3dInfo &info);

    // Override the generic save calls for the supported pixel types;
    // these use the stored imageInfo from SetImageInfo or WriteImageInfo.
    virtual void SaveImageData(const GRAY8*  data) override { WriteImageData(data, imageInfo); }
    virtual void SaveImageData(const bool*   data) override { WriteImageData(data, imageInfo); }
    virtual void SaveImageData(const RGB*    data) override { WriteImageData(data, imageInfo); }
    virtual void SaveImageData(const GRAY16* data) override { WriteImageData(data, imageInfo); }
    virtual void SaveImageData(const RGB16*  data) override { WriteImageData(data, imageInfo); }

    // Allows the user to set metadata ahead of actually writing pixel data.
    void SetImageInfo(const Image3dInfo &info) { imageInfo = info; }
};

} // namespace i3d

#endif // _NIFTI_TOOLS_
