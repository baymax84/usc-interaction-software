# Building HumanoidRetargeter #
The kinematic\_retargeting package depends on rtk, located under the kinematic\_retargeting branch of the usc-interaction-software ros repository. It also depends on a few packages, such as humanoid and quickdev, that are located under the [electric\_unstable](http://code.google.com/p/usc-interaction-software/source/browse?repo=ros&name=electric_unstable) branch. Once all of these packages are checked out, the package can be built using rosmake.

# Running HumanoidRetargeter #
There are three launch files related to the HumanoidRetargeter node:
### humanoid\_retargeter.launch ###
This file contains the most basic commands necessary to run the node. It sets the location of the urdf file that is loaded for the target chain, the location of the yaml file used to define the endpoints of the target and source chains, and the name of the "world frame" in tf. All of the frames used by tf should have transforms to the world frame. This includes the humanoid to which the node subscribes.

### humanoid\_retargeter\_demo.launch ###
In addition to running the humanoid\_retargeter node, this launch file runs all of the nodes necessary to produce a source humanoid for the retargeter. The type of source humanoid is dependent on the **simulate** argument. If simulate is true, a node called virtual\_humanoid from the humanoid\_models package is run. The virtual\_humanoid node publishes a dummy humanoid with all of the joint angles set from dynamic\_reconfigure. If simulate is false, the openni skeleton tracker and a node called openni\_adapter are launched. openni\_adapter is located in the humanoid-sensing package, and converts the messages published by openni to the humanoid data structure.

### bandit\_demo.launch ###
This launch file includes humanoid\_retargeter\_demo.launch, but also runs an instance of the bandit\_driver\_prototype node and remaps the joint states from the humanoid\_retargeter to the driver.

# YAML Chain Definitions #
The humanoid\_retargeter node must load a YAML file in which the chains that it will retargete are defined. Each YAML file may contain an arbitrary number of chains, with the minimum number being one. Each chain definition has three major components:
  * target
    * Defines the beginning and end links of the target chain
  * source
    * Defines the beginning and end joints of the source chain, as well as the parent joint of the chaine
  * target\_to\_source
    * An arbitrary transformation which will be applied to the target chain before retargeting.

Many premade definitions are located in kinematic\_retargeting/params.

# Visualizing #
The HumanoidRetargeter node publishes visualization data for the normalized source and target chains in the form of MarkerArray messages. These messages can be viewed in rviz by subscribing to the topic `/humanoid_retargeter/marker_array`. tf frames for the virtual\_humanoid node can be viewed as well.

# Building rtk Documenation #
Run rosdoc using the command:

```
rosrun rosdoc rosdoc rtk
```

Documentation will be built in `rtk/doc`. A good place to start learning would be viewing `rtk/doc/rtk/html/index.html`.