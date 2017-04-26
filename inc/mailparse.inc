mailparseinstall() {
   if [[ "$PHP_INSTALL" = [yY] ]]; then
   if [[ "$PHPMAILPARSE" = [yY] ]]; then

   mailparsephpexttarball

   echo "*************************************************"
   cecho "* Installing mailparse PHP Extension" $boldgreen
   echo "*************************************************"

   pwd
 
   cd mailparse-${MAILPARSEPHP_VER}
   if [[ "$INITIALINSTALL" != [yY] ]]; then
      make clean
   fi
   /usr/local/bin/phpize
   ./configure --with-php-config=/usr/local/bin/php-config
   make${MAKETHREADS}
   make install

   #######################################################
   # check if mailparse.so exists in php.ini

   MAILPARSESOCHECK=$(grep 'extension=mailparse.so' /usr/local/lib/php.ini >/dev/null 2>&1; echo $?)
   MAILPARSESOCHECKB=$(grep 'extension=mailparse.so' "${CONFIGSCANDIR}/mailparse.ini" >/dev/null 2>&1; echo $?)

   if [[ "$MAILPARSESOCHECK" = '1' || "$MAILPARSESOCHECKB" = '1' ]]; then
      echo -e "\nCopying mailparse.ini > ${CONFIGSCANDIR}/mailparse.ini\n"
      echo "extension=mailparse.so" > ${CONFIGSCANDIR}/mailparse.ini
   fi #check if mailparse.so exists in php.ini

   cmservice php-fpm restart
   echo "*************************************************"
   cecho "* mailparse PHP installed" $boldgreen
   echo "*************************************************"

   fi
   fi # PHP_INSTALL=y
}