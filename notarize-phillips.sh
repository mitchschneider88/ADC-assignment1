#!/bin/sh  

if [ "$EUID" -ne 0 ]
  then echo "Error: Please run this command as root using sudo..."
  echo ""
  exit
fi	

PLUGIN_NAME="Phillips_Assignment1"
DEVELOPER_ID="Mitchell Schneider (JLV6NFB3FD)"
APP_PASS="txzz-twwt-tngr-vgrs"
APPLE_ID="mitchschneider88@gmail.com"
IDENTIFIER="com.phillips_assignment1.pkg"
VERSION="1.0"
VERSION_UNDERSCORES="1_0"
WC_GUID="818C3880-3FCD-11EE-88C0-00505692AD3E"

if ! [ -d "tmp" ]; then
  `mkdir -p tmp`
fi

if ! [ -d "install" ]; then
  `mkdir -p install`
fi

UPPER=$(echo "$PLUGIN_NAME" |  tr '[:lower:]' '[:upper:]' )
BUILD_FOLDER=/Users/mitchschneider_/Desktop/Code/C++/Phillips_Assignment1/Builds/MacOSX/build/Release/

#cp -r "$BUILD_FOLDER""$PLUGIN_NAME".component tmp/
cp -r "$BUILD_FOLDER""$PLUGIN_NAME".vst3 tmp/
#cp -r "$BUILD_FOLDER""$PLUGIN_NAME".aaxplugin tmp/

#cp EULA.txt install/resources
#cp README.txt install/resources
#cp Welcome.txt install/resources
#cp resources/img/BG.png install/resources


#AU_PATH=tmp/"$PLUGIN_NAME".component
VST3_PATH=tmp/"$PLUGIN_NAME".vst3
#AAX_PATH=tmp/"$PLUGIN_NAME".aaxplugin

#use pkgbuild to build the  plugin installers - and sign with Apple Developer ID Application certificate

#sudo codesign -s "Developer ID Application: $DEVELOPER_ID" "$AU_PATH" --timestamp --force --deep -o runtime,hard

#pkgbuild --install-location /Library/Audio/Plug-Ins/Components --identifier "$IDENTIFIER"au --version $VERSION --component "$AU_PATH" install/"$PLUGIN_NAME"_au.pkg

sudo codesign -s "Developer ID Application: $DEVELOPER_ID" "$VST3_PATH" --timestamp --force --deep -o runtime,hard

pkgbuild --install-location /Library/Audio/Plug-Ins/VST3 --identifier "$IDENTIFIER"vst3 --version $VERSION --component "$VST3_PATH" install/"$PLUGIN_NAME"_vst3.pkg

#special avid codesigning

#/Applications/PaceAntiPiracy/Eden/Fusion/Current/bin/wraptool sign --verbose --account mitchschneider_ --password Designed4$ --wcguid "$WC_GUID" --signid "Developer ID Application: $DEVELOPER_ID" --in "$AAX_PATH" --out "$AAX_PATH" --autoinstall on --dsigharden

#pkgbuild --install-location /Library/Application\ Support/Avid/Audio/Plug-Ins --identifier "$IDENTIFIER"aaxplugin --version $VERSION --component "$AAX_PATH" install/"$PLUGIN_NAME"_aaxplugin.pkg

cd install

#use productbuild to synthesize installers

productbuild --synthesize --package "$PLUGIN_NAME"_vst3.pkg distribution.xml #--package "$PLUGIN_NAME"_aaxplugin.pkg "$PLUGIN_NAME"_au.pkg --package

# include EULA, README, Welcome file, and Background image in installer
#gsed -i~ '3i\    <license file="EULA.txt" mime-type="text/plain" />' distribution.xml
#gsed -i~ '3i\    <welcome file="Welcome.txt" mime-type="text/plain" />' distribution.xml
#gsed -i~ '3i\    <readme file="README.txt" mime-type="text/plain" />' distribution.xml
#gsed -i~ '3i\    <background file="BG.png" alignment="bottomleft" mime-type="image/png" scaling="tofit" />' distribution.xml
#gsed -i~ "3i\    <title>$PLUGIN_NAME</title>" distribution.xml

# use productbuild to combine everything into a single installer and sign with Apple Developer ID Installer certificate

productbuild --distribution distribution.xml "$PLUGIN_NAME".pkg

productsign --sign "Developer ID Installer: $DEVELOPER_ID" "$PLUGIN_NAME".pkg "$PLUGIN_NAME"_MacOS_v_"$VERSION_UNDERSCORES".pkg

# notarize the installer

xcrun notarytool submit "$PLUGIN_NAME"_MacOS_v_"$VERSION_UNDERSCORES".pkg --keychain-profile "Mitchell Schneider"

cd ..
rm -r tmp
echo "Done!"

#check history to see if notarizationw was accepted with this command:
#xcrun notarytool history --keychain-profile "Mitchell Schneider"

#staple packaged with this command:
#sudo xcrun stapler staple install/$PLUGIN_NAME_MacOS_v_VERSION_UNDERSCORES.pkg
#sudo xcrun stapler staple install/Phillips_Assignment1_MacOS_v_1_0.pkg

#test the .pkg with this command

#spctl -a -t install -vvv $PLUGIN_NAME_MacOS_v_$VERSION_UNDERSCORES.pkg
#cd install spctl -a -t install -vvv Phillips_Assignment1_MacOS_v_1_0.pkg
