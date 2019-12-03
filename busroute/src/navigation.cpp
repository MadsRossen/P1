#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <getSensor1/getSensor.h>
#include <explore1/explore.h>
#include <geometry_msgs/Twist.h>
#include <color/colordetect.h> 
//#include <color/color.h>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <cv_bridge/cv_bridge.h>
#include <cstdlib>
#include <string>
//#include <task1/tasks.h>
#include <nav_msgs/GetMap.h>
#include <visualization_msgs/Marker.h>
//#include <kobuki_msgs/AutoDockingAction.h>

/*
Here we do all the action. We include these packages
The getSensor library (Our own library)
The explore_lite package (Package for autonomous mapping)
The Autodocking package (Package for autonomous docking)
The MoveBase action package (Package for sending commands to movebase)
The simple_action_client package (Package for starting the action client)
*/

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

//typedef actionlib::SimpleActionClient<kobuki_msgs::AutoDockingAction> AutoDockingClient;

int main(int argc, char **argv){
  bool runningnav = false;
  bool runningexp = true;
  bool running = false;
  bool startGetMap = false;
  int map_size_i_;
  int map_size_j_;
  int j; 
  int i;
  int u=0;
  double goal_x, goal_y, x_pos, y_pos, map_res;
  bool sendEnter = false;
  bool firstobstacle = true;
  int jcount = 0;
  int countbefore = 0;
  int icount = 0;
  int x_first_ob, y_first_ob;
  
  ros::init(argc, argv, "explore");
  // Calling new Sensoract class:
  SensorAct sAct;
  //Task task;
  // Calling new explore class:
  
  // Create the soundClient:
  sound_play::SoundClient sc;
  
  //LineDetect det;
  //ImageConverter imgcon;
 //tell the clients that we want to spin a thread by default
  
  ros::NodeHandle nh;
  ros::ServiceClient GetMapClient = nh.serviceClient<nav_msgs::GetMap>("dynamic_map");
  nav_msgs::GetMap srv_map;
  explore::Explore explore;
  ros::Publisher vis_pub = nh.advertise<visualization_msgs::Marker>( "visualization_marker", 0 );

  //AutoDockingClient dc ("dock_drive_action", true);

   // Create docking goal object:
  //kobuki_msgs::AutoDockingGoal dockGoal;

  // Assign the docking initial state:
  //actionlib::SimpleClientGoalState dock_state = actionlib::SimpleClientGoalState::LOST;


//This while loop should not be necessary, but it need to be tested

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
//runningnav = true;
//running = true;
while (runningnav)
{
if (!sAct.bumper_pressed_center)
{
  ROS_INFO_STREAM("BUMPER NOT DETECTED");
}
  ROS_INFO_STREAM("TESTTEST");
    //explore.~Explore();
    MoveBaseClient ac("move_base", true);
    while(!ac.waitForServer(ros::Duration(5.0))){
      ROS_INFO_STREAM("Waiting for the move_base action server to come up");
    }
  

  ROS_INFO_STREAM("RUNNING LOOP");
     if (GetMapClient.call(srv_map))
      {
        ROS_INFO("Map service called successfully");
        map_size_i_= srv_map.response.map.info.height -1;
        map_size_j_ = srv_map.response.map.info.width -1;
        //x_pos = srv_map.response.map.info.origin.position.x;
        //y_pos = srv_map.response.map.info.origin.position.y;
        int sorted_costmap[map_size_i_][map_size_j_];
        int unsorted_costmap[map_size_i_][map_size_j_];
        std::cout << "Total Map_size_i_"<<"="<<map_size_i_<< std::endl;
        std::cout << "Total Map_size_j_"<<"="<<map_size_j_<< std::endl;

        for (i=map_size_i_; i>=0; i-- )
          {
               for (j=0; j<=map_size_j_; j++ )
               {
                    if (srv_map.response.map.data[u] == 100 && firstobstacle)
                    {
                      x_first_ob = j;
                      y_first_ob = i;
                      firstobstacle = false;
                    }
                    if (srv_map.response.map.data[u] >= 0)
                    {
                      sorted_costmap[j][i] = srv_map.response.map.data[u];    
                      //std::cout << costmap[i][j]<<",";
                      sendEnter = true;
                      jcount ++;
                    }
                    u++;
                    
                    
               }
               if (sendEnter)
               {
                 //std::cout << std::endl; 
                 sendEnter = false;
                 icount ++;
                 if (jcount > countbefore)  
                 {
                    countbefore = jcount;
                 }
                 jcount = 0;
               }
                 
          }
      std::cout << "Used Map size"<<"="<<"y:"<<icount <<","<<"x:"<<countbefore<< std::endl;
      startGetMap = false;
      }
    else
      {
        ROS_ERROR("Failed to call service GetMap");
        return 1;
      }

  move_base_msgs::MoveBaseGoal goal;
  goal_y = 1.0;
  goal_x = 1.0;
  //we'll send a goal to the robot to move 1 meter forward
  //We need to figure out what the frame_id is, base_link, /map or /odom ??
  goal.target_pose.header.frame_id = "/map";
  goal.target_pose.header.stamp = ros::Time::now();

  //goal.target_pose.pose.position.x = 0.5;
  //goal.target_pose.pose.position.y = 0.5;
  //goal.target_pose.pose.orientation.w = 1.0;
  std::cout << "x_origin"<<"="<<srv_map.response.map.info.origin.position.x<< std::endl;
  std::cout << "y_origin"<<"="<<srv_map.response.map.info.origin.position.y<< std::endl;
    if (GetMapClient.call(srv_map))
      {
        map_res = srv_map.response.map.info.resolution;
        goal.target_pose.pose.position.x =  x_first_ob * map_res + goal_x;
        goal.target_pose.pose.position.y = y_first_ob * map_res + goal_y;
        goal.target_pose.pose.orientation.w = 1.0;
      }
    else
      {
        ROS_ERROR("Failed to call service GetMap");
        return 1;
      }
  
std::cout << "First obstacle X"<<"="<<x_first_ob<< std::endl;
std::cout << "First obstacle Y"<<"="<<y_first_ob<< std::endl;
std::cout << "First obstacle X * mapres"<<"="<<x_first_ob * map_res<< std::endl;
std::cout << "First obstacle Y * mapres"<<"="<<y_first_ob * map_res<< std::endl;
std::cout << "Turtlebot pos_x"<<"="<<sAct.x_pose<< std::endl;
std::cout << "Turtlebot pos_y"<<"="<<sAct.y_pose<< std::endl;
  visualization_msgs::Marker marker;
marker.header.frame_id = "/map";
marker.header.stamp = ros::Time();
marker.ns = "my_namespace";
marker.id = 0;
marker.type = visualization_msgs::Marker::SPHERE;
marker.action = visualization_msgs::Marker::ADD;
marker.pose.position.x = x_first_ob * -1;
marker.pose.position.y = y_first_ob * -1;
marker.pose.position.z = 0;
marker.pose.orientation.x = 0.0;
marker.pose.orientation.y = 0.0;
marker.pose.orientation.z = 0.0;
marker.pose.orientation.w = 1.0;
marker.scale.x = 0.3;
marker.scale.y = 0.3;
marker.scale.z = 0.3;
marker.color.a = 1.0; // Don't forget to set the alpha!
marker.color.r = 0.0;
marker.color.g = 1.0;
marker.color.b = 0.0;
vis_pub.publish( marker );
  //https://answers.ros.org/question/197046/sending-map-co-ordinates-as-goal-to-move_base/ 
  ROS_INFO_STREAM("Sending goal");
  ac.sendGoal(goal);
  while (running)
{
  
  /*if (!det.img.empty()) {
            // Perform image processing
            det.img_filt = det.Gauss(det.img);
            det.colorthresh(det.img_filt);
            }*/
  
//If bumpers is pressed, we want to cancel goal, and get away from obstacle
//Where we then sends the goal again.
 if (sAct.bumper_pressed_center || sAct.bumper_pressed_left || sAct.bumper_pressed_right )
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
    startGetMap = true;
    
  } 
  //if something went wrong and it did not succed:
  if(ac.getState() == actionlib::SimpleClientGoalState::ABORTED)
  {
    ROS_INFO("The base failed to move forward 1 meter for some reason");
    running = false;  
    runningnav = false;
    startGetMap = true;
  }

 if (startGetMap)
  {
    
  }
ros::spinOnce(); 
}
ros::spinOnce();
}

  ROS_INFO("DONE MAIN TASK");
  return 0;

}