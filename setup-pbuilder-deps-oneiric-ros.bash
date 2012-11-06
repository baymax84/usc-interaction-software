#!/bin/bash

chmod +x ~/.pbuilder/hooks/F00install-oneiric-fuerte-deps
sudo DIST=oneiric ARCH=amd64 MOD=ros-fuerte cowbuilder --login --save-after-login
sudo DIST=oneiric ARCH=i386 MOD=ros-fuerte cowbuilder --login --save-after-login
chmod -x ~/.pbuilder/hooks/F00install-oneiric-fuerte-deps

chmod +x ~/.pbuilder/hooks/F00install-oneiric-electric-deps
sudo DIST=oneiric ARCH=amd64 MOD=ros-electric cowbuilder --login --save-after-login
sudo DIST=oneiric ARCH=i386 MOD=ros-electric cowbuilder --login --save-after-login
chmod -x ~/.pbuilder/hooks/F00install-oneiric-electric-deps
