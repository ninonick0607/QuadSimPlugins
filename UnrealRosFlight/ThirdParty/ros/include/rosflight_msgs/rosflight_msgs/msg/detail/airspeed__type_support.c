// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from rosflight_msgs:msg/Airspeed.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "rosflight_msgs/msg/detail/airspeed__rosidl_typesupport_introspection_c.h"
#include "rosflight_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosflight_msgs/msg/detail/airspeed__functions.h"
#include "rosflight_msgs/msg/detail/airspeed__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  rosflight_msgs__msg__Airspeed__init(message_memory);
}

void rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_fini_function(void * message_memory)
{
  rosflight_msgs__msg__Airspeed__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_message_member_array[4] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosflight_msgs__msg__Airspeed, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "velocity",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosflight_msgs__msg__Airspeed, velocity),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "differential_pressure",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosflight_msgs__msg__Airspeed, differential_pressure),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "temperature",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosflight_msgs__msg__Airspeed, temperature),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_message_members = {
  "rosflight_msgs__msg",  // message namespace
  "Airspeed",  // message name
  4,  // number of fields
  sizeof(rosflight_msgs__msg__Airspeed),
  rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_message_member_array,  // message members
  rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_init_function,  // function to initialize message memory (memory has to be allocated)
  rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_message_type_support_handle = {
  0,
  &rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_rosflight_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosflight_msgs, msg, Airspeed)() {
  rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  if (!rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_message_type_support_handle.typesupport_identifier) {
    rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &rosflight_msgs__msg__Airspeed__rosidl_typesupport_introspection_c__Airspeed_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
