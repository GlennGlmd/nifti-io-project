# NIfTI IO Project

A C++ utility for reading and writing NIfTI (`.nii` / `.nii.gz`) files using **libnifti** and the **i3d** wrapper, with accompanying Python scripts for verification.

---

## Project Structure

```
/src
  ├── imgNIFTI.h
  ├── imgNIFTI.cpp
  ├── main.cpp
  ├── testImgGen.cpp
/scripts
  └── compare_voxels.py    # Python script to verify round-trip voxel data
CMakeLists.txt
.gitignore
README.md
```

---

## Prerequisites

* **CMake** ≥ 3.10
* A **C++17**-compatible compiler (GCC, Clang, MSVC)
* **libnifti** (`niftiio`, `znz`, `zlib`) installed on your system
* **i3dcore** library installed or available for linking
* **Python 3.x** with the following packages:

  ```bash
  sudo apt update
  sudo apt install -y cmake g++ build-essential \
    zlib1g-dev libznz-dev libnifti-dev
  ```

---

## Building (C++)

```bash
mkdir build
cd build
cmake ..
make        # or: cmake --build .
```

This generates two executables in `build/`:

* `nifti_io_test`
* `test_image_batch`

---

## Usage (C++)

### 1. nifti\_io\_test

Read an existing NIfTI file and write it back out (round-trip):

```bash
./nifti_io_test path/to/input.nii path/to/output.nii
```

### 2. test\_image\_batch

Run batch image generation tests:

```bash
./test_image_batch
```

---

## Python Verification Script

A helper Python script `scripts/compare_voxels.py` compares voxel data between the original and round‑tripped files.

**Example contents of ****\`\`****:**

```python
import nibabel as nib
import numpy as np
import sys

if len(sys.argv) != 3:
    print("Usage: python compare_voxels.py <original.nii> <roundtrip.nii>")
    sys.exit(1)

orig_file, round_file = sys.argv[1], sys.argv[2]
img1 = nib.load(orig_file)
img2 = nib.load(round_file)
vox1 = img1.get_fdata()
vox2 = img2.get_fdata()

print("Data type (original):", img1.get_data_dtype())
print("Data type (round-trip):", img2.get_data_dtype())

if np.array_equal(vox1, vox2):
    print("✅ The voxel data are identical.")
else:
    print("❌ The voxel data differ.")
```

**Run the verification:**

```bash
python3 scripts/compare_voxels.py path/to/input.nii path/to/output.nii
```

---

## Integrating into Your Project

1. **Include** the header:

   ```cpp
   #include "imgNIFTI.h"
   ```

2. **Link** against the required libraries:

   ```cmake
   target_link_libraries(your_target
     niftiio
     znz
     z
     i3dcore
   )
   ```

3. **See** `src/main.cpp` for a minimal example of using `NIFTIReader` and `NIFTIWriter`.

---
