#include <ros/ros.h>
#include <iostream>
#include <stdio.h>
#include <fstream>

#include <aruco_mapping/ArucoMarker.h>
//#include <std_msgs/Header.h>
#include <geometry_msgs/Twist.h>

using namespace ros;
using namespace std;
using namespace std_msgs;
using namespace aruco_mapping;
using namespace geometry_msgs;

geometry_msgs::Twist twist;
ros::Publisher vel_pub;

//time t, t_prev, t_sum;
aruco_mapping::ArucoMarker prev_am;
ros::Time t_prev(0);
double t_sum;
double p[3], p_prev[3], p_sum[3], vel[3];
int cnt = 0;
int flag = 0;
void initialCallback(const ArucoMarkerConstPtr& am)
{	
	//Header h_new, h_prev,h_sum;
	p[0] = am->global_camera_pose.position.x;
	p[1] = am->global_camera_pose.position.y;
	p[2] = am->global_camera_pose.position.z;
	//h_new.stamp = am->header.stamp;
	//cout<<"Time now: "<<t<<"\n";
	if(flag == 0)
	{
		cnt = 0;
		//h_sum.stamp = 0;
		//prev_am.header.stamp = 0;
		//t_prev(0);
		t_sum = 0;
		p_sum[0] = 0;
		p_sum[1] = 0;
		p_sum[2] = 0;
		flag += 1;
	}
	else
	{
		cnt += 1;
		//h_sum.stamp += h_new.stamp - h_prev.stamp;
		t_sum += (am->header.stamp - t_prev).toSec();
		p_sum[0] += p[0] - p_prev[0];
		p_sum[1] += p[1] - p_prev[1];
		p_sum[2] += p[2] - p_prev[2];

		if(t_sum >= 1)    //t_sum is in nanoseconds
		{
			//t_sum = t_sum/10^9;
			vel[0] = (p_sum[0]/cnt)/t_sum;
			vel[1] = (p_sum[1]/cnt)/t_sum;
			vel[2] = (p_sum[2]/cnt)/t_sum;
			cout<<"Velocity in x: "<<vel[0]<<"\tin y: "<<vel[1]<<"\tin z: "<<vel[2]<<"\n";

			flag = 0;
		}

	}
	//h_prev.stamp = h_new.stamp;
	cout<<"Time sum: "<<t_sum<<"\n";
	//prev_am.header.stamp = am->header.stamp;
	t_prev = am->header.stamp;
	p_prev[0] = p[0];
	p_prev[1] = p[1];
	p_prev[2] = p[2];

	twist.linear.x = vel[0];
	twist.linear.y = vel[1];
	twist.linear.z = vel[2];
	vel_pub.publish(twist);

}
int main(int argc, char** argv)
{
  ros::init(argc, argv, "velCALCULATE");
  ros::NodeHandle nh;
  ros::Subscriber pose_sub;
  pose_sub = nh.subscribe("/aruco_poses", 1, initialCallback);
  vel_pub = nh.advertise<geometry_msgs::Twist>("/velcal/obs_vel", 1);  
  ros::spin();
  return 0;
}
