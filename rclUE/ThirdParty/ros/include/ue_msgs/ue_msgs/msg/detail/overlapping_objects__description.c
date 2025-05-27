// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from ue_msgs:msg/OverlappingObjects.idl
// generated code does not contain a copyright notice

#include "ue_msgs/msg/detail/overlapping_objects__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_ue_msgs
const rosidl_type_hash_t *
ue_msgs__msg__OverlappingObjects__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x6b, 0x34, 0x85, 0x54, 0x32, 0xac, 0x3e, 0xc3,
      0xa1, 0x37, 0x75, 0xc8, 0xa4, 0x50, 0xa4, 0xc7,
      0x33, 0x17, 0x6c, 0x20, 0xeb, 0x01, 0x51, 0x9b,
      0x25, 0xa2, 0x77, 0xa8, 0xff, 0x09, 0xe2, 0x74,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char ue_msgs__msg__OverlappingObjects__TYPE_NAME[] = "ue_msgs/msg/OverlappingObjects";

// Define type names, field names, and default values
static char ue_msgs__msg__OverlappingObjects__FIELD_NAME__actors[] = "actors";
static char ue_msgs__msg__OverlappingObjects__FIELD_NAME__components[] = "components";

static rosidl_runtime_c__type_description__Field ue_msgs__msg__OverlappingObjects__FIELDS[] = {
  {
    {ue_msgs__msg__OverlappingObjects__FIELD_NAME__actors, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__OverlappingObjects__FIELD_NAME__components, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
ue_msgs__msg__OverlappingObjects__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {ue_msgs__msg__OverlappingObjects__TYPE_NAME, 30, 30},
      {ue_msgs__msg__OverlappingObjects__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string[] actors\n"
  "string[] components";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
ue_msgs__msg__OverlappingObjects__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {ue_msgs__msg__OverlappingObjects__TYPE_NAME, 30, 30},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 35, 35},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
ue_msgs__msg__OverlappingObjects__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *ue_msgs__msg__OverlappingObjects__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
