// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from ue_msgs:srv/SetEntityState.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "ue_msgs/srv/set_entity_state.hpp"


#ifndef UE_MSGS__SRV__DETAIL__SET_ENTITY_STATE__STRUCT_HPP_
#define UE_MSGS__SRV__DETAIL__SET_ENTITY_STATE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'state'
#include "ue_msgs/msg/detail/entity_state__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__ue_msgs__srv__SetEntityState_Request __attribute__((deprecated))
#else
# define DEPRECATED__ue_msgs__srv__SetEntityState_Request __declspec(deprecated)
#endif

namespace ue_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct SetEntityState_Request_
{
  using Type = SetEntityState_Request_<ContainerAllocator>;

  explicit SetEntityState_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : state(_init)
  {
    (void)_init;
  }

  explicit SetEntityState_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : state(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _state_type =
    ue_msgs::msg::EntityState_<ContainerAllocator>;
  _state_type state;

  // setters for named parameter idiom
  Type & set__state(
    const ue_msgs::msg::EntityState_<ContainerAllocator> & _arg)
  {
    this->state = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    ue_msgs::srv::SetEntityState_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const ue_msgs::srv::SetEntityState_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ue_msgs::srv::SetEntityState_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ue_msgs::srv::SetEntityState_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::SetEntityState_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::SetEntityState_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::SetEntityState_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::SetEntityState_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ue_msgs::srv::SetEntityState_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ue_msgs::srv::SetEntityState_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ue_msgs__srv__SetEntityState_Request
    std::shared_ptr<ue_msgs::srv::SetEntityState_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ue_msgs__srv__SetEntityState_Request
    std::shared_ptr<ue_msgs::srv::SetEntityState_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SetEntityState_Request_ & other) const
  {
    if (this->state != other.state) {
      return false;
    }
    return true;
  }
  bool operator!=(const SetEntityState_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SetEntityState_Request_

// alias to use template instance with default allocator
using SetEntityState_Request =
  ue_msgs::srv::SetEntityState_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace ue_msgs


#ifndef _WIN32
# define DEPRECATED__ue_msgs__srv__SetEntityState_Response __attribute__((deprecated))
#else
# define DEPRECATED__ue_msgs__srv__SetEntityState_Response __declspec(deprecated)
#endif

namespace ue_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct SetEntityState_Response_
{
  using Type = SetEntityState_Response_<ContainerAllocator>;

  explicit SetEntityState_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
    }
  }

  explicit SetEntityState_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
    }
  }

  // field types and members
  using _success_type =
    bool;
  _success_type success;

  // setters for named parameter idiom
  Type & set__success(
    const bool & _arg)
  {
    this->success = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    ue_msgs::srv::SetEntityState_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const ue_msgs::srv::SetEntityState_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ue_msgs::srv::SetEntityState_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ue_msgs::srv::SetEntityState_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::SetEntityState_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::SetEntityState_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::SetEntityState_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::SetEntityState_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ue_msgs::srv::SetEntityState_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ue_msgs::srv::SetEntityState_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ue_msgs__srv__SetEntityState_Response
    std::shared_ptr<ue_msgs::srv::SetEntityState_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ue_msgs__srv__SetEntityState_Response
    std::shared_ptr<ue_msgs::srv::SetEntityState_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SetEntityState_Response_ & other) const
  {
    if (this->success != other.success) {
      return false;
    }
    return true;
  }
  bool operator!=(const SetEntityState_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SetEntityState_Response_

// alias to use template instance with default allocator
using SetEntityState_Response =
  ue_msgs::srv::SetEntityState_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace ue_msgs


// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__ue_msgs__srv__SetEntityState_Event __attribute__((deprecated))
#else
# define DEPRECATED__ue_msgs__srv__SetEntityState_Event __declspec(deprecated)
#endif

namespace ue_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct SetEntityState_Event_
{
  using Type = SetEntityState_Event_<ContainerAllocator>;

  explicit SetEntityState_Event_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_init)
  {
    (void)_init;
  }

  explicit SetEntityState_Event_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _info_type =
    service_msgs::msg::ServiceEventInfo_<ContainerAllocator>;
  _info_type info;
  using _request_type =
    rosidl_runtime_cpp::BoundedVector<ue_msgs::srv::SetEntityState_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<ue_msgs::srv::SetEntityState_Request_<ContainerAllocator>>>;
  _request_type request;
  using _response_type =
    rosidl_runtime_cpp::BoundedVector<ue_msgs::srv::SetEntityState_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<ue_msgs::srv::SetEntityState_Response_<ContainerAllocator>>>;
  _response_type response;

  // setters for named parameter idiom
  Type & set__info(
    const service_msgs::msg::ServiceEventInfo_<ContainerAllocator> & _arg)
  {
    this->info = _arg;
    return *this;
  }
  Type & set__request(
    const rosidl_runtime_cpp::BoundedVector<ue_msgs::srv::SetEntityState_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<ue_msgs::srv::SetEntityState_Request_<ContainerAllocator>>> & _arg)
  {
    this->request = _arg;
    return *this;
  }
  Type & set__response(
    const rosidl_runtime_cpp::BoundedVector<ue_msgs::srv::SetEntityState_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<ue_msgs::srv::SetEntityState_Response_<ContainerAllocator>>> & _arg)
  {
    this->response = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    ue_msgs::srv::SetEntityState_Event_<ContainerAllocator> *;
  using ConstRawPtr =
    const ue_msgs::srv::SetEntityState_Event_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ue_msgs::srv::SetEntityState_Event_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ue_msgs::srv::SetEntityState_Event_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::SetEntityState_Event_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::SetEntityState_Event_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::SetEntityState_Event_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::SetEntityState_Event_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ue_msgs::srv::SetEntityState_Event_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ue_msgs::srv::SetEntityState_Event_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ue_msgs__srv__SetEntityState_Event
    std::shared_ptr<ue_msgs::srv::SetEntityState_Event_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ue_msgs__srv__SetEntityState_Event
    std::shared_ptr<ue_msgs::srv::SetEntityState_Event_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SetEntityState_Event_ & other) const
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
  bool operator!=(const SetEntityState_Event_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SetEntityState_Event_

// alias to use template instance with default allocator
using SetEntityState_Event =
  ue_msgs::srv::SetEntityState_Event_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace ue_msgs

namespace ue_msgs
{

namespace srv
{

struct SetEntityState
{
  using Request = ue_msgs::srv::SetEntityState_Request;
  using Response = ue_msgs::srv::SetEntityState_Response;
  using Event = ue_msgs::srv::SetEntityState_Event;
};

}  // namespace srv

}  // namespace ue_msgs

#endif  // UE_MSGS__SRV__DETAIL__SET_ENTITY_STATE__STRUCT_HPP_
