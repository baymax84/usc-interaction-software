#!/bin/bash

chmod +x ~/.pbuilder/hooks/F00install-precise-fuerte-deps
sudo DIST=precise ARCH=amd64 MOD=ros-fuerte cowbuilder --login --save-after-login
sudo DIST=precise ARCH=i386 MOD=ros-fuerte cowbuilder --login --save-after-login
chmod -x ~/.pbuilder/hooks/F00install-precise-fuerte-deps

chmod +x ~/.pbuilder/hooks/F00install-precise-groovy-deps
sudo DIST=precise ARCH=amd64 MOD=ros-groovy cowbuilder --login --save-after-login
sudo DIST=precise ARCH=i386 MOD=ros-groovy cowbuilder --login --save-after-login
chmod -x ~/.pbuilder/hooks/F00install-precise-groovy-deps
