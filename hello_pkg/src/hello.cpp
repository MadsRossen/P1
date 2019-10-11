#include <iostream>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>

int main(int argc, char *argv[])
{
    ros::init(argc, argv,"hello");
    ros::NodeHandle n;
    ros::Publisher cmd_vel_pub = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel",1);

    geometry_msgs::Twist cmd_vel_message1;
    cmd_vel_message1.angular.z = 2.0;
    cmd_vel_message1.linear.x = 5.0;
    
    std::cout<<"Hello, World! Hello World" <<std::endl;

     ros::Rate loop_rate(20);
    while (ros::ok())
    {
        cmd_vel_pub.publish(cmd_vel_message1);
        
        loop_rate.sleep();
       
        cmd_vel_message1.angular.z = cmd_vel_message1.angular.z = 0.3 ;
        //cmd_vel_message1.linear.x = cmd_vel_message1.linear.x  ;
        
   
        
        
    }
    return 0;
}
