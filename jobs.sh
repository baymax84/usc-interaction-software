#!/bin/sh

echo "`date` cron start"
echo "----------"

bash ~/cron/update-sources.bash

bash ~/build_system/update-all.bash

bash ~/cron/build-debs.bash

echo "----------"
echo "`date` cron stop"

mail -s "calculon cron log `date +%F-%R-%Z`" ekaszubski@gmail.com < /home/ekaszubski/cron/log

mv /home/ekaszubski/cron/log "/home/ekaszubski/cron/logs/log-`date +%F-%R`"
