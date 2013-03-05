#!/bin/bash

echo "`date` starting $0"
echo "----------"

cd /home/ekaszubski/build_space/seabee3-ros-pkg

for dir in `find -mindepth 1 -maxdepth 1 -type d`
do
    pushd $dir
    if [ $? ]
    then
        git pull --all -v

        for branch in `git branch -l | sed 's:^..::g'`
        do
          git scheckout $branch && git pull
        done
        
        popd
    fi
done

echo "----------"
echo "`date` finished $0"
