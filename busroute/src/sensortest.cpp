#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <iostream>
#include <kobuki_msgs/BumperEvent.h>
#include <kobuki_msgs/CliffEvent.h>
#include <kobuki_msgs/WheelDropEvent.h>
#include <kobuki_msgs/DigitalInputEvent.h>
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
wheeldropped_left(false)

{
//Initializing subscribers and publishers:
bumper_event_subscriber_ = nh_.subscribe("/mobile_base/events/bumper", 10, &SensorAct::bumperEventCB, this);
cliff_event_subscriber_ = nh_.subscribe("mobile_base/events/cliff", 10, &SensorAct::cliffEventCB, this);
wheel_event_subscriber_ = nh_.subscribe("mobile_base/events/wheel_drop", 10, &SensorAct::wheeldropEventCB, this);
digitalInput_event_subsriber_ = nh_.subscribe("mobile_base/events/digital_input", 10, &SensorAct::digitalInputCB, this);
cmd_vel_pub = nh_.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop", 10);
cmd_sound_pub = nh_.advertise<kobuki_msgs::Sound>("mobile_base/commands/sound", 10);

}

~SensorAct()
  {
       
  }
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
/**
 * @brief Trigger boolean when turtlebot is getting sensordata
 * @param msg sensor event
 */
     void digitalInputCB(const kobuki_msgs::DigitalInputEventConstPtr msg);


};

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
     if (msg->state == kobuki_msgs::WheelDropEvent::LEFT || msg->state == kobuki_msgs::WheelDropEvent::RIGHT)
     {
          switch (msg->wheel)
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
          }
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
     if (msg->values[1] == 1)
     {
          ROS_INFO_STREAM("INPUT FROM SENSOR");
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
     //Running loop for publishers and listeners:
     ros::spinOnce();
     //tjek for cliff
     if (ms.cliffDetected_center)
     {
          std::cout << "cliff center" << std::endl;
          ms.cliffDetected_center=false;
          for (int i=0; i<5; i++){
          ms.vel.linear.x = -0.1;
          ms.vel.angular.z = 0.0;
          ms.cmd_vel_pub.publish(ms.vel);
          ROS_INFO("BACKWARDS");
          ros::Duration(0.1).sleep();
          }
     }
     //If bumper is pressed turtlebot will drive backwards and rotate:
     if (ms.bumper_pressed_center || ms.bumper_pressed_left || ms.bumper_pressed_right)
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
          ms.bumper_pressed_center = false;
          ms.bumper_pressed_left = false;
          ms.bumper_pressed_right = false;
          //ms.vel.angular.z = 4.0;
          
     }
     //If bumper is not pressed, then drive forward:
     if (!ms.bumper_pressed_center && !ms.bumper_pressed_left && !ms.bumper_pressed_right)
     {
          ms.vel.linear.x = 0.0;
          ms.vel.angular.z = 0.0;
          //ROS_INFO_STREAM("FORWARD");
          ms.cmd_vel_pub.publish(ms.vel);
          
     }
     
     loop_rate.sleep();  
  }
return 0;
}