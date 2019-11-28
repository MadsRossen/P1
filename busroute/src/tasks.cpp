#include <ros/ros.h>
#include <task1/tasks.h>
#include <nav_msgs/OccupancyGrid.h>
#include <nav_msgs/MapMetaData.h>

     void Task::mapSizeCB(const nav_msgs::OccupancyGridConstPtr msg)
{    
     
     //retrieving the cell size of the map from topic "move_base/local_costmap/costmap"
     map_size_y_ = msg->info.height;
     map_size_x_ = msg->info.width;
     map_res_ = msg->info.resolution;
     //int costmap[map_size_x_][map_size_y_];

     bool calcu = true;
     int j;
     int i;
     int u=0;
     for (i=0; i<=map_size_x_; i++ )
     {
          for (j=0; j<=map_size_y_; j++ )
          {
           u++; 
               costmap[i][j] = msg->data[u];    
          
          }     
     }
}