modsecurity3install() {
   if [[ "$NGINX_MODSECURITY3" = [yY] ]]; then
      if [[ "${0##*/}" == 'modsecurity2.sh' ]]; then
         echo
         echo "**********************************************************************"
         echo "*****************    ModSecurity v3 installation    ******************"
         echo "**********************************************************************"
      else
         if [[ -f "$(which figlet)" ]]; then
            figlet -ckf standard "ModSecurity v3 install"
         fi
      fi
      echo
      cecho "Installing dependencies..." $boldyellow
      echo

      if [[ ! -d "$DIR_TMP" ]]; then
         yum -y install flex curl curl-devel GeoIP GeoIP-devel libxml2 libxml2-devel doxygen valgrind yajl yajl-devel lmdb lmdb-devel --enablerepo=epel
	   else
         yum -y install doxygen valgrind yajl yajl-devel lmdb lmdb-devel --enablerepo=epel
      fi
      rpm -Uvh https://dl.fedoraproject.org/pub/fedora/linux/updates/23/x86_64/b/bison-3.0.4-3.fc23.x86_64.rpm

      echo
      cecho "Installing ModSecurity v3..." $boldyellow
      echo

      cd $DIR_TMP
      rm -rf ModSecurity
      git clone https://github.com/SpiderLabs/ModSecurity
      ERR=$?; [[ "$ERR" != 0 ]] && { cecho "\nERROR: git clone failed!" $boldred; exit $ERR; }
      cd ModSecurity
      git checkout -b v3/master origin/v3/master
      ERR=$?; [[ "$ERR" != 0 ]] && { cecho "\nERROR: git checkout failed!" $boldred; exit $ERR; }
      echo
      ./build.sh
      ERR=$?; [[ "$ERR" != 0 ]] && { cecho "\nERROR: build.sh failed!" $boldred; exit $ERR; }
      git submodule init
      git submodule update
      echo
      ./configure
      ERR=$?; [[ "$ERR" != 0 ]] && { cecho "\nERROR: configure failed!" $boldred; exit $ERR; }
      echo
      make
      ERR=$?; [[ "$ERR" != 0 ]] && { cecho "\nERROR: make failed!" $boldred; exit $ERR; }
      echo
      make install
      ERR=$?; [[ "$ERR" != 0 ]] && { cecho "\nERROR: make install failed!" $boldred; exit $ERR; }
   fi
}