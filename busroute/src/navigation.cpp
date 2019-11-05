#include <algorithm>
#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/simple_client_goal_state.h>
#include <geometry_msgs/PoseStamped.h>
#include <std_msgs/String.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> client;
 std::string frame = "map";
int main(int argc, char *argv[])
{
    ros::init(argc, argv, "navigation");
    client Client("move_base_simple/goal", true);
    geometry_msgs::PoseStamped cord;
    Client.waitForServer();

    cord.pose.position.x = 0;
    cord.pose.position.y = 0;
    cord.pose.position.z = 1;
    cord.header.stamp = ros::Time::now();
    cord.header.frame_id = frame.c_str();
    move_base_msgs::MoveBaseGoal goal;
    goal.target_pose.pose.position = cord.pose.position;
    Client.sendGoal(goal);
    Client.waitForResult(ros::Duration(5.0));
    if (Client.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    {
        printf("Yay! The dishes are now clean");
        printf("Current State: %s\n", Client.getState().toString().c_str());
    }
    ros::spin();
    return 0;
}
