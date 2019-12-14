#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <getSensor1/getSensor.h>
#include <explore1/explore.h>
#include <geometry_msgs/Twist.h>
//#include <color/colordetect.h> 
#include <color/color.h>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <cv_bridge/cv_bridge.h>
#include <cstdlib>
#include <string>
//#include <task1/tasks.h>
#include <nav_msgs/GetMap.h>
#include <visualization_msgs/Marker.h>
#include <kobuki_msgs/AutoDockingAction.h>
#include "ros/callback_queue.h"
#include "std_msgs/String.h"
#include <iostream>
#include <actionlib/client/simple_client_goal_state.h>
#include <sound_play/sound_play.h>

/*
Here we do all the action. We include these packages
The getSensor library (Our own library)
The explore_lite package (Package for autonomous mapping)
The Autodocking package (Package for autonomous docking)
The MoveBase action package (Package for sending commands to movebase)
The simple_action_client package (Package for starting the action client)
*/

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

typedef actionlib::SimpleActionClient<kobuki_msgs::AutoDockingAction> AutoDockingClient;

/*int exploremapping(int returnerValue)
{
  if(ros::ok)
  {  
    if (ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME,
                                ros::console::levels::Debug)) {
      ros::console::notifyLoggerLevelsChanged();
      }
      explore::Explore explore;
      ros::spin();
      //ros::spinOnce();
      //r.sleep();
  }
  return returnerValue + 1;
}*/
 

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
  int runner = 1;

 
  /*ros::init(argc, argv, "explore");
  int runnner = exploremapping(1);
  std::cout << "runner"<<"="<<runnner<< std::endl;
  //ros::shutdown();
*/
  ros::init(argc, argv, "cleaner");

  // Calling new Sensoract class:
  SensorAct sAct;
  // Create the soundClient:
  sound_play::SoundClient sc;
  ImageConverter imgc;
  ros::NodeHandle nh;
  ros::ServiceClient GetMapClient = nh.serviceClient<nav_msgs::GetMap>("dynamic_map");
  nav_msgs::GetMap srv_map;
  ros::Publisher vis_pub = nh.advertise<visualization_msgs::Marker>( "visualization_marker", 0 );
  visualization_msgs::Marker marker;
  ros::Rate loop_rate(50);
  

  AutoDockingClient dc ("dock_drive_action", true);

   // Create docking goal object:
  kobuki_msgs::AutoDockingGoal dockGoal;

  // Assign the docking initial state:
  actionlib::SimpleClientGoalState dock_state = actionlib::SimpleClientGoalState::LOST;

  float x_InitialPose = sAct.x_currentPose;
  float y_InitialPose = sAct.y_currentPose;

  while (ros::ok)
  {
    while (sAct.runner == 2)
    {
    //tell the clients that we want to spin a thread by default
      MoveBaseClient ac("move_base", true);
    //Wait until the server has come up
      while(!ac.waitForServer(ros::Duration(5.0))){
        ROS_INFO_STREAM("Waiting for the move_base action server to come up");
      }
      move_base_msgs::MoveBaseGoal goal;
    //Reading the costmap and getting size of the mapped area.
      ROS_INFO_STREAM("RUNNING LOOP");
        if (GetMapClient.call(srv_map))
          {
            ROS_INFO("Map service called successfully");
            //Get the size of the costmap
            map_size_i_= srv_map.response.map.info.width -1;
            map_size_j_ = srv_map.response.map.info.height -1;
            //x_pos = srv_map.response.map.info.origin.position.x;
            //y_pos = srv_map.response.map.info.origin.position.y;
            int sorted_costmap[map_size_i_][map_size_j_];
            int unsorted_costmap[map_size_i_][map_size_j_];
            //Output costmap size
            std::cout << "Total Map_size_i_"<<"="<<map_size_i_<< std::endl;
            std::cout << "Total Map_size_j_"<<"="<<map_size_j_<< std::endl;

            //Running the loop to get costmap data;
            for (j=0; j<=map_size_j_; j++ )
              {
                  for (i=0; i<=map_size_i_; i++ )
                  {
                        if (srv_map.response.map.data[u] == 100 && firstobstacle)
                        {
                          x_first_ob = i;
                          y_first_ob = j;
                          
                          firstobstacle = false;
                        }
                        if (srv_map.response.map.data[u] >= 0)
                        {
                          sorted_costmap[i][j] = srv_map.response.map.data[u];    
                          //std::cout << costmap[i][j]<<",";
                          sendEnter = true;
                          icount ++;
                        }
                        u++;      
                  }
                  if (sendEnter)
                  {
                    //std::cout << std::endl; 
                    sendEnter = false;
                    jcount ++;
                    if (icount > countbefore)  
                    {
                        countbefore = icount;
                    }
                    icount = 0;
                  }             
              }
          std::cout << "Used Map size"<<"="<<"y:"<<jcount <<","<<"x:"<<countbefore<< std::endl;
          startGetMap = false;
          }
        else
          {
            ROS_ERROR("Failed to call service GetMap");
            return 1;
          }
    //Starting the pathplanner:
      std::cout << "First obstacle X"<<"="<<x_first_ob<< std::endl;
      std::cout << "First obstacle Y"<<"="<<y_first_ob<< std::endl;
      
      map_res = srv_map.response.map.info.resolution;
    //Variables for the first obstacle detected from lower left corner of costmap.
      double x_firstObst_pos = x_first_ob * map_res + srv_map.response.map.info.origin.position.x;
      std::cout << "x_firstObst_pos"<<"="<<x_firstObst_pos<< std::endl;
      double y_firstObst_pos = y_first_ob * map_res + srv_map.response.map.info.origin.position.y;
      std::cout << "y_firstObst_pos"<<"="<<y_firstObst_pos<< std::endl;

    //Variables for the mapsize of the costmap with mapped data converted to meter.
      double y_mappedsize = jcount * map_res;
      double x_mappedsize = countbefore * map_res;

    //Variables for the Origin of the costmap (Lower left corner) in relation to the coordinate system.
      double x_origin = srv_map.response.map.info.origin.position.x;
      std::cout << "x_origin"<<"="<<x_origin<< std::endl;
      double y_origin = srv_map.response.map.info.origin.position.y;
      std::cout << "y_origin"<<"="<<y_origin<< std::endl;

    //Bool for running the pathplanner loop.
      bool runPathPlanner = true;
    //Variables for the size of the mapped costmap with a safety margin for the turtlebot.
      double X_MAX = x_mappedsize - 1.3; 
      double Y_MIN = y_firstObst_pos + 1.2;
      double X_MIN = x_firstObst_pos + 0.5;
    //Variable for the Right upper corner with a safety margin for the turtlebot.
      double r_U_C = y_mappedsize + y_firstObst_pos - 0.7;

      double jj = r_U_C;
      std::cout << "r_U_C"<<"="<<x_InitialPose<< std::endl;
      double ii = 0.5;
      int turn = 2;
    //Bool for running goalState loop.
      bool goalreached = false;
      bool colordetected = false;
      bool runme = true; 

      std::cout << "Turtlebot pos_x"<<"="<<x_InitialPose<< std::endl;
      std::cout << "Turtlebot pos_y"<<"="<<y_InitialPose<< std::endl;
    //Starting the pathplanner loop.
      while(runPathPlanner)
      {
      //Sending Goal;
        goal.target_pose.header.frame_id = "/map";
        goal.target_pose.header.stamp = ros::Time::now();
        goal.target_pose.pose.position.x = ii;
        goal.target_pose.pose.position.y = jj;
        goal.target_pose.pose.orientation.w = 1.0;
        ROS_INFO_STREAM("Sending goal");
        goalreached = false;
        ac.sendGoal(goal);
        std::cout << "Turtlebot goal i"<<"="<<ii<< std::endl;
        std::cout << "Turtlebot goal j"<<"="<<jj<< std::endl;
        std::cout << "X_MAX"<<"="<<X_MAX<< std::endl;
        std::cout << "Y_MIN"<<"="<<Y_MIN<< std::endl;
        std::cout << "X_MIN"<<"="<<X_MIN<< std::endl;
        std::cout << "Turn"<<"="<<turn<< std::endl;
      //Sending marker for current goal
        marker.header.frame_id = "/map";
        marker.header.stamp = ros::Time();
        marker.ns = "Goal objective";
        marker.id = 0;
        marker.type = visualization_msgs::Marker::SPHERE;
        marker.action = visualization_msgs::Marker::ADD;        
        marker.pose.position.x = ii;
        marker.pose.position.y = jj;
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
      //running new goal looper
        while (!goalreached)
        {
        //If turtlebot is being lifted.
          if (sAct.wheeldropped)
          {
            sc.playWave("/home/ubu/ws/src/P1/busroute/sounds/Reee.wav", 1.0);
            ROS_INFO("Turtlebot is being lifted or tilted! Goal canceled. Heading back to docking station");
            ac.cancelAllGoals();
            runPathPlanner = false; 
            runningnav = false;
            goalreached = true;
            ros::Duration(2.0).sleep();
          }
        //Blue color detected
          if (imgc.trashDetected_blue)
          {
            ac.cancelGoal();
            std::cout << "Blue object detected"<< std::endl;
            colordetected = true;
            sc.playWave("/home/ubu/ws/src/P1/busroute/sounds/blue.wav", 1.0);
            ros::Duration(2.0).sleep();
            
          }
        //Green color detected
          if (imgc.trashDetected_green)
          {
            ac.cancelGoal();
            std::cout << "Green object detected"<< std::endl;
            colordetected = true;
            sc.playWave("/home/ubu/ws/src/P1/busroute/sounds/green.wav", 1.0);
            ros::Duration(2.0).sleep();
            
          }
        //Red color detected
          if (imgc.trashDetected_red)
          {
            ac.cancelGoal();
            std::cout << "Red object detected"<< std::endl;
            colordetected = true;
            sc.playWave("/home/ubu/ws/src/P1/busroute/sounds/red.wav", 1.0);
            ros::Duration(2.0).sleep();
            
          }
          if (sAct.button1Pres == true)
          {
            ac.cancelAllGoals();
            runPathPlanner = false; 
            runningnav = false;
            goalreached = true;
            std::cout << "Battery low, ending task "<< std::endl;
          }
        //If color have been detected, but no more.
          if(!imgc.trashDetected_blue && !imgc.trashDetected_green && !imgc.trashDetected_red && colordetected)
          {
            ac.sendGoal(goal);
            std::cout << "Resending goal after color detected"<< std::endl;
            colordetected = false;
          }
        //if something went wrong and it did not succed:
          if(ac.getState() == actionlib::SimpleClientGoalState::ABORTED)
          { 
            ROS_INFO("The base failed to archive the goal");
            ac.cancelGoal();
            ac.sendGoal(goal);
            vis_pub.publish( marker );
          }
      
        //If we receive result and its succeded:
          if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
          {
            std::cout << "Reached Goal destination"<< std::endl;
            goalreached = true;
            if (jj <= Y_MIN)
            {
              if (ii >= X_MAX)
              {
                runPathPlanner = false; 
                runningnav = false;
                std::cout << "If1.1"<< std::endl;
              }
              else
              {
                ii = X_MAX;
              //ROS_INFO("If1.2");
                std::cout << "If1.2"<< std::endl;
              } 
            }
            else 
            {
              if (ii >= X_MAX)
              {
                if (turn == 1)
                {
                  ii = X_MAX;
                  jj = jj - 0.5;
                  turn = 2;
                //ROS_INFO("If2.1");
                  std::cout << "If2.1"<< std::endl;
                }
                else
                {
                  ii = 0.5;
                  turn = 1;
                //ROS_INFO("If2.2");
                  std::cout << "If2.2"<< std::endl;
                }
              }
              else
              {
                if (turn == 1)
                {
                  ii = 0.5;
                  jj = jj - 0.5;
                  turn = 2;
                  //ROS_INFO("If3.1");
                  std::cout << "If3.1"<< std::endl;
                }
                else
                {
                  ii = X_MAX;
                  turn = 1;
                  //ROS_INFO("If3.2");
                  std::cout << "If3.2"<< std::endl;
                }
              }
            }
          }//End if succeded
          ros::spinOnce();
          loop_rate.sleep();  
        }//End new goal looper
        ros::spinOnce();
        loop_rate.sleep();  
      }//End pathplanner loop

    //Return to docking station:
      bool dockingreached = false;
      goal.target_pose.header.frame_id = "/map";
        goal.target_pose.header.stamp = ros::Time::now();
        goal.target_pose.pose.position.x = 0.4;
        goal.target_pose.pose.position.y = 0;
        goal.target_pose.pose.orientation.w = 1.0;
        ROS_INFO("Sending docking goal");
        ac.sendGoal(goal);
      while (!dockingreached)
      {
        
        
        //ac.waitForResult();
        if(ac.getState() == actionlib::SimpleClientGoalState::ABORTED)
          { 
            ROS_INFO("The base failed to archive the docking goal");
            ac.cancelGoal();
            ac.sendGoal(goal);
          }
        //If we receive result and its succeded:
        if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
          {
            std::cout << "Reached docking Goal destination"<< std::endl;
            dockingreached = true;
          }
        ros::spinOnce();
        loop_rate.sleep(); 
      }
    //Start Auto Docking process:
      dc.sendGoal(dockGoal);
      ros::Time time = ros::Time::now();
      // Monitor progress
      while (!dc.waitForResult(ros::Duration(3))) 
      {
        dock_state = dc.getState();
        ROS_INFO("Docking status: %s",dock_state.toString().c_str());

        if (ros::Time::now() > (time+ros::Duration(1000))) 
        {
          ROS_INFO("Docking took more than 1000 seconds, canceling.");
          dc.cancelGoal();
          break;
        }// end if
      }// end while
      if (dc.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      {
        ROS_INFO("Completed docking succesfully");
        ros::shutdown();
      }
      ros::spinOnce();
      loop_rate.sleep();
    }//End sAct.runner loop
      ros::spinOnce();
      loop_rate.sleep();
  }//Ending last loop
  ROS_INFO("DONE MAIN TASK");
  return 0;

}