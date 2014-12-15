SDK_VERSION=2.8.1
PACKAGE_NAME=PebbleSDK-$SDK_VERSION
SDK_INCLUDE_FOLDER=$PACKAGE_NAME/Pebble/include
INCLUDE_FOLDER=tests/include
SDK_ZIP_NAME=$PACKAGE_NAME.tar.gz

wget http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/sdk2/$SDK_ZIP_NAME
tar -zxf $SDK_ZIP_NAME
mkdir $INCLUDE_FOLDER
mv $SDK_INCLUDE_FOLDER/pebble.h $INCLUDE_FOLDER
mv $SDK_INCLUDE_FOLDER/pebble_fonts.h $INCLUDE_FOLDER
mv $SDK_INCLUDE_FOLDER/pebble_app_info.h $INCLUDE_FOLDER
touch $INCLUDE_FOLDER/pebble_warn_unsupported_functions.h