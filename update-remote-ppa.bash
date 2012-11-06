#!/bin/bash

echo "`date` starting $0"
echo "----------"

apt_update="true"
update_local="true"

while [ "$1" != "" ]; do
    case $1 in
        --no-update )   shift
                        apt_update="false"
                        ;;
        --no-local )    shift
                        update_local="false"
                        ;;
        * )             shift
                        ;;
    esac
done

rm ~/archive/*/Packages*

if [ "$update_local" == "true" ]; then
    bash ~/cron/update-local-ppa.bash --no-update
fi

mini-dinstall --batch --no-db --no-log

#ssh asimov "rm -r ~/interaction-ppa/ubuntu/unstable/*/$1*"
rsync -azrptLW -e "ssh" ~/archive/* asimov:interaction-ppa/ubuntu/

if [ "$apt_update" == "true" ]; then
    sudo apt-get update
fi

echo "----------"
echo "`date` finished $0"
