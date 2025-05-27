// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from builtin_interfaces:msg/Duration.idl
// generated code does not contain a copyright notice
#ifndef BUILTIN_INTERFACES__MSG__DETAIL__DURATION__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define BUILTIN_INTERFACES__MSG__DETAIL__DURATION__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "builtin_interfaces/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "builtin_interfaces/msg/detail/duration__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_builtin_interfaces
bool cdr_serialize_builtin_interfaces__msg__Duration(
  const builtin_interfaces__msg__Duration * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_builtin_interfaces
bool cdr_deserialize_builtin_interfaces__msg__Duration(
  eprosima::fastcdr::Cdr &,
  builtin_interfaces__msg__Duration * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_builtin_interfaces
size_t get_serialized_size_builtin_interfaces__msg__Duration(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_builtin_interfaces
size_t max_serialized_size_builtin_interfaces__msg__Duration(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_builtin_interfaces
bool cdr_serialize_key_builtin_interfaces__msg__Duration(
  const builtin_interfaces__msg__Duration * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_builtin_interfaces
size_t get_serialized_size_key_builtin_interfaces__msg__Duration(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_builtin_interfaces
size_t max_serialized_size_key_builtin_interfaces__msg__Duration(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_builtin_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, builtin_interfaces, msg, Duration)();

#ifdef __cplusplus
}
#endif

#endif  // BUILTIN_INTERFACES__MSG__DETAIL__DURATION__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
