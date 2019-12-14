#include <color/color.h>

void ImageConverter::imageCb(const sensor_msgs::ImageConstPtr& msg)
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
    // shelf is used for testing in gazebo (changed to see the white cube)

    trashDetected_blue = false;
      trashDetected_green = false;
      trashDetected_red = false;

    int shelfLowerB = 160;
    int shelfLowerG = 160;
    int shelfLowerR = 160;
    int shelfUpperB = 170;
    int shelfUpperG = 170;
    int shelfUpperR = 170; 

    //Google "color picker":
    
    int blueLowerB = 20; //20
    int blueLowerG = 0;
    int blueLowerR = 0;
    int blueUpperB = 110;//110 //Prøv med 255
    int blueUpperG = 20;//20
    int blueUpperR = 25;//25 
    
    int greenLowerB = 0;
    int greenLowerG = 10;//10
    int greenLowerR = 0;
    int greenUpperB = 30;//45
    int greenUpperG = 80;//80 //Prøv med 255 
    int greenUpperR = 20;//25

    int redLowerB = 0;
    int redLowerG = 0;
    int redLowerR = 25;//80
    int redUpperB = 30;//35
    int redUpperG = 20;//20
    int redUpperR = 160;//160  //Prøv med 255
        
  
    cv::Mat mask, part, maskBlue, maskGreen, maskRed, maskShelf;
    
    cv::inRange(cvImage->image, cv::Scalar(blueLowerB,blueLowerG,blueLowerR), cv::Scalar(blueUpperB,blueUpperG,blueUpperR), maskBlue);
    cv::inRange(cvImage->image, cv::Scalar(greenLowerB,greenLowerG,greenLowerR), cv::Scalar(greenUpperB,greenUpperG,greenUpperR), maskGreen);
    cv::inRange(cvImage->image, cv::Scalar(redLowerB,redLowerG,redLowerR), cv::Scalar(redUpperB,redUpperG,redUpperR), maskRed);

    mask = maskBlue + maskGreen + maskRed;

    //first range is hight of image, second range is width of image
    maskBlue = maskBlue(cv::Range(350,480),cv::Range(0,640));
    maskGreen = maskGreen(cv::Range(350,480),cv::Range(0,640));
    maskRed = maskRed(cv::Range(350,480),cv::Range(0,640));

  
    cv::inRange(cvImage->image, cv::Scalar(shelfLowerB,shelfLowerG,shelfLowerR), cv::Scalar(shelfUpperB,shelfUpperG,shelfUpperR), maskShelf);
    part = mask(cv::Range(350,480),cv::Range(0,640));

    //cv::namedWindow(OPENCV_WINDOW,600);
    //cv::imshow(OPENCV_WINDOW, part);
    //cv::imshow(OPENCV_WINDOW_UNCROPPED, mask);


    cv::Mat blob = ~part;
    
    using namespace cv;
    using namespace std;




	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;


    params.thresholdStep = 5;
    params.minThreshold = 150;
    params.maxThreshold = 300;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 250;
  params.maxArea = 100000;

  // filter by color
  params.filterByColor = true;
  params.blobColor = 0;
	
  // Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.4;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.87;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.20;



  Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params); 
  
  // Storage for blobs
	vector<KeyPoint> keypoints; 
  vector<KeyPoint> keypoints_red;
  vector<KeyPoint> keypoints_blue;
  vector<KeyPoint> keypoints_green;


 detector->detect( blob, keypoints);
 detector->detect( ~maskBlue, keypoints_blue);
 detector->detect( ~maskRed, keypoints_red);
 detector->detect( ~maskGreen, keypoints_green);
  
  Mat im_with_keypoints;
	Mat im_with_keypoints_red;
  Mat im_with_keypoints_blue;
  Mat im_with_keypoints_green;
	drawKeypoints( blob, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
  drawKeypoints( ~maskBlue, keypoints_blue, im_with_keypoints_blue, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
  drawKeypoints( ~maskRed, keypoints_red, im_with_keypoints_red, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
  drawKeypoints( ~maskGreen, keypoints_green, im_with_keypoints_green, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
  
  // detector->detect( blob, keypoints);
  // cout<<keypoints[1].pt<<endl;
  imshow("keypoints", im_with_keypoints);
  //imshow("keypoints_red", im_with_keypoints_red);
  //imshow("keypoints_blue", im_with_keypoints_blue);
  //imshow("keypoints_green", im_with_keypoints_green);
  

  /*if (keypoints.size() >0)
  {
    //cout<< "trash detected" << endl;
  

    int whitePixels_blue = cv::countNonZero(maskBlue);
    int whitePixels_red = cv::countNonZero(maskRed);
    int whitePixels_green = cv::countNonZero(maskGreen);
    int whitePixels = cv::countNonZero(mask);
    //std::cout << whitePixels << std::endl;
    
    if (whitePixels_blue >= 3000 && trashDetected_blue == false)
    {
      std::cout << "trash detected (blue)" << std::endl;
      trashDetected_blue = true;
    }
    if (whitePixels_red >= 3000 && trashDetected_red == false)
    {
      std::cout << "trash detected (red)" << std::endl;
      trashDetected_red = true;
    }
    if (whitePixels_green >= 3000 && trashDetected_green ==false)
    {
      std::cout << "trash detected (green)" << std::endl;
      trashDetected_green = true;
    }
    
    if (whitePixels < 200)
    {
      trashDetected_blue = false;
      trashDetected_green = false;
      trashDetected_red = false;
    }
  }*/

  if (keypoints_blue.size() >0)
  {
     if (trashDetected_blue == false)
    {
      std::cout << "trash detected (blue)" << std::endl;
      trashDetected_blue = true;
    }
  }
  else
  {
   trashDetected_blue = false; 
  }
  if (keypoints_red.size() >0)
  {
     if (trashDetected_red == false)
    {
      std::cout << "trash detected (red)" << std::endl;
      trashDetected_red = true;
    }
  }
  else
  {
   trashDetected_red = false; 
  }
  if (keypoints_green.size() >0)
  {
     if (trashDetected_green == false)
    {
      std::cout << "trash detected (green)" << std::endl;
      trashDetected_green = true;
    }
  }
  else
  {
   trashDetected_green = false; 
  }

    cv::waitKey(3);

    // Update GUI Window
    cv::imshow(OPENCV_WINDOW_ORIGINAL, cvImage->image);
    cv::waitKey(3);

    // Output modified video stream
    //image_pub_.publish(cvImage->toImageMsg());

    
  }
/*int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}*/