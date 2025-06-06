// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from tf2_msgs:msg/TFMessage.idl
// generated code does not contain a copyright notice
#ifndef TF2_MSGS__MSG__DETAIL__TF_MESSAGE__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define TF2_MSGS__MSG__DETAIL__TF_MESSAGE__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "tf2_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "tf2_msgs/msg/detail/tf_message__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_tf2_msgs
bool cdr_serialize_tf2_msgs__msg__TFMessage(
  const tf2_msgs__msg__TFMessage * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_tf2_msgs
bool cdr_deserialize_tf2_msgs__msg__TFMessage(
  eprosima::fastcdr::Cdr &,
  tf2_msgs__msg__TFMessage * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_tf2_msgs
size_t get_serialized_size_tf2_msgs__msg__TFMessage(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_tf2_msgs
size_t max_serialized_size_tf2_msgs__msg__TFMessage(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_tf2_msgs
bool cdr_serialize_key_tf2_msgs__msg__TFMessage(
  const tf2_msgs__msg__TFMessage * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_tf2_msgs
size_t get_serialized_size_key_tf2_msgs__msg__TFMessage(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_tf2_msgs
size_t max_serialized_size_key_tf2_msgs__msg__TFMessage(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_tf2_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, tf2_msgs, msg, TFMessage)();

#ifdef __cplusplus
}
#endif

#endif  // TF2_MSGS__MSG__DETAIL__TF_MESSAGE__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
