#!/bin/sh

set -e

# Get version
version=$(grep ^DERMSHARE_VERSION app/dermshare.py | cut -f2 -d= | tr -d " '\"")
date=$(grep ^DERMSHARE_DATE app/dermshare.py | cut -f2 -d= | tr -d " '\"")
echo "DermShare ${version}, ${date}"

# Make release directory
releasedir="dermshare-${version}"
rm -rf "${releasedir}"
git archive --prefix="${releasedir}/" HEAD | tar x
cd "${releasedir}"

# Clean out release directory
rm -f build-release.sh
find . -name .gitignore -delete

# Autoconfiscate
cd autosegmenter
autoreconf -i
cd ..

# Archive
cd ..
tar cf "${releasedir}.tar" --owner 0 --group 0 "${releasedir}"
xz -9f "${releasedir}.tar"
rm -r "${releasedir}"
