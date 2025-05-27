// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from pcl_msgs:msg/Vertices.idl
// generated code does not contain a copyright notice

#include "pcl_msgs/msg/detail/vertices__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_pcl_msgs
const rosidl_type_hash_t *
pcl_msgs__msg__Vertices__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x4b, 0xe2, 0x37, 0x4c, 0x99, 0xf6, 0xf9, 0xbb,
      0x80, 0x91, 0xec, 0xd1, 0x88, 0x18, 0xb6, 0x8f,
      0x83, 0x09, 0x94, 0x05, 0x31, 0x0e, 0xd1, 0x4e,
      0x4d, 0xdc, 0x4e, 0x0e, 0x89, 0x31, 0xd7, 0xbb,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char pcl_msgs__msg__Vertices__TYPE_NAME[] = "pcl_msgs/msg/Vertices";

// Define type names, field names, and default values
static char pcl_msgs__msg__Vertices__FIELD_NAME__vertices[] = "vertices";

static rosidl_runtime_c__type_description__Field pcl_msgs__msg__Vertices__FIELDS[] = {
  {
    {pcl_msgs__msg__Vertices__FIELD_NAME__vertices, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
pcl_msgs__msg__Vertices__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {pcl_msgs__msg__Vertices__TYPE_NAME, 21, 21},
      {pcl_msgs__msg__Vertices__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# List of point indices\n"
  "uint32[] vertices";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
pcl_msgs__msg__Vertices__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {pcl_msgs__msg__Vertices__TYPE_NAME, 21, 21},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 42, 42},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
pcl_msgs__msg__Vertices__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *pcl_msgs__msg__Vertices__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
