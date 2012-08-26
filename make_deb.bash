#!/bin/bash

target=$1

target_name=`cat $target/debian/control | grep Source: | sed -r 's/Source: *//g'`
target_branch=`git branch -l | grep \* | sed 's:\* ::g'`

target_release=`echo $target_branch | sed -r 's:[a-zA-Z\-]*-?([0-9\.]+)-?.*:\1:1'`
target_version=`echo $target_branch | sed -r 's:[a-zA-Z\-]*-?[0-9\.]+-?([0-9\.]+):\1:1'`

target_releasename=$target_name"_"$target_release
target_fullname=$target_name"_"$target_release"-"$target_version
echo $target_fullname

#latest_log=`git log -n 1`
#target_version=`echo "$latest_log" | tr '[A-Z]' '[a-z]' | grep date | sed -r 's/date: +//g' | sed -r 's: -.+::g' | sed -r 's: :-:g' | sed 's/:/-/g'`
#echo $target_version
#cd ..

mkdir $target_fullname-debuild
cp $target $target_fullname-debuild/$target_releasename -r

cd $target_fullname-debuild/$target_releasename && make clean && cd ..
tar -czf $target_releasename.orig.tar.gz $target_releasename

cd $target_releasename

debuild -S
sudo pbuilder build ../*.dsc
