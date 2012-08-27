#!/bin/bash

usage()
{
        echo ""
        echo "Usage: make_deb [-d distname -d distname2] [-a archname -a archname2] [-m modname -m modname2]"
        echo ""
}

addDist()
{
    if [ "$dists" == "" ]; then
        dists=$1
    else
        dists="$dists $1"
    fi
}

addArch()
{
    if [ "$archs" == "" ]; then
        archs=$1
    else
        archs="$archs $1"
    fi
}

addMod()
{
    if [ "$mods" == "" ]; then
        mods=$1
    else
        mods="$mods $1"
    fi
}

target=$1; shift

if [ "$target" == "" ]; then usage; exit; fi

while [ "$1" != "" ]; do
    case $1 in
        -d )        shift
                    addDist "$1"
                    shift
                    ;;
        -a )        shift
                    addArch "$1"
                    shift
                    ;;
        -m )        shift
                    addMod "$1"
                    shift
                    ;;
        --help )    usage
                    exit
                    ;;
    esac
done

if [ "$archs" == "" ]; then archs=`dpkg --print-architecture`; fi
if [ "$dists" == "" ]; then dists=`cat /etc/*-release | grep CODENAME | sed 's:DISTRIB_CODENAME=::g'`; fi
if [ "$mods" == "" ]; then mods="interaction"; fi

echo "Selected archs: $archs"
echo "Selected dists: $dists"
echo "Selected mods: $mods"

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

for dist in $dists; do
  echo "Starting build for dist group $dist"
  echo "----------"
  for arch in $archs; do
    echo "Starting build for arch group $arch"
    echo "----------"
    for mod in $mods; do
      echo "Starting build for mod group $mod"
      echo "----------"
      sudo DIST=$dist ARCH=$arch MOD=$mod cowbuilder --build ../*.dsc
    done
  done
done
