// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from rmw_dds_common:msg/Gid.idl
// generated code does not contain a copyright notice
#ifndef RMW_DDS_COMMON__MSG__DETAIL__GID__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define RMW_DDS_COMMON__MSG__DETAIL__GID__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "rmw_dds_common/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "rmw_dds_common/msg/detail/gid__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_rmw_dds_common
bool cdr_serialize_rmw_dds_common__msg__Gid(
  const rmw_dds_common__msg__Gid * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_rmw_dds_common
bool cdr_deserialize_rmw_dds_common__msg__Gid(
  eprosima::fastcdr::Cdr &,
  rmw_dds_common__msg__Gid * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_rmw_dds_common
size_t get_serialized_size_rmw_dds_common__msg__Gid(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_rmw_dds_common
size_t max_serialized_size_rmw_dds_common__msg__Gid(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_rmw_dds_common
bool cdr_serialize_key_rmw_dds_common__msg__Gid(
  const rmw_dds_common__msg__Gid * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_rmw_dds_common
size_t get_serialized_size_key_rmw_dds_common__msg__Gid(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_rmw_dds_common
size_t max_serialized_size_key_rmw_dds_common__msg__Gid(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_rmw_dds_common
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, rmw_dds_common, msg, Gid)();

#ifdef __cplusplus
}
#endif

#endif  // RMW_DDS_COMMON__MSG__DETAIL__GID__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
