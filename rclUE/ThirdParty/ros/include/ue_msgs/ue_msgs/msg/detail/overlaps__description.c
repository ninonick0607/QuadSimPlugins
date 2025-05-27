// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from ue_msgs:msg/Overlaps.idl
// generated code does not contain a copyright notice

#include "ue_msgs/msg/detail/overlaps__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_ue_msgs
const rosidl_type_hash_t *
ue_msgs__msg__Overlaps__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x4a, 0x1d, 0x3b, 0x37, 0x5b, 0x73, 0xf1, 0x18,
      0xe4, 0x6a, 0x35, 0x40, 0xe2, 0x77, 0x2f, 0x43,
      0xdc, 0xba, 0x29, 0x2e, 0x75, 0x2a, 0xba, 0x12,
      0xa8, 0x2a, 0x1f, 0x0a, 0x46, 0xce, 0x63, 0x02,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "ue_msgs/msg/detail/overlapping_objects__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t ue_msgs__msg__OverlappingObjects__EXPECTED_HASH = {1, {
    0x6b, 0x34, 0x85, 0x54, 0x32, 0xac, 0x3e, 0xc3,
    0xa1, 0x37, 0x75, 0xc8, 0xa4, 0x50, 0xa4, 0xc7,
    0x33, 0x17, 0x6c, 0x20, 0xeb, 0x01, 0x51, 0x9b,
    0x25, 0xa2, 0x77, 0xa8, 0xff, 0x09, 0xe2, 0x74,
  }};
#endif

static char ue_msgs__msg__Overlaps__TYPE_NAME[] = "ue_msgs/msg/Overlaps";
static char ue_msgs__msg__OverlappingObjects__TYPE_NAME[] = "ue_msgs/msg/OverlappingObjects";

// Define type names, field names, and default values
static char ue_msgs__msg__Overlaps__FIELD_NAME__targets[] = "targets";
static char ue_msgs__msg__Overlaps__FIELD_NAME__overlaps[] = "overlaps";

static rosidl_runtime_c__type_description__Field ue_msgs__msg__Overlaps__FIELDS[] = {
  {
    {ue_msgs__msg__Overlaps__FIELD_NAME__targets, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__Overlaps__FIELD_NAME__overlaps, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {ue_msgs__msg__OverlappingObjects__TYPE_NAME, 30, 30},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription ue_msgs__msg__Overlaps__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {ue_msgs__msg__OverlappingObjects__TYPE_NAME, 30, 30},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
ue_msgs__msg__Overlaps__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {ue_msgs__msg__Overlaps__TYPE_NAME, 20, 20},
      {ue_msgs__msg__Overlaps__FIELDS, 2, 2},
    },
    {ue_msgs__msg__Overlaps__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&ue_msgs__msg__OverlappingObjects__EXPECTED_HASH, ue_msgs__msg__OverlappingObjects__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = ue_msgs__msg__OverlappingObjects__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string[] targets\n"
  "ue_msgs/OverlappingObjects[] overlaps";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
ue_msgs__msg__Overlaps__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {ue_msgs__msg__Overlaps__TYPE_NAME, 20, 20},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 54, 54},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
ue_msgs__msg__Overlaps__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *ue_msgs__msg__Overlaps__get_individual_type_description_source(NULL),
    sources[1] = *ue_msgs__msg__OverlappingObjects__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
