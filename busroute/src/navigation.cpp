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

int exploremapping(int returnerValue, bool runningexp)
  {
      if (ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME,
                                        ros::console::levels::Debug)) {
        ros::console::notifyLoggerLevelsChanged();
      }
      explore::Explore explore;
      ros::spin();
      //ros::spinOnce();
      //r.sleep();
    return returnerValue + 1;
  }

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
  
  ros::NodeHandle nh;
  ros::ServiceClient GetMapClient = nh.serviceClient<nav_msgs::GetMap>("dynamic_map");
  nav_msgs::GetMap srv_map;
  ros::Publisher vis_pub = nh.advertise<visualization_msgs::Marker>( "visualization_marker", 0 );
  visualization_msgs::Marker marker;
  
  ros::MultiThreadedSpinner spinner(4);
  //AutoDockingClient dc ("dock_drive_action", true);

   // Create docking goal object:
  //kobuki_msgs::AutoDockingGoal dockGoal;

  // Assign the docking initial state:
  //actionlib::SimpleClientGoalState dock_state = actionlib::SimpleClientGoalState::LOST;

float x_InitialPose = sAct.x_currentPose;
float y_InitialPose = sAct.y_currentPose;
ros::Rate r(25);

//runningnav = true;
//running = true;
while (runningnav)
{
  //tell the clients that we want to spin a thread by default
    MoveBaseClient ac("move_base", true);
  //Wait until the server has come up
    while(!ac.waitForServer(ros::Duration(5.0))){
      ROS_INFO_STREAM("Waiting for the move_base action server to come up");
    }
  
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
  move_base_msgs::MoveBaseGoal goal;

  map_res = srv_map.response.map.info.resolution;
  //Variables for the first obstacle detected from lower left corner of costmap.
  double x_firstObst_pos = x_first_ob * map_res + srv_map.response.map.info.origin.position.x;
  std::cout << "x_firstIbst_pos"<<"="<<x_firstObst_pos<< std::endl;
  double y_firstObst_pos = y_first_ob * map_res + srv_map.response.map.info.origin.position.y;
  std::cout << "y_firstObst_pos"<<"="<<y_firstObst_pos<< std::endl;

  //Variables for the mapsize of the costmap with mapped data converted to meter.
  double y_mappedsize = jcount * map_res;
  double x_mappedsize = countbefore * map_res;

  //Variables for the Origin of the costmap (0,0) (Lower left corner)
  double x_origin = srv_map.response.map.info.origin.position.x;
  std::cout << "x_origin"<<"="<<x_origin<< std::endl;
  double y_origin = srv_map.response.map.info.origin.position.y;
  std::cout << "y_origin"<<"="<<y_origin<< std::endl;

  //Bool for running the pathplanner loop.
  bool runPathPlanner = true;
  //Variables for the size of the mapped costmap with a safety margin for the turtlebot.
  double X_MAX = x_mappedsize - 0.5; 
  double Y_MIN = y_firstObst_pos + 0.5;
  //Variable for the Right upper corner with a safety margin for the turtlebot.
  double r_U_C = y_mappedsize + y_firstObst_pos - 0.5;

  double jj = r_U_C;
  std::cout << "r_U_C"<<"="<<x_InitialPose<< std::endl;
  double ii = 0.5;
  int turn = 1;
  //Bool for running goalState loop.
  bool goalreached = false; 

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
    while (!goalreached)
    {
      //If turtlebot is being lifted = abort the goal.
      if (sAct.wheeldropped)
      {
        sc.playWave("/home/ubu/ws/src/P1/busroute/sounds/Reee.wav", 1.0);
        ac.cancelGoal(); 
        ROS_INFO("Turtlebot is being lifted or tilted! Goal canceled. Heading back to docking station");
        sAct.wheeldropped = false;
      }
  
      //If we receive result and its succeded:
      if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      {
        ROS_INFO("Reached Goal destination!");
        goalreached = true;
        if (jj <= Y_MIN)
        {
          if (ii >= X_MAX)
          {
            runPathPlanner = false; 
            runningnav = false; 
          }
          else
          {
            ii = X_MAX;
          } 
        }
        else 
        {
          if (ii >= X_MAX)
          {
            if (turn = 1)
            {
              ii = X_MAX;
              jj = r_U_C - 0.5;
              turn = 2;
            }
            else
            {
              ii = 0.5;
              turn = 1;
            }
          }
          else
          {
            if (turn = 1)
            {
              ii = 0.5;
              jj = r_U_C -0.5;
              turn = 2;
            }
            else
            {
              ii = X_MAX;
              turn = 1;
            }
          }
        }
      } 
      //if something went wrong and it did not succed:
      if(ac.getState() == actionlib::SimpleClientGoalState::ABORTED)
      { 
        ROS_INFO("The base failed to archive the goal");
        ac.cancelAllGoals();
        ac.sendGoal(goal);
        vis_pub.publish( marker );
      }
    }
    //Algorithm for defining new goal in the mapped area. 
  }
  //https://answers.ros.org/question/197046/sending-map-co-ordinates-as-goal-to-move_base/ 
  ros::spin();
}

  ROS_INFO("DONE MAIN TASK");
  return 0;

}