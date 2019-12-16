#include "ros/ros.h"
#include "ros/time.h"

#include "opencv_apps/FaceArrayStamped.h"
#include "ros2vsrc/VsrcControl.h"

double face_x = 0;
double face_y = 0;

void face_callback(const opencv_apps::FaceArrayStamped::ConstPtr& facearraystamped)
{
  face_x = 0; // 未検出のときは0
  face_y = 0; // 未検出のときは0

  if ( facearraystamped->faces.size() > 0 ){
    face_x = facearraystamped->faces[0].face.x;
    face_y = facearraystamped->faces[0].face.y;
  }
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "face2happy");  // ノードの初期化
  ros::NodeHandle nh; // ノードハンドラ

  ros::Subscriber sub_face;  // サブスクライバの作成
  sub_face = nh.subscribe("/face_detection/faces", 10, face_callback);

	ros2vsrc::VsrcControl vsrcctl;  // パブリッシャの登録
	ros::Publisher vsrc_pub = nh.advertise<ros2vsrc::VsrcControl>("vsrc_control",10);

  ros::Rate loop_rate(10);  // 制御周期10Hz
  
  while(ros::ok()) {

	  vsrcctl.w_torque =	1;  // VS-RC003トルクオン

    if ((face_x == 0)&&(face_y == 0)){        // 顔を未検出のとき
      ROS_INFO("Not found face (R3 = OFF)");
  		vsrcctl.b_R3 = 0;
    }
    else{                                     // 顔を検出したとき
      ROS_INFO("found face !!! (R3 =  ON) | Face = (%f , %f)",face_x,face_y);
  		vsrcctl.b_R3 = 1;
    }
		vsrc_pub.publish(vsrcctl);			// コントローラーの値を送信
    ros::spinOnce();                // コールバック関数を呼ぶ
    loop_rate.sleep();
  }

  return 0;
}

