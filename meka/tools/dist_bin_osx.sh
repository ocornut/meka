#!/bin/bash

MEKA_APPBIN=Dist/Meka.app/Contents/MacOS/meka
RESOURCE_DIR=./Dist/Meka.app/Contents/Resources
BIN_DIR=./Dist/Meka.app/Contents/MacOS
DIR=`dirname $0`

# Ignoring meka.dat while the 0.8x branch is using the Data/ folder (which we should aim to fix)
DATA_FILES="meka.nam meka.thm meka.pat meka.blt meka.inp meka.msg"
DOC_FILES="meka.txt compat.txt multi.txt changes.txt debugger.txt"
#CONF_FILES="meka.cfg"
CONF_FILES=""

function copy_to_resources {
	for file in $1; do
		cp $file $2
	done
}

# Directories
# Since these most likely are supposed to be written to, they should be 
# created in ~/Library/Application Support/MEKA or some place like that. 
DIRS="Screenshots Saves Music Debug"

cd $DIR/..

# Create Meka.app from template.
rm -fR Dist/Meka.app
mkdir -p Dist
cp -r tools/dist_osx/Meka.app_template Dist/Meka.app
mkdir -p $BIN_DIR
mkdir -p $RESOURCE_DIR
cp meka $BIN_DIR

#copy resources files

copy_to_resources "$DATA_FILES" $RESOURCE_DIR
copy_to_resources "$DOC_FILES" $RESOURCE_DIR
copy_to_resources "$CONF_FILES" $RESOURCE_DIR
cp -r Data $RESOURCE_DIR
cp -r Themes $RESOURCE_DIR


# copy libraries from /usr/local/lib to $BIN_DIR

LOCAL_LIBS=$(otool -L $MEKA_APPBIN | awk '/\/usr\/local\/lib\// { print $1}')

for local_lib in $LOCAL_LIBS
do
	cp -r $local_lib $BIN_DIR
	file_name=$(basename $local_lib)

	chmod +w $BIN_DIR/$file_name

	install_name_tool -id @executable_path/$file_name $BIN_DIR/$file_name
	install_name_tool -change $local_lib @executable_path/$file_name $MEKA_APPBIN

	otool -L $BIN_DIR/$file_name
done

# special case, fix path to libpng inside libfreetype.
# TODO: Make a generic function that can do this for any lib.
install_name_tool -change /usr/local/lib/libpng16.16.dylib @loader_path/libpng16.16.dylib $BIN_DIR/libfreetype.6.dylib

otool -L $MEKA_APPBIN
