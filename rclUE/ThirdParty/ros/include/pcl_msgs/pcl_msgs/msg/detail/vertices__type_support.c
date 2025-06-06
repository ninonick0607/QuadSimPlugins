// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from pcl_msgs:msg/Vertices.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "pcl_msgs/msg/detail/vertices__rosidl_typesupport_introspection_c.h"
#include "pcl_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "pcl_msgs/msg/detail/vertices__functions.h"
#include "pcl_msgs/msg/detail/vertices__struct.h"


// Include directives for member types
// Member `vertices`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__Vertices_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  pcl_msgs__msg__Vertices__init(message_memory);
}

void pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__Vertices_fini_function(void * message_memory)
{
  pcl_msgs__msg__Vertices__fini(message_memory);
}

size_t pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__size_function__Vertices__vertices(
  const void * untyped_member)
{
  const rosidl_runtime_c__uint32__Sequence * member =
    (const rosidl_runtime_c__uint32__Sequence *)(untyped_member);
  return member->size;
}

const void * pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__get_const_function__Vertices__vertices(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__uint32__Sequence * member =
    (const rosidl_runtime_c__uint32__Sequence *)(untyped_member);
  return &member->data[index];
}

void * pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__get_function__Vertices__vertices(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__uint32__Sequence * member =
    (rosidl_runtime_c__uint32__Sequence *)(untyped_member);
  return &member->data[index];
}

void pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__fetch_function__Vertices__vertices(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint32_t * item =
    ((const uint32_t *)
    pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__get_const_function__Vertices__vertices(untyped_member, index));
  uint32_t * value =
    (uint32_t *)(untyped_value);
  *value = *item;
}

void pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__assign_function__Vertices__vertices(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint32_t * item =
    ((uint32_t *)
    pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__get_function__Vertices__vertices(untyped_member, index));
  const uint32_t * value =
    (const uint32_t *)(untyped_value);
  *item = *value;
}

bool pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__resize_function__Vertices__vertices(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__uint32__Sequence * member =
    (rosidl_runtime_c__uint32__Sequence *)(untyped_member);
  rosidl_runtime_c__uint32__Sequence__fini(member);
  return rosidl_runtime_c__uint32__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__Vertices_message_member_array[1] = {
  {
    "vertices",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(pcl_msgs__msg__Vertices, vertices),  // bytes offset in struct
    NULL,  // default value
    pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__size_function__Vertices__vertices,  // size() function pointer
    pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__get_const_function__Vertices__vertices,  // get_const(index) function pointer
    pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__get_function__Vertices__vertices,  // get(index) function pointer
    pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__fetch_function__Vertices__vertices,  // fetch(index, &value) function pointer
    pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__assign_function__Vertices__vertices,  // assign(index, value) function pointer
    pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__resize_function__Vertices__vertices  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__Vertices_message_members = {
  "pcl_msgs__msg",  // message namespace
  "Vertices",  // message name
  1,  // number of fields
  sizeof(pcl_msgs__msg__Vertices),
  false,  // has_any_key_member_
  pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__Vertices_message_member_array,  // message members
  pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__Vertices_init_function,  // function to initialize message memory (memory has to be allocated)
  pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__Vertices_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__Vertices_message_type_support_handle = {
  0,
  &pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__Vertices_message_members,
  get_message_typesupport_handle_function,
  &pcl_msgs__msg__Vertices__get_type_hash,
  &pcl_msgs__msg__Vertices__get_type_description,
  &pcl_msgs__msg__Vertices__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_pcl_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, pcl_msgs, msg, Vertices)() {
  if (!pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__Vertices_message_type_support_handle.typesupport_identifier) {
    pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__Vertices_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &pcl_msgs__msg__Vertices__rosidl_typesupport_introspection_c__Vertices_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
