#!/bin/sh

echo "`date` cron start"
echo "----------"

bash ~/cron/update-sources.bash > $CRON_LOG_DIR/update-sources.log 2>&1

bash ~/build_system/update-all.bash > $CRON_LOG_DIR/update-all.log 2>&1

bash ~/cron/build-debs.bash > $CRON_LOG_DIR/build-debs.log 2>&1

echo "----------"
echo "`date` cron stop"

# when running, the log dir is known by its start time; when completed, it's known by its end time

STOP_TIME=`date +%F-%R-%S-%N`
NEW_CRON_LOG_DIR=$HOME/cron/logs/$STOP_TIME

mv $CRON_LOG_DIR $NEW_CRON_LOG_DIR
zip -r $HOME/cron/logs/$STOP_TIME.zip $NEW_CRON_LOG_DIR -x "*cron.log"
cat $NEW_CRON_LOG_DIR/cron.log | mutt -s "calculon cron log $STOP_TIME" -a $HOME/cron/logs/$STOP_TIME.zip -- ekaszubski@gmail.com 
