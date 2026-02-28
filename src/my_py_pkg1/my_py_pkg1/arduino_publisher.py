#!/usr/bin/env python3
import rclpy
import serial
from rclpy.node import Node
from std_msgs.msg import String

 
 
class SimpleSerialTransmitterNode(Node):
    def __init__(self):
        super().__init__("simple_serial_transmitter")

        self.declare_parameter("serial_port", "/dev/ttyUSB0")
        self.declare_parameter("baud_rate", 9600)

        self.serial_port = self.get_parameter("serial_port").value
        self.baud_rate = self.get_parameter("baud_rate").value
        self.arduino_serial = serial.Serial(port=self.serial_port, baudrate=self.baud_rate, timeout=0.1)
        self.subscriber_ = self.create_subscription(String ,"serial_transmitter", self.serial_transmitter_callback, 10)

    def serial_transmitter_callback(self, msg: String):
        data = msg.data
        self.arduino_serial.write(data.encode("utf-8"))
        self.get_logger().info(f"Sent to Arduino: {data}")
 
 
def main(args=None):
    rclpy.init(args=args)
    node = SimpleSerialTransmitterNode()
    rclpy.spin(node)
    rclpy.shutdown()
 
 
if __name__ == "__main__":
    main()
