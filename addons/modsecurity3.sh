#!/bin/bash
THISFILEDIR=$(CDPATH= cd -- "${BASH_SOURCE[0]%/*}" && pwd)
CM_INSTALLDIR=${THISFILEDIR%/*}

source "${CM_INSTALLDIR}/inc/init.inc"
source "${CM_INSTALLDIR}/inc/gcc.inc"
source "${CM_INSTALLDIR}/inc/modsecurity.inc"

{

[[ ! -d "$DIR_TMP" ]] && { mkdir -p "$DIR_TMP"; chmod 0750 "$DIR_TMP"; }

INITIALINSTALL='y'
if [[ "$CLANG" = [yY] ]]; then
   enable_clang
else
   enable_devtoolset
fi

modsecurity3install

} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_modsecurity3_install_${DT}.log