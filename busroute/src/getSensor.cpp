#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <iostream>
#include <kobuki_msgs/BumperEvent.h>
#include <kobuki_msgs/CliffEvent.h>
#include <kobuki_msgs/WheelDropEvent.h>
#include <kobuki_msgs/DigitalInputEvent.h>
#include <kobuki_msgs/PowerSystemEvent.h>
#include <kobuki_msgs/Sound.h>
#include <ros/console.h>
#include <std_msgs/String.h>
#include <sound_play/sound_play.h>
#include <unistd.h>
#include <time.h>
#include <getSensor1/getSensor.h>
#include <nav_msgs/OccupancyGrid.h>
#include <nav_msgs/MapMetaData.h>
#include "ros/callback_queue.h"
#include "std_msgs/String.h"
//#include "getSensor.h"

/*
In this file we retrieve the output from the different subsribers
we declared in getSensor.h and act on it as we want.
*/

//Checking the powerstate of the battery:
void SensorAct::powerSystemCB(const kobuki_msgs::PowerSystemEventConstPtr msg)
{
     if (msg->event == kobuki_msgs::PowerSystemEvent::BATTERY_LOW)
     {
          ROS_INFO_STREAM("BATTERY LOW");
     }
     if (msg->event == kobuki_msgs::PowerSystemEvent::BATTERY_CRITICAL)
     {
         ROS_INFO_STREAM("BATTERY CRITICAL LOW"); 
     }

}

//Checking if bumper is pressed
void SensorAct::bumperEventCB(const kobuki_msgs::BumperEventConstPtr msg)
{
     
    //If bumper is pressed:
     if (msg->state == kobuki_msgs::BumperEvent::PRESSED)
     {    
          ROS_INFO_STREAM("Bumper PRESSED");
          switch (msg->bumper)
          {
               case kobuki_msgs::BumperEvent::LEFT:
                    if (!bumper_pressed_left)
                    {
                         ROS_INFO_STREAM("Bumper: LEFT");
                         bumper_pressed_left = true;
                         
                    }           
                    break;
               case kobuki_msgs::BumperEvent::CENTER:
                    if (!bumper_pressed_center)
                    {
                    ROS_INFO_STREAM("Bumper: CENTER");
                    bumper_pressed_center = true;
                    
                    }
                    break;
               case kobuki_msgs::BumperEvent::RIGHT:
                    if (!bumper_pressed_right)
                    {
                    ROS_INFO_STREAM("Bumper: RIGHT");
                    bumper_pressed_right = true;
                    
                    }
                    break;
    }
          //Play file wav for p1-ros machine:
          //sc.playWave("/home/p1-ros/ws/src/P1/busroute/sounds/Ouch.wav", 1.0);

          //Play wav file for ubu machine:
          int nrSound = rand() %2 + 1;
          if(nrSound == 1)
          {
               sc.playWave("/home/ubu/ws/src/P1/busroute/sounds/Ouch.wav", 1.0);
          }
          else
          {
               sc.playWave("/home/ubu/ws/src/P1/busroute/sounds/roblox.wav", 1.0);
          } 
     }
     //Else we do this:
     else
     {
          ROS_INFO_STREAM("Bumper RELEASED");
          switch (msg->bumper)
          {
               case kobuki_msgs::BumperEvent::LEFT:    bumper_pressed_left   = false; break;
               case kobuki_msgs::BumperEvent::CENTER:  bumper_pressed_center = false; break;
               case kobuki_msgs::BumperEvent::RIGHT:   bumper_pressed_right  = false; break;
          }
     }
     
 
}
//Checking if cliff have been registered:
void SensorAct::cliffEventCB(const kobuki_msgs::CliffEventConstPtr msg)
{
     //if cliff have been registered:
    if (msg->state == kobuki_msgs::CliffEvent::CLIFF)
    {
         switch (msg->sensor)
         {
          case kobuki_msgs::CliffEvent::LEFT:
          if (!cliffDetected_left)
          {
               ROS_INFO_STREAM("Cliff: LEFT");
               cliffDetected_left = true;
          }
          break;
          case kobuki_msgs::CliffEvent::RIGHT:
          if (!cliffDetected_right)
          {
               ROS_INFO_STREAM("cliff: RIGHT");
               cliffDetected_right = true;
          }
          break;
          case kobuki_msgs::CliffEvent::CENTER:
          if (!cliffDetected_center)
          {
               ROS_INFO_STREAM("Cliff: CENTER");
               cliffDetected_center = true;
          }
          break;
         }
    }
     else // kobuki_msgs::BumperEvent::FLOOR
     {
     switch (msg->sensor)
     {
      case kobuki_msgs::CliffEvent::LEFT:    cliffDetected_left   = false; break;
      case kobuki_msgs::CliffEvent::CENTER:  cliffDetected_center = false; break;
      case kobuki_msgs::CliffEvent::RIGHT:   cliffDetected_right  = false; break;
     }
  }   

    //If not then we do this:
    if (msg->state == kobuki_msgs::CliffEvent::FLOOR)
    {
        ROS_INFO_STREAM("DETECTED FLOOR");
        cliffDetected_floor = true;
    }
    

}
//Checking if wheelsensors are getting a input
void SensorAct::wheeldropEventCB(const kobuki_msgs::WheelDropEventConstPtr msg)
{    
     //If turtlebot wheels is dropped:
     if (msg->state == kobuki_msgs::WheelDropEvent::DROPPED)
     {
          /*switch (msg->wheel)
          {
               case kobuki_msgs::WheelDropEvent::LEFT:
                    if(!wheeldropped_left)
                    {
                         ROS_WARN("left wheel dropped");
                         wheeldropped_left = true;
                    }
                    break;

               case kobuki_msgs::WheelDropEvent::RIGHT:
                    if(!wheeldropped_right)
                    {
                         ROS_WARN("right wheel dropped");
                         wheeldropped_right = true;
                    }
                    break;
               default:
               if(!wheeldropped)
                    {
                         ROS_WARN("both wheels dropped");
                         wheeldropped = true;
                    }
                    break;
          }*/
          wheeldropped = true;
          
     }
   
     /*
     if (msg->state == kobuki_msgs::WheelDropEvent::DROPPED)
     {
          ROS_WARN("both wheels dropped");
          wheeldropped_left =true;
          wheeldropped_right = true;
     }*/
     

     //If turtlebot wheels is raised:
     if (msg->state == kobuki_msgs::WheelDropEvent::RAISED)
     {
          ROS_INFO_STREAM("WHEELS RAISED");
          wheeldropped = false;
          
     }
}  

