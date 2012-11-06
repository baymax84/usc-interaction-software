#!/bin/bash

DIST=maverick ARCH=i386 MOD=ros-electric cowbuilder --login --save-after-login
DIST=maverick ARCH=amd64 MOD=ros-electric cowbuilder --login --save-after-login
