//
// Created by peter on 2021/5/17.
//
#ifndef RM_MANUAL_REFEREE_H_
#define RM_MANUAL_REFEREE_H_

#include <cstdint>
#include <serial/serial.h>
#include <ros/ros.h>

#include <rm_msgs/Referee.h>
#include <rm_msgs/PowerManagerData.h>
#include <rm_msgs/ChassisCmd.h>
#include <rm_msgs/GimbalCmd.h>
#include <rm_msgs/ShootCmd.h>

#include "rm_manual/referee/protocol.h"

namespace rm_manual {
class PowerManagerData {
 public:
  void read(const std::vector<uint8_t> &rx_buffer);

  float parameters[4] = {0};

  ros::Time last_get_powermanager_data_ = ros::Time::now();
 private:
  void dtpReceivedCallBack(unsigned char receive_byte);
  void receiveCallBack(unsigned char package_id, unsigned char *data);
  static float int16ToFloat(unsigned short data0);

  unsigned char receive_buffer_[1024] = {0};
  unsigned char ping_pong_buffer_[1024] = {0};
  unsigned int receive_buf_counter_ = 0;
};

struct RefereeData {
  GameStatus game_status_;
  GameResult game_result_;
  GameRobotHp game_robot_hp_;
  DartStatus dart_status_;
  IcraBuffDebuffZoneStatus icra_buff_debuff_zone_status;
  EventData event_data_;
  SupplyProjectileAction supply_projectile_action_;
  RefereeWarning referee_warning_;
  DartRemainingTime dart_remaining_time_;
  GameRobotStatus game_robot_status_;
  PowerHeatData power_heat_data_;
  GameRobotPos game_robot_pos_;
  Buff buff_;
  AerialRobotEnergy aerial_robot_energy_;
  RobotHurt robot_hurt_;
  ShootData shoot_data_;
  BulletRemaining bullet_remaining_;
  RfidStatus rfid_status_;
  DartClientCmd dart_client_cmd_;
  InteractiveData student_interactive_data_;
  GraphicDataStruct graphic_data_struct_;
  RobotInteractiveData robot_interactive_data_;
  RobotCommand robot_command_;
};

class Referee {
 public:
  Referee(ros::NodeHandle nh) {
    nh_ = nh;
  }
  void init();
  void read();

  int getShootSpeedLimit(int shoot_speed) const;

  ros::NodeHandle nh_;
  ros::Publisher referee_pub_;
  ros::Publisher power_manager_pub_;

  rm_msgs::Referee referee_pub_data_;
  rm_msgs::PowerManagerData power_manager_pub_data_;
  RefereeData referee_data_{};
  PowerManagerData power_manager_data_;

  bool is_open_ = false;

  int robot_id_ = 0;
  int client_id_ = 0;

  void displayArmorInfo(double yaw2baselink, const ros::Time &now);
  void displayCapInfo(GraphicOperateType graph_operate_type);
  void displayChassisInfo(uint8_t chassis_mode, bool unlimit_flag, GraphicOperateType graph_operate_type);
  void displayGimbalInfo(uint8_t gimbal_mode, GraphicOperateType graph_operate_type);
  void displayShooterInfo(uint8_t shooter_mode, bool burst_flag, GraphicOperateType graph_operate_type);
  void displayAttackTargetInfo(bool attack_base_flag, GraphicOperateType graph_operate_type);
  void sendInteractiveData(int data_cmd_id, int receiver_id, unsigned char data);

 private:
  int unpack(uint8_t *rx_data);
  void pack(uint8_t *tx_buffer, uint8_t *data, int cmd_id, int len);
  void getRobotId();
  void drawCircle(int center_x, int center_y, int radius, int picture_id,
                  GraphicColorType color, GraphicOperateType operate_type);
  void drawString(int x, int y, int picture_id, std::string data,
                  GraphicColorType color, GraphicOperateType operate_type);
  void publishData();

  serial::Serial serial_;
  const std::string serial_port_ = "/dev/usbReferee";
  const int kUnpackLength = 256;
  const int kProtocolFrameLength = 128, kProtocolHeaderLength = 5, kProtocolCmdIdLength = 2, kProtocolTailLength = 2;
  std::vector<uint8_t> rx_data_;

  ros::Time last_get_referee_data_ = ros::Time::now();
  ros::Time last_update_armor0_time_, last_update_armor1_time_, last_update_armor2_time_, last_update_armor3_time_;
};

// CRC verification
uint8_t getCRC8CheckSum(unsigned char *pch_message, unsigned int dw_length, unsigned char uc_crc_8);
uint32_t verifyCRC8CheckSum(unsigned char *pch_message, unsigned int dw_length);
void appendCRC8CheckSum(unsigned char *pch_message, unsigned int dw_length);
uint16_t getCRC16CheckSum(uint8_t *pch_message, uint32_t dw_length, uint16_t w_crc);
uint32_t verifyCRC16CheckSum(uint8_t *pch_message, uint32_t dw_length);
void appendCRC16CheckSum(unsigned char *pch_message, unsigned int dw_length);
} // namespace rm_manual

#endif //RM_MANUAL_REFEREE_H_