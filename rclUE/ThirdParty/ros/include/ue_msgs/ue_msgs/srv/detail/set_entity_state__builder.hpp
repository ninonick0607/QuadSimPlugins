// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from ue_msgs:srv/SetEntityState.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "ue_msgs/srv/set_entity_state.hpp"


#ifndef UE_MSGS__SRV__DETAIL__SET_ENTITY_STATE__BUILDER_HPP_
#define UE_MSGS__SRV__DETAIL__SET_ENTITY_STATE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "ue_msgs/srv/detail/set_entity_state__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace ue_msgs
{

namespace srv
{

namespace builder
{

class Init_SetEntityState_Request_state
{
public:
  Init_SetEntityState_Request_state()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::ue_msgs::srv::SetEntityState_Request state(::ue_msgs::srv::SetEntityState_Request::_state_type arg)
  {
    msg_.state = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ue_msgs::srv::SetEntityState_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ue_msgs::srv::SetEntityState_Request>()
{
  return ue_msgs::srv::builder::Init_SetEntityState_Request_state();
}

}  // namespace ue_msgs


namespace ue_msgs
{

namespace srv
{

namespace builder
{

class Init_SetEntityState_Response_success
{
public:
  Init_SetEntityState_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::ue_msgs::srv::SetEntityState_Response success(::ue_msgs::srv::SetEntityState_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ue_msgs::srv::SetEntityState_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ue_msgs::srv::SetEntityState_Response>()
{
  return ue_msgs::srv::builder::Init_SetEntityState_Response_success();
}

}  // namespace ue_msgs


namespace ue_msgs
{

namespace srv
{

namespace builder
{

class Init_SetEntityState_Event_response
{
public:
  explicit Init_SetEntityState_Event_response(::ue_msgs::srv::SetEntityState_Event & msg)
  : msg_(msg)
  {}
  ::ue_msgs::srv::SetEntityState_Event response(::ue_msgs::srv::SetEntityState_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ue_msgs::srv::SetEntityState_Event msg_;
};

class Init_SetEntityState_Event_request
{
public:
  explicit Init_SetEntityState_Event_request(::ue_msgs::srv::SetEntityState_Event & msg)
  : msg_(msg)
  {}
  Init_SetEntityState_Event_response request(::ue_msgs::srv::SetEntityState_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_SetEntityState_Event_response(msg_);
  }

private:
  ::ue_msgs::srv::SetEntityState_Event msg_;
};

class Init_SetEntityState_Event_info
{
public:
  Init_SetEntityState_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SetEntityState_Event_request info(::ue_msgs::srv::SetEntityState_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_SetEntityState_Event_request(msg_);
  }

private:
  ::ue_msgs::srv::SetEntityState_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ue_msgs::srv::SetEntityState_Event>()
{
  return ue_msgs::srv::builder::Init_SetEntityState_Event_info();
}

}  // namespace ue_msgs

#endif  // UE_MSGS__SRV__DETAIL__SET_ENTITY_STATE__BUILDER_HPP_
