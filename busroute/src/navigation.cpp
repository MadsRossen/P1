#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <geometry_msgs/Twist.h>   /*subscribers=/mobile_base_nodelet_manager*/
#include "getSensor.h"
/*#include "auto_docking.cpp"*/

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

ros::Subscriber mobile_base_nodelet_manager; /*subscribers=/mobile_base_nodelet_manager   
Bruges til velosity og  af robotten link http://wiki.ros.org/kobuki/Tutorials/Kobuki%27s%20Control%20System
*/
#include "getSensor.h"

int main(int argc, char** argv){
  bool running = true;
  ros::init(argc, argv, "simple_navigation_goals");
  //Calling new Sensoract class:
  SensorAct sAct;
  sound_play::SoundClient sc;
  
  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);
  
  
  //wait for the action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO_STREAM("Waiting for the move_base action server to come up");
  }

  move_base_msgs::MoveBaseGoal goal;

  //we'll send a goal to the robot to move 1 meter forward
  goal.target_pose.header.frame_id = "odom";
  goal.target_pose.header.stamp = ros::Time::now();

  goal.target_pose.pose.position.x = 3.0;
  goal.target_pose.pose.orientation.w = 1.0;


  ROS_INFO_STREAM("Sending goal");
  ac.sendGoal(goal);
  
if (!sAct.bumper_pressed_center)
{
  ROS_INFO_STREAM("BUMPER NOT DETECTED");
}
while (running)
{
ros::spinOnce();
//If bumpers is pressed, we want to cancel goal, and get away from obstacle
//Where we then sends the goal again.
 if (sAct.bumper_pressed_center || sAct.bumper_pressed_left || sAct.bumper_pressed_right)
     {
       ac.cancelGoal();    
       ROS_INFO_STREAM("BUMPER DETECTED");
          for (int i=0; i<5; i++)
          {
            sAct.vel.linear.x = -0.5;
            sAct.vel.angular.z = 0.0;
            sAct.cmd_vel_pub.publish(sAct.vel);
            ROS_INFO_STREAM("BACKWARDS" << i);
            ros::Duration(0.1).sleep();
          }
          for(int i=0; i<45; i++)
          {
            sAct.vel.linear.x = 0.0;
            sAct.vel.angular.z = 1.0;
            sAct.cmd_vel_pub.publish(sAct.vel);
            ROS_INFO_STREAM("ROTATING " << i);
            ros::Duration(0.1).sleep();
          }
          sAct.bumper_pressed_center = false;
          sAct.bumper_pressed_left = false;
          sAct.bumper_pressed_right = false;
          //ms.vel.angular.z = 4.0;
          ac.sendGoal(goal);
     }
//If turtlebot is tilted we want to cancel goal and send warning sound.
  if (sAct.wheeldropped)
  {
    sc.playWave("/home/ubu/ws/src/P1/busroute/sounds/Reee.wav", 1.0);
    ac.cancelGoal(); 
    ROS_INFO("Turtlebot is being lifted or tilted! Goal canceled. Heading back to docking station");
    //sAct.wheeldropped = false;
    //sAct.wheeldropped_left = false;
    //sAct.wheeldropped_right = false;
    /*while (sAct.wheeldropped || sAct.wheeldropped_left || sAct.wheeldropped_right){   
    }*/
    sAct.wheeldropped = false;
    
  }
  
  //ac.waitForResult();

  //If we receive result and its succeded:
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
  {
    ROS_INFO("Hooray, the base moved 1 meter forward");
    running = false;
  } 
  //if something went wrong and it did not succed:
  if(ac.getState() == actionlib::SimpleClientGoalState::ABORTED)
  {
    ROS_INFO("The base failed to move forward 1 meter for some reason");
    running = false;  
  }
}

  return 0;
}