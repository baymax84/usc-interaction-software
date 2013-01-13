#!/bin/bash

echo "`date` starting $0"
echo "----------"

cd /home/ekaszubski/build_space/seabee3-ros-pkg && git pull --all -v

for branch in `git branch -l | sed 's:^..::g'`; do
  git scheckout $branch && git pull
done

echo "----------"
echo "`date` finished $0"
