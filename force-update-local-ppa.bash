#!/bin/bash

echo "`date` starting $0"
echo "----------"

rm ~/archive/*/Packages*
dput -U local /var/cache/pbuilder/*/result/*.changes

sleep 1

mini-dinstall --batch --no-db --no-log

#ssh asimov "rm -r ~/interaction-ppa/ubuntu/unstable/*/$1*"
rsync -azrptLW -e "ssh" ~/archive/unstable asimov:interaction-ppa/ubuntu/

sudo apt-get update
bash ~/cron/clean-pbuilder.bash

echo "----------"
echo "`date` finished $0"
