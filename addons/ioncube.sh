#!/bin/bash
#############################################################
# official ioncube loader PHP extension Addon for
# Centmin Mod centminmod.com
# written by George Liu (eva2000)
#############################################################
PHPCURRENTVER=$(php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)
#############################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

PHPALLOWEDVER="4.1 4.2 4.3 4.4 5.0 5.1 5.2 5.3 5.4 5.5 5.6 7.0"
if [[ $PHPALLOWEDVER != *${PHPCURRENTVER}* ]]; then
  echo "Your current PHP version $PHPCURRENTVER is incompatible with ioncube loader"
  echo "Aborting installation"
  exit
fi

echo
echo "ioncube loader installation started"
echo

cd /svr-setup
mkdir -p ioncube
cd ioncube

rm -rf ioncube
if [[ $(uname -m) == 'x86_64' ]]; then
   IONCUBEFILE='ioncube_loaders_lin_x86-64.tar.gz'
else
   IONCUBEFILE='ioncube_loaders_lin_x86.tar.gz'
fi
wget -4 -cnv https://downloads.ioncube.com/loader_downloads/ioncube_loaders_lin_x86-64.tar.gz
tar xvzf ioncube_loaders_lin_x86-64.tar.gz

# move current ioncube version to existing PHP extension directory
PHPEXTDIRD=`cat /usr/local/bin/php-config | awk '/^extension_dir/ {extdir=$1} END {gsub(/\047|extension_dir|=|)/,"",extdir); print extdir}'`
cp -fa ./ioncube/ioncube_loader_lin_${PHPCURRENTVER}.so "${PHPEXTDIRD}/ioncube.so"
chown root:root "${PHPEXTDIRD}/ioncube.so"
chmod 755 "${PHPEXTDIRD}/ioncube.so"

cd ..
rm -rf ioncube

CONFIGSCANDIR='/etc/centminmod/php.d'

if [[ -f "${CONFIGSCANDIR}/ioncube.ini" ]]; then
  rm -rf ${CONFIGSCANDIR}/ioncube.ini
fi

touch ${CONFIGSCANDIR}/ioncube.ini

cat > "${CONFIGSCANDIR}/ioncube.ini" <<EOF
zend_extension=${PHPEXTDIRD}/ioncube.so
EOF

ls -lah ${CONFIGSCANDIR}
echo ""
ls -lah ${PHPEXTDIRD}

service php-fpm restart >/dev/null 2>&1

if [ -f "${PHPEXTDIRD}/ioncube.so" ]; then
  echo ""
  echo "Check if PHP module: ionCube Loader loaded"
  php --ri 'ionCube Loader'
  
  echo
  echo "ioncube loader installation completed"
  echo "you'll need to rerun ioncube.sh after each major PHP version upgrades"
  echo "PHP 5.3 to 5.4 or PHP 5.4 to PHP 5.5 to PHP 5.6 to PHP 7.0 etc"
  echo
else
  echo ""
  echo "ionCube Loader failed to install properly"
fi