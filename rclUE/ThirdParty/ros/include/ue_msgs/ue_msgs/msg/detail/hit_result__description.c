// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from ue_msgs:msg/HitResult.idl
// generated code does not contain a copyright notice

#include "ue_msgs/msg/detail/hit_result__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_ue_msgs
const rosidl_type_hash_t *
ue_msgs__msg__HitResult__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x12, 0x38, 0xdc, 0x33, 0x3c, 0xa3, 0x43, 0xa1,
      0xd2, 0xa0, 0x26, 0x37, 0x3e, 0xd0, 0xd2, 0xf1,
      0x5e, 0x1b, 0xdc, 0x6a, 0x1e, 0x47, 0x12, 0x05,
      0x0b, 0x9e, 0xd1, 0x31, 0xe1, 0x23, 0x4b, 0x26,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "geometry_msgs/msg/detail/point__functions.h"
#include "geometry_msgs/msg/detail/vector3__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t geometry_msgs__msg__Point__EXPECTED_HASH = {1, {
    0x69, 0x63, 0x08, 0x48, 0x42, 0xa9, 0xb0, 0x44,
    0x94, 0xd6, 0xb2, 0x94, 0x1d, 0x11, 0x44, 0x47,
    0x08, 0xd8, 0x92, 0xda, 0x2f, 0x4b, 0x09, 0x84,
    0x3b, 0x9c, 0x43, 0xf4, 0x2a, 0x7f, 0x68, 0x81,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Vector3__EXPECTED_HASH = {1, {
    0xcc, 0x12, 0xfe, 0x83, 0xe4, 0xc0, 0x27, 0x19,
    0xf1, 0xce, 0x80, 0x70, 0xbf, 0xd1, 0x4a, 0xec,
    0xd4, 0x0f, 0x75, 0xa9, 0x66, 0x96, 0xa6, 0x7a,
    0x2a, 0x1f, 0x37, 0xf7, 0xdb, 0xb0, 0x76, 0x5d,
  }};
#endif

static char ue_msgs__msg__HitResult__TYPE_NAME[] = "ue_msgs/msg/HitResult";
static char geometry_msgs__msg__Point__TYPE_NAME[] = "geometry_msgs/msg/Point";
static char geometry_msgs__msg__Vector3__TYPE_NAME[] = "geometry_msgs/msg/Vector3";

// Define type names, field names, and default values
static char ue_msgs__msg__HitResult__FIELD_NAME__blocking_hit[] = "blocking_hit";
static char ue_msgs__msg__HitResult__FIELD_NAME__start_penetrating[] = "start_penetrating";
static char ue_msgs__msg__HitResult__FIELD_NAME__component_name[] = "component_name";
static char ue_msgs__msg__HitResult__FIELD_NAME__actor_name[] = "actor_name";
static char ue_msgs__msg__HitResult__FIELD_NAME__distance[] = "distance";
static char ue_msgs__msg__HitResult__FIELD_NAME__element_index[] = "element_index";
static char ue_msgs__msg__HitResult__FIELD_NAME__face_index[] = "face_index";
static char ue_msgs__msg__HitResult__FIELD_NAME__bone_name[] = "bone_name";
static char ue_msgs__msg__HitResult__FIELD_NAME__item[] = "item";
static char ue_msgs__msg__HitResult__FIELD_NAME__my_bone_name[] = "my_bone_name";
static char ue_msgs__msg__HitResult__FIELD_NAME__my_item[] = "my_item";
static char ue_msgs__msg__HitResult__FIELD_NAME__penetration_depth[] = "penetration_depth";
static char ue_msgs__msg__HitResult__FIELD_NAME__physics_material_name[] = "physics_material_name";
static char ue_msgs__msg__HitResult__FIELD_NAME__time[] = "time";
static char ue_msgs__msg__HitResult__FIELD_NAME__impact_normal[] = "impact_normal";
static char ue_msgs__msg__HitResult__FIELD_NAME__impact_point[] = "impact_point";
static char ue_msgs__msg__HitResult__FIELD_NAME__normal[] = "normal";
static char ue_msgs__msg__HitResult__FIELD_NAME__location[] = "location";
static char ue_msgs__msg__HitResult__FIELD_NAME__trace_start[] = "trace_start";
static char ue_msgs__msg__HitResult__FIELD_NAME__trace_end[] = "trace_end";

static rosidl_runtime_c__type_description__Field ue_msgs__msg__HitResult__FIELDS[] = {
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__blocking_hit, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__start_penetrating, 17, 17},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__component_name, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__actor_name, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__distance, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__element_index, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__face_index, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__bone_name, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__item, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__my_bone_name, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__my_item, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__penetration_depth, 17, 17},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__physics_material_name, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__time, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__impact_normal, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__impact_point, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__normal, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__location, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__trace_start, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
  {
    {ue_msgs__msg__HitResult__FIELD_NAME__trace_end, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription ue_msgs__msg__HitResult__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
ue_msgs__msg__HitResult__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {ue_msgs__msg__HitResult__TYPE_NAME, 21, 21},
      {ue_msgs__msg__HitResult__FIELDS, 20, 20},
    },
    {ue_msgs__msg__HitResult__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&geometry_msgs__msg__Point__EXPECTED_HASH, geometry_msgs__msg__Point__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = geometry_msgs__msg__Point__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# HitResult from UnrealEngine\n"
  "# ref https://docs.unrealengine.com/5.1/en-US/API/Runtime/Engine/Engine/FHitResult/\n"
  "\n"
  "bool blocking_hit\n"
  "bool start_penetrating\n"
  "string component_name\n"
  "string actor_name\n"
  "float32 distance\n"
  "uint8 element_index\n"
  "int32 face_index\n"
  "string bone_name\n"
  "int32 item\n"
  "string my_bone_name\n"
  "int32 my_item\n"
  "float32 penetration_depth\n"
  "string physics_material_name\n"
  "float32 time\n"
  "geometry_msgs/Vector3 impact_normal\n"
  "geometry_msgs/Point impact_point\n"
  "geometry_msgs/Vector3 normal\n"
  "geometry_msgs/Point location\n"
  "geometry_msgs/Point trace_start\n"
  "geometry_msgs/Point trace_end\n"
  "";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
ue_msgs__msg__HitResult__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {ue_msgs__msg__HitResult__TYPE_NAME, 21, 21},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 570, 570},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
ue_msgs__msg__HitResult__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *ue_msgs__msg__HitResult__get_individual_type_description_source(NULL),
    sources[1] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
