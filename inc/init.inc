#####################################################
DT=$(date +"%d%m%y-%H%M%S")
HN=$(uname -n)
CURL_TIMEOUTS=' --max-time 5 --connect-timeout 5'
WGETOPT='-cnv --no-dns-cache -4 --no-check-certificate'
WGETRETRY=''
CURL_TIMEOUTS=' --max-time 5 --connect-timeout 5'
CMSDEBUG='n'

DIR_TMP='/svr-setup'
CENTMINLOGDIR='/root/centminlogs'
CONFIGSCANBASE='/etc/centminmod'
CONFIGSCANDIR="${CONFIGSCANBASE}/php.d"

THISFILEDIR=$(CDPATH= cd -- "${BASH_SOURCE[0]%/*}" && pwd)
CM_INSTALLDIR=${THISFILEDIR%/*}
SCRIPT_DIR=$CM_INSTALLDIR
CUR_DIR=$CM_INSTALLDIR

php -v > /dev/null 2>&1
if [[ $? == 0 ]]; then
   PHPCURRENTVER=$(php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)
fi

#####################################################
NGINX_IPV='n' # NGINX IPV6 support for unattended mode only, option deprecated from 1.11.5+ 
UNATTENDED='y' # please leave at 'y' for best compatibility as at .07 release
CMVERSION_CHECK='n'

#ARCH_OVERRIDE='i386' # Uncomment this line if you are running a 32bit Paravirtulized Xen VPS
#####################################################

[ ! -d "$CENTMINLOGDIR" ] && mkdir -p "$CENTMINLOGDIR"
[ ! -d "$CONFIGSCANBASE" ] && mkdir -p "$CONFIGSCANBASE"
[ ! -d "$CONFIGSCANDIR" ] && mkdir -p "$CONFIGSCANDIR"

set -o pipefail

# set locale temporarily to english
# for php compile due to some non-english
# locale compilation issues i.e. Turkish 
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)
if [ "$CENTOSVER" == 'release' ]; then
   CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
   if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
      CENTOS_SEVEN=7
   fi
fi
if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
   CENTOS_SIX=6
fi
if [ "$CENTOSVER" == 'Enterprise' ]; then
   CENTOSVER=$(cat /etc/redhat-release | awk '{ print $7 }')
   OLS='y'
fi
if [[ -f /etc/system-release && "$(awk '{print $1,$2,$3}' /etc/system-release)" = 'Amazon Linux AMI' ]]; then
   CENTOS_SIX=6
fi

MACHINE_TYPE=$(uname -m) # Used to detect if OS is 64bit or not.
if [ "$ARCH_OVERRIDE" ]; then
   ARCH=$ARCH_OVERRIDE
else
   if [ "${MACHINE_TYPE}" == 'x86_64' ]; then
      ARCH='x86_64'
      MDB_ARCH='amd64'
   else
      ARCH='i386'
   fi
fi

if [[ "$MACHINE_TYPE" = 'x86_64' ]]; then
   SARCALL='/usr/lib64/sa/sa1'
else
   SARCALL='/usr/lib/sa/sa1'
fi

if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS=" -j$CPUS"
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        CPUS=$(echo $(($CPUS+2)))
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
else
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        CPUS=$(echo $(($CPUS+4)))
    elif [[ "$CPUS" -eq '8' ]]; then
        CPUS=$(echo $(($CPUS+2)))
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
fi

if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
  DISABLEREPO_DNF=' --disablerepo=rpmforge'
else
  DISABLEREPO_DNF=""
fi
if [[ "$CENTOS_SEVEN" = '7' && "$DNF_ENABLE" = [yY] ]]; then
   YUMDNFBIN="dnf${DISABLEREPO_DNF}"
   DISABLEREPO_DNF=""
else
   YUMDNFBIN='yum'
fi

if [[ ! -f /usr/bin/sar ]]; then
   $YUMDNFBIN -y -q install sysstat${DISABLEREPO_DNF}
fi

if [ -f "${CM_INSTALLDIR}/inc/custom_config.inc" ]; then
   source "${CM_INSTALLDIR}/inc/custom_config.inc"
fi
if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
   source "${CONFIGSCANBASE}/custom_config.inc"
fi
if [ -f "${CM_INSTALLDIR}/inc/z_custom.inc" ]; then
   source "${CM_INSTALLDIR}/inc/z_custom.inc"
fi


###############################################################
# FUNCTIONS
###############################################################


KEYPRESS_PARAM='-s -n1 -p'   # Read a keypress without hitting ENTER.
		# -s means do not echo input.
		# -n means accept only N characters of input.
		# -p means echo the following prompt before reading input
ASKCMD="read $KEYPRESS_PARAM "
ASK () {
   keystroke=''
   while [[ "$keystroke" != [yYnN] ]]; do
      $ASKCMD "$1" keystroke
      echo "$keystroke";
   done
   key=$(echo $keystroke)
}

# Setup Colours
black='\E[30;40m'
red='\E[31;40m'
green='\E[32;40m'
yellow='\E[33;40m'
blue='\E[34;40m'
magenta='\E[35;40m'
cyan='\E[36;40m'
white='\E[37;40m'

boldblack='\E[1;30;40m'
boldred='\E[1;31;40m'
boldgreen='\E[1;32;40m'
boldyellow='\E[1;33;40m'
boldblue='\E[1;34;40m'
boldmagenta='\E[1;35;40m'
boldcyan='\E[1;36;40m'
boldwhite='\E[1;37;40m'

Reset="tput sgr0"      #  Reset text attributes to normal without clearing screen.

# Coloured-echo.
# Argument $1 = message
# Argument $2 = color
cecho () {
   message=$1
   color=$2
   echo -e "$color$message" ; $Reset
   return
}

download_cmd() {
   if [[ "$CENTOS_SEVEN" == 7 || "$CENTOS_SIX" == 6 ]]; then
      DOWNLOADAPP='axel'
      local proto=${1%%://*}
      if [[ "$proto" == 'https' ]]; then
         if [[ "$CENTOS_SIX" == 6 ]]; then
            echo "CentOS 6 Axel fallback to wget for HTTPS download"
            DOWNLOADAPP="wget ${WGETOPT} --progress=bar --tries=3"
         elif [[ "$(curl -Isv $1 2>&1 | egrep 'ECDSA')" ]]; then
            # axel doesn't natively support ECC 256bit ssl certs with ECDSA ciphers due to CentOS system OpenSSL 1.0.2e
            echo "ECDSA SSL Cipher BASED HTTPS detected, switching from axel to wget"
            DOWNLOADAPP="wget ${WGETOPT} --progress=bar --tries=3"
         fi
      fi
   else
      DOWNLOADAPP="wget ${WGETOPT} --progress=bar --tries=3"
   fi
   
   local addopt=''
   if [[ -n "$2" && "$2" != '--tries=3' ]]; then
      if [[ "$DOWNLOADAPP" == 'axel' ]]; then
         addopt="-o $2"
      else
         addopt="-O $2"
      fi
   fi

   $DOWNLOADAPP $1 $addopt
}

sar_call() {
  $SARCALL 1 1
}

cmservice() {
   servicename=$1
   action=$2
   if [[ "$CENTOS_SEVEN" != '7' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
      echo "service ${servicename} $action"
      if [[ "$CMSDEBUG" = [nN] ]]; then
         service "${servicename}" "$action"
      fi
   else
      echo "systemctl $action ${servicename}.service"
      if [[ "$CMSDEBUG" = [nN] ]]; then
         systemctl "$action" "${servicename}.service"
      fi
   fi
}

cmchkconfig() {
   servicename=$1
   status=$2
   if [[ "$CENTOS_SEVEN" != '7' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
      echo "chkconfig ${servicename} $status"
      if [[ "$CMSDEBUG" = [nN] ]]; then
         chkconfig "${servicename}" "$status"
      fi
   else
      if [ "$status" = 'on' ]; then
         status=enable
      fi
      if [ "$status" = 'off' ]; then
         status=disable
      fi
      echo "systemctl $status ${servicename}.service"
      if [[ "$CMSDEBUG" = [nN] ]]; then
         systemctl "$status" "${servicename}.service"
      fi
   fi
}

# http://stackoverflow.com/questions/4023830/how-compare-two-strings-in-dot-separated-version-format-in-bash
# if $1 = $2 $?=0
# if $1 > $2 $?=1
# if $1 < $2 $?=2
vcmp() {
   if [[ $1 == $2 ]]; then
      return 0
   fi
   local IFS=.
   local i ver1=($1) ver2=($2)
   # fill empty fields in ver1 with zeros
   for ((i=${#ver1[@]}; i<${#ver2[@]}; i++)); do
      ver1[i]=0
   done
   for ((i=0; i<${#ver1[@]}; i++)); do
      if [[ -z ${ver2[i]} ]]; then
         # fill empty fields in ver2 with zeros
         ver2[i]=0
      fi
      if ((10#${ver1[i]} > 10#${ver2[i]})); then
         return 1
      fi
      if ((10#${ver1[i]} < 10#${ver2[i]})); then
         return 2
      fi
   done
   return 0
}

vcmp_gt() {
   vcmp $1 $2
   [[ $? == 1 ]] && echo 1
}

vcmp_ge() {
   vcmp $1 $2
   [[ $? != 2 ]] && echo 1
}

vcmp_lt() {
   vcmp $1 $2
   [[ $? == 2 ]] && echo 1
}

vcmp_le() {
   vcmp $1 $2
   [[ $? != 1 ]] && echo 1
}

