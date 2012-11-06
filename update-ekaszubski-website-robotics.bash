#!/bin/bash

echo "`date` starting $0"
echo "----------"

cd /home/ekaszubski/workspace/ekaszubski/website-robotics && git pull --all -v && git checkout master

echo "----------"
echo "`date` finished $0"
