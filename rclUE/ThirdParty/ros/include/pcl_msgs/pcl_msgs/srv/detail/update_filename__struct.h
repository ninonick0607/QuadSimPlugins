// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from pcl_msgs:srv/UpdateFilename.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "pcl_msgs/srv/update_filename.h"


#ifndef PCL_MSGS__SRV__DETAIL__UPDATE_FILENAME__STRUCT_H_
#define PCL_MSGS__SRV__DETAIL__UPDATE_FILENAME__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'filename'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/UpdateFilename in the package pcl_msgs.
typedef struct pcl_msgs__srv__UpdateFilename_Request
{
  rosidl_runtime_c__String filename;
} pcl_msgs__srv__UpdateFilename_Request;

// Struct for a sequence of pcl_msgs__srv__UpdateFilename_Request.
typedef struct pcl_msgs__srv__UpdateFilename_Request__Sequence
{
  pcl_msgs__srv__UpdateFilename_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} pcl_msgs__srv__UpdateFilename_Request__Sequence;

// Constants defined in the message

/// Struct defined in srv/UpdateFilename in the package pcl_msgs.
typedef struct pcl_msgs__srv__UpdateFilename_Response
{
  bool success;
} pcl_msgs__srv__UpdateFilename_Response;

// Struct for a sequence of pcl_msgs__srv__UpdateFilename_Response.
typedef struct pcl_msgs__srv__UpdateFilename_Response__Sequence
{
  pcl_msgs__srv__UpdateFilename_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} pcl_msgs__srv__UpdateFilename_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  pcl_msgs__srv__UpdateFilename_Event__request__MAX_SIZE = 1
};
// response
enum
{
  pcl_msgs__srv__UpdateFilename_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/UpdateFilename in the package pcl_msgs.
typedef struct pcl_msgs__srv__UpdateFilename_Event
{
  service_msgs__msg__ServiceEventInfo info;
  pcl_msgs__srv__UpdateFilename_Request__Sequence request;
  pcl_msgs__srv__UpdateFilename_Response__Sequence response;
} pcl_msgs__srv__UpdateFilename_Event;

// Struct for a sequence of pcl_msgs__srv__UpdateFilename_Event.
typedef struct pcl_msgs__srv__UpdateFilename_Event__Sequence
{
  pcl_msgs__srv__UpdateFilename_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} pcl_msgs__srv__UpdateFilename_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // PCL_MSGS__SRV__DETAIL__UPDATE_FILENAME__STRUCT_H_
