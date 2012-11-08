#!/bin/sh

echo "`date` cron start"
echo "----------"

bash ~/cron/update-ekaszubski-code-prototyping.bash
bash ~/cron/update-ekaszubski-website-robotics.bash
bash ~/cron/update-interaction-ros-pkg.bash
bash ~/cron/update-seabee3-ros-pkg.bash
bash ~/cron/update-usc-ros-pkg.bash
bash ~/cron/update-usc-interaction-software.bash

bash ~/build_system/update-all.bash

bash ~/cron/build-debs.bash

echo "----------"
echo "`date` cron stop"

mail -s "calculon cron log `date +%F-%R-%Z`" ekaszubski@gmail.com < /home/ekaszubski/cron/log

mv /home/ekaszubski/cron/log "/home/ekaszubski/cron/logs/log-`date +%F-%R`"
