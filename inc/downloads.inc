validcheck_downloads() {
   {
   echo
   echo "check download source fie integrity"
   for f in $(find $DIR_TMP -maxdepth 1 -type f -name "*.gz" -o -name "*.tgz"); do echo -n "$f = "; if [[ "$(tar -tzf $f >/dev/null; echo $?)" = '0' ]]; then echo ok; else echo invalid; fi; done | sort
   echo
   } 2>&1 | tee "${CENTMINLOGDIR}/validcheck_downloads_${DT}.log"
}

checklogdetails() {
   echo
   cecho "**************************************************************" $boldred
   cecho "Error! Check Centmin Mod log for details at /root/centminlogs/" $boldred
   cecho "Aborting script..." $boldred
   cecho "**************************************************************" $boldred
}

waitpm() {
   local res=0
   for pid in $1; do
      wait $pid || let "res=64"
   done
   [ "$res" -eq 0 ] || exit 64
}

###############################################
# yumcentosfivedownload moved
# yumcentossixdownload moved
# yumcentossevendownload moved

###############################################
phpxz_detect() {
   local phpv=$(echo "${phpver:-$PHP_VERSION}" | cut -d . -f1,2)
   if [[ -f /usr/bin/xz && "$phpv" > 5.4 && "$lessphpmem" != [yY] ]]; then
      PHPEXTSION='xz'
      PHPTAR_FLAGS='xJf'
   else
      PHPEXTSION='gz'
      PHPTAR_FLAGS='xzf'
   fi
}

