# for ngx_brotli https://github.com/google/ngx_brotli
# + libbrotli https://github.com/bagder/libbrotli

libbroti_install() {
	if [[ "$NGINX_LIBBROTLI" = [yY] ]]; then
		# if devtoolset-4 doesn't exist install it otherwise check if gcc & g++ versions are less than 4.7 (407)
    if [[ "$CENTOS_SIX" = '6' ]]; then
      if [[ ! -f /opt/rh/devtoolset-4/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]] && [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
      	scl_install
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
        gcc --version
        g++ --version
      elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
        source /opt/rh/devtoolset-6/enable
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
        gcc --version
        g++ --version
      elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
        source /opt/rh/devtoolset-4/enable
        unset CC
        unset CXX
        export CC="ccache gcc"
        export CXX="ccache g++"
        gcc --version
        g++ --version
      fi
    fi # centos 6 only needed

		echo "install libbrotli"
		cd $DIR_TMP

		LIBBROTLI_LINK='https://github.com/bagder/libbrotli'
		# fallback mirror if official github is down, use gitlab mirror
		curl -4Is --connect-timeout 5 --max-time 5 $LIBBROTLI_LINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
		LIBBROTLI_CURLCHECK=$?
		if [[ "$LIBBROTLI_CURLCHECK" != '0' ]]; then
			LIBBROTLI_LINK='https://gitlab.com/centminmod-github-mirror/libbrotli.git'
		fi
    if [ ! -d libbrotli ]; then
      time git clone "$LIBBROTLI_LINK"
    elif [ -d libbrotli ]; then
      rm -rf libbrotli
      time git clone "$LIBBROTLI_LINK"
    fi
		cd libbrotli
		# if [[ "$INITIALINSTALL" != [yY] ]]; then
  #   		make clean
  #   		git stash
  #   		git pull
		# fi
		./autogen.sh
		./configure
		make${MAKETHREADS}
		make install
		echo

    if [[ "$CENTOS_SIX" = '6' ]]; then
    if [[ ! -f /opt/rh/devtoolset-4/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]] && [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
      scl_install
      unset CC
      unset CXX		
    elif [[ "$DEVTOOLSETSIX" = [yY] && -f /opt/rh/devtoolset-6/root/usr/bin/gcc && -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
      unset CC
      unset CXX
    elif [[ -f /opt/rh/devtoolset-4/root/usr/bin/gcc && -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
      unset CC
      unset CXX	
    fi		
		fi # centos 6 only needed
	fi
}

ngxbrotli_download() {
	if [[ "$NGINX_LIBBROTLI" = [yY] ]]; then
    # only if GCC used to compile nginx with ngx_brotli
    # set BROTLI_CFLAG https://community.centminmod.com/posts/38284/
    if [[ "$CLANG" = [nN] ]]; then
      BROTLI_CFLAG=' -Wno-deprecated-declarations'
    else
      BROTLI_CFLAG=""
    fi
    echo "BROTLI_CFLAG=$BROTLI_CFLAG"
    echo "download ngx_brotli module"
		cd $DIR_TMP

		NGXBROTLILINK='https://github.com/google/ngx_brotli'
		# fallback mirror if official github is down, use gitlab mirror
		curl -4Is --connect-timeout 5 --max-time 5 $NGXBROTLILINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
		NGXBROTLICURLCHECK=$?
		if [[ "$NGXBROTLICURLCHECK" != '0' ]]; then
			NGXBROTLILINK='https://gitlab.com/centminmod-github-mirror/ngx_brotli.git'
		fi

    if [ ! -d ngx_brotli ]; then
      time git clone --recursive "$NGXBROTLILINK"
    fi
		if [[ "$INITIALINSTALL" != [yY] ]]; then
        cd ngx_brotli
        # git stash
        git fetch origin
        git reset --hard origin/master
        git pull
        git submodule update --init
		fi
		echo
	fi
}