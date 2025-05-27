// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from ue_msgs:srv/SpawnEntities.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "ue_msgs/srv/detail/spawn_entities__rosidl_typesupport_introspection_c.h"
#include "ue_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "ue_msgs/srv/detail/spawn_entities__functions.h"
#include "ue_msgs/srv/detail/spawn_entities__struct.h"


// Include directives for member types
// Member `type`
// Member `tags`
// Member `json_parameters`
#include "rosidl_runtime_c/string_functions.h"
// Member `state`
#include "ue_msgs/msg/entity_state.h"
// Member `state`
#include "ue_msgs/msg/detail/entity_state__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  ue_msgs__srv__SpawnEntities_Request__init(message_memory);
}

void ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_fini_function(void * message_memory)
{
  ue_msgs__srv__SpawnEntities_Request__fini(message_memory);
}

size_t ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__size_function__SpawnEntities_Request__type(
  const void * untyped_member)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return member->size;
}

const void * ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Request__type(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Request__type(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__fetch_function__SpawnEntities_Request__type(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Request__type(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__assign_function__SpawnEntities_Request__type(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Request__type(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

bool ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__resize_function__SpawnEntities_Request__type(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  rosidl_runtime_c__String__Sequence__fini(member);
  return rosidl_runtime_c__String__Sequence__init(member, size);
}

size_t ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__size_function__SpawnEntities_Request__state(
  const void * untyped_member)
{
  const ue_msgs__msg__EntityState__Sequence * member =
    (const ue_msgs__msg__EntityState__Sequence *)(untyped_member);
  return member->size;
}

const void * ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Request__state(
  const void * untyped_member, size_t index)
{
  const ue_msgs__msg__EntityState__Sequence * member =
    (const ue_msgs__msg__EntityState__Sequence *)(untyped_member);
  return &member->data[index];
}

void * ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Request__state(
  void * untyped_member, size_t index)
{
  ue_msgs__msg__EntityState__Sequence * member =
    (ue_msgs__msg__EntityState__Sequence *)(untyped_member);
  return &member->data[index];
}

void ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__fetch_function__SpawnEntities_Request__state(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const ue_msgs__msg__EntityState * item =
    ((const ue_msgs__msg__EntityState *)
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Request__state(untyped_member, index));
  ue_msgs__msg__EntityState * value =
    (ue_msgs__msg__EntityState *)(untyped_value);
  *value = *item;
}

void ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__assign_function__SpawnEntities_Request__state(
  void * untyped_member, size_t index, const void * untyped_value)
{
  ue_msgs__msg__EntityState * item =
    ((ue_msgs__msg__EntityState *)
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Request__state(untyped_member, index));
  const ue_msgs__msg__EntityState * value =
    (const ue_msgs__msg__EntityState *)(untyped_value);
  *item = *value;
}

bool ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__resize_function__SpawnEntities_Request__state(
  void * untyped_member, size_t size)
{
  ue_msgs__msg__EntityState__Sequence * member =
    (ue_msgs__msg__EntityState__Sequence *)(untyped_member);
  ue_msgs__msg__EntityState__Sequence__fini(member);
  return ue_msgs__msg__EntityState__Sequence__init(member, size);
}

size_t ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__size_function__SpawnEntities_Request__tags(
  const void * untyped_member)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return member->size;
}

const void * ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Request__tags(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Request__tags(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__fetch_function__SpawnEntities_Request__tags(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Request__tags(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__assign_function__SpawnEntities_Request__tags(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Request__tags(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

bool ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__resize_function__SpawnEntities_Request__tags(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  rosidl_runtime_c__String__Sequence__fini(member);
  return rosidl_runtime_c__String__Sequence__init(member, size);
}

size_t ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__size_function__SpawnEntities_Request__json_parameters(
  const void * untyped_member)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return member->size;
}

const void * ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Request__json_parameters(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Request__json_parameters(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__fetch_function__SpawnEntities_Request__json_parameters(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Request__json_parameters(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__assign_function__SpawnEntities_Request__json_parameters(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Request__json_parameters(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

bool ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__resize_function__SpawnEntities_Request__json_parameters(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  rosidl_runtime_c__String__Sequence__fini(member);
  return rosidl_runtime_c__String__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_message_member_array[4] = {
  {
    "type",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ue_msgs__srv__SpawnEntities_Request, type),  // bytes offset in struct
    NULL,  // default value
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__size_function__SpawnEntities_Request__type,  // size() function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Request__type,  // get_const(index) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Request__type,  // get(index) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__fetch_function__SpawnEntities_Request__type,  // fetch(index, &value) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__assign_function__SpawnEntities_Request__type,  // assign(index, value) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__resize_function__SpawnEntities_Request__type  // resize(index) function pointer
  },
  {
    "state",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ue_msgs__srv__SpawnEntities_Request, state),  // bytes offset in struct
    NULL,  // default value
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__size_function__SpawnEntities_Request__state,  // size() function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Request__state,  // get_const(index) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Request__state,  // get(index) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__fetch_function__SpawnEntities_Request__state,  // fetch(index, &value) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__assign_function__SpawnEntities_Request__state,  // assign(index, value) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__resize_function__SpawnEntities_Request__state  // resize(index) function pointer
  },
  {
    "tags",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ue_msgs__srv__SpawnEntities_Request, tags),  // bytes offset in struct
    NULL,  // default value
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__size_function__SpawnEntities_Request__tags,  // size() function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Request__tags,  // get_const(index) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Request__tags,  // get(index) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__fetch_function__SpawnEntities_Request__tags,  // fetch(index, &value) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__assign_function__SpawnEntities_Request__tags,  // assign(index, value) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__resize_function__SpawnEntities_Request__tags  // resize(index) function pointer
  },
  {
    "json_parameters",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ue_msgs__srv__SpawnEntities_Request, json_parameters),  // bytes offset in struct
    NULL,  // default value
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__size_function__SpawnEntities_Request__json_parameters,  // size() function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Request__json_parameters,  // get_const(index) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Request__json_parameters,  // get(index) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__fetch_function__SpawnEntities_Request__json_parameters,  // fetch(index, &value) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__assign_function__SpawnEntities_Request__json_parameters,  // assign(index, value) function pointer
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__resize_function__SpawnEntities_Request__json_parameters  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_message_members = {
  "ue_msgs__srv",  // message namespace
  "SpawnEntities_Request",  // message name
  4,  // number of fields
  sizeof(ue_msgs__srv__SpawnEntities_Request),
  false,  // has_any_key_member_
  ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_message_member_array,  // message members
  ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_message_type_support_handle = {
  0,
  &ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_message_members,
  get_message_typesupport_handle_function,
  &ue_msgs__srv__SpawnEntities_Request__get_type_hash,
  &ue_msgs__srv__SpawnEntities_Request__get_type_description,
  &ue_msgs__srv__SpawnEntities_Request__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_ue_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, srv, SpawnEntities_Request)() {
  ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, msg, EntityState)();
  if (!ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_message_type_support_handle.typesupport_identifier) {
    ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "ue_msgs/srv/detail/spawn_entities__rosidl_typesupport_introspection_c.h"
// already included above
// #include "ue_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "ue_msgs/srv/detail/spawn_entities__functions.h"
// already included above
// #include "ue_msgs/srv/detail/spawn_entities__struct.h"


// Include directives for member types
// Member `status_message`
// already included above
// #include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  ue_msgs__srv__SpawnEntities_Response__init(message_memory);
}

void ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_fini_function(void * message_memory)
{
  ue_msgs__srv__SpawnEntities_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_message_member_array[2] = {
  {
    "success",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ue_msgs__srv__SpawnEntities_Response, success),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "status_message",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ue_msgs__srv__SpawnEntities_Response, status_message),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_message_members = {
  "ue_msgs__srv",  // message namespace
  "SpawnEntities_Response",  // message name
  2,  // number of fields
  sizeof(ue_msgs__srv__SpawnEntities_Response),
  false,  // has_any_key_member_
  ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_message_member_array,  // message members
  ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_message_type_support_handle = {
  0,
  &ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_message_members,
  get_message_typesupport_handle_function,
  &ue_msgs__srv__SpawnEntities_Response__get_type_hash,
  &ue_msgs__srv__SpawnEntities_Response__get_type_description,
  &ue_msgs__srv__SpawnEntities_Response__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_ue_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, srv, SpawnEntities_Response)() {
  if (!ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_message_type_support_handle.typesupport_identifier) {
    ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "ue_msgs/srv/detail/spawn_entities__rosidl_typesupport_introspection_c.h"
// already included above
// #include "ue_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "ue_msgs/srv/detail/spawn_entities__functions.h"
// already included above
// #include "ue_msgs/srv/detail/spawn_entities__struct.h"


// Include directives for member types
// Member `info`
#include "service_msgs/msg/service_event_info.h"
// Member `info`
#include "service_msgs/msg/detail/service_event_info__rosidl_typesupport_introspection_c.h"
// Member `request`
// Member `response`
#include "ue_msgs/srv/spawn_entities.h"
// Member `request`
// Member `response`
// already included above
// #include "ue_msgs/srv/detail/spawn_entities__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  ue_msgs__srv__SpawnEntities_Event__init(message_memory);
}

void ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_fini_function(void * message_memory)
{
  ue_msgs__srv__SpawnEntities_Event__fini(message_memory);
}

size_t ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__size_function__SpawnEntities_Event__request(
  const void * untyped_member)
{
  const ue_msgs__srv__SpawnEntities_Request__Sequence * member =
    (const ue_msgs__srv__SpawnEntities_Request__Sequence *)(untyped_member);
  return member->size;
}

const void * ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Event__request(
  const void * untyped_member, size_t index)
{
  const ue_msgs__srv__SpawnEntities_Request__Sequence * member =
    (const ue_msgs__srv__SpawnEntities_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void * ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Event__request(
  void * untyped_member, size_t index)
{
  ue_msgs__srv__SpawnEntities_Request__Sequence * member =
    (ue_msgs__srv__SpawnEntities_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__fetch_function__SpawnEntities_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const ue_msgs__srv__SpawnEntities_Request * item =
    ((const ue_msgs__srv__SpawnEntities_Request *)
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Event__request(untyped_member, index));
  ue_msgs__srv__SpawnEntities_Request * value =
    (ue_msgs__srv__SpawnEntities_Request *)(untyped_value);
  *value = *item;
}

void ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__assign_function__SpawnEntities_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  ue_msgs__srv__SpawnEntities_Request * item =
    ((ue_msgs__srv__SpawnEntities_Request *)
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Event__request(untyped_member, index));
  const ue_msgs__srv__SpawnEntities_Request * value =
    (const ue_msgs__srv__SpawnEntities_Request *)(untyped_value);
  *item = *value;
}

bool ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__resize_function__SpawnEntities_Event__request(
  void * untyped_member, size_t size)
{
  ue_msgs__srv__SpawnEntities_Request__Sequence * member =
    (ue_msgs__srv__SpawnEntities_Request__Sequence *)(untyped_member);
  ue_msgs__srv__SpawnEntities_Request__Sequence__fini(member);
  return ue_msgs__srv__SpawnEntities_Request__Sequence__init(member, size);
}

size_t ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__size_function__SpawnEntities_Event__response(
  const void * untyped_member)
{
  const ue_msgs__srv__SpawnEntities_Response__Sequence * member =
    (const ue_msgs__srv__SpawnEntities_Response__Sequence *)(untyped_member);
  return member->size;
}

const void * ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Event__response(
  const void * untyped_member, size_t index)
{
  const ue_msgs__srv__SpawnEntities_Response__Sequence * member =
    (const ue_msgs__srv__SpawnEntities_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void * ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Event__response(
  void * untyped_member, size_t index)
{
  ue_msgs__srv__SpawnEntities_Response__Sequence * member =
    (ue_msgs__srv__SpawnEntities_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__fetch_function__SpawnEntities_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const ue_msgs__srv__SpawnEntities_Response * item =
    ((const ue_msgs__srv__SpawnEntities_Response *)
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Event__response(untyped_member, index));
  ue_msgs__srv__SpawnEntities_Response * value =
    (ue_msgs__srv__SpawnEntities_Response *)(untyped_value);
  *value = *item;
}

void ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__assign_function__SpawnEntities_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  ue_msgs__srv__SpawnEntities_Response * item =
    ((ue_msgs__srv__SpawnEntities_Response *)
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Event__response(untyped_member, index));
  const ue_msgs__srv__SpawnEntities_Response * value =
    (const ue_msgs__srv__SpawnEntities_Response *)(untyped_value);
  *item = *value;
}

bool ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__resize_function__SpawnEntities_Event__response(
  void * untyped_member, size_t size)
{
  ue_msgs__srv__SpawnEntities_Response__Sequence * member =
    (ue_msgs__srv__SpawnEntities_Response__Sequence *)(untyped_member);
  ue_msgs__srv__SpawnEntities_Response__Sequence__fini(member);
  return ue_msgs__srv__SpawnEntities_Response__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_message_member_array[3] = {
  {
    "info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ue_msgs__srv__SpawnEntities_Event, info),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "request",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(ue_msgs__srv__SpawnEntities_Event, request),  // bytes offset in struct
    NULL,  // default value
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__size_function__SpawnEntities_Event__request,  // size() function pointer
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Event__request,  // get_const(index) function pointer
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Event__request,  // get(index) function pointer
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__fetch_function__SpawnEntities_Event__request,  // fetch(index, &value) function pointer
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__assign_function__SpawnEntities_Event__request,  // assign(index, value) function pointer
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__resize_function__SpawnEntities_Event__request  // resize(index) function pointer
  },
  {
    "response",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(ue_msgs__srv__SpawnEntities_Event, response),  // bytes offset in struct
    NULL,  // default value
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__size_function__SpawnEntities_Event__response,  // size() function pointer
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__get_const_function__SpawnEntities_Event__response,  // get_const(index) function pointer
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__get_function__SpawnEntities_Event__response,  // get(index) function pointer
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__fetch_function__SpawnEntities_Event__response,  // fetch(index, &value) function pointer
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__assign_function__SpawnEntities_Event__response,  // assign(index, value) function pointer
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__resize_function__SpawnEntities_Event__response  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_message_members = {
  "ue_msgs__srv",  // message namespace
  "SpawnEntities_Event",  // message name
  3,  // number of fields
  sizeof(ue_msgs__srv__SpawnEntities_Event),
  false,  // has_any_key_member_
  ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_message_member_array,  // message members
  ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_message_type_support_handle = {
  0,
  &ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_message_members,
  get_message_typesupport_handle_function,
  &ue_msgs__srv__SpawnEntities_Event__get_type_hash,
  &ue_msgs__srv__SpawnEntities_Event__get_type_description,
  &ue_msgs__srv__SpawnEntities_Event__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_ue_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, srv, SpawnEntities_Event)() {
  ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, service_msgs, msg, ServiceEventInfo)();
  ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, srv, SpawnEntities_Request)();
  ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, srv, SpawnEntities_Response)();
  if (!ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_message_type_support_handle.typesupport_identifier) {
    ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "ue_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "ue_msgs/srv/detail/spawn_entities__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers ue_msgs__srv__detail__spawn_entities__rosidl_typesupport_introspection_c__SpawnEntities_service_members = {
  "ue_msgs__srv",  // service namespace
  "SpawnEntities",  // service name
  // the following fields are initialized below on first access
  NULL,  // request message
  // ue_msgs__srv__detail__spawn_entities__rosidl_typesupport_introspection_c__SpawnEntities_Request_message_type_support_handle,
  NULL,  // response message
  // ue_msgs__srv__detail__spawn_entities__rosidl_typesupport_introspection_c__SpawnEntities_Response_message_type_support_handle
  NULL  // event_message
  // ue_msgs__srv__detail__spawn_entities__rosidl_typesupport_introspection_c__SpawnEntities_Response_message_type_support_handle
};


static rosidl_service_type_support_t ue_msgs__srv__detail__spawn_entities__rosidl_typesupport_introspection_c__SpawnEntities_service_type_support_handle = {
  0,
  &ue_msgs__srv__detail__spawn_entities__rosidl_typesupport_introspection_c__SpawnEntities_service_members,
  get_service_typesupport_handle_function,
  &ue_msgs__srv__SpawnEntities_Request__rosidl_typesupport_introspection_c__SpawnEntities_Request_message_type_support_handle,
  &ue_msgs__srv__SpawnEntities_Response__rosidl_typesupport_introspection_c__SpawnEntities_Response_message_type_support_handle,
  &ue_msgs__srv__SpawnEntities_Event__rosidl_typesupport_introspection_c__SpawnEntities_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    ue_msgs,
    srv,
    SpawnEntities
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    ue_msgs,
    srv,
    SpawnEntities
  ),
  &ue_msgs__srv__SpawnEntities__get_type_hash,
  &ue_msgs__srv__SpawnEntities__get_type_description,
  &ue_msgs__srv__SpawnEntities__get_type_description_sources,
};

// Forward declaration of message type support functions for service members
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, srv, SpawnEntities_Request)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, srv, SpawnEntities_Response)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, srv, SpawnEntities_Event)(void);

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_ue_msgs
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, srv, SpawnEntities)(void) {
  if (!ue_msgs__srv__detail__spawn_entities__rosidl_typesupport_introspection_c__SpawnEntities_service_type_support_handle.typesupport_identifier) {
    ue_msgs__srv__detail__spawn_entities__rosidl_typesupport_introspection_c__SpawnEntities_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)ue_msgs__srv__detail__spawn_entities__rosidl_typesupport_introspection_c__SpawnEntities_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, srv, SpawnEntities_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, srv, SpawnEntities_Response)()->data;
  }
  if (!service_members->event_members_) {
    service_members->event_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ue_msgs, srv, SpawnEntities_Event)()->data;
  }

  return &ue_msgs__srv__detail__spawn_entities__rosidl_typesupport_introspection_c__SpawnEntities_service_type_support_handle;
}
