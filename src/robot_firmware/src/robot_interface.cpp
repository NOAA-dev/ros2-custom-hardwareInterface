#include "robot_firmware/robot_interface.hpp"
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <pluginlib/class_list_macros.hpp>


namespace robot_firmware
{
robotInterface::robotInterface()
{
}


robotInterface::~robotInterface()
{
  if (arduino_.IsOpen())
  {
    try
    {
      arduino_.Close();
    }
    catch (...)
    {
      RCLCPP_FATAL_STREAM(rclcpp::get_logger("robotInterface"),
                          "Something went wrong while closing connection with port " << port_);
    }
  }
}


CallbackReturn robotInterface::on_init(const hardware_interface::HardwareInfo &hardware_info)
{
  CallbackReturn result = hardware_interface::SystemInterface::on_init(hardware_info);
  if (result != CallbackReturn::SUCCESS)
  {
    return result;
  }

  try
  {
    port_ = info_.hardware_parameters.at("port");
  }
  catch (const std::out_of_range &e)
  {
    RCLCPP_FATAL(rclcpp::get_logger("robotInterface"), "No Serial Port provided! Aborting");
    return CallbackReturn::FAILURE;
  }

  velocity_commands_.resize(info_.joints.size(), 0.0);
  position_states_.resize(info_.joints.size(), 0.0);
  velocity_states_.resize(info_.joints.size(), 0.0);
  last_run_ = rclcpp::Clock().now();

  return CallbackReturn::SUCCESS;
}


std::vector<hardware_interface::StateInterface> robotInterface::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> state_interfaces;

  // Provide only a position Interafce
  for (size_t i = 0; i < info_.joints.size(); i++)
  {
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        info_.joints[i].name, hardware_interface::HW_IF_POSITION, &position_states_[i]));
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &velocity_states_[i]));
  }

  return state_interfaces;
}


std::vector<hardware_interface::CommandInterface> robotInterface::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> command_interfaces;

  // Provide only a velocity Interafce
  for (size_t i = 0; i < info_.joints.size(); i++)
  {
    command_interfaces.emplace_back(hardware_interface::CommandInterface(
        info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &velocity_commands_[i]));
  }

  return command_interfaces;
}


CallbackReturn robotInterface::on_activate(const rclcpp_lifecycle::State &)
{
  RCLCPP_INFO(rclcpp::get_logger("robotInterface"), "Starting robot hardware ...");

  // Reset commands and states
  velocity_commands_ = { 0.0, 0.0 };
  position_states_ = { 0.0, 0.0 };
  velocity_states_ = { 0.0, 0.0 };

  try
  {
    arduino_.Open(port_);
    arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
  }
  catch (...)
  {
    RCLCPP_FATAL_STREAM(rclcpp::get_logger("robotInterface"),
                        "Something went wrong while interacting with port " << port_);
    return CallbackReturn::FAILURE;
  }

  RCLCPP_INFO(rclcpp::get_logger("robotInterface"),
              "Hardware started, ready to take commands");
  return CallbackReturn::SUCCESS;
}


CallbackReturn robotInterface::on_deactivate(const rclcpp_lifecycle::State &)
{
  RCLCPP_INFO(rclcpp::get_logger("robotInterface"), "Stopping robot hardware ...");

  if (arduino_.IsOpen())
  {
    try
    {
      arduino_.Close();
    }
    catch (...)
    {
      RCLCPP_FATAL_STREAM(rclcpp::get_logger("robotInterface"),
                          "Something went wrong while closing connection with port " << port_);
    }
  }

  RCLCPP_INFO(rclcpp::get_logger("robotInterface"), "Hardware stopped");
  return CallbackReturn::SUCCESS;
}

// hardware_interface::return_type robotInterface::read(
//     const rclcpp::Time &, const rclcpp::Duration &)
// {
//   // ---- time update (ALWAYS) ----
//   auto now = rclcpp::Clock().now();
//   double dt = (now - last_run_).seconds();
//   last_run_ = now;

//   if (!arduino_.IsDataAvailable()) {
//     return hardware_interface::return_type::OK;
//   }

