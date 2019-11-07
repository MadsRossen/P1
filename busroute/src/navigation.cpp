#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include "getSensor.h"
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  ros::init(argc, argv, "simple_navigation_goals");
  //Calling new Sensoract class:
  SensorAct sAct;
  
  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);
  
  
  //wait for the action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  move_base_msgs::MoveBaseGoal goal;

  //we'll send a goal to the robot to move 1 meter forward
  goal.target_pose.header.frame_id = "base_link";
  goal.target_pose.header.stamp = ros::Time::now();

  goal.target_pose.pose.position.x = 1.0;
  goal.target_pose.pose.orientation.w = 1.0;


  ROS_INFO("Sending goal");
  ac.sendGoal(goal);
 if (sAct.bumper_pressed_center || sAct.bumper_pressed_left || sAct.bumper_pressed_right)
     {    
          for (int i=0; i<5; i++)
          {
            sAct.vel.linear.x = -0.0;
            sAct.vel.angular.z = 0.0;
            sAct.cmd_vel_pub.publish(sAct.vel);
            ROS_INFO_STREAM("BACKWARDS" << i);
            ros::Duration(0.1).sleep();
          }
          for(int i=0; i<45; i++)
          {
            sAct.vel.linear.x = 0.0;
            sAct.vel.angular.z = 0.0;
            sAct.cmd_vel_pub.publish(sAct.vel);
            ROS_INFO_STREAM("ROTATING " << i);
            ros::Duration(0.1).sleep();
          }
          sAct.bumper_pressed_center = false;
          sAct.bumper_pressed_left = false;
          sAct.bumper_pressed_right = false;
          //ms.vel.angular.z = 4.0;
          
     }

  if (sAct.wheeldropped || sAct.wheeldropped_left || sAct.wheeldropped_right)
  {
    ac.cancelGoal();
    ROS_INFO("Turtlebot is being lifted or tilted! Goal canceled");

  }
  ac.waitForResult();

  //If we receive result and its succeded:
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    ROS_INFO("Hooray, the base moved 1 meter forward");
  //if something went wrong and it did not succed:
  else
    ROS_INFO("The base failed to move forward 1 meter for some reason");
    

  return 0;
}