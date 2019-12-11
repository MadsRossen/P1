#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"

class ImageConverter
{
  public:
  ImageConverter():
  imageTransport_(nh_),
  trashDetected_red(false),
  trashDetected_green(false),
  trashDetected_blue(false)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = imageTransport_.subscribe("/camera/rgb/image_raw", 1, &ImageConverter::imageCb, this);
    //image_pub_ = imageTransport _.advertise("/image_converter/output_video", 1);

  }
  ~ImageConverter()
  {
       
  };
public:
bool trashDetected_red;
bool trashDetected_green;
bool trashDetected_blue;
private:
ros::NodeHandle nh_;
  image_transport::ImageTransport imageTransport_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  std::string OPENCV_WINDOW = "Image window";
  std::string OPENCV_WINDOW_ORIGINAL = "Original Image window";
  std::string OPENCV_WINDOW_UNCROPPED = "Image window uncropped";



void imageCb(const sensor_msgs::ImageConstPtr& msg);
};