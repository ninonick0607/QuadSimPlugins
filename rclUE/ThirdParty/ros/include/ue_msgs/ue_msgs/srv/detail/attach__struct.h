// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from ue_msgs:srv/Attach.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "ue_msgs/srv/attach.h"


#ifndef UE_MSGS__SRV__DETAIL__ATTACH__STRUCT_H_
#define UE_MSGS__SRV__DETAIL__ATTACH__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'name1'
// Member 'name2'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/Attach in the package ue_msgs.
typedef struct ue_msgs__srv__Attach_Request
{
  rosidl_runtime_c__String name1;
  rosidl_runtime_c__String name2;
} ue_msgs__srv__Attach_Request;

// Struct for a sequence of ue_msgs__srv__Attach_Request.
typedef struct ue_msgs__srv__Attach_Request__Sequence
{
  ue_msgs__srv__Attach_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} ue_msgs__srv__Attach_Request__Sequence;

// Constants defined in the message

/// Struct defined in srv/Attach in the package ue_msgs.
typedef struct ue_msgs__srv__Attach_Response
{
  bool success;
} ue_msgs__srv__Attach_Response;

// Struct for a sequence of ue_msgs__srv__Attach_Response.
typedef struct ue_msgs__srv__Attach_Response__Sequence
{
  ue_msgs__srv__Attach_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} ue_msgs__srv__Attach_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  ue_msgs__srv__Attach_Event__request__MAX_SIZE = 1
};
// response
enum
{
  ue_msgs__srv__Attach_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/Attach in the package ue_msgs.
typedef struct ue_msgs__srv__Attach_Event
{
  service_msgs__msg__ServiceEventInfo info;
  ue_msgs__srv__Attach_Request__Sequence request;
  ue_msgs__srv__Attach_Response__Sequence response;
} ue_msgs__srv__Attach_Event;

// Struct for a sequence of ue_msgs__srv__Attach_Event.
typedef struct ue_msgs__srv__Attach_Event__Sequence
{
  ue_msgs__srv__Attach_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} ue_msgs__srv__Attach_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // UE_MSGS__SRV__DETAIL__ATTACH__STRUCT_H_
