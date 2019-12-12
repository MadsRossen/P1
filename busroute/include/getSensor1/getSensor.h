#ifndef __GETSENSOR_H
#define __GETSENSOR_H

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <iostream>
#include <kobuki_msgs/BumperEvent.h>
#include <kobuki_msgs/CliffEvent.h>
#include <kobuki_msgs/WheelDropEvent.h>
#include <kobuki_msgs/DigitalInputEvent.h>
#include <kobuki_msgs/PowerSystemEvent.h>
#include <kobuki_msgs/ButtonEvent.h>
#include <kobuki_msgs/Sound.h>
#include <ros/console.h>
#include <std_msgs/String.h>
#include <sound_play/sound_play.h>
#include <unistd.h>
#include <time.h>
#include <nav_msgs/MapMetaData.h>
#include <nav_msgs/OccupancyGrid.h>
#include "std_msgs/String.h"

/*
Here we declare the necessary subscribers
publishers, nodehandlers and variables that we will use in getSensor.cpp  
*/

//Class for all the functions:
class SensorAct
{
  
//initializing public variables:
public:
SensorAct() :   
//Declaring all booleans to be false at start.
pressedBump(false),
bumper_pressed_left(false),
bumper_pressed_center(false),
bumper_pressed_right(false),
cliffDetected_left(false),
cliffDetected_right(false),
cliffDetected_center(false),
cliffDetected_floor(false),
wheeldropped(false),
wheeldropped_right(false),
wheeldropped_left(false),
poseFirstrun(true),
button1Pres(false)

{
//Initializing subscribers and publishers:
bumper_event_subscriber_ = nh_.subscribe("mobile_base/events/bumper", 10, &SensorAct::bumperEventCB, this);
cliff_event_subscriber_ = nh_.subscribe("mobile_base/events/cliff", 10, &SensorAct::cliffEventCB, this);
wheel_event_subscriber_ = nh_.subscribe("mobile_base/events/wheel_drop", 10, &SensorAct::wheeldropEventCB, this);
digitalInput_event_subsriber_ = nh_.subscribe("mobile_base/events/digital_input", 10, &SensorAct::digitalInputCB, this);
powerSystem_event_subscriber_ = nh_.subscribe("mobile_base/events/power_system", 10, &SensorAct::powerSystemCB, this);
cmd_vel_pub = nh_.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop", 10);
cmd_sound_pub = nh_.advertise<kobuki_msgs::Sound>("mobile_base/commands/sound", 10);
robot_pose_subscriber_ = nh_.subscribe("robot_pose", 10 ,&SensorAct::robotPoseCB, this);
subscriber = nh_.subscribe("hello_publisher", 1, &SensorAct::callback, this);
button_subscriber_ = nh_.subscribe("mobile_base/events/button", 1, &SensorAct::buttonCB, this);
}

~SensorAct()
  {
       
  };
//Declaring public and private variables and nodehanlders:
public:
//Booleans for use with bumpers:
bool pressedBump;
bool bumper_pressed_left;
bool bumper_pressed_center;
bool bumper_pressed_right;
//Booleans for use with cliff sensors:
bool cliffDetected_right;
bool cliffDetected_center;
bool cliffDetected_left;
bool cliffDetected_floor;
//Booleans for use with wheel drop sensors:
bool wheeldropped_right;
bool wheeldropped_left;
bool wheeldropped;
// Variables used for pose:
float x_InitialPose, y_InitialPose , x_currentPose, y_currentPose;
bool poseFirstrun;
int runner;
bool button1Pres;


geometry_msgs::Twist vel;
kobuki_msgs::Sound smsg;
ros::Publisher cmd_vel_pub;
ros::NodeHandle nh_;
private:
sound_play::SoundClient sc;
ros::Subscriber bumper_event_subscriber_;
ros::Subscriber cliff_event_subscriber_;
ros::Subscriber digitalInput_event_subsriber_;
ros::Subscriber wheel_event_subscriber_;
ros::Subscriber powerSystem_event_subscriber_;
ros::Publisher cmd_sound_pub;
ros::Subscriber robot_pose_subscriber_;
ros::Subscriber subscriber;
ros::Subscriber button_subscriber_;




/**
   * @brief Trigger boolean when a bumper is pressed
   * @param msg bumper event
   */
    void bumperEventCB(const kobuki_msgs::BumperEventConstPtr msg);

/**
    * @brief Trigger boolean when cliff is detected
    * @param msg cliff event
    */
    void cliffEventCB(const kobuki_msgs::CliffEventConstPtr msg);
/**
 * @brief Trigger boolean when turtlebot is being lifted
 * @param msg wheel lift event
 */
     void wheeldropEventCB(const kobuki_msgs::WheelDropEventConstPtr msg);
/**
 * @brief Trigger boolean when turtlebot is getting sensordata
 * @param msg sensor event
 */
     void digitalInputCB(const kobuki_msgs::DigitalInputEventConstPtr msg);
/**
 * @brief Get boolean when battery is low (15%) or critical low (5%)
 * @param msg 
 */
     void powerSystemCB(const kobuki_msgs::PowerSystemEventConstPtr msg);

    void robotPoseCB(const geometry_msgs::PoseConstPtr msg);

    void callback(const std_msgs::String::ConstPtr& msg);

    void buttonCB(const kobuki_msgs::ButtonEventConstPtr msg);

};
#endif 
