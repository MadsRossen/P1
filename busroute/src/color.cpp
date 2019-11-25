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
    image_sub_ = it_.subscribe("/camera/rgb/image_raw", 1,
      &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);
  }

  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImagePtr cvImage;
    try
    {
      cvImage = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }
  
    int shelfLowerB = 30;
    int shelfLowerG = 80;
    int shelfLowerR = 160;
    int shelfUpperB = 80;
    int shelfUpperG = 150;
    int shelfUpperR = 220; 

    
    int blueLowerB = 220;
    int blueLowerG = 0;
    int blueLowerR = 0;
    int blueUpperB = 255;
    int blueUpperG = 30;
    int blueUpperR = 30; 

    int greenLowerB = 0;
    int greenLowerG = 220;
    int greenLowerR = 0;
    int greenUpperB = 30;
    int greenUpperG = 255;
    int greenUpperR = 30; 

    int redLowerB = 0;
    int redLowerG = 0;
    int redLowerR = 220;
    int redUpperB = 30;
    int redUpperG = 30;
    int redUpperR = 255; 
    



    
    cv::Mat mask, part, maskBlue, maskGreen, maskRed, maskShelf;
    

    cv::inRange(cvImage->image, cv::Scalar(blueLowerB,blueLowerG,blueLowerR), cv::Scalar(blueUpperB,blueUpperG,blueUpperR), maskBlue);
    cv::inRange(cvImage->image, cv::Scalar(greenLowerB,greenLowerG,greenLowerR), cv::Scalar(greenUpperB,greenUpperG,greenUpperR), maskGreen);
    cv::inRange(cvImage->image, cv::Scalar(redLowerB,redLowerG,redLowerR), cv::Scalar(redUpperB,redUpperG,redUpperR), maskRed);

    mask = maskBlue + maskGreen + maskRed;
  
    cv::inRange(cvImage->image, cv::Scalar(shelfLowerB,shelfLowerG,shelfLowerR), cv::Scalar(shelfUpperB,shelfUpperG,shelfUpperR), maskShelf);
    part = maskShelf(cv::Range(400,480),cv::Range(0,640));

    cv::namedWindow(OPENCV_WINDOW,600);
    cv::imshow(OPENCV_WINDOW, part);
    cv::imshow(OPENCV_WINDOW_UNCROPPED, maskShelf);
    int whitePixels = cv::countNonZero(part);
    
    //std::cout << whitePixels << std::endl;

    if (whitePixels >= 48000)
    {
      std::cout << "shelf detected" << std::endl;
    }

    cv::waitKey(3);

    // Update GUI Window
    cv::imshow(OPENCV_WINDOW_ORIGINAL, cvImage->image);
    cv::waitKey(3);

    // Output modified video stream
    image_pub_.publish(cvImage->toImageMsg());

    
  }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}