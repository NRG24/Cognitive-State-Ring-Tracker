#!/bin/bash
# Strip extended attributes from Flutter framework to prevent codesigning errors

echo "Stripping extended attributes from Flutter frameworks..."

# Strip from build directory
find "${BUILT_PRODUCTS_DIR}" -name "*.framework" -type d 2>/dev/null | while read framework; do
    xattr -cr "$framework" 2>/dev/null || true
done

# Strip from source build directory
if [ -d "${SOURCE_ROOT}/../build/ios" ]; then
    xattr -cr "${SOURCE_ROOT}/../build/ios" 2>/dev/null || true
fi

echo "Done stripping extended attributes"
exit 0
