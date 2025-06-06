// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from ue_msgs:srv/GetEntityState.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "ue_msgs/srv/get_entity_state.hpp"


#ifndef UE_MSGS__SRV__DETAIL__GET_ENTITY_STATE__STRUCT_HPP_
#define UE_MSGS__SRV__DETAIL__GET_ENTITY_STATE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__ue_msgs__srv__GetEntityState_Request __attribute__((deprecated))
#else
# define DEPRECATED__ue_msgs__srv__GetEntityState_Request __declspec(deprecated)
#endif

namespace ue_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct GetEntityState_Request_
{
  using Type = GetEntityState_Request_<ContainerAllocator>;

  explicit GetEntityState_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->name = "";
      this->reference_frame = "";
    }
  }

  explicit GetEntityState_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : name(_alloc),
    reference_frame(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->name = "";
      this->reference_frame = "";
    }
  }

  // field types and members
  using _name_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _name_type name;
  using _reference_frame_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _reference_frame_type reference_frame;

  // setters for named parameter idiom
  Type & set__name(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->name = _arg;
    return *this;
  }
  Type & set__reference_frame(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->reference_frame = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    ue_msgs::srv::GetEntityState_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const ue_msgs::srv::GetEntityState_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ue_msgs::srv::GetEntityState_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ue_msgs::srv::GetEntityState_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::GetEntityState_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::GetEntityState_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::GetEntityState_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::GetEntityState_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ue_msgs::srv::GetEntityState_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ue_msgs::srv::GetEntityState_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ue_msgs__srv__GetEntityState_Request
    std::shared_ptr<ue_msgs::srv::GetEntityState_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ue_msgs__srv__GetEntityState_Request
    std::shared_ptr<ue_msgs::srv::GetEntityState_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const GetEntityState_Request_ & other) const
  {
    if (this->name != other.name) {
      return false;
    }
    if (this->reference_frame != other.reference_frame) {
      return false;
    }
    return true;
  }
  bool operator!=(const GetEntityState_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct GetEntityState_Request_

// alias to use template instance with default allocator
using GetEntityState_Request =
  ue_msgs::srv::GetEntityState_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace ue_msgs


// Include directives for member types
// Member 'state'
#include "ue_msgs/msg/detail/entity_state__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__ue_msgs__srv__GetEntityState_Response __attribute__((deprecated))
#else
# define DEPRECATED__ue_msgs__srv__GetEntityState_Response __declspec(deprecated)
#endif

namespace ue_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct GetEntityState_Response_
{
  using Type = GetEntityState_Response_<ContainerAllocator>;

  explicit GetEntityState_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : state(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
    }
  }

  explicit GetEntityState_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : state(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
    }
  }

  // field types and members
  using _state_type =
    ue_msgs::msg::EntityState_<ContainerAllocator>;
  _state_type state;
  using _success_type =
    bool;
  _success_type success;

  // setters for named parameter idiom
  Type & set__state(
    const ue_msgs::msg::EntityState_<ContainerAllocator> & _arg)
  {
    this->state = _arg;
    return *this;
  }
  Type & set__success(
    const bool & _arg)
  {
    this->success = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    ue_msgs::srv::GetEntityState_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const ue_msgs::srv::GetEntityState_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ue_msgs::srv::GetEntityState_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ue_msgs::srv::GetEntityState_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::GetEntityState_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::GetEntityState_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::GetEntityState_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::GetEntityState_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ue_msgs::srv::GetEntityState_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ue_msgs::srv::GetEntityState_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ue_msgs__srv__GetEntityState_Response
    std::shared_ptr<ue_msgs::srv::GetEntityState_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ue_msgs__srv__GetEntityState_Response
    std::shared_ptr<ue_msgs::srv::GetEntityState_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const GetEntityState_Response_ & other) const
  {
    if (this->state != other.state) {
      return false;
    }
    if (this->success != other.success) {
      return false;
    }
    return true;
  }
  bool operator!=(const GetEntityState_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct GetEntityState_Response_

// alias to use template instance with default allocator
using GetEntityState_Response =
  ue_msgs::srv::GetEntityState_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace ue_msgs


// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__ue_msgs__srv__GetEntityState_Event __attribute__((deprecated))
#else
# define DEPRECATED__ue_msgs__srv__GetEntityState_Event __declspec(deprecated)
#endif

namespace ue_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct GetEntityState_Event_
{
  using Type = GetEntityState_Event_<ContainerAllocator>;

  explicit GetEntityState_Event_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_init)
  {
    (void)_init;
  }

  explicit GetEntityState_Event_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _info_type =
    service_msgs::msg::ServiceEventInfo_<ContainerAllocator>;
  _info_type info;
  using _request_type =
    rosidl_runtime_cpp::BoundedVector<ue_msgs::srv::GetEntityState_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<ue_msgs::srv::GetEntityState_Request_<ContainerAllocator>>>;
  _request_type request;
  using _response_type =
    rosidl_runtime_cpp::BoundedVector<ue_msgs::srv::GetEntityState_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<ue_msgs::srv::GetEntityState_Response_<ContainerAllocator>>>;
  _response_type response;

  // setters for named parameter idiom
  Type & set__info(
    const service_msgs::msg::ServiceEventInfo_<ContainerAllocator> & _arg)
  {
    this->info = _arg;
    return *this;
  }
  Type & set__request(
    const rosidl_runtime_cpp::BoundedVector<ue_msgs::srv::GetEntityState_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<ue_msgs::srv::GetEntityState_Request_<ContainerAllocator>>> & _arg)
  {
    this->request = _arg;
    return *this;
  }
  Type & set__response(
    const rosidl_runtime_cpp::BoundedVector<ue_msgs::srv::GetEntityState_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<ue_msgs::srv::GetEntityState_Response_<ContainerAllocator>>> & _arg)
  {
    this->response = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    ue_msgs::srv::GetEntityState_Event_<ContainerAllocator> *;
  using ConstRawPtr =
    const ue_msgs::srv::GetEntityState_Event_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ue_msgs::srv::GetEntityState_Event_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ue_msgs::srv::GetEntityState_Event_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::GetEntityState_Event_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::GetEntityState_Event_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ue_msgs::srv::GetEntityState_Event_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ue_msgs::srv::GetEntityState_Event_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ue_msgs::srv::GetEntityState_Event_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ue_msgs::srv::GetEntityState_Event_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ue_msgs__srv__GetEntityState_Event
    std::shared_ptr<ue_msgs::srv::GetEntityState_Event_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ue_msgs__srv__GetEntityState_Event
    std::shared_ptr<ue_msgs::srv::GetEntityState_Event_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const GetEntityState_Event_ & other) const
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
  bool operator!=(const GetEntityState_Event_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct GetEntityState_Event_

// alias to use template instance with default allocator
using GetEntityState_Event =
  ue_msgs::srv::GetEntityState_Event_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace ue_msgs

namespace ue_msgs
{

namespace srv
{

struct GetEntityState
{
  using Request = ue_msgs::srv::GetEntityState_Request;
  using Response = ue_msgs::srv::GetEntityState_Response;
  using Event = ue_msgs::srv::GetEntityState_Event;
};

}  // namespace srv

}  // namespace ue_msgs

#endif  // UE_MSGS__SRV__DETAIL__GET_ENTITY_STATE__STRUCT_HPP_
