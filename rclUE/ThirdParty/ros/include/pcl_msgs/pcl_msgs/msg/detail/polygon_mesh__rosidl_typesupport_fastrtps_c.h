// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from pcl_msgs:msg/PolygonMesh.idl
// generated code does not contain a copyright notice
#ifndef PCL_MSGS__MSG__DETAIL__POLYGON_MESH__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define PCL_MSGS__MSG__DETAIL__POLYGON_MESH__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "pcl_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "pcl_msgs/msg/detail/polygon_mesh__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_pcl_msgs
bool cdr_serialize_pcl_msgs__msg__PolygonMesh(
  const pcl_msgs__msg__PolygonMesh * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_pcl_msgs
bool cdr_deserialize_pcl_msgs__msg__PolygonMesh(
  eprosima::fastcdr::Cdr &,
  pcl_msgs__msg__PolygonMesh * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_pcl_msgs
size_t get_serialized_size_pcl_msgs__msg__PolygonMesh(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_pcl_msgs
size_t max_serialized_size_pcl_msgs__msg__PolygonMesh(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_pcl_msgs
bool cdr_serialize_key_pcl_msgs__msg__PolygonMesh(
  const pcl_msgs__msg__PolygonMesh * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_pcl_msgs
size_t get_serialized_size_key_pcl_msgs__msg__PolygonMesh(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_pcl_msgs
size_t max_serialized_size_key_pcl_msgs__msg__PolygonMesh(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_pcl_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, pcl_msgs, msg, PolygonMesh)();

#ifdef __cplusplus
}
#endif

#endif  // PCL_MSGS__MSG__DETAIL__POLYGON_MESH__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
