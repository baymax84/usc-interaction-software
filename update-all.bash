#!/bin/bash

chmod +x ~/.pbuilder/hooks/F00dist-upgrade
for dist in maverick precise; do
  for arch in i386 amd64; do
    for mod in vanilla interaction ros-diamondback ros-electric ros-fuerte ros-groovy; do
      sudo DIST=$dist ARCH=$arch MOD=$mod cowbuilder --login --save-after-login
    done
  done
done
chmod -x ~/.pbuilder/hooks/F00dist-upgrade
