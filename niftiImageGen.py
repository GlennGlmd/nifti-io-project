import nibabel as nib
import numpy as np
import os

# Output folder
OUTPUT_DIR = "test_images"
os.makedirs(OUTPUT_DIR, exist_ok=True)
SHAPE = (25, 25, 25)

def save_nifti(data, name):
    path = os.path.join(OUTPUT_DIR, f"{name}.nii")
    try:
        img = nib.Nifti1Image(data, affine=np.eye(4))
        nib.save(img, path)
        print(f"✅ Saved: {name}.nii | shape: {data.shape} | dtype: {data.dtype}")
    except Exception as e:
        print(f"❌ Failed to save {name} | dtype: {data.dtype} | Error: {e}")

# --------------------------
# Generate NIfTI test images
# --------------------------

data_types = [
    ("bool_as_uint8", np.random.choice([0, 1], size=SHAPE).astype(np.uint8)),
    ("gray8", np.random.randint(0, 256, size=SHAPE, dtype=np.uint8)),
    ("gray16", np.random.randint(0, 65536, size=SHAPE, dtype=np.uint16)),
    ("int8", np.random.randint(-128, 128, size=SHAPE, dtype=np.int8)),
    ("int16", np.random.randint(-32768, 32768, size=SHAPE, dtype=np.int16)),
    ("int32", np.random.randint(-2**31, 2**31 - 1, size=SHAPE, dtype=np.int32)),
    ("uint32", np.random.randint(0, 2**31 - 1, size=SHAPE, dtype=np.uint32)),
    ("float32", np.random.rand(*SHAPE).astype(np.float32)),
    ("float64", np.random.rand(*SHAPE).astype(np.float64)),
    ("complex64", (np.random.rand(*SHAPE) + 1j * np.random.rand(*SHAPE)).astype(np.complex64)),
    ("rgb", np.random.randint(0, 256, size=SHAPE + (3,), dtype=np.uint8)),
]

for name, data in data_types:
    save_nifti(data, name)

# --------------------------
# Verify saved NIfTI files
# --------------------------

print("\n🔎 Verifying generated NIfTI files:\n")

summary = []

for file in sorted(os.listdir(OUTPUT_DIR)):
    if file.endswith(".nii"):
        filepath = os.path.join(OUTPUT_DIR, file)
        try:
            img = nib.load(filepath)
            data = img.get_fdata()
            summary.append({
                "filename": file,
                "shape": str(data.shape),
                "dtype": str(data.dtype),
                "min": np.min(data),
                "max": np.max(data)
            })
            print(f"✅ {file:<20} | shape: {data.shape} | dtype: {data.dtype} | min: {np.min(data):.2f} | max: {np.max(data):.2f}")
        except Exception as e:
            summary.append({
                "filename": file,
                "shape": "ERROR",
                "dtype": "ERROR",
                "min": "ERROR",
                "max": "ERROR"
            })
            print(f"❌ {file:<20} | Error: {e}")