phpgeolocation() {
   phpxz_detect

   # determine server geo location country wise and setup php download mirror 
   # closest to that country for faster downloads to speed up installs upgrades
  
   # Get server's 2 digit country code
   SERVER_COUNTRY=$(curl -s ipinfo.io/geo 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' | egrep -v 'phone|postal|loc' | awk -F ": " '/country/ {print $2}')

   local AT="http://at1.php.net"
   local AU="http://au2.php.net"
   local BE="http://be2.php.net"
   local BR="http://br2.php.net"
   local CA="http://ca2.php.net"
   local CH="http://ch1.php.net"
   local CL="http://cl1.php.net"
   local CZ="http://cz1.php.net"
   local DE="http://de1.php.net"
   local DK="http://dk2.php.net"
   local ES="http://es1.php.net"
   local FI="http://fi2.php.net"
   local FR="http://fr2.php.net"
   local GB="http://uk1.php.net"
   local HK="http://hk1.php.net"
   local ID="http://id1.php.net"
   local IE="http://ie1.php.net"
   local IL="http://il1.php.net"
   local IN="http://in1.php.net"
   local IR="http://ir2.php.net"
   local IS="http://is1.php.net"
   local IT="http://it2.php.net"
   local JP="http://jp2.php.net"
   local LV="http://lv1.php.net"
   local LT="http://lt1.php.net"
   local MD="http://md1.php.net"
   local MY="http://my1.php.net"
   local MX="http://mx1.php.net"
   local NL="http://nl1.php.net"
   local NC="http://nc1.php.net"
   local NZ="http://nz2.php.net"
   local NO="http://no2.php.net"
   local PA="http://pa1.php.net"
   local PH="http://sg2.php.net"
   local PL="http://pl1.php.net"
   local RO="http://ro1.php.net"
   local RU="http://ua2.php.net"
   local SG="http://sg2.php.net"
   local SI="http://si1.php.net"
   local SE="http://se1.php.net"
   local TW="http://tw1.php.net"
   local TH="http://th1.php.net"
   local TR="http://tr1.php.net"
   local TZ="http://tz1.php.net"
   local VN="http://my1.php.net"
   local UA="http://ua2.php.net"
   local UK="http://uk1.php.net"
   local US="http://php.net"

   local success=1
   if [[ -n "$SERVER_COUNTRY" && -n ${!SERVER_COUNTRY} ]]; then
      echo "$SERVER_COUNTRY server detected"
      PHP_MIRRORURL="${!SERVER_COUNTRY}"
      echo "Checking PHP mirror ${PHP_MIRRORURL}/get/php-${PHP_VERSION}.tar.${PHPEXTSION}/from/this/mirror"
      curl -4sIL --connect-timeout 5 --max-time 5 "${PHP_MIRRORURL}/get/php-${PHP_VERSION}.tar.${PHPEXTSION}/from/this/mirror" | grep 'HTTP\/' | grep '200'
      success=$?
      if [ "$success" != 0 ]; then
         echo "The mirror is not available."
      else
         echo 'The mirror is available.'
      fi
   fi
   if [ "$success" -ne 0 ]; then
      PHP_MIRRORURL='http://php.net'
      echo "Use default PHP mirror - $PHP_MIRRORURL"
   fi
   
   PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"
}

###############################################
libresslgeolocation() {
   # determine server geo location country wise and setup libressl download mirror 
   # closest to that country for faster downloads http://www.openbsd.org/ftp.html
  
   # Get server's 2 digit country code
   SERVER_COUNTRY=$(curl -s --max-time 5 --connect-timeout 5 ipinfo.io/geo 2>&1 | sed -e 's|[{}]||' -e 's/\(^"\|"\)//g' -e 's|,||' | egrep -v 'phone|postal|loc' | awk -F ": " '/country/ {print $2}')
   
   local AT=( 'http://ftp2.eu.openbsd.org' )
   local AU=( 'http://mirror.aarnet.edu.au' )
   local BG=( 'https://openbsd.ipacct.com' )
   local BR=( 'http://mirrors.unb.br' )
   local CA=( 'http://openbsd.cs.toronto.edu' )
   local CH=( 'http://mirror.switch.ch/ftp' )
   local CN=( 'http://mirror.yandex.ru' )
   local CR=( 'http://mirrors.ucr.ac.cr' )
   local DE=( 'http://ftp.hostserver.de' )
   local DK=( 'http://mirror.one.com' )
   local EE=( 'http://ftp.aso.ee' )
   local FR=( 'http://ftp.fr.openbsd.org' )
   local GB=( 'http://mirror.bytemark.co.uk' )
   local GR=( 'http://ftp.cc.uoc.gr' )
   local HK=( 'https://openbsd.hk' )
   local HU=( 'http://ftp.fsn.hu' )
   local ID=( 'http://kartolo.sby.datautama.net.id' )
   local IE=( 'http://ftp.heanet.ie' )
   local IN=( 'http://mirror.rise.ph' )
   local IT=( 'http://openbsd.mirror.garr.it' )
   local LT=( 'http://mirror.litnet.lt' )
   local NL=( 'http://ftp.bit.nl' )
   local JP=( 'http://www.ftp.ne.jp' )
   local PH=( 'http://mirror.rise.ph' )
   local PL=( 'http://ftp.icm.edu.pl' )
   local RO=( 'http://mirrors.pidginhost.com' )
   local RU=( 'http://mirror.yandex.ru' )
   local SE=( 'http://ftp.eu.openbsd.org' )
   local SI=( 'http://www.obsd.si' )
   local SG=( 'http://mirror.rise.ph' )
   local TH=( 'http://mirror.rise.ph' )
   local TW=( 'http://ftp.yzu.edu.tw' )
   local US=( 'http://mirrors.mit.edu' 'http://ftp3.usa.openbsd.org' 'http://mirrors.sonic.net' )
   local VN=( 'http://www.ftp.ne.jp' )

   local dir='pub/OpenBSD/LibreSSL'
   
   local success=1
   if [ -n "$SERVER_COUNTRY" ]; then
      local a="${SERVER_COUNTRY}[@]"
      for url in "${!a}"; do
         echo "$SERVER_COUNTRY server detected"
         LIBRESSL_MIRRORURL="${url}/${dir}"
         LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
         echo "Checking LibreSSL mirror ${LIBRESSL_LINK} ..."
         curl -4sI --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
         success=$?
         if [ "$success" != 0 ]; then 
            echo 'The mirror is not available.'
         else
            echo 'The mirror is available.'
            break; 
         fi
      done
   fi

   if [ "$success" -ne 0 ]; then
      LIBRESSL_MIRRORURL="http://ftp.openbsd.org/pub/OpenBSD/LibreSSL"
      LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
      curl -4sI --connect-timeout 5 --max-time 5 "$LIBRESSL_LINK" | grep 'HTTP\/' | grep '200'
      if [ "$?" != 0 ]; then
         LIBRESSL_MIRRORURL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/libressl"
         LIBRESSL_LINK="${LIBRESSL_MIRRORURL}/libressl-${LIBRESSL_VERSION}.tar.gz"
         echo "Use backup LibrSSL mirror - $LIBRESSL_MIRRORURL"
      else
         echo "Use main LibrSSL mirror - $LIBRESSL_MIRRORURL"
      fi
   fi
}

###############################################
# when adding additional modules, make sure to update inc/nginx_upgrade.inc
# checknginxmodules function needs updating as well

download_file() {
   local fileurl="$1"
   local filename="${1##*/}"
   local filenamesave=''
   local dlapp=${3:-def}

   filename=${2:-$filename}
   filenamesave=${2:-$filenamesave}

   cecho "Download $fileurl ..." $boldyellow
   if [ -s "$filename" ]; then
      cecho "$filename found. Download skipped." $boldgreen
   else
      if [ "$dlapp" = 'def' ]; then
         download_cmd "$fileurl" "$filenamesave"
      else
         wget $WGETOPT --progress=bar --tries=3 $fileurl -O $filenamesave
      fi
      ERROR=$?
      if [ "$ERROR" != 0 ]; then
         cecho "Error: download failed!" $boldred
         echo
         rm -f $filenamesave
#         if [[ "$CENTOSVER" > 6 ]]; then
#            ping -c4 $(echo "$fileurl" | cut -d'/' -f3)
#            dig $(echo "$fileurl" | cut -d'/' -f3)
#         fi
         exit $ERROR
      else 
         cecho "Download done. File saved as $filename" $boldyellow
      fi
   fi
}

untar_file() {
   local filename=$1
   local dir=${2:-$DIR_TMP}

   [[ ! -d "$dir" ]] && mkdir -p $dir
   
   local ext=${filename##*.}
   local param=
   case "$ext" in
      'gz')  param='-xzf';;
      'tgz') param='-xzf';;
      'xz')  param='-xJf';;
      'bz2') param='-xjf';;
   esac
   [[ -z "$param" ]] && { echo "Error: unknown extention for $filename file"; exit 64; }

   tar $param $filename -C $dir
   ERROR=$?
   if [ "$ERROR" != 0 ]; then
      cecho "Error: $filename extraction failed." $boldred
      exit $ERROR
   else 
      cecho "$filename is valid file." $boldyellow
      echo
   fi
}


