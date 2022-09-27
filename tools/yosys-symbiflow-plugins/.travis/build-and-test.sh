#! /bin/bash

set -e

source .travis/common.sh

##########################################################################

echo
echo 'Configuring...' && echo -en 'travis_fold:start:script.configure\\r'
echo

if [ "$CONFIG" = "gcc" ]; then
	echo "Configuring for gcc."
	make config-gcc
elif [ "$CONFIG" = "clang" ]; then
	echo "Configuring for clang."
	make config-clang
fi

echo
echo -en 'travis_fold:end:script.configure\\r'
echo

##########################################################################

echo
echo 'Building plugins..' && echo -en 'travis_fold:start:script.build\\r'
echo

make plugins

echo
echo -en 'travis_fold:end:script.build\\r'
echo

##########################################################################

echo
echo 'Installing plugins...' && echo -en 'travis_fold:start:script.build\\r'
echo

make install

echo
echo -en 'travis_fold:end:script.build\\r'
echo

##########################################################################

echo
echo 'Testing...' && echo -en 'travis_fold:start:script.test\\r'
echo

make test

echo
echo -en 'travis_fold:end:script.test\\r'
echo

##########################################################################
