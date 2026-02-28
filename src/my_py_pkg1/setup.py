from setuptools import find_packages, setup

package_name = 'my_py_pkg1'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='chirag',
    maintainer_email='chirag@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            "joint_speed_controller = my_py_pkg1.joint_speed_controller:main",
            "kalman_filter = my_py_pkg1.kalman_filter:main",
            "imu_republisher = my_py_pkg1.imu_republisher:main",
            "simple_lifecycle_node = my_py_pkg1.first_lifecycle_node:main",
            "serial_receiver = my_py_pkg1.arduino_reciever:main",
            "serial_transmitter = my_py_pkg1.arduino_publisher:main",
            "mpu6050_driver = my_py_pkg1.mpu6050_driver:main"
        ],
    },
)