filetarball() {
   local fileurl="$1"
   local filename="${1##*/}"
   filename=${2:-$filename}
   local dlapp=${3:-def}
   
   cd $DIR_TMP
   download_file "$fileurl" "$filename" "$dlapp"
   untar_file "$filename"
}


###############################################
phptarball() {
   if [[ "$PHP_INSTALL" == [yY] ]]; then
      cd $DIR_TMP
      phpxz_detect
      local phparchfile="php-${PHP_VERSION}.tar.${PHPEXTSION}"

      if [ -s "$phparchfile" ]; then
         cecho "$phparchfile found. Download skipped." $boldgreen
      else
         phpgeolocation
         download_file "${PHP_MIRRORURL}/get/${phparchfile}/from/this/mirror" "$phparchfile"
      fi
      
      untar_file "$phparchfile"
   fi
}

###############################################
nginxpcretarball() {
   filetarball "$PCRELINK"
}

###############################################
nginxzlibtarball() {
   filetarball "$NGX_ZLIBLINK"
}

###############################################
nginxwebdavtarball() {
   filetarball "$NGX_WEBDAVLINK" "$NGX_WEBDAVLINKFILE"
}

###############################################
nginxpgspeedtarball() {
   if [[ "$NGINX_PAGESPEED" == [yY] ]]; then
      filetarball "$NGX_PAGESPEEDLINK" "$NGX_PAGESPEEDLINKFILE"

      cecho "Download ${NGX_PAGESPEEDPSOLINKLFILE} PSOL Library..." $boldyellow
      if [[ -s "$psol_file" ]]; then
         cecho "$psol_file found. Download skipped." $boldgreen
      else
         # 1.12+ new method https://github.com/pagespeed/ngx_pagespeed/issues/1337
         PS_BRANCHVER=$(echo $NGXPGSPEED_VER | cut -c1-4| sed -e 's|\.|00|')
         NGXPGSPEED_DIR=$(tar -tzf "$DIR_TMP/$NGX_PAGESPEEDLINKFILE" | head -1 | cut -f1 -d"/")
         if [ "$PS_BRANCHVER" -ge 10012 ]; then
            cd $DIR_TMP/$NGXPGSPEED_DIR
            local url=$(scripts/format_binary_url.sh PSOL_BINARY_URL)
            cd $DIR_TMP
            download_file "$url" "$NGX_PAGESPEEDPSOLINKLFILE" "wget"
         else
            download_file "$NGX_PAGESPEEDPSOLINKL" "$NGX_PAGESPEEDPSOLINKLFILE" "wget"
         fi
      fi
      untar_file "$NGX_PAGESPEEDPSOLINKLFILE" "$NGXPGSPEED_DIR"
   fi
}

###############################################
ngxmoduletarball() {
   echo 'Download ngx-rtmp Nginx module:'
   filetarball "$NGX_RTMPLINK" "$NGX_RTMPLINKFILE"
   
   echo 'Download ngx-fancyindex Nginx module:'
   filetarball "$NGX_FANCYINDEXLINK" "$NGX_FANCYINDEXLINKFILE"

   echo 'Download ngx_cache_purge Nginx module:'
   filetarball "$NGX_CACHEPURGE" "$NGX_CACHEPURGEFILE"

   echo 'Download nginx-accesskey Nginx module:'
   local majorver=$(echo "${ngver:-$NGINX_VERSION}" | cut -d . -f1)
   local minorver=$(echo "${ngver:-$NGINX_VERSION}" | cut -d . -f2)
   if [[ $majorver == 1 && $minorver -lt 12 ]]; then
      NGX_ACCESSKEYVER=$NGINX_ACCESSKEYVER
   else
      NGX_ACCESSKEYVER=$NGINX_ACCESSKEYVER2
   fi
   NGX_ACCESSKEYLINKFILE="nginx-accesskey-${NGX_ACCESSKEYVER}.tar.gz"
   NGX_ACCESSKEYLINK="https://github.com/Martchus/nginx-accesskey/archive/v${NGX_ACCESSKEYVER}.tar.gz"
   filetarball "$NGX_ACCESSKEYLINK" "$NGX_ACCESSKEYLINKFILE"

   echo 'Download nginx-http-concat Nginx module:'
   filetarball "$NGX_CONCATLINK" "$NGX_CONCATLINKFILE"

   echo 'Download ngx_headers_more Nginx module:'
   filetarball "$NGX_HEADERSMORELINK" "$NGX_HEADERSMORELINKFILE"

   echo 'Download nginx-sticky-module-ng Nginx module:'
   filetarball "$NGX_STICKYLINK" "$NGX_STICKYLINKFILE"

   echo 'Download nginx_http_upstream_check_module Nginx module:'
   filetarball "$NGX_UPSTREAMCHECKLINK" "$NGX_UPSTREAMCHECKLINKFILE"

   echo 'Download ngx_http_redis Nginx module:'
   filetarball "$NGX_HTTPREDISLINK" "$NGX_HTTPREDISLINKFILE"

   nginxpcretarball
   nginxwebdavtarball
   nginxpgspeedtarball
}


