
#include "ros/ros.h"
#include "robotics_hw1/MotorSpeed.h"
#include "geometry_msgs/TwistStamped.h"
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
class filter {

public: 
  const float gear=30;
  const float r_wheel=0.1575; // wheel radius
  const float y0=1.34; // estimated skid-steering base

private:
  ros::NodeHandle n;

  ros::Publisher pub; 
  float v_left, v_right, v_bot,theta_bot;
  
  

 
public:
  filter(){

  pub = n.advertise<geometry_msgs::TwistStamped>("velocity_bot", 1);

  message_filters::Subscriber<robotics_hw1::MotorSpeed> sub1(n, "motor_speed_fl", 1);
  message_filters::Subscriber<robotics_hw1::MotorSpeed> sub2(n, "motor_speed_fr", 1);
  message_filters::Subscriber<robotics_hw1::MotorSpeed> sub3(n, "motor_speed_rl", 1);
  message_filters::Subscriber<robotics_hw1::MotorSpeed> sub4(n, "motor_speed_rr", 1);
  typedef message_filters::sync_policies
      ::ApproximateTime<robotics_hw1::MotorSpeed, robotics_hw1::MotorSpeed, robotics_hw1::MotorSpeed, robotics_hw1::MotorSpeed> MySyncPolicy;
  message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), sub1, sub2, sub3, sub4);

  sync.registerCallback(boost::bind(&filter::callback, this, _1, _2, _3, _4));
  ROS_INFO("Callback 0 triggered");


}

void callback (const robotics_hw1::MotorSpeedConstPtr& msg1, 
               const robotics_hw1::MotorSpeedConstPtr& msg2,
               const robotics_hw1::MotorSpeedConstPtr& msg3,
               const robotics_hw1::MotorSpeedConstPtr& msg4){

    
    ROS_INFO("Callback 1 triggered");
    v_left= ((msg1->rpm+msg3->rpm)*r_wheel*3.14)/(gear*60); // velocity in m/s ((rpm)*r_wheel* !2! *3.14)/( !2!  *gear* 60)
    v_right=-((msg2->rpm+msg4->rpm)*r_wheel)*3.14/(gear*60);

    v_bot= (v_left+v_right)/2;
    theta_bot=(-v_left+v_right)/(y0);

    geometry_msgs::TwistStamped msg_final;
    msg_final.twist.linear.x=v_bot;
    msg_final.twist.angular.z=theta_bot;

    pub.publish(msg_final);
    ROS_INFO("Callback 2 triggered");
}

};


int main(int argc, char** argv) {
  ros::init(argc, argv, "filter_and_synchronize");
  ROS_INFO("Callback 0.0 triggered");
  filter my_filter;
 
  ros::spin();

  return 0;
}
