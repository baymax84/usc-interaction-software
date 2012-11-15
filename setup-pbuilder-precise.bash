sudo DIST=precise ARCH=amd64 MOD=vanilla cowbuilder --create --debootstrapopts --variant=buildd
sudo DIST=precise ARCH=amd64 MOD=interaction cowbuilder --create --debootstrapopts --variant=buildd
sudo DIST=precise ARCH=amd64 MOD=ros-fuerte cowbuilder --create --debootstrapopts --variant=buildd
sudo DIST=precise ARCH=amd64 MOD=ros-groovy cowbuilder --create --debootstrapopts --variant=buildd

sudo DIST=precise ARCH=i386 MOD=vanilla cowbuilder --create --debootstrapopts --variant=buildd
sudo DIST=precise ARCH=i386 MOD=interaction cowbuilder --create --debootstrapopts --variant=buildd
sudo DIST=precise ARCH=i386 MOD=ros-fuerte cowbuilder --create --debootstrapopts --variant=buildd
sudo DIST=precise ARCH=i386 MOD=ros-groovy cowbuilder --create --debootstrapopts --variant=buildd