###############################################
openrestytarball() {
   if [[ "$NGINX_OPENRESTY" == [yY] ]]; then
      filetarball "$NGX_MEMCLINK" "$NGX_MEMCLINKFILE"
      filetarball "$NGX_SRCACHELINK" "$NGX_SRCACHELINKFILE"
      filetarball "$NGX_DEVELKITLINK" "$NGX_DEVELKITLINKFILE"
      filetarball "$NGX_SETMISCLINK" "$NGX_SETMISCLINKFILE"
      filetarball "$NGX_ECHOLINK" "$NGX_ECHOLINKFILE"
      filetarball "$NGX_REDISLINK" "$NGX_REDISLINKFILE"
   fi
   if [[ "$ORESTY_LUANGINX" == [yY] ]]; then
      filetarball "$NGX_LUANGINXLINK" "$NGX_LUANGINXLINKFILE"
      filetarball "$NGX_LUAGITLINK" "$NGX_LUAGITLINKFILE"
      filetarball "$NGX_LUAMEMCACHEDLINK" "$NGX_LUAMEMCACHEDLINKFILE"
      filetarball "$NGX_LUAMYSQLLINK" "$NGX_LUAMYSQLLINKFILE"
      filetarball "$NGX_LUAREDISLINK" "$NGX_LUAREDISLINKFILE"
      filetarball "$NGX_LUADNSLINK" "$NGX_LUADNSLINKFILE"
      filetarball "$NGX_LUAUPLOADLINK" "$NGX_LUAUPLOADLINKFILE"
      filetarball "$NGX_LUAWEBSOCKETLINK" "$NGX_LUAWEBSOCKETLINKFILE"
      filetarball "$NGX_LUALOCKLINK" "$NGX_LUALOCKLINKFILE"
      filetarball "$NGX_LUASTRINGLINK" "$NGX_LUASTRINGLINKFILE"
      filetarball "$NGX_LUAREDISPARSERLINK" "$NGX_LUAREDISPARSERLINKFILE"
      filetarball "$NGX_LUAUPSTREAMCHECKLINK" "$NGX_LUAUPSTREAMCHECKLINKFILE"
      filetarball "$NGX_LUALRUCACHELINK" "$NGX_LUALRUCACHELINKFILE"
      filetarball "$NGX_LUARESTYCORELINK" "$NGX_LUARESTYCORELINKFILE"
      filetarball "$NGX_LUAUPSTREAMLINK" "$NGX_LUAUPSTREAMLINKFILE"
      filetarball "$NGX_LUALOGGERSOCKETLINK" "$NGX_LUALOGGERSOCKETLINKFILE"
      filetarball "$NGX_LUACOOKIELINK" "$NGX_LUACOOKIELINKFILE"
      filetarball "$NGX_LUAUPSTREAMCACHELINK" "$NGX_LUAUPSTREAMCACHELINKFILE"
      filetarball "$NGX_LUACJSONLINK" "$NGX_LUACJSONLINKFILE"
   fi   
}

###############################################
nginxtarball() {
   if [[ "$NGINX_INSTALL" == [yY] ]]; then
      echo
      if [[ "$UNATTENDED" == 'n' ]]; then
         read -ep "Would you like to compile nginx with IPv6 support? [y/n] " asknginxipv
      else
         asknginxipv="$NGINX_IPV"
      fi

      filetarball "$NGX_LINK"

      if [[ "$PARALLEL_MODE" == [yY] ]] && [[ "$(grep -c "processor" /proc/cpuinfo)" -gt '1' ]]; then
         local pids=''
         ngxmoduletarball & pids="$pids $!"
         openrestytarball & pids="$pids $!"
         waitpm "$pids"
      else
         ngxmoduletarball
         openrestytarball
      fi
   fi
}

###############################################
libeventtarball() {
   filetarball "$LIBEVENTLINK" "$LIBEVENTLINKFILE"
}

