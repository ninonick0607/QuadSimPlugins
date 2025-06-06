// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from ue_msgs:srv/SpawnWorld.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "ue_msgs/srv/spawn_world.hpp"


#ifndef UE_MSGS__SRV__DETAIL__SPAWN_WORLD__STRUCT_HPP_
#define UE_MSGS__SRV__DETAIL__SPAWN_WORLD__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'pose'
#include "geometry_msgs/msg/detail/pose__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__ue_msgs__srv__SpawnWorld_Request __attribute__((deprecated))
#else
# define DEPRECATED__ue_msgs__srv__SpawnWorld_Request __declspec(deprecated)
#endif

namespace ue_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct SpawnWorld_Request_
{
  using Type = SpawnWorld_Request_<ContainerAllocator>;

  explicit SpawnWorld_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : pose(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->world_model = "";
      this->world_instance_name = "";
      this->json_parameters = "";
    }
  }

  explicit SpawnWorld_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : world_model(_alloc),
    world_instance_name(_alloc),
    pose(_alloc, _init),
    json_parameters(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->world_model = "";
      this->world_instance_name = "";
      this->json_parameters = "";
    }
  }

  // field types and members
  using _world_model_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _world_model_type world_model;
  using _world_instance_name_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _world_instance_name_type world_instance_name;
  using _pose_type =
    geometry_msgs::msg::Pose_<ContainerAllocator>;
  _pose_type pose;
  using _json_parameters_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _json_parameters_type json_parameters;

  // setters for named parameter idiom
  Type & set__world_model(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->world_model = _arg;
    return *this;
  }
  Type & set__world_instance_name(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->world_instance_name = _arg;
    return *this;
  }
  Type & set__pose(
    const geometry_msgs::msg::Pose_<ContainerAllocator> & _arg)
  {
    this->pose = _arg;
    return *this;
  }
  Type & set__json_parameters(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->json_parameters = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ue_msgs__srv__SpawnWorld_Request
    std::shared_ptr<ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ue_msgs__srv__SpawnWorld_Request
    std::shared_ptr<ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SpawnWorld_Request_ & other) const
  {
    if (this->world_model != other.world_model) {
      return false;
    }
    if (this->world_instance_name != other.world_instance_name) {
      return false;
    }
    if (this->pose != other.pose) {
      return false;
    }
    if (this->json_parameters != other.json_parameters) {
      return false;
    }
    return true;
  }
  bool operator!=(const SpawnWorld_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SpawnWorld_Request_

// alias to use template instance with default allocator
using SpawnWorld_Request =
  ue_msgs::srv::SpawnWorld_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace ue_msgs


#ifndef _WIN32
# define DEPRECATED__ue_msgs__srv__SpawnWorld_Response __attribute__((deprecated))
#else
# define DEPRECATED__ue_msgs__srv__SpawnWorld_Response __declspec(deprecated)
#endif

namespace ue_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct SpawnWorld_Response_
{
  using Type = SpawnWorld_Response_<ContainerAllocator>;

  explicit SpawnWorld_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
      this->status_message = "";
    }
  }

  explicit SpawnWorld_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
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
    ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ue_msgs__srv__SpawnWorld_Response
    std::shared_ptr<ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ue_msgs__srv__SpawnWorld_Response
    std::shared_ptr<ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SpawnWorld_Response_ & other) const
  {
    if (this->success != other.success) {
      return false;
    }
    if (this->status_message != other.status_message) {
      return false;
    }
    return true;
  }
  bool operator!=(const SpawnWorld_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SpawnWorld_Response_

// alias to use template instance with default allocator
using SpawnWorld_Response =
  ue_msgs::srv::SpawnWorld_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace ue_msgs


// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__ue_msgs__srv__SpawnWorld_Event __attribute__((deprecated))
#else
# define DEPRECATED__ue_msgs__srv__SpawnWorld_Event __declspec(deprecated)
#endif

namespace ue_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct SpawnWorld_Event_
{
  using Type = SpawnWorld_Event_<ContainerAllocator>;

  explicit SpawnWorld_Event_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_init)
  {
    (void)_init;
  }

  explicit SpawnWorld_Event_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _info_type =
    service_msgs::msg::ServiceEventInfo_<ContainerAllocator>;
  _info_type info;
  using _request_type =
    rosidl_runtime_cpp::BoundedVector<ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator>>>;
  _request_type request;
  using _response_type =
    rosidl_runtime_cpp::BoundedVector<ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator>>>;
  _response_type response;

  // setters for named parameter idiom
  Type & set__info(
    const service_msgs::msg::ServiceEventInfo_<ContainerAllocator> & _arg)
  {
    this->info = _arg;
    return *this;
  }
  Type & set__request(
    const rosidl_runtime_cpp::BoundedVector<ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<ue_msgs::srv::SpawnWorld_Request_<ContainerAllocator>>> & _arg)
  {
    this->request = _arg;
    return *this;
  }
  Type & set__response(
    const rosidl_runtime_cpp::BoundedVector<ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<ue_msgs::srv::SpawnWorld_Response_<ContainerAllocator>>> & _arg)
  {
    this->response = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    ue_msgs::srv::SpawnWorld_Event_<ContainerAllocator> *;
  using ConstRawPtr =
    const ue_msgs::srv::SpawnWorld_Event_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ue_msgs::srv::SpawnWorld_Event_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ue_msgs::srv::SpawnWorld_Event_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::SpawnWorld_Event_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::SpawnWorld_Event_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::SpawnWorld_Event_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::SpawnWorld_Event_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ue_msgs::srv::SpawnWorld_Event_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ue_msgs::srv::SpawnWorld_Event_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ue_msgs__srv__SpawnWorld_Event
    std::shared_ptr<ue_msgs::srv::SpawnWorld_Event_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ue_msgs__srv__SpawnWorld_Event
    std::shared_ptr<ue_msgs::srv::SpawnWorld_Event_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SpawnWorld_Event_ & other) const
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
  bool operator!=(const SpawnWorld_Event_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SpawnWorld_Event_

// alias to use template instance with default allocator
using SpawnWorld_Event =
  ue_msgs::srv::SpawnWorld_Event_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace ue_msgs

namespace ue_msgs
{

namespace srv
{

struct SpawnWorld
{
  using Request = ue_msgs::srv::SpawnWorld_Request;
  using Response = ue_msgs::srv::SpawnWorld_Response;
  using Event = ue_msgs::srv::SpawnWorld_Event;
};

}  // namespace srv

}  // namespace ue_msgs

#endif  // UE_MSGS__SRV__DETAIL__SPAWN_WORLD__STRUCT_HPP_
