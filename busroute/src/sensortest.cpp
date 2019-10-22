#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include <iostream>
#include <kobuki_msgs/BumperEvent.h>
#include <kobuki_msgs/CliffEvent.h>
#include <kobuki_msgs/WheelDropEvent.h>
#include <kobuki_msgs/Sound.h>
#include <yocs_controllers/default_controller.hpp>
#include <std_msgs/Empty.h>
#include <ros/console.h>
#include <std_msgs/String.h>

//Class for all the functions:
class SensorAct

{
//initializing public variables:
public:
SensorAct() :
pressedBump(false),
cliffDetected(false),
wheeldropped(false)
{
//Initializing subscribers and publishers:
bumper_event_subscriber_ = nh_.subscribe("/mobile_base/events/bumper", 10, &SensorAct::bumperEventCB, this);
cliff_event_subscriber_ = nh_.subscribe("mobile/base/events/cliff", 10, &SensorAct::cliffEventCB, this);
wheel_event_subscriber_ = nh_.subscribe("mobile_base/event/wheeldrop", 10, &SensorAct::wheeldropEventCB, this);
cmd_vel_pub = nh_.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
cmd_sound_pub = nh_.advertise<kobuki_msgs::Sound>("mobile_base/msg/sound", 10);
}

~SensorAct()
  {
  }
//Declaring public and private variables and nodehanlders:
public:
bool pressedBump;
bool cliffDetected;
bool wheeldropped;
geometry_msgs::Twist vel;
kobuki_msgs::Sound smsg;
ros::Publisher cmd_vel_pub;
private:
ros::NodeHandle nh_;
ros::NodeHandle n;
ros::Subscriber bumper_event_subscriber_;
ros::Subscriber cliff_event_subscriber_;
ros::Subscriber wheel_event_subscriber_;
ros::Publisher cmd_sound_pub;


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

  //void moveEventCB();


};

//Checking if bumper is pressed
void SensorAct::bumperEventCB(const kobuki_msgs::BumperEventConstPtr msg)
{
     
    //If bumper is pressed:
     if (msg->state == kobuki_msgs::BumperEvent::PRESSED)
     {    
          ROS_INFO_STREAM("Bumper PRESSED");
          pressedBump = true;
          smsg.value = 4;
          cmd_sound_pub.publish(smsg);

          
     }
     //Else we do this:
     else
     {
          ROS_INFO_STREAM("Bumper RELEASED");
          pressedBump = false;
     }
      //cmd_vel_pub.publish(vel);
 //std::cout << "[ INFO] [" << ros::Time::now() << "]: main catch" << std::endl;
}

void SensorAct::cliffEventCB(const kobuki_msgs::CliffEventConstPtr msg)
{
    if (msg->state == kobuki_msgs::CliffEvent::CLIFF)
    {
        ROS_INFO_STREAM("DETECTED CLIFF");
    }
    if (msg->state == kobuki_msgs::CliffEvent::FLOOR)
    {
        ROS_INFO_STREAM("DETECTED FLOOR");
    }
    

}
void SensorAct::wheeldropEventCB(const kobuki_msgs::WheelDropEventConstPtr msg)
{
     if (msg->state == kobuki_msgs::WheelDropEvent::DROPPED)
     {
          ROS_INFO_STREAM("WHEELS DROPPED");
     }
     if (msg->state == kobuki_msgs::WheelDropEvent::RAISED)
     {
          ROS_INFO_STREAM("WHEELS RAISED");
     }
     
}
/*void SensorAct::moveEventCB()
{    geometry_msgs::Twist vel;
     //geometry_msgs::TwistPtr veloci;
     //veloci.reset(new geometry_msgs::Twist());
     if (pressedBump)
     {
          vel.linear.x = 0.0;
          vel.angular.z = 9.0;
          ROS_INFO_STREAM("Bot Stopping ");
     }
     else
     {
          //vel.linear.x = 1.0;
          vel.angular.z = 0.0;
          ROS_INFO_STREAM("Bot Moving ");
     }
     cmd_vel_pub.publish(vel);
     ROS_INFO_STREAM("In loop");
     //ros::Duration(1.0).sleep();
}*/

int main(int argc, char **argv) {
  ros::init(argc, argv, "move_turtle");
  //ros::NodeHandle nh;
  //ros::Publisher cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
  //BumperBot checker;

  //Calling new SensorAct class with name ms: 
  SensorAct ms;
  
  //Setting looprate of our while loop:
  ros::Rate loop_rate(100);
  while(ros::ok())
  {  
     //If bumper is pressed turtlebot will drive backwards and rotate:
     if (ms.pressedBump)
     {    
          for (int i=0; i<5; i++)
          {
            ms.vel.linear.x = -0.3;
            ms.vel.angular.z = 0.0;
            ms.cmd_vel_pub.publish(ms.vel);
            ROS_INFO_STREAM("BACKWARDS" << i);
            ros::Duration(0.1).sleep();
          }
          for(int i=0; i<45; i++)
          {
            ms.vel.linear.x = 0.0;
            ms.vel.angular.z = 1.0;
            ms.cmd_vel_pub.publish(ms.vel);
            ROS_INFO_STREAM("ROTATING " << i);
            ros::Duration(0.1).sleep();
          }
          
          //ms.vel.angular.z = 4.0;
          
     }
     //If bumper is not pressed, then drive forward:
     if (!ms.pressedBump)
     {
          ms.vel.linear.x = 0.3;
          ms.vel.angular.z = 0.0;
          ROS_INFO_STREAM("FORWARD");
          ms.cmd_vel_pub.publish(ms.vel);
     }
     //Running loop for publishers and listeners:
     ros::spinOnce();
     loop_rate.sleep();
  }
  //ros::Subscriber botbumper = nh.subscribe("bumper", 10, &BumperBot::botBump, &checker);
  
return 0;
}



