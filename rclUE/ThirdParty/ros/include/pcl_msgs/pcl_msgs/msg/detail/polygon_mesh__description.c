// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from pcl_msgs:msg/PolygonMesh.idl
// generated code does not contain a copyright notice

#include "pcl_msgs/msg/detail/polygon_mesh__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_pcl_msgs
const rosidl_type_hash_t *
pcl_msgs__msg__PolygonMesh__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x3b, 0x0f, 0xba, 0xc5, 0x94, 0x0a, 0xb3, 0x7b,
      0xa3, 0xb5, 0x80, 0x21, 0x48, 0xf3, 0xcf, 0xd4,
      0xb3, 0xe0, 0x6f, 0x49, 0x4e, 0x09, 0x53, 0x7d,
      0xce, 0xe8, 0x9a, 0xb1, 0x03, 0x4b, 0x71, 0xf2,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "std_msgs/msg/detail/header__functions.h"
#include "sensor_msgs/msg/detail/point_cloud2__functions.h"
#include "sensor_msgs/msg/detail/point_field__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "pcl_msgs/msg/detail/vertices__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t pcl_msgs__msg__Vertices__EXPECTED_HASH = {1, {
    0x4b, 0xe2, 0x37, 0x4c, 0x99, 0xf6, 0xf9, 0xbb,
    0x80, 0x91, 0xec, 0xd1, 0x88, 0x18, 0xb6, 0x8f,
    0x83, 0x09, 0x94, 0x05, 0x31, 0x0e, 0xd1, 0x4e,
    0x4d, 0xdc, 0x4e, 0x0e, 0x89, 0x31, 0xd7, 0xbb,
  }};
static const rosidl_type_hash_t sensor_msgs__msg__PointCloud2__EXPECTED_HASH = {1, {
    0x91, 0x98, 0xca, 0xbf, 0x7d, 0xa3, 0x79, 0x6a,
    0xe6, 0xfe, 0x19, 0xc4, 0xcb, 0x3b, 0xdd, 0x35,
    0x25, 0x49, 0x29, 0x88, 0xc7, 0x05, 0x22, 0x62,
    0x8a, 0xf5, 0xda, 0xa1, 0x24, 0xba, 0xe2, 0xb5,
  }};
static const rosidl_type_hash_t sensor_msgs__msg__PointField__EXPECTED_HASH = {1, {
    0x5c, 0x6a, 0x47, 0x50, 0x72, 0x8c, 0x2b, 0xcf,
    0xbb, 0xf7, 0x03, 0x72, 0x25, 0xb2, 0x0b, 0x02,
    0xd4, 0x42, 0x96, 0x34, 0x73, 0x21, 0x46, 0xb7,
    0x42, 0xde, 0xe1, 0x72, 0x66, 0x37, 0xef, 0x01,
  }};
static const rosidl_type_hash_t std_msgs__msg__Header__EXPECTED_HASH = {1, {
    0xf4, 0x9f, 0xb3, 0xae, 0x2c, 0xf0, 0x70, 0xf7,
    0x93, 0x64, 0x5f, 0xf7, 0x49, 0x68, 0x3a, 0xc6,
    0xb0, 0x62, 0x03, 0xe4, 0x1c, 0x89, 0x1e, 0x17,
    0x70, 0x1b, 0x1c, 0xb5, 0x97, 0xce, 0x6a, 0x01,
  }};
#endif

static char pcl_msgs__msg__PolygonMesh__TYPE_NAME[] = "pcl_msgs/msg/PolygonMesh";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char pcl_msgs__msg__Vertices__TYPE_NAME[] = "pcl_msgs/msg/Vertices";
static char sensor_msgs__msg__PointCloud2__TYPE_NAME[] = "sensor_msgs/msg/PointCloud2";
static char sensor_msgs__msg__PointField__TYPE_NAME[] = "sensor_msgs/msg/PointField";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char pcl_msgs__msg__PolygonMesh__FIELD_NAME__header[] = "header";
static char pcl_msgs__msg__PolygonMesh__FIELD_NAME__cloud[] = "cloud";
static char pcl_msgs__msg__PolygonMesh__FIELD_NAME__polygons[] = "polygons";

static rosidl_runtime_c__type_description__Field pcl_msgs__msg__PolygonMesh__FIELDS[] = {
  {
    {pcl_msgs__msg__PolygonMesh__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {pcl_msgs__msg__PolygonMesh__FIELD_NAME__cloud, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {sensor_msgs__msg__PointCloud2__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {pcl_msgs__msg__PolygonMesh__FIELD_NAME__polygons, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {pcl_msgs__msg__Vertices__TYPE_NAME, 21, 21},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription pcl_msgs__msg__PolygonMesh__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {pcl_msgs__msg__Vertices__TYPE_NAME, 21, 21},
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__PointCloud2__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__PointField__TYPE_NAME, 26, 26},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
pcl_msgs__msg__PolygonMesh__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {pcl_msgs__msg__PolygonMesh__TYPE_NAME, 24, 24},
      {pcl_msgs__msg__PolygonMesh__FIELDS, 3, 3},
    },
    {pcl_msgs__msg__PolygonMesh__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&pcl_msgs__msg__Vertices__EXPECTED_HASH, pcl_msgs__msg__Vertices__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = pcl_msgs__msg__Vertices__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&sensor_msgs__msg__PointCloud2__EXPECTED_HASH, sensor_msgs__msg__PointCloud2__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = sensor_msgs__msg__PointCloud2__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&sensor_msgs__msg__PointField__EXPECTED_HASH, sensor_msgs__msg__PointField__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = sensor_msgs__msg__PointField__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Separate header for the polygonal surface\n"
  "std_msgs/Header header\n"
  "# Vertices of the mesh as a point cloud\n"
  "sensor_msgs/PointCloud2 cloud\n"
  "# List of polygons\n"
  "Vertices[] polygons";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
pcl_msgs__msg__PolygonMesh__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {pcl_msgs__msg__PolygonMesh__TYPE_NAME, 24, 24},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 176, 176},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
pcl_msgs__msg__PolygonMesh__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *pcl_msgs__msg__PolygonMesh__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *pcl_msgs__msg__Vertices__get_individual_type_description_source(NULL);
    sources[3] = *sensor_msgs__msg__PointCloud2__get_individual_type_description_source(NULL);
    sources[4] = *sensor_msgs__msg__PointField__get_individual_type_description_source(NULL);
    sources[5] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
