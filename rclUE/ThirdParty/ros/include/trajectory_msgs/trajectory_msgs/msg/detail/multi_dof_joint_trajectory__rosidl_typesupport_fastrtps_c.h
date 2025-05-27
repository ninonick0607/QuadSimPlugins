// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from trajectory_msgs:msg/MultiDOFJointTrajectory.idl
// generated code does not contain a copyright notice
#ifndef TRAJECTORY_MSGS__MSG__DETAIL__MULTI_DOF_JOINT_TRAJECTORY__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define TRAJECTORY_MSGS__MSG__DETAIL__MULTI_DOF_JOINT_TRAJECTORY__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "trajectory_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "trajectory_msgs/msg/detail/multi_dof_joint_trajectory__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_trajectory_msgs
bool cdr_serialize_trajectory_msgs__msg__MultiDOFJointTrajectory(
  const trajectory_msgs__msg__MultiDOFJointTrajectory * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_trajectory_msgs
bool cdr_deserialize_trajectory_msgs__msg__MultiDOFJointTrajectory(
  eprosima::fastcdr::Cdr &,
  trajectory_msgs__msg__MultiDOFJointTrajectory * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_trajectory_msgs
size_t get_serialized_size_trajectory_msgs__msg__MultiDOFJointTrajectory(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_trajectory_msgs
size_t max_serialized_size_trajectory_msgs__msg__MultiDOFJointTrajectory(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_trajectory_msgs
bool cdr_serialize_key_trajectory_msgs__msg__MultiDOFJointTrajectory(
  const trajectory_msgs__msg__MultiDOFJointTrajectory * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_trajectory_msgs
size_t get_serialized_size_key_trajectory_msgs__msg__MultiDOFJointTrajectory(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_trajectory_msgs
size_t max_serialized_size_key_trajectory_msgs__msg__MultiDOFJointTrajectory(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_trajectory_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, trajectory_msgs, msg, MultiDOFJointTrajectory)();

#ifdef __cplusplus
}
#endif

#endif  // TRAJECTORY_MSGS__MSG__DETAIL__MULTI_DOF_JOINT_TRAJECTORY__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
