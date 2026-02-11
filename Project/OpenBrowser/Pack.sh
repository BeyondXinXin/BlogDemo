# build

# unpack

# 路径设置  
export QT_PATH=/home/yc/Qt5.11.3/5.11.3/gcc_64         
export openBrowser_PATH=/home/yc/Documents/QT/openBrowser
export PATH=$QT_PATH/bin:$PATH


#  判断是否有relese后生成的文件，如果有则删除之前打包好的文件
# -f filename 如果 filename为常规文件，则为真
if [ -f "$openBrowser_PATH/progrom/bin/openBrowser" ]
then
rm -rf "./openBrowser.AppImage"
cp "$openBrowser_PATH/progrom/bin/openBrowser" "$openBrowser_PATH/App/usr/bin/openBrowser"
else
echo "can not find openBrowser"
exit 0
fi

#  删除之前生打包生成文件
rm -rf "$openBrowser_PATH/App/AppRun"
rm -rf "$openBrowser_PATH/App/.DirIcon"
rm -rf "$openBrowser_PATH/App/usr/bin/qt.conf"
rm -rf "$openBrowser_PATH/App/usr/lib"
rm -rf "$openBrowser_PATH/App/usr/plugins"
rm -rf "$openBrowser_PATH/App/usr/share/doc"
rm -rf "$openBrowser_PATH/App/usr/translations"
mkdir "$openBrowser_PATH/App/usr/lib"

#  中文输入（fcitx），就是谷歌输入法里的之前移动到qt plugins里
cp "$QT_PATH/plugins/platforminputcontexts/libfcitxplatforminputcontextplugin.so" "$openBrowser_PATH/App/usr/lib/libfcitxplatforminputcontextplugin.so"

if [ -f "$QT_PATH/plugins/sqldrivers/libqsqlmysql.so" ] 
then
mv "$QT_PATH/plugins/sqldrivers/libqsqlmysql.so" "$QT_PATH/plugins/sqldrivers/libqsqlmysql.so.bak"
fi

if [ -f "$QT_PATH/plugins/sqldrivers/libqsqlpsql.so" ] 
then
mv "$QT_PATH/plugins/sqldrivers/libqsqlpsql.so" "$QT_PATH/plugins/sqldrivers/libqsqlpsql.so.bak"
fi

#  打包插件
./linuxdeployqt.AppImage "$openBrowser_PATH/App/usr/bin/openBrowser" -appimage

if [ -f "$QT_PATH/plugins/sqldrivers/libqsqlmysql.so.bak" ] 
then
mv "$QT_PATH/plugins/sqldrivers/libqsqlmysql.so.bak" "$QT_PATH/plugins/sqldrivers/libqsqlmysql.so"
fi

if [ -f "$QT_PATH/plugins/sqldrivers/libqsqlpsql.so.bak" ] 
then
mv "$QT_PATH/plugins/sqldrivers/libqsqlpsql.so.bak" "$QT_PATH/plugins/sqldrivers/libqsqlpsql.so"
fi