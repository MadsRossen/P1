Make a new workspace:
------------------------------------------------------------------------
Make a new folder:
mkdir "name of folder"
As an example:
mkdir catkin_ws

Initialize folder as a workspace (stay in folder):
catkin init

Make new source space folder in the Initialize folder:
mkdir src

Run config command to confirm new source space folder:
catkin config
-----------------------------------------------------------

Make a new ROS package
------------------------------------------------------------
Navigate to source space folder in new workspace folder.
As an example:
cd catkin_ws/src

Make new package:
catkin create pkg "name" --catkin-deps roscpp
As an example:
catkin create pkg hello_pkg --catkin-deps roscpp
-------------------------------------------------------------

Run turtlesim
------------------------------------------------------------
Initialize new roscore:
roscore

Run new turtlesim node:
rosrun turtlesim turtlesim_node
-------------------------------------------------------------

Build and run code
-------------------------------------------------------------
Go to workspace folder and build:
As an example:
cd catkin_ws
catkin build

find setup.bash (stay in folder)
source devel/setup.bash

run cpp code:
rosrun "pkg_name" "cpp_file_name"
as an example:
rosrun hello_pkg roundround 
---------------------------------------------------------------


Startup turtlebot simulation
-------------------------------------------------------
Startup Gazebo world simulator:
roslaunch turtlebot_gazebo turtlebot_world.launch

Start virtuel sensors and navigation:
roslaunch turtlebot_navigation gmapping_demo.launch
--------------------------------------------------------



Startup turtlebot real robot:
------------------------------------------------------------
startup turtlebot machine:
roslaunch turtlebot_bringup minimal.launch
---------------------------------------------------------------



Commands for both simulation and real life:
---------------------------------------------------------------
Run own turtlesim code:
rosrun --- --- /turtle1/cmd_vel:=/cmd_vel_mux/input/teleop
As an example:
rosrun my_first_pkg roundround /turtle1/cmd_vel:=/cmd_vel_mux/input/teleop

Control turtlebot with keyboard:
roslaunch turtlebot_teleop keyboard_teleop.launch

Start RVIZ:
rviz
---------------------------------------------------------------------------------



Run launch file:
----------------------------------------------------------------------------------
roslaunch "package name" "launchfile".launch
As an example turtlebot_gazebo_gmapping.launch will be launched in package busroute:

roslaunch busroute turtlebot_gazebo_gmapping.launch
-----------------------------------------------------------------------------------