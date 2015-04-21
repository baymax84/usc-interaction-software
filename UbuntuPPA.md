**Contents:**


# Introduction #

A subset of our software is available for Ubuntu through a PPA for maximum portability. This page contains information about these packages, namely how to install them

# State #

The Ubuntu PPA is currently in a setup/testing phase and its location and/or availability in the short-term are subject to change without notice.


---


| 08/30/12 | Repo initialized |
|:---------|:-----------------|
| 08/31/12 | GPG keys added |
| 11/05/12 | Dist-specific release added |
| 12/09/13 | Packages subdomain added |

# Installation #

Currently, the PPA can be "installed" in two steps

1. Add the package list to your sources

```
sudo sh -c 'echo "deb http://packages.robotics.usc.edu/ubuntu `cat /etc/*-release | grep CODENAME | sed -e s:DISTRIB_CODENAME=::g`/" > /etc/apt/sources.list.d/usc-latest.list'
```

2. Import the PPA's GPG key and update your package index

```
wget http://packages.robotics.usc.edu/ubuntu/uscrobotics.key -O - | sudo apt-key add -
sudo apt-get update
```

Packages can then be located/installed via synaptic, apt, or similar

```
sudo apt-get install <package>
```

Supported platforms:
  * Ubuntu Maverick (10.10) i386/amd64
    * ROS Electric
  * Ubuntu Precise (12.04) i386/amd64
    * ROS Groovy
    * ROS Hydro
  * Ubuntu Raring (13.04) i386/amd64
    * ROS Hydro

Packages include:
  * quickdev ( `ros-<version>-interaction-quickdev` )
  * `interaction-scripts`
  * humanoid ( `ros-<version>-interaction-humanoid` )
  * humanoid-sensing ( `ros-<version>-interaction-humanoid-sensing` )
  * bandit ( `ros-<version>-interaction-bandit` )