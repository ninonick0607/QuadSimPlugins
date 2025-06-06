// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from sensor_msgs:srv/SetCameraInfo.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/srv/set_camera_info.hpp"


#ifndef SENSOR_MSGS__SRV__DETAIL__SET_CAMERA_INFO__STRUCT_HPP_
#define SENSOR_MSGS__SRV__DETAIL__SET_CAMERA_INFO__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'camera_info'
#include "sensor_msgs/msg/detail/camera_info__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__sensor_msgs__srv__SetCameraInfo_Request __attribute__((deprecated))
#else
# define DEPRECATED__sensor_msgs__srv__SetCameraInfo_Request __declspec(deprecated)
#endif

namespace sensor_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct SetCameraInfo_Request_
{
  using Type = SetCameraInfo_Request_<ContainerAllocator>;

  explicit SetCameraInfo_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : camera_info(_init)
  {
    (void)_init;
  }

  explicit SetCameraInfo_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : camera_info(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _camera_info_type =
    sensor_msgs::msg::CameraInfo_<ContainerAllocator>;
  _camera_info_type camera_info;

  // setters for named parameter idiom
  Type & set__camera_info(
    const sensor_msgs::msg::CameraInfo_<ContainerAllocator> & _arg)
  {
    this->camera_info = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__sensor_msgs__srv__SetCameraInfo_Request
    std::shared_ptr<sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__sensor_msgs__srv__SetCameraInfo_Request
    std::shared_ptr<sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SetCameraInfo_Request_ & other) const
  {
    if (this->camera_info != other.camera_info) {
      return false;
    }
    return true;
  }
  bool operator!=(const SetCameraInfo_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SetCameraInfo_Request_

// alias to use template instance with default allocator
using SetCameraInfo_Request =
  sensor_msgs::srv::SetCameraInfo_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace sensor_msgs


#ifndef _WIN32
# define DEPRECATED__sensor_msgs__srv__SetCameraInfo_Response __attribute__((deprecated))
#else
# define DEPRECATED__sensor_msgs__srv__SetCameraInfo_Response __declspec(deprecated)
#endif

namespace sensor_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct SetCameraInfo_Response_
{
  using Type = SetCameraInfo_Response_<ContainerAllocator>;

  explicit SetCameraInfo_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
      this->status_message = "";
    }
  }

  explicit SetCameraInfo_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : status_message(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
      this->status_message = "";
    }
  }

  // field types and members
  using _success_type =
    bool;
  _success_type success;
  using _status_message_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _status_message_type status_message;

  // setters for named parameter idiom
  Type & set__success(
    const bool & _arg)
  {
    this->success = _arg;
    return *this;
  }
  Type & set__status_message(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->status_message = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__sensor_msgs__srv__SetCameraInfo_Response
    std::shared_ptr<sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__sensor_msgs__srv__SetCameraInfo_Response
    std::shared_ptr<sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SetCameraInfo_Response_ & other) const
  {
    if (this->success != other.success) {
      return false;
    }
    if (this->status_message != other.status_message) {
      return false;
    }
    return true;
  }
  bool operator!=(const SetCameraInfo_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SetCameraInfo_Response_

// alias to use template instance with default allocator
using SetCameraInfo_Response =
  sensor_msgs::srv::SetCameraInfo_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace sensor_msgs


// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__sensor_msgs__srv__SetCameraInfo_Event __attribute__((deprecated))
#else
# define DEPRECATED__sensor_msgs__srv__SetCameraInfo_Event __declspec(deprecated)
#endif

namespace sensor_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct SetCameraInfo_Event_
{
  using Type = SetCameraInfo_Event_<ContainerAllocator>;

  explicit SetCameraInfo_Event_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_init)
  {
    (void)_init;
  }

  explicit SetCameraInfo_Event_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _info_type =
    service_msgs::msg::ServiceEventInfo_<ContainerAllocator>;
  _info_type info;
  using _request_type =
    rosidl_runtime_cpp::BoundedVector<sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator>>>;
  _request_type request;
  using _response_type =
    rosidl_runtime_cpp::BoundedVector<sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator>>>;
  _response_type response;

  // setters for named parameter idiom
  Type & set__info(
    const service_msgs::msg::ServiceEventInfo_<ContainerAllocator> & _arg)
  {
    this->info = _arg;
    return *this;
  }
  Type & set__request(
    const rosidl_runtime_cpp::BoundedVector<sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<sensor_msgs::srv::SetCameraInfo_Request_<ContainerAllocator>>> & _arg)
  {
    this->request = _arg;
    return *this;
  }
  Type & set__response(
    const rosidl_runtime_cpp::BoundedVector<sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<sensor_msgs::srv::SetCameraInfo_Response_<ContainerAllocator>>> & _arg)
  {
    this->response = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    sensor_msgs::srv::SetCameraInfo_Event_<ContainerAllocator> *;
  using ConstRawPtr =
    const sensor_msgs::srv::SetCameraInfo_Event_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<sensor_msgs::srv::SetCameraInfo_Event_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<sensor_msgs::srv::SetCameraInfo_Event_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      sensor_msgs::srv::SetCameraInfo_Event_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<sensor_msgs::srv::SetCameraInfo_Event_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      sensor_msgs::srv::SetCameraInfo_Event_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<sensor_msgs::srv::SetCameraInfo_Event_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<sensor_msgs::srv::SetCameraInfo_Event_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<sensor_msgs::srv::SetCameraInfo_Event_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__sensor_msgs__srv__SetCameraInfo_Event
    std::shared_ptr<sensor_msgs::srv::SetCameraInfo_Event_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__sensor_msgs__srv__SetCameraInfo_Event
    std::shared_ptr<sensor_msgs::srv::SetCameraInfo_Event_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SetCameraInfo_Event_ & other) const
  {
    if (this->info != other.info) {
      return false;
    }
    if (this->request != other.request) {
      return false;
    }
    if (this->response != other.response) {
      return false;
    }
    return true;
  }
  bool operator!=(const SetCameraInfo_Event_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SetCameraInfo_Event_

// alias to use template instance with default allocator
using SetCameraInfo_Event =
  sensor_msgs::srv::SetCameraInfo_Event_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace sensor_msgs

namespace sensor_msgs
{

namespace srv
{

struct SetCameraInfo
{
  using Request = sensor_msgs::srv::SetCameraInfo_Request;
  using Response = sensor_msgs::srv::SetCameraInfo_Response;
  using Event = sensor_msgs::srv::SetCameraInfo_Event;
};

}  // namespace srv

}  // namespace sensor_msgs

#endif  // SENSOR_MSGS__SRV__DETAIL__SET_CAMERA_INFO__STRUCT_HPP_
