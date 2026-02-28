#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Imu
 
 
class ImuRepublisherNode(Node):
    def __init__(self):
        super().__init__("imu_republisher")
        self.get_logger().info("IMU Republisher node has been started")

        self.subscriber_ = self.create_subscription(Imu, "/imu", self.imu_callback, 10)
        self.publisher_ = self.create_publisher(Imu, "imu_ekf", 10)
 
    def imu_callback(self, msg: Imu):
        msg.header.frame_id = "base_footprint_ekf"
        self.publisher_.publish(msg)
 
 
def main(args=None):
    rclpy.init(args=args)
    node = ImuRepublisherNode() 
    rclpy.spin(node)
    rclpy.shutdown()
 
 
if __name__ == "__main__":
    main()
