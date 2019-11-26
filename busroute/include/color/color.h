#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

static const std::string OPENCV_WINDOW = "Image window";
static const std::string OPENCV_WINDOW_ORIGINAL = "Original Image window";
static const std::string OPENCV_WINDOW_UNCROPPED = "Image window uncropped";



class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport imageTransport_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;


public:
  ImageConverter()
    : imageTransport_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = imageTransport_.subscribe("/camera/rgb/image_raw", 1, &ImageConverter::imageCb, this);
    //image_pub_ = imageTransport _.advertise("/image_converter/output_video", 1);

  }

}