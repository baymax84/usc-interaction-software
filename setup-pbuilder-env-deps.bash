#!/bin/bash

DIST=maverick ARCH=i386 MOD=vanilla cowbuilder --login --save-after-login
DIST=maverick ARCH=i386 MOD=interaction cowbuilder --login --save-after-login
DIST=maverick ARCH=i386 MOD=ros-diamondback cowbuilder --login --save-after-login
DIST=maverick ARCH=i386 MOD=ros-electric cowbuilder --login --save-after-login

DIST=maverick ARCH=amd64 MOD=vanilla cowbuilder --login --save-after-login
DIST=maverick ARCH=amd64 MOD=interaction cowbuilder --login --save-after-login
DIST=maverick ARCH=amd64 MOD=ros-diamondback cowbuilder --login --save-after-login
DIST=maverick ARCH=amd64 MOD=ros-electric cowbuilder --login --save-after-login
