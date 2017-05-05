IMVER=${IMAGEMAGICK_VER:-6}

checkimagicksys() {

    if [ -f /usr/bin/re2c ]; then
        if [[ "$(/usr/bin/re2c -v | awk '{print $2}')" != '0.14.3' ]]; then
            rerpm
        fi
    fi

    CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)
    
    if [ "$CENTOSVER" == 'release' ]; then
        CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
        if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
            CENTOS_SEVEN='7'
        fi
    fi
    
    if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
        CENTOS_SIX='6'
    fi


    if [[ "$CENTOS_SEVEN" = '7' && "$DNF_ENABLE" = [yY] ]]; then
        # yum -y -q install epel-release
        if [[ ! -f /usr/bin/dnf ]]; then
            yum -y -q install dnf
        fi
        if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
            DISABLEREPO_DNF=' --disablerepo=rpmforge'
            YUMDNFBIN="dnf${DISABLEREPO_DNF}"
        else
            DISABLEREPO_DNF=""
            YUMDNFBIN='dnf'
        fi
    else
        YUMDNFBIN='yum'
    fi
}

imagemagickinstall() {
   if [[ "$REMIREPO_DISABLE" = [yY] ]]; then
      if [[ $(rpm -q ImageMagick >/dev/null 2>&1; echo $?) = 0 ]]; then
         cecho "Updating ImageMagick 6..." $boldyellow
         time $YUMDNFBIN -y update ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel --disableplugin=priorities${DISABLEREPO_DNF}
      else
         cecho "Installing ImageMagick 6..." $boldyellow
         time $YUMDNFBIN -y install ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel${DISABLEREPO_DNF}
      fi
      sar_call
   else # REMIREPO_DISABLE=n
      if [[ ! -f /etc/yum.repos.d/remi.repo ]]; then
         # for very old centmin mod installs which never had remi yum repo setup
         if [[ "$CENTOS_SIX" = '6' ]]; then
            remisixyum
         elif [[ "$CENTOS_SEVEN" = '7' ]]; then
            remisevenyum
         fi
      fi

      if [[ $(rpm -q ImageMagick${IMVER} >/dev/null 2>&1; echo $?) = 0 ]]; then
         cecho "Updating ImageMagick ${IMVER} from Remi YUM repo..." $boldyellow
         time $YUMDNFBIN -y update ImageMagick${IMVER} ImageMagick${IMVER}-devel ImageMagick${IMVER}-c++ ImageMagick${IMVER}-c++-devel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities
         sar_call
      else
         if [[ $(rpm -q ImageMagick >/dev/null 2>&1; echo $?) = 0 ]]; then
            cecho "Removing old version of ImageMaick..." $boldyellow
            yum -y remove ImageMagick${DISABLEREPO_DNF}
            sar_call
         fi

         if [[ "$IMVER" == 7 && $(rpm -q ImageMagick6 >/dev/null 2>&1; echo $?) = 0 ]]; then
            cecho "Removing ImageMaick 6..." $boldyellow
            yum -y remove ImageMagick6${DISABLEREPO_DNF}
            sar_call
         fi

         if [[ "$IMVER" == 6 && $(rpm -q ImageMagick7 >/dev/null 2>&1; echo $?) = 0 ]]; then
            cecho "Removing ImageMaick 7..." $boldyellow
            yum -y remove ImageMagick6${DISABLEREPO_DNF}
            sar_call
         fi

         cecho "Installing ImageMagick ${IMVER} from Remi YUM repo..." $boldyellow
         local skipbroken = ''
         [[ "$CENTOS_SIX" = 6 ]] && skipbroken='--skip-broken'
         time $YUMDNFBIN -y install ImageMagick${IMVER} ImageMagick${IMVER}-devel ImageMagick${IMVER}-c++ ImageMagick${IMVER}-c++-devel --enablerepo=remi${DISABLEREPO_DNF} --disableplugin=priorities $skipbroken
         sar_call
      fi
   fi
}

imagickinstall() {
if [[ "$PHP_INSTALL" = [yY] ]]; then
if [[ "$IMAGEMAGICK_VER" != [nN] ]]; then
   echo "*************************************************"
   cecho "* Installing ImageMagick" $boldgreen
   echo "*************************************************"
   echo

   checkimagicksys
   imagemagickinstall
   imagick_fixes

fi # IMAGEMAGICK_VER != n

if [[ "$PHPIMAGICK" = [yY] ]]; then
   echo "*************************************************"
   cecho "* Installing imagick PHP Extension" $boldgreen
   echo "*************************************************"
   echo

   pwd
   echo "cd $DIR_TMP"
   cd $DIR_TMP

   if [[ -z "$phpver" && "$PHPMVER" > 7 || "$PHPMUVER" > 7 ]] && [[ "$(echo $IMAGICKPHP_VER | cut -d . -f1,2)" < '3.3' ]]; then
      IMAGICKGITLINK='https://github.com/mkoppanen/imagick'
      # fallback mirror if official github is down, use gitlab mirror
      curl -4Is --connect-timeout 5 --max-time 5 $IMAGICKGITLINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
      IMAGICKGITCURLCHECK=$?
      if [[ "$IMAGICKGITCURLCHECK" != '0' ]]; then
         IMAGICKGITLINK='https://gitlab.com/centminmod-github-mirror/imagick.git'
      fi

      if [[ -d "imagick-php7" && -d "imagick-php7/.git" ]]; then       
         cd imagick-php7
         git stash
         git pull
         git log -3
      else
         rm -rf imagick-php7
         git clone -b phpseven "$IMAGICKGITLINK" imagick-php7
      fi
      sar_call
      echo
      echo "compiling imagick PHP extension for PHP 7.x ..."
      cd imagick-php7
   else    
      if [[ ! -d "${DIR_TMP}/imagick-${IMAGICKPHP_VER}" ]]; then
         echo "Downloading imagick extension"
         imagickphpexttarball
      fi
      cd imagick-${IMAGICKPHP_VER}
   fi # php 7 or not

   [[ "$INITIALINSTALL" != [yY] ]] && make clean
   /usr/local/bin/phpize
   ./configure --with-php-config=/usr/local/bin/php-config
   sar_call
   make${MAKETHREADS}
   sar_call
   make install
   sar_call

   #######################################################
   # check if imagick.so exists in php.ini
   IMAGICKSOCHECK=$(grep 'extension=imagick.so' /usr/local/lib/php.ini >/dev/null 2>&1; echo $?)
   IMAGICKSOCHECKB=$(grep 'extension=imagick.so' "${CONFIGSCANDIR}/imagick.ini" >/dev/null 2>&1; echo $?)
   if [[ "$IMAGICKSOCHECK" != 0 || "$IMAGICKSOCHECKB" != 0 ]]; then
      echo -e "\nCopying imagick.ini > ${CONFIGSCANDIR}/imagick.ini\n"
      echo "extension=imagick.so" > "${CONFIGSCANDIR}/imagick.ini"
      echo ";imagick.skip_version_check=1" >> "${CONFIGSCANDIR}/imagick.ini"
   fi

   cmservice php-fpm restart

   echo "*************************************************"
   cecho "* imagick PHP installed" $boldgreen
   echo "*************************************************"

fi # PHPIMAGICK = y
fi # PHP_INSTALL = y
}