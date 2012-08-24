#!/bin/bash

target=$1

cd $target
target_branch=`git branch -l | grep \* | sed 's:\* ::g'`
echo $target_branch
latest_log=`git log -n 1`
target_version=`echo "$latest_log" | tr '[A-Z]' '[a-z]' | grep date | sed -r 's/date: +//g' | sed -r 's: -.+::g' | sed -r 's: :-:g' | sed 's/:/-/g'`
echo $target_version
cd ..

deb_target=$target-$target_branch-$target_version
mkdir $target-debuild
cp $target $target-debuild/$deb_target -r

cd $target-debuild/$deb_target && make clean && cd ..
tar -czf $deb_target.orig.tar.gz $deb_target

cd $deb_target

debuild -S
sudo pbuilder build ../*.dsc
