// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from action_msgs:msg/GoalStatusArray.idl
// generated code does not contain a copyright notice

#include "action_msgs/msg/detail/goal_status_array__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_action_msgs
const rosidl_type_hash_t *
action_msgs__msg__GoalStatusArray__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x6c, 0x16, 0x84, 0xb0, 0x0f, 0x17, 0x7d, 0x37,
      0x43, 0x8f, 0xeb, 0xe6, 0xe7, 0x09, 0xfc, 0x4e,
      0x2b, 0x0d, 0x42, 0x48, 0xdc, 0xa4, 0x85, 0x49,
      0x46, 0xf9, 0xed, 0x8b, 0x30, 0xcd, 0xa8, 0x3e,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "action_msgs/msg/detail/goal_info__functions.h"
#include "unique_identifier_msgs/msg/detail/uuid__functions.h"
#include "action_msgs/msg/detail/goal_status__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t action_msgs__msg__GoalInfo__EXPECTED_HASH = {1, {
    0x63, 0x98, 0xfe, 0x76, 0x31, 0x54, 0x55, 0x43,
    0x53, 0x93, 0x07, 0x16, 0xb2, 0x25, 0x94, 0x7f,
    0x93, 0xb6, 0x72, 0xf0, 0xfb, 0x2e, 0x49, 0xfd,
    0xd0, 0x1b, 0xb7, 0xa7, 0xe3, 0x79, 0x33, 0xe9,
  }};
static const rosidl_type_hash_t action_msgs__msg__GoalStatus__EXPECTED_HASH = {1, {
    0x32, 0xf4, 0xcf, 0xd7, 0x17, 0x73, 0x5d, 0x17,
    0x65, 0x7e, 0x11, 0x78, 0xf2, 0x44, 0x31, 0xc1,
    0xce, 0x99, 0x6c, 0x87, 0x8c, 0x51, 0x52, 0x30,
    0xf6, 0xc5, 0xb3, 0x47, 0x68, 0x19, 0xdb, 0xb9,
  }};
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t unique_identifier_msgs__msg__UUID__EXPECTED_HASH = {1, {
    0x1b, 0x8e, 0x8a, 0xca, 0x95, 0x8c, 0xbe, 0xa2,
    0x8f, 0xe6, 0xef, 0x60, 0xbf, 0x6c, 0x19, 0xb6,
    0x83, 0xc9, 0x7a, 0x9e, 0xf6, 0x0b, 0xb3, 0x47,
    0x52, 0x06, 0x7d, 0x0f, 0x2f, 0x7a, 0xb4, 0x37,
  }};
#endif

static char action_msgs__msg__GoalStatusArray__TYPE_NAME[] = "action_msgs/msg/GoalStatusArray";
static char action_msgs__msg__GoalInfo__TYPE_NAME[] = "action_msgs/msg/GoalInfo";
static char action_msgs__msg__GoalStatus__TYPE_NAME[] = "action_msgs/msg/GoalStatus";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char unique_identifier_msgs__msg__UUID__TYPE_NAME[] = "unique_identifier_msgs/msg/UUID";

// Define type names, field names, and default values
static char action_msgs__msg__GoalStatusArray__FIELD_NAME__status_list[] = "status_list";

static rosidl_runtime_c__type_description__Field action_msgs__msg__GoalStatusArray__FIELDS[] = {
  {
    {action_msgs__msg__GoalStatusArray__FIELD_NAME__status_list, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {action_msgs__msg__GoalStatus__TYPE_NAME, 26, 26},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription action_msgs__msg__GoalStatusArray__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {action_msgs__msg__GoalInfo__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {action_msgs__msg__GoalStatus__TYPE_NAME, 26, 26},
    {NULL, 0, 0},
  },
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {unique_identifier_msgs__msg__UUID__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
action_msgs__msg__GoalStatusArray__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {action_msgs__msg__GoalStatusArray__TYPE_NAME, 31, 31},
      {action_msgs__msg__GoalStatusArray__FIELDS, 1, 1},
    },
    {action_msgs__msg__GoalStatusArray__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&action_msgs__msg__GoalInfo__EXPECTED_HASH, action_msgs__msg__GoalInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = action_msgs__msg__GoalInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&action_msgs__msg__GoalStatus__EXPECTED_HASH, action_msgs__msg__GoalStatus__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = action_msgs__msg__GoalStatus__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&unique_identifier_msgs__msg__UUID__EXPECTED_HASH, unique_identifier_msgs__msg__UUID__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = unique_identifier_msgs__msg__UUID__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# An array of goal statuses.\n"
  "GoalStatus[] status_list";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
action_msgs__msg__GoalStatusArray__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {action_msgs__msg__GoalStatusArray__TYPE_NAME, 31, 31},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 54, 54},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
action_msgs__msg__GoalStatusArray__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *action_msgs__msg__GoalStatusArray__get_individual_type_description_source(NULL),
    sources[1] = *action_msgs__msg__GoalInfo__get_individual_type_description_source(NULL);
    sources[2] = *action_msgs__msg__GoalStatus__get_individual_type_description_source(NULL);
    sources[3] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[4] = *unique_identifier_msgs__msg__UUID__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
