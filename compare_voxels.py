import nibabel as nib    # Library for reading/writing neuroimaging data (NIfTI, etc.)
import numpy as np       # Numerical Python for array operations

# Load the original NIfTI file into a Nibabel image object
img1 = nib.load("../build/testFile/chris_MRA.nii")

# Load the output (round-tripped) NIfTI file
img2 = nib.load("../build/testFile/output_chris_MRA.nii")

# Extract the voxel data as floating-point NumPy arrays
vox1 = img1.get_fdata()  
vox2 = img2.get_fdata()  

# Print the reported data type of each image
print("Data type:", img1.get_data_dtype())  # e.g., int16, float32, etc.
print("Data type:", img2.get_data_dtype())  # should match img1 if preserved correctly

# Compare the two arrays element-wise for exact equality
if np.array_equal(vox1, vox2):
    print("The voxel data are identical.")  # Voxels match exactly
else:
    print("The voxel data differ.")         # Voxels differ