//A sleep function not curently being used:
void sleepok(int t, ros::NodeHandle &nh_)
{
     if(nh_.ok())
     {
          sleep(t);    
     }
}

void SensorAct::digitalInputCB(const kobuki_msgs::DigitalInputEventConstPtr msg)
{
     if (msg->values[1] == true)
     {
          ROS_INFO_STREAM("INPUT FROM SENSOR 1");
     }
     if (msg->values[2] == true)
     {
          ROS_INFO_STREAM("INPUT FROM SENSOR 2");
     }
     if (msg->values[3] == true)
     {
          ROS_INFO_STREAM("INPUT FROM SENSOR 3");
     }
     if (msg->values[0] == true)
     {
          ROS_INFO_STREAM("INPUT FROM SENSOR 0");
     }
}

void SensorAct::robotPoseCB(const geometry_msgs::PoseConstPtr msg)
{    /*if (poseFirstrun)
     {
          x_InitialPose = msg->position.x;
          y_InitialPose = msg->position.y;
          poseFirstrun = false;
     }
     else
     {
          
     }*/
     x_currentPose = msg->position.x;
     y_currentPose = msg->position.y;
     
     
     
}

void SensorAct::callback(const std_msgs::String::ConstPtr& msg)
  {
    //std::cout << "I heard: [" << msg->data << "]" << std::endl;
    //ROS_INFO_STREAM(msg->data);
    if (msg->data == "running")
    {
      runner = 1;
    }
    if (msg->data == "stopped")
    {
      runner = 2;
    }
  }
void SensorAct::buttonCB(const kobuki_msgs::ButtonEventConstPtr msg)
{
     if (msg->Button1 == kobuki_msgs::ButtonEvent::PRESSED)
     {
          button1Pres = true;  
     }
     else
     {
          button1Pres = false;
     }
     

}


