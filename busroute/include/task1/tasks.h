
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <iostream>
#include <ros/console.h>
#include <std_msgs/String.h>
#include <sound_play/sound_play.h>
#include <unistd.h>
#include <time.h>
#include <nav_msgs/MapMetaData.h>
#include <nav_msgs/OccupancyGrid.h>

class Task
{
public:
Task() :
run_map_getter(false)
{
map_metaData_subscriber_ = nh.subscribe("move_base/global_costmap/costmap", 10, &Task::mapSizeCB, this);
}
~Task()
{

};
private:
ros::NodeHandle nh;
ros::Subscriber map_metaData_subscriber_;

public:
bool run_map_getter;
double map_res_;
unsigned int map_size_x_, map_size_y_;
int costmap[600][600];
/**
 * @brief Get the basic information about the loaded map 
 * @param msg 
 */
    void mapSizeCB(const nav_msgs::OccupancyGridConstPtr msg);

    void stopgetSensor();
};