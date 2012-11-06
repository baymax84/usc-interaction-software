#!/bin/bash

echo "`date` starting $0"
echo "----------"

typeset -A current_version
typeset -A current_release

apt_update="true"

while [ "$1" != "" ]; do
    case $1 in
        --no-update )   shift
                        apt_update="false"
                        ;;
        * )             shift
                        ;;
    esac
done

if [ "$apt_update" == "true" ]; then
    sudo apt-get update
fi

for item in /var/cache/pbuilder/*/result/*.deb; do
    basename=`echo "$item" | sed -r 's:(.+)_.+\.deb$:\1:1'`
    dpkg_info=`dpkg -I $item`
    version=`echo "$dpkg_info" | grep --max-count 1 "Version: " | sed -r 's/.*Version: (.+)/\1/1'`
    release=`echo "$version" | sed -r 's/[0-9\.]*-?([0-9]+)/\1/1'`
    name=`echo "$dpkg_info" | grep --max-count 1 "Package: " | sed -r 's/.*Package: (.+)/\1/1'`
    arch=`echo "$dpkg_info" | grep --max-count 1 "Architecture: " | sed -r 's/.*Architecture: (.+)/\1/1'`
    id=$name$arch

    if [ "${current_version[$id]}" == "" ]; then
        current_version[$id]=`apt-cache search $name --names-only --full | grep --max-count 1 "Version: " | sed -r 's/.*Version: (.+)/\1/1'`
        if [ "${current_version[$id]}" == "" ]; then
            current_version[$id]="none"
        else
            current_release[$id]=`echo "${current_version[$id]}" | sed -r 's/[0-9\.]*-?([0-9]+)/\1/1'`
        fi
        echo -e "\e[34mNewest version of $name $arch set to \e[4m${current_version[$id]}\e[0m"
    fi

#    echo "$item"
#    echo "$name : [ $version ] ? [ $current_version ] | [ $release ] ? [ $current_release ]"

    if [ "${current_release[$id]}" != "" ]; then
        if [ $release -lt ${current_release[$id]} ]; then
            echo -e "\e[33mPackage [ $name \e[4m$version\e[0;33m $arch ] is out-of-date; newest version is [ ${current_version[$id]} ]\e[0m"
#            dcut -i $basename*.changes local
            sudo rm $basename*
        elif [ $release -gt ${current_release[$id]} ]; then
            echo -e "\e[0;36mPackage [ $name \e[4m$version\e[0;36m $arch ] is newer than the apt cache; invoking dput\e[0m"
            dput -U local $basename*.changes
        else
            echo -e "\e[0;32mPackage [ $name \e[4m$version\e[0;32m $arch ] is up-to-date\e[0m"
        fi
    else
        echo -e "\e[0;35mPackage [ $name \e[4m$version\e[0;35m $arch ] was not found in the apt cache; invoking dput\e[0m"
        dput -U local $basename*.changes
    fi
done

echo "----------"
echo "`date` finished $0"
