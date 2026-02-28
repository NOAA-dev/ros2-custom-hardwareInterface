#!/usr/bin/env python3
import rclpy
import serial
from rclpy.node import Node
from std_msgs.msg import String

 
 
class SimpleSerialReceiverNode(Node):
    def __init__(self):
        super().__init__("simple_serial_receiver")

        self.declare_parameter("serial_port", "/dev/ttyUSB0")
        self.declare_parameter("baud_rate", 9600)

        self.serial_port = self.get_parameter("serial_port").value
        self.baud_rate = self.get_parameter("baud_rate").value
        self.arduino_serial = serial.Serial(port=self.serial_port, baudrate=self.baud_rate, timeout=0.1)
        self.publisher_ = self.create_publisher(String, "serial_receiver", 10)
        self.timer = self.create_timer(0.01, self.read_from_serial)

    def read_from_serial(self):
        if rclpy.ok():
            if self.arduino_serial.in_waiting > 0:
                data = self.arduino_serial.readline().decode("utf-8").rstrip()
                msg = String()
                msg.data = data
                self.publisher_.publish(msg)
                self.get_logger().info(f"Received from Arduino: {data}")
 
def main(args=None):
    rclpy.init(args=args)
    node = SimpleSerialReceiverNode()
    rclpy.spin(node)
    rclpy.shutdown()
 
 
if __name__ == "__main__":
    main()
