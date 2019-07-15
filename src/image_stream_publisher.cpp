#include <cv_bridge/cv_bridge.h>
#include <dirent.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <ros/ros.h>

int main(int argc, char **argv) {
  ros::init(argc, argv, "image_stream_publisher");
  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub = it.advertise("camera/image", 1);
  cv::Mat image;
  int frameRate = std::stoi(argv[2]);

  ros::Rate loop_rate(frameRate);
  DIR *dir;
  struct dirent *ent;
  std::string basedir = std::string(argv[1]) + "/";
  std::list<std::string> images;

  if ((dir = opendir(argv[1])) != NULL) {
    while ((ent = readdir(dir)) != NULL && nh.ok()) {
      if (ent->d_type == DT_REG) {
        images.push_back(ent->d_name);
      }
    }
    closedir(dir);
  } else {
    /* could not open directory */
    perror("");
    return EXIT_FAILURE;
  }
  images.sort();

  for (auto img : images) {
    if (ros::ok()) {
      image = cv::imread(basedir + img, CV_LOAD_IMAGE_UNCHANGED);
      if (image.empty()) {
        // can't go on !
        std::cout << "image '" << ent->d_name << "' empty" << std::endl;
      }
      sensor_msgs::ImagePtr msg =
          cv_bridge::CvImage(std_msgs::Header(), "bgr8", image).toImageMsg();

      pub.publish(msg);
      ros::spinOnce();
      loop_rate.sleep();
    }
  }
  return 0;
}
