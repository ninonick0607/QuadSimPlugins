// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from ue_msgs:srv/DeleteEntity.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "ue_msgs/srv/delete_entity.h"


#ifndef UE_MSGS__SRV__DETAIL__DELETE_ENTITY__STRUCT_H_
#define UE_MSGS__SRV__DETAIL__DELETE_ENTITY__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'name'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/DeleteEntity in the package ue_msgs.
typedef struct ue_msgs__srv__DeleteEntity_Request
{
  /// Entity name.
  rosidl_runtime_c__String name;
} ue_msgs__srv__DeleteEntity_Request;

// Struct for a sequence of ue_msgs__srv__DeleteEntity_Request.
typedef struct ue_msgs__srv__DeleteEntity_Request__Sequence
{
  ue_msgs__srv__DeleteEntity_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} ue_msgs__srv__DeleteEntity_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'status_message'
// already included above
// #include "rosidl_runtime_c/string.h"

/// Struct defined in srv/DeleteEntity in the package ue_msgs.
typedef struct ue_msgs__srv__DeleteEntity_Response
{
  /// Return true if spawned successfully.
  bool success;
  /// Comments if available.
  rosidl_runtime_c__String status_message;
} ue_msgs__srv__DeleteEntity_Response;

// Struct for a sequence of ue_msgs__srv__DeleteEntity_Response.
typedef struct ue_msgs__srv__DeleteEntity_Response__Sequence
{
  ue_msgs__srv__DeleteEntity_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} ue_msgs__srv__DeleteEntity_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  ue_msgs__srv__DeleteEntity_Event__request__MAX_SIZE = 1
};
// response
enum
{
  ue_msgs__srv__DeleteEntity_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/DeleteEntity in the package ue_msgs.
typedef struct ue_msgs__srv__DeleteEntity_Event
{
  service_msgs__msg__ServiceEventInfo info;
  ue_msgs__srv__DeleteEntity_Request__Sequence request;
  ue_msgs__srv__DeleteEntity_Response__Sequence response;
} ue_msgs__srv__DeleteEntity_Event;

// Struct for a sequence of ue_msgs__srv__DeleteEntity_Event.
typedef struct ue_msgs__srv__DeleteEntity_Event__Sequence
{
  ue_msgs__srv__DeleteEntity_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} ue_msgs__srv__DeleteEntity_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // UE_MSGS__SRV__DETAIL__DELETE_ENTITY__STRUCT_H_
