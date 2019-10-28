#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <iostream>
#include <kobuki_msgs/BumperEvent.h>
#include <kobuki_msgs/CliffEvent.h>
#include <kobuki_msgs/WheelDropEvent.h>
#include <kobuki_msgs/Sound.h>
//#include <std_msgs/Empty.h>
#include <ros/console.h>
#include <std_msgs/String.h>
#include <sound_play/sound_play.h>
#include <unistd.h>
#include <time.h>

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
cliff_event_subscriber_ = nh_.subscribe("mobile_base/events/cliff", 10, &SensorAct::cliffEventCB, this);
wheel_event_subscriber_ = nh_.subscribe("mobile_base/events/wheel_drop", 10, &SensorAct::wheeldropEventCB, this);
cmd_vel_pub = nh_.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
cmd_sound_pub = nh_.advertise<kobuki_msgs::Sound>("mobile_base/commands/sound", 10);

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
ros::NodeHandle nh_;
private:
sound_play::SoundClient sc;
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

     


};

//Checking if bumper is pressed
void SensorAct::bumperEventCB(const kobuki_msgs::BumperEventConstPtr msg)
{
     
    //If bumper is pressed:
     if (msg->state == kobuki_msgs::BumperEvent::PRESSED)
     {    
          ROS_INFO_STREAM("Bumper PRESSED");
          pressedBump = true;

          /**Code for send sound not being used:
          *smsg.value = 4;
          *cmd_sound_pub.publish(smsg);
          */

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
          pressedBump = false;
     }
     
 
}
//Checking if cliff have been registered:
void SensorAct::cliffEventCB(const kobuki_msgs::CliffEventConstPtr msg)
{
     //if cliff have been registered:
    if (msg->state == kobuki_msgs::CliffEvent::CLIFF)
    {
        ROS_INFO_STREAM("DETECTED CLIFF");
        
    }
    //If not then we do this:
    if (msg->state == kobuki_msgs::CliffEvent::FLOOR)
    {
        ROS_INFO_STREAM("DETECTED FLOOR");
    }
    

}
//Checking if wheelsensors are getting a input
void SensorAct::wheeldropEventCB(const kobuki_msgs::WheelDropEventConstPtr msg)
{    
     //If turtlebot wheels is dropped:
     if (msg->state == kobuki_msgs::WheelDropEvent::DROPPED)
     {
          ROS_INFO_STREAM("WHEELS DROPPED");

          //Play wav file for p1-ros machine:
		//sc.playWave("/home/p1-ros/ws/src/P1/busroute/sounds/Reee.wav", 1.0);

          //Play wav file for ubu machine:
          sc.playWave("/home/ubu/ws/src/P1/busroute/sounds/Reee.wav", 1.0);
          
          
     }
     //If turtlebot wheels is raised:
     if (msg->state == kobuki_msgs::WheelDropEvent::RAISED)
     {
          ROS_INFO_STREAM("WHEELS RAISED");
          
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


int main(int argc, char **argv) {
  ros::init(argc, argv, "move_turtle");
  

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
            ms.vel.linear.x = -0.0;
            ms.vel.angular.z = 0.0;
            ms.cmd_vel_pub.publish(ms.vel);
            ROS_INFO_STREAM("BACKWARDS" << i);
            ros::Duration(0.1).sleep();
          }
          for(int i=0; i<45; i++)
          {
            ms.vel.linear.x = 0.0;
            ms.vel.angular.z = 0.0;
            ms.cmd_vel_pub.publish(ms.vel);
            ROS_INFO_STREAM("ROTATING " << i);
            ros::Duration(0.1).sleep();
          }
          ms.pressedBump = false;
          //ms.vel.angular.z = 4.0;
          
     }
     //If bumper is not pressed, then drive forward:
     if (!ms.pressedBump)
     {
          ms.vel.linear.x = 0.0;
          ms.vel.angular.z = 0.0;
          //ROS_INFO_STREAM("FORWARD");
          ms.cmd_vel_pub.publish(ms.vel);
          
     }
     //Running loop for publishers and listeners:
     ros::spinOnce();
     loop_rate.sleep();
  }
  
return 0;
}



