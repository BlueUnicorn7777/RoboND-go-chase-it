#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");

}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    //int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    bool ball_found = false;
    float lin_x = 0;
    float ang_z = 0;
    int bin_size = 1;
    if(img.step!=0)
    bin_size = img.step/5;
    
    for(int k = 0;k<img.height * img.step;k+=3){
    	
     		if(img.data[k]==255 && img.data[k+1]==255 && img.data[k+2]==255){
                	ball_found = true;
			int i = k % img.step;
                        if(i<(1*bin_size)){lin_x = 0.2, ang_z = 0.6;}
			else if(i<(2*bin_size)){lin_x = 0.4, ang_z = 0.4;}
			else if(i<(3*bin_size)){lin_x = 0.6, ang_z = 0;}
                        else if(i<(4*bin_size)){lin_x = 0.4, ang_z = -0.4;}
			else {lin_x = 0.2, ang_z = -0.6;}
     		        break;
		}
    	
    }

   //if (ball_found == true)
        drive_robot(lin_x,ang_z);


}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
