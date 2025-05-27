// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from ue_msgs:msg/HitEvent.idl
// generated code does not contain a copyright notice
#ifndef UE_MSGS__MSG__DETAIL__HIT_EVENT__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define UE_MSGS__MSG__DETAIL__HIT_EVENT__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "ue_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "ue_msgs/msg/detail/hit_event__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_ue_msgs
bool cdr_serialize_ue_msgs__msg__HitEvent(
  const ue_msgs__msg__HitEvent * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_ue_msgs
bool cdr_deserialize_ue_msgs__msg__HitEvent(
  eprosima::fastcdr::Cdr &,
  ue_msgs__msg__HitEvent * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_ue_msgs
size_t get_serialized_size_ue_msgs__msg__HitEvent(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_ue_msgs
size_t max_serialized_size_ue_msgs__msg__HitEvent(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_ue_msgs
bool cdr_serialize_key_ue_msgs__msg__HitEvent(
  const ue_msgs__msg__HitEvent * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_ue_msgs
size_t get_serialized_size_key_ue_msgs__msg__HitEvent(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_ue_msgs
size_t max_serialized_size_key_ue_msgs__msg__HitEvent(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_ue_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, ue_msgs, msg, HitEvent)();

#ifdef __cplusplus
}
#endif

#endif  // UE_MSGS__MSG__DETAIL__HIT_EVENT__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
