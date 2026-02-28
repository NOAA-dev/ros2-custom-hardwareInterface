from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    joystick_teleop_config = os.path.join(
        get_package_share_directory('program_bringup'),
        'config',
        'joystick_teleop.yaml'
    )

    joystick_node = Node(
        package='joy_teleop',
        executable='joy_teleop',
        name='joy_teleop',
        output='screen',
        parameters=[joystick_teleop_config]
    )

    joy_node = Node(
        package='joy',
        executable='joy_node',
        name='joystick',
        output='screen',
        parameters = [os.path.join(
            get_package_share_directory('program_bringup'),
            'config',
            'joystick_config.yaml')]
    )

    return LaunchDescription([
        joystick_node,
        joy_node
    ])