###############################################
mailparsephpexttarball() {
   # if upgrade then PHPMUVER is not null so take it, otherwise take PHPMVER
   local phpv=$(echo "${phpver:-$PHP_VERSION}" | cut -d . -f1)
   if [ "$phpv" > 6 ]; then
      # mailparse 3.0.0 only works with PHP 7.0 branch so use 2.1.6 if not PHP 7.0 branch
      MAILPARSEPHP_VER="$MAILPARSEPHP_COMPATVER"
      PHPEXT_MAILPARSELINKFILE="mailparse-${MAILPARSEPHP_COMPATVER}.tgz"
      PHPEXT_MAILPARSELINK="https://pecl.php.net/get/${PHPEXT_MAILPARSELINKFILE}"
      PHPEXT_MAILPARSELINKLOCAL="${LOCALCENTMINMOD_MIRROR}/centminmodparts/mailparse/${PHPEXT_MAILPARSELINKFILE}"
   fi
   if [ "$1" = 'local' ]; then
      filetarball "$PHPEXT_MAILPARSELINKLOCAL"
   else
      filetarball "$PHPEXT_MAILPARSELINK"
   fi
}

###############################################
mailparsephpextlocaltarball() {
   cd $DIR_TMP
   rm -rf mailparse*   
   mailparsephpexttarball 'local'
}

###############################################
imagickphpextlocaltarball() {
   cd $DIR_TMP
   rm -rf imagick*
   filetarball "$PHPEXT_IMAGICKLINKLOCAL"
}

###############################################
imagickphpexttarball() {
   filetarball "$PHPEXT_IMAGICKLINK"
}

###############################################
memcacheextlocaltarball() {
   cd $DIR_TMP
   rm -rf memcache*
   filetarball "$PHPEXT_MEMCACHELINKLOCAL"
}

###############################################
memcacheexttarball() {
   filetarball "$PHPEXT_MEMCACHELINK"
}

###############################################
memcachedphplocaltarball() {
   cd $DIR_TMP
   rm -rf memcached*
   filetarball "$PHPEXT_MEMCACHEDLINKLOCAL"
}

###############################################
memcachedphptarball() {
   filetarball "$PHPEXT_MEMCACHEDLINK"
}

###############################################
redisphplocaltarball() {
   cd $DIR_TMP
   rm -rf redis*
   filetarball "$PHPEXT_REDISLINKLOCAL" "$PHPEXT_REDISLINKFILE"
}

###############################################
redisphptarball() {
   filetarball "$PHPEXT_REDISLINK" "$PHPEXT_REDISLINKFILE"
}

###############################################
mongodbphplocaltarball() {
   cd $DIR_TMP
   rm -rf mongo*
   filetarball "$PHPEXT_MONGODBLINKLOCAL"
}

###############################################
mongodbphptarball() {
   filetarball "$PHPEXT_MONGODBLINK"
}

###############################################
libmemcachedlocaltarball() {
   if [[ "$LIBMEMCACHED_YUM" == [nN] ]]; then
      filetarball "$PHPEXT_LIBMEMCACHEDLINKLOCAL" "$PHPEXT_LIBMEMCACHEDLINKFILE"
   fi
}

###############################################
libmemcachedtarball() {
   if [[ "$LIBMEMCACHED_YUM" == [nN] ]]; then
      filetarball "$PHPEXT_LIBMEMCACHEDLINK" "$PHPEXT_LIBMEMCACHEDLINKFILE"
   fi
}

###############################################
twemperftarball() {
   filetarball "$TWEMPERF_LINK" "$TWEMPERF_LINKFILE"
}

###############################################
memcachetarball() {
   local MEMCACHED_ALT="$1"
   echo
   
   askmemcinstall='y'
   if [[ "$UNATTENDED" == 'n' ]]; then
      read -ep "Install Memcached Server? (default uses 16MB RAM each) [y/n] " askmemcinstall
   fi
   if [[ "$askmemcinstall" == [yY] ]]; then
      MEMCINSTALL='y'
      echo
      nomemcachedservers=1
      if [[ "$UNATTENDED" == 'n' ]]; then
         read -ep "Install how many Memcached servers ? 1 or 2 ?: " nomemcachedservers
         echo "will install $nomemcachedservers Memcached servers"
         echo ""
      fi

      if [[ "$MEMCACHED_ALT" != 'local' ]]; then
         filetarball "$MEMCACHEDSERVER_LINK"
      else
         filetarball "$MEMCACHEDSERVER_LINKLOCAL"
      fi

      libeventtarball
      memcacheexttarball
      memcachedphptarball
      libmemcachedtarball
      # twemperftarball
   fi
}

###############################################
mysqltools() {
   cd /root
   download_file "$MYSQLREPORT_LINK" "$MYSQLREPORT_LINKFILE" "wget"
   download_file "$MYSQLTUNER_LINK" "$MYSQLTUNER_LINKFILE" "wget"
   chmod +x mysqlreport "${MYSQLTUNER_LINKFILE}"
   cd $DIR_TMP
}

