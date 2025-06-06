// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from std_msgs:msg/UInt32MultiArray.idl
// generated code does not contain a copyright notice
#ifndef STD_MSGS__MSG__DETAIL__U_INT32_MULTI_ARRAY__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define STD_MSGS__MSG__DETAIL__U_INT32_MULTI_ARRAY__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "std_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "std_msgs/msg/detail/u_int32_multi_array__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_std_msgs
bool cdr_serialize_std_msgs__msg__UInt32MultiArray(
  const std_msgs__msg__UInt32MultiArray * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_std_msgs
bool cdr_deserialize_std_msgs__msg__UInt32MultiArray(
  eprosima::fastcdr::Cdr &,
  std_msgs__msg__UInt32MultiArray * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_std_msgs
size_t get_serialized_size_std_msgs__msg__UInt32MultiArray(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_std_msgs
size_t max_serialized_size_std_msgs__msg__UInt32MultiArray(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_std_msgs
bool cdr_serialize_key_std_msgs__msg__UInt32MultiArray(
  const std_msgs__msg__UInt32MultiArray * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_std_msgs
size_t get_serialized_size_key_std_msgs__msg__UInt32MultiArray(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_std_msgs
size_t max_serialized_size_key_std_msgs__msg__UInt32MultiArray(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_std_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, std_msgs, msg, UInt32MultiArray)();

#ifdef __cplusplus
}
#endif

#endif  // STD_MSGS__MSG__DETAIL__U_INT32_MULTI_ARRAY__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
