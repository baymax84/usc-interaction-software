#!/bin/bash

echo "`date` starting $0"
echo "----------"

typeset -A current_version
typeset -A current_release

apt_update="true"

while [ "$1" != "" ]; do
    case $1 in
        * )             shift
                        ;;
    esac
done

for item in /var/cache/pbuilder/*/result/*.changes; do
    basepath=`echo "$item" | sed -r 's:(.+result)/.+:\1:1'`
    dpkg_info=`cat $item`
    version=`echo "$dpkg_info" | grep --max-count 1 "Version: " | sed -r 's/Version: (.+)/\1/1'`
    release=`echo "$version" | sed -r 's/[0-9\.]*-?([0-9]+)/\1/1'`
    api=`echo "$version" | sed -r 's/([0-9\.]*)-?([0-9]+)/\1/1'`
    dist=`echo "$dpkg_info" | grep --max-count 1 "Distribution: " | sed -r 's/Distribution: (.+)/\1/1'`
    name=`echo "$dpkg_info" | grep --max-count 1 "Source: " | sed -r 's/Source: (.+)/\1/1'`
    packages=`echo "$dpkg_info" | grep --max-count 1 "Binary: " | sed -r 's/Binary: (.+)/\1/1'`
    arch=`echo "$dpkg_info" | grep --max-count 1 "Architecture: " | sed -r 's/Architecture: (.+)/\1/1' | sed -r 's/source //1'`
#    if [ "$arch" == "all" ]; then
#        arch_searchterm=*
#    else
#        arch_searchterm="$arch"
#    fi

    for package in $packages; do
        id="$package$dist$arch"

#        echo "$basepath | $item | $version | $release | $api | $dist | $name [ $package ] | $arch | $id"

        if [ "${current_version[$id]}" == "" ]; then
            current_deb_paths=`ls -r "/home/$USER/archive/"$dist"/"$package"_"$api-*_$arch".deb"`
            newest_deb_path=`echo "$current_deb_paths" | head -n 1`
#            current_version[$id]=`cat $current_changes_path | grep --max-count 1 "Version: " | sed -r 's/Version: (.+)/\1/1'`
            current_version[$id]=`[ "$newest_deb_path" != "" ] && dpkg --info $newest_deb_path | grep --max-count 1 "Version: " | sed -r 's/.*Version: (.+)/\1/1'`
            if [ "${current_version[$id]}" == "" ]; then
                current_version[$id]="none"
            else
                current_release[$id]=`echo "${current_version[$id]}" | sed -r 's/[0-9\.]*-?([0-9]+)/\1/1'`
            fi
            echo -e "\e[34mNewest version of $package $dist $arch set to \e[4m${current_version[$id]}\e[0m"
        fi

#    echo "$item"
#    echo "$name : [ $version ] ? [ $current_version ] | [ $release ] ? [ $current_release ]"

        if [ "${current_release[$id]}" != "" ]; then
            if [ $release -lt ${current_release[$id]} ]; then
                echo -e "\e[33mPackage [ $package \e[4m$version\e[0;33m $dist $arch ] is out-of-date; newest version is [ ${current_version[$id]} ]\e[0m"
#                dcut -i $basename*.changes local
                sudo rm $basepath/$package"_"$version*
            elif [ $release -gt ${current_release[$id]} ]; then
                echo -e "\e[0;36mPackage [ $package \e[4m$version\e[0;36m $dist $arch ] is newer than the apt cache; invoking dput\e[0m"
                dput -U local $item
            else
                echo -e "\e[0;32mPackage [ $package \e[4m$version\e[0;32m $dist $arch ] is up-to-date\e[0m"
            fi
        else
            echo -e "\e[0;35mPackage [ $package \e[4m$version\e[0;35m $dist $arch ] was not found in the apt cache; invoking dput\e[0m"
            dput -U local $item
        fi
    done
done

echo "----------"
echo "`date` finished $0"