###############################################
mariadbrpms() {
   if [[ "$MDB_INSTALL" == [yY] ]]; then
      # The MariaDB mirror uses x86 and amd64 prefixes for rpm folders instead of i386/x84_64, so compensate for that...
      if [[ "$ARCH" == 'x86_64' ]]; then
         MDB_ARCH='amd64'
      else
         MDB_ARCH='x86'
         ARCH='i386'
      fi

      cd $DIR_TMP
      local url="http://ftp.osuosl.org/pub/mariadb/mariadb-${MDB_VERONLY}/centos5-${MDB_ARCH}/rpms"
      
      download_file "${url}/MariaDB-client-${MDB_VERSION}.el5.${ARCH}.rpm"
      download_file "${url}/MariaDB-devel-${MDB_VERSION}.el5.${ARCH}.rpm"
      download_file "${url}/MariaDB-server-${MDB_VERSION}.el5.${ARCH}.rpm"
      download_file "${url}/MariaDB-shared-${MDB_VERSION}.el5.${ARCH}.rpm"
      download_file "${url}/MariaDB-test-${MDB_VERSION}.el5.${ARCH}.rpm"
   fi
}

###############################################
mariadbyumrepo() {
   if [[ "$MDB_YUMREPOINSTALL" == [yY] ]]; then
      cd $DIR_TMP
      download_file "http://master.ourdelta.org/yum/CentOS-MariaDB52/ourdelta-release-5-1.noarch.rpm"
   fi
}

###############################################
gperftools() {
   if [[ "$GPERFTOOLS_SOURCEINSTALL" == [yY] ]]; then
      filetarball "$LIBUNWIND_LINK"
      filetarball "$GPERFTOOL_LINK"
   fi
}

###############################################
openssldownload() {
   cecho "Download $OPENSSL_LINKFILE ..." $boldyellow
   cd $DIR_TMP
   if [ -s "${OPENSSL_LINKFILE}" ]; then
      cecho "openssl $OPENSSL_LINKFILE found, skipping download..." $boldgreen
   else
      curl -4sI --connect-timeout 5 --max-time 5 "$OPENSSL_LINK" | grep 'HTTP\/' | grep '200'
      if [ "$?" != 0 ]; then
         OPENSSL_LINK="${LOCALCENTMINMOD_MIRROR}/centminmodparts/openssl/openssl-${OPENSSL_VERSION}.tar.gz"
      fi
      download_file "$OPENSSL_LINK"      
   fi
   untar_file "$OPENSSL_LINKFILE"
}

###############################################
libressldownload() {
   if [ -s "$DIR_TMP/$LIBRESSL_LINKFILE" ]; then
      cecho "$LIBRESSL_LINKFILE found. Download skipped." $boldgreen
      cd $DIR_TMP
      untar_file "$LIBRESSL_LINKFILE"
   else
      libresslgeolocation
      filetarball "$LIBRESSL_LINK" "$LIBRESSL_LINKFILE"
   fi
}

###############################################
pcretarball() {
   if [[ "$PCRE_SOURCEINSTALL" == [yY] ]]; then
      filetarball "$PCRE_SOURCELINK"
   fi
}

###############################################
siegetarball() {
#SIEGE_VERSION
   echo
   asksiegeinstall='y'
   if [[ "$UNATTENDED" == 'n' ]]; then
      read -ep "Install Siege Benchmark script? [y/n] " asksiegeinstall
   fi
   if [[ "$asksiegeinstall" == [yY] ]]; then
      SIEGEINSTALL='y'
      # filetarball "$SIEGE_LINK"
      filetarball "$SIEGE_LINKLOCAL"
      # filetarball "$SPROXY_LINK"
      filetarball "$SPROXY_LINKLOCAL"
   fi
}

###############################################
pythontarball() {
   echo
   askpythonupdate='n'
   if [[ "$UNATTENDED" == 'n' ]]; then
      read -ep "Install Python Update ? [y/n] " askpythonupdate
   fi
   if [[ "$askpythonupdate" == [yY] ]]; then
      PYTHONUPDATE='y'
      filetarball "$PYTHON_LINK"
      download_file "$PYTHON_SETUPTOOLSLINK"
#test egg file validity check
   fi
}

###############################################
xcachetarball() {
   echo
   askxcacheinstall='n'
   if [[ "$UNATTENDED" == 'n' ]]; then
      read -ep "Install XCache? (By default uses 32MB RAM) If XCache installed DO NOT install APC [y/n] " askxcacheinstall
   fi
   # menu #8 install xcache
   if [[ "$MANXCACHEINSTALL" == 'y' ]]; then
      askxcacheinstall='y'
   fi
   if [[ "$askxcacheinstall" == [yY] ]];then
      XCACHESRCINSTALL='y'
      filetarball "$XCACHE_LINK"
   fi
}

