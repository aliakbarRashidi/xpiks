#!/bin/bash

if [ ! -f ../deps/exiftool ]; then
    echo "Exiftool not found! Please put latest production release into the ../deps/ dir"
    exit
fi

APP_NAME=xpiks-qt
VERSION="1.4.2"
VOL_NAME="Xpiks"

DMG_BACKGROUND_IMG="dmg-background.jpg"
DMG_BACKGROUND_PATH="../deps/$DMG_BACKGROUND_IMG"

DMG_TMP="xpiks-qt-v${VERSION}.tmp.dmg"
DMG_FINAL="xpiks-qt-v${VERSION}.dmg"
STAGING_DIR="./osx_deploy"

EXIFTOOL_DIR="../deps"

BUILD_DIR="../../build-xpiks-qt-Desktop_Qt_5_6_2_clang_64bit-Release"

if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found: $BUILD_DIR"
    exit
fi

if [ -d "/Volumes/${VOL_NAME}" ]; then
    echo "Please unmount existing Volume ${VOL_NAME} first!"
    exit
fi

pushd "$BUILD_DIR"

~/Qt5.6.2/5.6/clang_64/bin/macdeployqt xpiks-qt.app -verbose=2 -executable=xpiks-qt.app/Contents/MacOS/xpiks-qt -qmldir=../xpiks-qt/ -qmldir=../xpiks-qt/Components/ -qmldir=../xpiks-qt/Constants/ -qmldir=../xpiks-qt/Dialogs/ -qmldir=../xpiks-qt/StyledControls/ -qmldir=../xpiks-qt/StackViews/ -qmldir=../xpiks-qt/CollapserTabs/

popd

echo "Copying libraries..."

LIBS_PATH="../../../libs/release"

LIBS_TO_COPY=(
    libavcodec.57.dylib
    libavdevice.57.dylib
    libavfilter.6.dylib
    libavformat.57.dylib
    libavutil.55.dylib
    libswscale.4.dylib
)

LIBS_TO_DEPLOY=(
    libssdll.1.0.0.dylib
    libface.1.0.0.dylib
    libquazip.1.0.0.dylib
    libavthumbnailer.1.0.0.dylib
    libthmbnlr.1.0.0.dylib
)

FRAMEWORKS_DIR="$BUILD_DIR/xpiks-qt.app/Contents/Frameworks"
RESOURCES_DIR="$BUILD_DIR/xpiks-qt.app/Contents/Resources"

for lib in "${LIBS_TO_DEPLOY[@]}"
do
    echo "Processing $lib..."
    cp "$LIBS_PATH/$lib" "$BUILD_DIR/xpiks-qt.app/Contents/Frameworks/"

    LIBENTRY="${lib%.0.0.dylib}.dylib"
    
    install_name_tool -change $LIBENTRY "@executable_path/../Frameworks/$LIBENTRY" "$BUILD_DIR/xpiks-qt.app/Contents/MacOS/xpiks-qt"

    ln -s "$FRAMEWORKS_DIR/$lib" "$FRAMEWORKS_DIR/$LIBENTRY"
done

# just copying

for lib in "${LIBS_TO_COPY[@]}"
do
    echo "Copying $lib..."
    cp "$LIBS_PATH/$lib" "$BUILD_DIR/xpiks-qt.app/Contents/Frameworks/"
    LIBENTRY="${lib%.0.0.dylib}.dylib"
    install_name_tool -change $LIBENTRY "@executable_path/../Frameworks/$LIBENTRY" "$BUILD_DIR/xpiks-qt.app/Contents/MacOS/xpiks-qt"
done

echo "Copying exiftool distribution"

cp "$EXIFTOOL_DIR/exiftool" "$RESOURCES_DIR"
cp -r "$EXIFTOOL_DIR/lib" "$RESOURCES_DIR/"

# ------------------------------

# clear out any old data
rm -rf "${STAGING_DIR}" "${DMG_TMP}" "${DMG_FINAL}"
# copy over the stuff we want in the final disk image to our staging dir
mkdir -p "${STAGING_DIR}"
cp -rpf "$BUILD_DIR/${APP_NAME}.app" "${STAGING_DIR}"

echo "App dir is packed. Creating DMG..."

# figure out how big our DMG needs to be
#  assumes our contents are at least 1M!
SIZE=`du -sh "${STAGING_DIR}" | sed 's/\([0-9\.]*\)M\(.*\)/\1/'`
SIZE=`echo "${SIZE} + 32.0" | bc | awk '{print int($1+0.5)}'`

if [ $? -ne 0 ]; then
   echo "Error: Cannot compute size of staging dir"
   exit
fi

hdiutil create -srcfolder "${STAGING_DIR}" -volname "${VOL_NAME}" -fs HFS+ \
-fsargs "-c c=64,a=16,e=16" -format UDRW -size ${SIZE}m "${DMG_TMP}"

echo "Created DMG: ${DMG_TMP}. Mounting..."

# mount it and save the device
DEVICE=$(hdiutil attach -readwrite -noverify "${DMG_TMP}" | \
         egrep '^/dev/' | sed 1q | awk '{print $1}')

sleep 2

# add a link to the Applications dir
echo "Add link to /Applications"
pushd /Volumes/"${VOL_NAME}"
ln -s /Applications
popd


# add a background image
mkdir /Volumes/"${VOL_NAME}"/.background
cp "${DMG_BACKGROUND_PATH}" /Volumes/"${VOL_NAME}"/.background/

# tell the Finder to resize the window, set the background,
#  change the icon size, place the icons in the right position, etc.
echo '
   tell application "Finder"
     tell disk "'${VOL_NAME}'"
           open
           set current view of container window to icon view
           set toolbar visible of container window to false
           set statusbar visible of container window to false
           set the bounds of container window to {480, 300, 980, 570}
           set statusbar visible of container window to false
           set position of every item to {600, 400}
           set viewOptions to the icon view options of container window
           set arrangement of viewOptions to not arranged
           set icon size of viewOptions to 128
           set background picture of viewOptions to file ".background:'${DMG_BACKGROUND_IMG}'"
           set position of item "'${APP_NAME}'.app" of container window to {110, 135}
           set position of item "Applications" of container window to {395, 130}
           set name of item "Applications" to " "
           close
           open
           update without registering applications
           delay 2
     end tell
   end tell
' | osascript

sync

# unmount it
hdiutil detach "${DEVICE}"

# now make the final image a compressed disk image
echo "Creating compressed image"
hdiutil convert "${DMG_TMP}" -format UDZO -imagekey zlib-level=9 -o "${DMG_FINAL}"

shasum "${DMG_FINAL}"

# clean up
rm -rf "${DMG_TMP}"
rm -rf "${STAGING_DIR}"

echo 'Done.'