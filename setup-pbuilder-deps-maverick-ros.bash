#!/bin/bash

chmod +x ~/.pbuilder/hooks/F00install-maverick-diamondback-deps
sudo DIST=maverick ARCH=amd64 MOD=ros-diamondback cowbuilder --login --save-after-login
sudo DIST=maverick ARCH=i386 MOD=ros-diamondback cowbuilder --login --save-after-login
chmod -x ~/.pbuilder/hooks/F00install-maverick-diamondback-deps

chmod +x ~/.pbuilder/hooks/F00install-maverick-electric-deps
sudo DIST=maverick ARCH=amd64 MOD=ros-electric cowbuilder --login --save-after-login
sudo DIST=maverick ARCH=i386 MOD=ros-electric cowbuilder --login --save-after-login
chmod -x ~/.pbuilder/hooks/F00install-maverick-electric-deps