###############################################
apclocaltarball() {
   echo
   askapcinstall='y'
   if [[ "$UNATTENDED" == 'n' ]]; then
      read -ep "Install APC? (By default uses 32MB RAM) [y/n] " askapcinstall
   fi
   if [[ "$askapcinstall" == [yY] ]]; then
      APCINSTALL='y'
      cd $DIR_TMP
      rm -rf APC*
      filetarball "$APCCACHE_LINKLOCAL"
   fi
}

###############################################
apctarball() {
   echo
   askapcinstall='y'
   if [[ "$UNATTENDED" == 'n' ]]; then
      read -ep "Install APC? (By default uses 32MB RAM) [y/n] " askapcinstall
   fi
   if [[ "$askapcinstall" == [yY] ]]; then
      APCINSTALL='y'
      filetarball "$APCCACHE_LINK"
   fi
}

###############################################
zopcachelocaltarball() {
   echo
   askzopcacheinstall='y'
   if [[ "$UNATTENDED" == 'n' ]]; then
      read -ep "Install Zend OPCache ? (By default uses 32MB RAM) [y/n] " askzopcacheinstall
   fi
   if [[ "$askzopcacheinstall" == [yY] ]]; then
      ZOPCACHEINSTALL='y'
      cd $DIR_TMP
      rm -rf zendopcache*
      filetarball "$ZENDOPCACHE_LINKLOCAL"      
   fi
}

###############################################
zopcachetarball() {
   echo
   askzopcacheinstall='y'
   if [[ "$UNATTENDED" == 'n' ]]; then
      read -ep "Install Zend OPCache ? (By default uses 32MB RAM) [y/n] " askzopcacheinstall
   fi
   if [[ "$askzopcacheinstall" == [yY] ]]; then
      ZOPCACHEINSTALL='y'
      filetarball "$ZENDOPCACHE_LINK"      
   fi
}

###############################################
nsdtarball() {
   if [[ "$NSD_INSTALL" == [yY] ]]; then
      filetarball "$NSD_LINK"
   fi
}

###############################################
csftarball() {
   askcsfinstall='y'
   if [[ "$UNATTENDED" == 'n' ]]; then
      read -ep "Install CSF firewall script ? [y/n] " askcsfinstall
   fi
   if [[ "$askcsfinstall" == [yY] ]]; then
      CSFINSTALLOK='y'
      # change to always look for remote csf firewall tarball and use it first 
      # and only fall back on local included csf tarball if you can not reach 
      # the official csf site tarball download
      CSF_SITECHECK=$(curl -4sI ${CSF_LINK} | grep 'HTTP\/' | awk '/200/ {print $2}')
      if [[ "$CSF_SITECHECK" == 200 ]]; then
         cd $DIR_TMP
         mv -f "${CSF_LINKFILE}" "${CSF_LINKFILE}-local"
      fi
      filetarball "$CSF_LINK"
   fi
}

###############################################
ccachetarball() {
   cd $DIR_TMP

   echo
   cecho "Download $CCCACHELINK ..." $boldyellow
   if [ -s "$CCACHEFILE" ]; then
      cecho "ccache $CCACHE_VER archive found, skipping download..." $boldgreen 
   else
      curl -4sI --connect-timeout 5 --max-time 5 "$CCCACHELINK" | grep 'HTTP\/' | grep '200'
      CCACHE_CURLCHECK=$?
      if [ "$CCACHE_CURLCHECK" != 0 ]; then
         CCCACHELINK="$CCCACHELINKLOCAL"
      fi
      download_cmd "$CCCACHELINK"
      ERROR=$?
      if [ "$ERROR" != 0 ]; then
         CCACHE_FAILEDDOWNLOAD='y'
         rm -rf "${DIR_TMP}/ccache-${CCACHE_VER}*"
         CCCACHELINK="https://centmin.sh/centminmodparts/ccache/$CCACHEFILE"
         cecho "Error: $CCACHEFILE download failed." $boldgreen
         echo        
         cecho "Re-try download via alternate ccache mirror $CCCACHELINK" $boldgreen 
         download_cmd "$CCCACHELINK"
         ERROR=$?
         if [ "$ERROR" != 0 ]; then
            rm -rf "${DIR_TMP}/ccache-${CCACHE_VER}*"
            CCCACHELINK="https://centmin.com/centminmodparts/ccache/$CCACHEFILE"
            cecho "Error: $CCACHEFILE download failed." $boldgreen
            echo
            cecho "Re-try again download via alternate ccache mirror $CCCACHELINK" $boldgreen 
            download_cmd $CCCACHELINK
            ERROR=$?
            if [ "$ERROR" != 0 ]; then
               cecho "Error: $CCACHEFILE download failed." $boldgreen
               rm -f $CCACHEFILE
               exit $ERROR
            fi
         else
            cecho "Download $CCACHEFILE done." $boldyellow
         fi
      else
         cecho "Download $CCACHEFILE done." $boldyellow
      fi
   fi

   untar_file "$CCACHEFILE"
}


