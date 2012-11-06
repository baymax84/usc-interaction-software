#!/bin/bash

echo "`date` starting $0"
echo "----------"

typeset -A current_version
typeset -A current_release

for item in /var/cache/pbuilder/*/result/*.deb; do
    dpkg_info=`dpkg -I $item`
    version=`echo "$dpkg_info" | grep --max-count 1 "Version: " | sed -r 's/.*Version: (.+)/\1/1'`
    release=`echo "$version" | sed -r 's/[0-9\.]*-?([0-9]+)/\1/1'`
    name=`echo "$dpkg_info" | grep --max-count 1 "Package: " | sed -r 's/.*Package: (.+)/\1/1'`
    arch=`echo "$dpkg_info" | grep --max-count 1 "Architecture: " | sed -r 's/.*Architecture: (.+)/\1/1'`
    id=$name$arch

    if [ "${current_version[$id]}" == "" ]; then
        current_version[$id]=`apt-cache show $name | grep --max-count 1 "Version: " | sed -r 's/.*Version: (.+)/\1/1'`
        current_release[$id]=`echo "${current_version[$id]}" | sed -r 's/[0-9\.]*-?([0-9]+)/\1/1'`
        echo -e "\e[1;34mNewest version of $name $arch set to ${current_version[$id]}\e[0m"
    fi

#    echo "$item"
#    echo "$name : [ $version ] ? [ $current_version ] | [ $release ] ? [ $current_release ]"

    if [ "${current_release[$id]}" != "" ]; then
        if [ $release -lt ${current_release[$id]} ]; then
            echo -e "\e[33mPackage [ $name $version $arch ] is out-of-date; newest version is [ ${current_version[$id]} ]\e[0m"
            sudo rm /var/cache/pbuilder/*/result/$name"_"$version*
        elif [ $release -gt ${current_release[$id]} ]; then
            echo -e "\e[36mPackage [ $name $version $arch ] is newer than the apt cache.\e[0m"
        else
            echo -e "\e[32mPackage [ $name $version $arch ] is up-to-date\e[0m"
        fi
    else
        echo -e "\e[1;36mPackage [ $name $version $arch ] was not found in the apt cache.\e[0m"
    fi
done

echo "----------"
echo "`date` finished $0"
