// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from sensor_msgs:msg/CompressedImage.idl
// generated code does not contain a copyright notice
#ifndef SENSOR_MSGS__MSG__DETAIL__COMPRESSED_IMAGE__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define SENSOR_MSGS__MSG__DETAIL__COMPRESSED_IMAGE__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "sensor_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "sensor_msgs/msg/detail/compressed_image__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sensor_msgs
bool cdr_serialize_sensor_msgs__msg__CompressedImage(
  const sensor_msgs__msg__CompressedImage * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sensor_msgs
bool cdr_deserialize_sensor_msgs__msg__CompressedImage(
  eprosima::fastcdr::Cdr &,
  sensor_msgs__msg__CompressedImage * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sensor_msgs
size_t get_serialized_size_sensor_msgs__msg__CompressedImage(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sensor_msgs
size_t max_serialized_size_sensor_msgs__msg__CompressedImage(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sensor_msgs
bool cdr_serialize_key_sensor_msgs__msg__CompressedImage(
  const sensor_msgs__msg__CompressedImage * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sensor_msgs
size_t get_serialized_size_key_sensor_msgs__msg__CompressedImage(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sensor_msgs
size_t max_serialized_size_key_sensor_msgs__msg__CompressedImage(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sensor_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, sensor_msgs, msg, CompressedImage)();

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__COMPRESSED_IMAGE__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
