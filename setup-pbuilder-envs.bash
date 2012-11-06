#!/bin/bash

#DIST=maverick ARCH=i386 MOD=vanilla cowbuilder --create
DIST=maverick ARCH=i386 MOD=interaction cowbuilder --create
DIST=maverick ARCH=i386 MOD=ros-diamondback cowbuilder --create
DIST=maverick ARCH=i386 MOD=ros-electric cowbuilder --create

DIST=maverick ARCH=amd64 MOD=vanilla cowbuilder --create
DIST=maverick ARCH=amd64 MOD=interaction cowbuilder --create
DIST=maverick ARCH=amd64 MOD=ros-diamondback cowbuilder --create
DIST=maverick ARCH=amd64 MOD=ros-electric cowbuilder --create