//   std::string message;
//   try {
//     arduino_.ReadLine(message, '\n');   // Arduino uses println
//   } catch (...) {
//     return hardware_interface::return_type::OK;
//   }

//   // Remove CR from println()
//   message.erase(std::remove(message.begin(), message.end(), '\r'), message.end());

//   // Expected EXACT format:
//   // rp0.00lp0.00;
//   auto r = message.find('r');
//   auto l = message.find('l');
//   auto s = message.find(';');

//   if (r == std::string::npos ||
//       l == std::string::npos ||
//       s == std::string::npos) {
//     return hardware_interface::return_type::OK;
//   }

//   try {
//     int sign_r = (message[r + 1] == 'p') ? 1 : -1;
//     int sign_l = (message[l + 1] == 'p') ? 1 : -1;

//     double vr = std::stod(message.substr(r + 2, l - r - 2));
//     double vl = std::stod(message.substr(l + 2, s - l - 2));

//     velocity_states_[0] = sign_r * vr;
//     velocity_states_[1] = sign_l * vl;

//     position_states_[0] += velocity_states_[0] * dt;
//     position_states_[1] += velocity_states_[1] * dt;
//   }
//   catch (...) {
//     // swallow malformed frame
//   }

//   return hardware_interface::return_type::OK;
// }
hardware_interface::return_type robotInterface::read(const rclcpp::Time &,
                                                          const rclcpp::Duration &)
{
  // Interpret the string
  if(arduino_.IsDataAvailable())
  {
    auto dt = (rclcpp::Clock().now() - last_run_).seconds();
    std::string message;
    arduino_.ReadLine(message);
    std::stringstream ss(message);
    std::string res;
    int multiplier_r = 1;
    int multiplier_l = 1;
    while(std::getline(ss, res, ';'))
    {
    if (res.empty() || res.size() < 4) {
        continue;
    }
      res.erase(std::remove(res.begin(), res.end(), '\r'), res.end());
      auto r_index = res.find('r');
      auto l_index = res.find('l');
      auto col_index = res.find(';');

      if(r_index == std::string::npos || l_index == std::string::npos)
      {
        RCLCPP_ERROR_STREAM(rclcpp::get_logger("robotInterface"),
                            "Received message " << message << " is not in the correct format");
        continue;
      }

      multiplier_r = res.at(r_index + 1) == 'p' ? 1 : -1;
      multiplier_l = res.at(l_index + 1) == 'p' ? 1 : -1;

      velocity_states_.at(0) = multiplier_r * std::stod(res.substr(r_index + 2, l_index - r_index - 2));
      position_states_.at(0) += velocity_states_.at(0) * dt;
      
      velocity_states_.at(1) = multiplier_l * std::stod(res.substr(l_index + 2, col_index - l_index - 2));
      position_states_.at(1) += velocity_states_.at(1) * dt;
    }

    last_run_ = rclcpp::Clock().now();
  }
  return hardware_interface::return_type::OK;
}


hardware_interface::return_type robotInterface::write(const rclcpp::Time &,
                                                          const rclcpp::Duration &)
{
  // Implement communication protocol with the Arduino
  std::stringstream message_stream;
  char right_wheel_sign = velocity_commands_.at(0) >= 0 ? 'p' : 'n';
  char left_wheel_sign = velocity_commands_.at(1) >= 0 ? 'p' : 'n';

  
  message_stream << std::fixed << std::setprecision(2) << 
    "r" << right_wheel_sign << std::abs(velocity_commands_.at(0)) << 
    "l" <<  left_wheel_sign << std::abs(velocity_commands_.at(1)) << ";\n";

  try
  {
    arduino_.Write(message_stream.str());
    // RCLCPP_INFO_STREAM(rclcpp::get_logger("robotInterface"),"TX: [" << message_stream.str() << "]");
  }
  catch (...)
  {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("robotInterface"),
                        "Something went wrong while sending the message "
                            << message_stream.str() << " to the port " << port_);
    return hardware_interface::return_type::ERROR;
  }

  return hardware_interface::return_type::OK;
}
}  // namespace robot_firmware

PLUGINLIB_EXPORT_CLASS(robot_firmware::robotInterface, hardware_interface::SystemInterface)