webptarball() {
   if [[ "$CUSTOM_WEBP" == [yY] ]]; then
      download_file "$LIBWEBP_LINK" "$LIBWEBP_FILE"
      untar_file "$LIBWEBP_FILE" "$LIBWEBP_DIR"
      filetarball "$LIBGD_LINK" "$LIBGD_FILE"
   fi
}


###############################################
alldownloads() {
   echo "*****************************************************"
   cecho "Downloading all required software for install routines." $boldgreen
   echo "*****************************************************"

   # use local download source files first less likely to encounter 
   # broken 3rd party urls and speed up initial install
   if [ -e "${CM_INSTALLDIR}/downloads/svr-setup.zip" ]; then
      /usr/bin/unzip -qo "${CM_INSTALLDIR}/downloads/svr-setup.zip" -d "$DIR_TMP"/
   fi
   if [ -e "${CM_INSTALLDIR}/downloads/rpms.zip" ]; then
      /usr/bin/unzip -qo "${CM_INSTALLDIR}/downloads/rpms.zip" -d "$DIR_TMP"/
   fi
   if [ -e "${CM_INSTALLDIR}/downloads/curlrpms.zip" ]; then
      /usr/bin/unzip -qo "${CM_INSTALLDIR}/downloads/curlrpms.zip" -d "$DIR_TMP"/
   fi
   if [ -e "${CM_INSTALLDIR}/downloads/ffmpeg-depends.zip" ]; then
      /usr/bin/unzip -qo "${CM_INSTALLDIR}/downloads/ffmpeg-depends.zip" -d "$DIR_TMP"/
   fi

   yuminstall
   questions

   # yum double check
   if [[ "$FAILEPEL" != 'y' || "$FAILRPMFORGE" != 'y' || "$FAILCENTALT" != 'y' ]]; then
      # echo "\${CUR_DIR} & \${CM_INSTALLDIR}"
      # echo ${CUR_DIR}
      # echo ${CM_INSTALLDIR}    

      ccachetarball
      ccacheinstall
      axelsetup
      nginxtarball

      if [[ "$PARALLEL_MODE" == [yY] ]] && [[ "$(grep -c "processor" /proc/cpuinfo)" -gt 1 ]]; then
         local pids=''
         csftarball & pids="$pids $!"
         pythontarball & pids="$pids $!"
         nginxzlibtarball & pids="$pids $!"
         waitpm "$pids"
      else
         csftarball
         pythontarball
         nginxzlibtarball
      fi

      if [[ "$PARALLEL_MODE" == [yY] ]] && [[ "$(grep -c "processor" /proc/cpuinfo)" -gt 1 ]]; then
         local pids=''
         phptarball & pids="$pids $!"
         xcachetarball & pids="$pids $!"
         apctarball & pids="$pids $!"
         zopcachetarball & pids="$pids $!"
         waitpm "$pids"
      else
         phptarball
         xcachetarball
         apctarball
         zopcachetarball
      fi

      if [[ "$PARALLEL_MODE" == [yY] ]] && [[ "$(grep -c "processor" /proc/cpuinfo)" -gt 1 ]]; then
         local pids=''
         memcachetarball & pids="$pids $!"
         imagickphpexttarball & pids="$pids $!"
         redisphptarball & pids="$pids $!"
         mongodbphptarball & pids="$pids $!"
         waitpm "$pids"
      else
         memcachetarball
         imagickphpexttarball
         redisphptarball
         mongodbphptarball
      fi

      if [[ "$PARALLEL_MODE" == [yY] ]] && [[ "$(grep -c "processor" /proc/cpuinfo)" -gt 1 ]]; then
         local pids=''
         gperftools & pids="$pids $!"
         if [[ "$LIBRESSL_SWITCH" == 'n' ]]; then
            openssldownload & pids="$pids $!"
         fi
         libressldownload & pids="$pids $!"
         pcretarball & pids="$pids $!"
         waitpm "$pids"
      else
         gperftools
         if [[ "$LIBRESSL_SWITCH" == 'n' ]]; then
            openssldownload
         fi
         libressldownload
         pcretarball
      fi

      if [[ "$PARALLEL_MODE" == [yY] ]] && [[ "$(grep -c "processor" /proc/cpuinfo)" -gt 1 ]]; then
         local pids=''
         siegetarball & pids="$pids $!"
         mysqltools & pids="$pids $!"
         nsdtarball & pids="$pids $!"
         mariadbrpms & pids="$pids $!"
         waitpm "$pids"
      else
         siegetarball
         mysqltools
         nsdtarball
         mariadbrpms
      fi

      webptarball

     # mariadbyumrepo
      # validcheck_downloads

      echo "*****************************************************"
      cecho "Downloads complete." $boldgreen
      echo "*****************************************************"

      if [[ "$UNATTENDED" == 'n' ]]; then
         echo
         read -ep "You you want to continue installation ?  [y/n] " downloadcontinstall
         [[ "$downloadcontinstall" == [nN] ]] && exit 100
      fi
   else
      echo
      echo "Error installing or downloading YUM repositories..."
      exit 64
   fi # yum double check
}