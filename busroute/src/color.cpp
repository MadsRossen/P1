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
    // shelf is used for testing in 
    int shelfLowerB = 30;
    int shelfLowerG = 80;
    int shelfLowerR = 160;
    int shelfUpperB = 80;
    int shelfUpperG = 150;
    int shelfUpperR = 220; 

    
    int blueLowerB = 50;
    int blueLowerG = 0;
    int blueLowerR = 0;
    int blueUpperB = 110;
    int blueUpperG = 20;
    int blueUpperR = 25; 

    int greenLowerB = 0;
    int greenLowerG = 30;
    int greenLowerR = 0;
    int greenUpperB = 45;
    int greenUpperG = 80;
    int greenUpperR = 25; 

    int redLowerB = 0;
    int redLowerG = 0;
    int redLowerR = 100;
    int redUpperB = 25 ;
    int redUpperG = 10;
    int redUpperR = 160; 
    



    
    cv::Mat mask, part, maskBlue, maskGreen, maskRed, maskShelf;
    

    cv::inRange(cvImage->image, cv::Scalar(blueLowerB,blueLowerG,blueLowerR), cv::Scalar(blueUpperB,blueUpperG,blueUpperR), maskBlue);
    cv::inRange(cvImage->image, cv::Scalar(greenLowerB,greenLowerG,greenLowerR), cv::Scalar(greenUpperB,greenUpperG,greenUpperR), maskGreen);
    cv::inRange(cvImage->image, cv::Scalar(redLowerB,redLowerG,redLowerR), cv::Scalar(redUpperB,redUpperG,redUpperR), maskRed);

    mask = maskBlue + maskGreen + maskRed;
  
    cv::inRange(cvImage->image, cv::Scalar(shelfLowerB,shelfLowerG,shelfLowerR), cv::Scalar(shelfUpperB,shelfUpperG,shelfUpperR), maskShelf);
    part = mask(cv::Range(400,480),cv::Range(200,465));

    cv::namedWindow(OPENCV_WINDOW,600);
    cv::imshow(OPENCV_WINDOW, part);
    cv::imshow(OPENCV_WINDOW_UNCROPPED, mask);
    int whitePixels = cv::countNonZero(part);
    
    //std::cout << whitePixels << std::endl;
    bool trashDetected;
    if (whitePixels >= 3000 && trashDetected == false)
    {
      std::cout << "trash detected" << std::endl;
      trashDetected = true;
    }
    if (whitePixels < 200)
    {
      trashDetected = false;
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