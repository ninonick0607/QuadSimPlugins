// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from ue_msgs:srv/SetInt32.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "ue_msgs/srv/set_int32.hpp"


#ifndef UE_MSGS__SRV__DETAIL__SET_INT32__BUILDER_HPP_
#define UE_MSGS__SRV__DETAIL__SET_INT32__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "ue_msgs/srv/detail/set_int32__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace ue_msgs
{

namespace srv
{

namespace builder
{

class Init_SetInt32_Request_data
{
public:
  Init_SetInt32_Request_data()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::ue_msgs::srv::SetInt32_Request data(::ue_msgs::srv::SetInt32_Request::_data_type arg)
  {
    msg_.data = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ue_msgs::srv::SetInt32_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ue_msgs::srv::SetInt32_Request>()
{
  return ue_msgs::srv::builder::Init_SetInt32_Request_data();
}

}  // namespace ue_msgs


namespace ue_msgs
{

namespace srv
{

namespace builder
{

class Init_SetInt32_Response_remarks
{
public:
  explicit Init_SetInt32_Response_remarks(::ue_msgs::srv::SetInt32_Response & msg)
  : msg_(msg)
  {}
  ::ue_msgs::srv::SetInt32_Response remarks(::ue_msgs::srv::SetInt32_Response::_remarks_type arg)
  {
    msg_.remarks = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ue_msgs::srv::SetInt32_Response msg_;
};

class Init_SetInt32_Response_success
{
public:
  Init_SetInt32_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SetInt32_Response_remarks success(::ue_msgs::srv::SetInt32_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return Init_SetInt32_Response_remarks(msg_);
  }

private:
  ::ue_msgs::srv::SetInt32_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ue_msgs::srv::SetInt32_Response>()
{
  return ue_msgs::srv::builder::Init_SetInt32_Response_success();
}

}  // namespace ue_msgs


namespace ue_msgs
{

namespace srv
{

namespace builder
{

class Init_SetInt32_Event_response
{
public:
  explicit Init_SetInt32_Event_response(::ue_msgs::srv::SetInt32_Event & msg)
  : msg_(msg)
  {}
  ::ue_msgs::srv::SetInt32_Event response(::ue_msgs::srv::SetInt32_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ue_msgs::srv::SetInt32_Event msg_;
};

class Init_SetInt32_Event_request
{
public:
  explicit Init_SetInt32_Event_request(::ue_msgs::srv::SetInt32_Event & msg)
  : msg_(msg)
  {}
  Init_SetInt32_Event_response request(::ue_msgs::srv::SetInt32_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_SetInt32_Event_response(msg_);
  }

private:
  ::ue_msgs::srv::SetInt32_Event msg_;
};

class Init_SetInt32_Event_info
{
public:
  Init_SetInt32_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SetInt32_Event_request info(::ue_msgs::srv::SetInt32_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_SetInt32_Event_request(msg_);
  }

private:
  ::ue_msgs::srv::SetInt32_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ue_msgs::srv::SetInt32_Event>()
{
  return ue_msgs::srv::builder::Init_SetInt32_Event_info();
}

}  // namespace ue_msgs

#endif  // UE_MSGS__SRV__DETAIL__SET_INT32__BUILDER_HPP_
