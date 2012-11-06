#!/bin/bash

echo "`date` starting $0"
echo "----------"

cd /home/ekaszubski/workspace/usc-ros-pkg-svn && svn up

echo "----------"
echo "`date` finished $0"
