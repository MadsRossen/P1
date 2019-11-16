#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/simple_client_goal_state.h>
#include <kobuki_msgs/AutoDockingAction.h>
#include <move_base_msgs/MoveBaseAction.h>

 typedef actionlib::SimpleActionClient<kobuki_msgs::AutoDockingAction> AutoDockingClient;

int main(int argc, char** argv){
    ros::init(argc, argv, "auto_docking");
  // Create the client
    AutoDockingClient dc ("dock_drive_action", true);

  while(!dc.waitForServer(ros::Duration(5.0)))
  {
      ROS_INFO_STREAM("Waiting for the autodocking action server to come up");
  }
  

  // Create goal object
  kobuki_msgs::AutoDockingGoal goal;

  // Assign initial state
  actionlib::SimpleClientGoalState dock_state = actionlib::SimpleClientGoalState::LOST;

  // Send the goal
  dc.sendGoal(goal);

  ros::Time time = ros::Time::now();

  // Monitor progress
  while (!dc.waitForResult(ros::Duration(3))) {

    dock_state = dc.getState();
    ROS_INFO("Docking status: %s",dock_state.toString().c_str());

    if (ros::Time::now() > (time+ros::Duration(1000))) {
      ROS_INFO("Docking took more than 10 seconds, canceling.");
      dc.cancelGoal();
      break;
    }// end if
  }// end while

  return 0;
}