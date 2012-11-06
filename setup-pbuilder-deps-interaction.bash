#!/bin/bash

chmod +x ~/.pbuilder/hooks/F00install-precise-interaction-deps
sudo DIST=precise ARCH=amd64 MOD=interaction cowbuilder --login --save-after-login
sudo DIST=precise ARCH=i386 MOD=interaction cowbuilder --login --save-after-login
chmod -x ~/.pbuilder/hooks/F00install-precise-interaction-deps

chmod +x ~/.pbuilder/hooks/F00install-oneiric-interaction-deps
sudo DIST=oneiric ARCH=amd64 MOD=interaction cowbuilder --login --save-after-login
sudo DIST=oneiric ARCH=i386 MOD=interaction cowbuilder --login --save-after-login
chmod -x ~/.pbuilder/hooks/F00install-oneiric-interaction-deps
