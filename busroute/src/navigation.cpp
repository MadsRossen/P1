#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <getSensor1/getSensor.h>
#include <explore1/explore.h>
//#include "getSensor.h"
/*#include "auto_docking.cpp"*/

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  bool runningnav = false;
  bool runningexp = true;
  bool running = false;

  ros::init(argc, argv, "explore");
  //Calling new Sensoract class:
  SensorAct sAct;
  explore::Explore explore;
  sound_play::SoundClient sc;
while (runningexp)
{
  
  if (!explore.stopped)
  {
    
    runningexp = false;
    runningnav = true;
    running = true;
  }
  ros::spinOnce();
}
while (runningnav)
{
if (!sAct.bumper_pressed_center)
{
  ROS_INFO_STREAM("BUMPER NOT DETECTED");
}
    //tell the action client that we want to spin a thread by default
    MoveBaseClient ac("move_base", true);
     //wait for the action server to come up
    while(!ac.waitForServer(ros::Duration(5.0))){
      ROS_INFO_STREAM("Waiting for the move_base action server to come up");
    }

  move_base_msgs::MoveBaseGoal goal;

  //we'll send a goal to the robot to move 1 meter forward
  goal.target_pose.header.frame_id = "base_link";
  goal.target_pose.header.stamp = ros::Time::now();

  goal.target_pose.pose.position.x = 1.0;
  goal.target_pose.pose.position.y = 0.0;
  goal.target_pose.pose.orientation.w = 1.0;

  ROS_INFO_STREAM("Sending goal");
  ac.sendGoal(goal);
  
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
            sAct.vel.linear.x = -1.0;
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
    sAct.wheeldropped = false;

  }
  
  //ac.waitForResult();

  //If we receive result and its succeded:
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
  {
    ROS_INFO("Hooray, the base moved 1 meter forward");
    running = false;
    runningnav = false;
  } 
  //if something went wrong and it did not succed:
  if(ac.getState() == actionlib::SimpleClientGoalState::ABORTED)
  {
    ROS_INFO("The base failed to move forward 1 meter for some reason");
    running = false;  
    runningnav = false;
  }
}
}

  return 0;
}