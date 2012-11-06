#!/bin/bash

DIST=maverick ARCH=i386 MOD=interaction cowbuilder --login --save-after-login
DIST=maverick ARCH=amd64 MOD=interaction cowbuilder --login --save-after-login
