webpinstallfunct() {
if [[ "$CUSTOM_WEBP" == [yY] ]]; then
   echo
   echo "**********************************************************************"
   echo "libwebp installation"
   echo "**********************************************************************"
   echo

   pushd $DIR_TMP/$LIBWEBP_DIR > /dev/null
   ./autogen.sh
   ERR=$?; [ "$ERR" != 0 ] && { echo "ERROR: libwebp autogen failed!"; exit $ERR; }
   echo
   ./configure --prefix=/usr/local
   ERR=$?; [ "$ERR" != 0 ] && { echo "ERROR: libwebp configure failed!"; exit 1; }
   echo
   make
   ERR=$?; [ "$ERR" != 0 ] && { echo "ERROR: libwebp make failed!"; exit 1; }
   echo
   make install
   ERR=$?; [ "$ERR" != 0 ] && { echo "ERROR: libwebp make install failed!"; exit 1; }
   echo
   popd > /dev/null

   echo
   echo "**********************************************************************"
   echo "libgd installation"
   echo "**********************************************************************"
   echo

   pushd $DIR_TMP > /dev/null
   LIBGD_DIR=$(tar -tzf "$LIBGD_FILE" | head -1 | cut -f1 -d"/")
   popd > /dev/null
   pushd $DIR_TMP/$LIBGD_DIR > /dev/null
   autoreconf -i
   ERR=$?; [ "$ERR" != 0 ] && { echo "ERROR: libgd autoreconf failed!"; exit $ERR; }
   echo
   ./configure --prefix=/usr/local --with-webp=/usr/local
   ERR=$?; [ "$ERR" != 0 ] && { echo "ERROR: libgd configure failed!"; exit $ERR; }
   echo
   make
   ERR=$?; [ "$ERR" != 0 ] && { echo "ERROR: libgd make failed!"; exit $ERR; }
   echo
   make install
   ERR=$?; [ "$ERR" != 0 ] && { echo "ERROR: libgd make install failed!"; exit $ERR; }
   echo
   popd > /dev/null
fi
}