// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from diagnostic_msgs:msg/KeyValue.idl
// generated code does not contain a copyright notice
#ifndef DIAGNOSTIC_MSGS__MSG__DETAIL__KEY_VALUE__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define DIAGNOSTIC_MSGS__MSG__DETAIL__KEY_VALUE__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "diagnostic_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "diagnostic_msgs/msg/detail/key_value__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_diagnostic_msgs
bool cdr_serialize_diagnostic_msgs__msg__KeyValue(
  const diagnostic_msgs__msg__KeyValue * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_diagnostic_msgs
bool cdr_deserialize_diagnostic_msgs__msg__KeyValue(
  eprosima::fastcdr::Cdr &,
  diagnostic_msgs__msg__KeyValue * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_diagnostic_msgs
size_t get_serialized_size_diagnostic_msgs__msg__KeyValue(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_diagnostic_msgs
size_t max_serialized_size_diagnostic_msgs__msg__KeyValue(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_diagnostic_msgs
bool cdr_serialize_key_diagnostic_msgs__msg__KeyValue(
  const diagnostic_msgs__msg__KeyValue * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_diagnostic_msgs
size_t get_serialized_size_key_diagnostic_msgs__msg__KeyValue(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_diagnostic_msgs
size_t max_serialized_size_key_diagnostic_msgs__msg__KeyValue(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_diagnostic_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, diagnostic_msgs, msg, KeyValue)();

#ifdef __cplusplus
}
#endif

#endif  // DIAGNOSTIC_MSGS__MSG__DETAIL__KEY_VALUE__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
