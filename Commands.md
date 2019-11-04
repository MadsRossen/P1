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


Edit Bash file to run ROS on MASTER_PC via network as standard :
REMEMBER! switch VMWare network adapter from NAT to bridged, when communicating with
ROS on MASTER_PC via network and back again when running ROS on your own pc.
-----------------------------------------------------------------------------------
Write in terminal:
gedit .bashrc

write these lines in the bottom of the bash file, remember to edit "MASTER_IP" and "YOUR_PC_IP" to the apropiate IP's:
export ROS_MASTER_URI=http://"MASTER_IP":11311/
export ROS_HOSTNAME="YOUR_PC_IP"
export ROS_IP="YOUR_PC_IP"

Save bash file and close it, then write in terminal:
source ~/.bashrc
-----------------------------------------------------------------------------------

Edit Bash file to run on your own pc again as standard: 
------------------------------------------------------------------------------------
Write in terminal:
gedit .bashrc

Comment out the these lines you wrote earlier in the bottom of the bash file with #:
#export ROS_MASTER_URI=http://"MASTER_IP":11311/
#export ROS_HOSTNAME="YOUR_PC_IP"
#export ROS_IP="YOUR_PC_IP"

Now write these lines in the bottom of the bash file: 
export ROS_MASTER_URI=http://localhost:11311/
export ROS_HOSTNAME=localhost

Save bash file and close it, then write in terminal:
source ~/.bashrc
-------------------------------------------------------------------------------------

