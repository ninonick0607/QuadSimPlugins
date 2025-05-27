// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from ue_msgs:srv/GetBoolFromId.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "ue_msgs/srv/get_bool_from_id.hpp"


#ifndef UE_MSGS__SRV__DETAIL__GET_BOOL_FROM_ID__BUILDER_HPP_
#define UE_MSGS__SRV__DETAIL__GET_BOOL_FROM_ID__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "ue_msgs/srv/detail/get_bool_from_id__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace ue_msgs
{

namespace srv
{

namespace builder
{

class Init_GetBoolFromId_Request_id
{
public:
  Init_GetBoolFromId_Request_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::ue_msgs::srv::GetBoolFromId_Request id(::ue_msgs::srv::GetBoolFromId_Request::_id_type arg)
  {
    msg_.id = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ue_msgs::srv::GetBoolFromId_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ue_msgs::srv::GetBoolFromId_Request>()
{
  return ue_msgs::srv::builder::Init_GetBoolFromId_Request_id();
}

}  // namespace ue_msgs


namespace ue_msgs
{

namespace srv
{

namespace builder
{

class Init_GetBoolFromId_Response_data
{
public:
  explicit Init_GetBoolFromId_Response_data(::ue_msgs::srv::GetBoolFromId_Response & msg)
  : msg_(msg)
  {}
  ::ue_msgs::srv::GetBoolFromId_Response data(::ue_msgs::srv::GetBoolFromId_Response::_data_type arg)
  {
    msg_.data = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ue_msgs::srv::GetBoolFromId_Response msg_;
};

class Init_GetBoolFromId_Response_remarks
{
public:
  explicit Init_GetBoolFromId_Response_remarks(::ue_msgs::srv::GetBoolFromId_Response & msg)
  : msg_(msg)
  {}
  Init_GetBoolFromId_Response_data remarks(::ue_msgs::srv::GetBoolFromId_Response::_remarks_type arg)
  {
    msg_.remarks = std::move(arg);
    return Init_GetBoolFromId_Response_data(msg_);
  }

private:
  ::ue_msgs::srv::GetBoolFromId_Response msg_;
};

class Init_GetBoolFromId_Response_success
{
public:
  Init_GetBoolFromId_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_GetBoolFromId_Response_remarks success(::ue_msgs::srv::GetBoolFromId_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return Init_GetBoolFromId_Response_remarks(msg_);
  }

private:
  ::ue_msgs::srv::GetBoolFromId_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ue_msgs::srv::GetBoolFromId_Response>()
{
  return ue_msgs::srv::builder::Init_GetBoolFromId_Response_success();
}

}  // namespace ue_msgs


namespace ue_msgs
{

namespace srv
{

namespace builder
{

class Init_GetBoolFromId_Event_response
{
public:
  explicit Init_GetBoolFromId_Event_response(::ue_msgs::srv::GetBoolFromId_Event & msg)
  : msg_(msg)
  {}
  ::ue_msgs::srv::GetBoolFromId_Event response(::ue_msgs::srv::GetBoolFromId_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ue_msgs::srv::GetBoolFromId_Event msg_;
};

class Init_GetBoolFromId_Event_request
{
public:
  explicit Init_GetBoolFromId_Event_request(::ue_msgs::srv::GetBoolFromId_Event & msg)
  : msg_(msg)
  {}
  Init_GetBoolFromId_Event_response request(::ue_msgs::srv::GetBoolFromId_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_GetBoolFromId_Event_response(msg_);
  }

private:
  ::ue_msgs::srv::GetBoolFromId_Event msg_;
};

class Init_GetBoolFromId_Event_info
{
public:
  Init_GetBoolFromId_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_GetBoolFromId_Event_request info(::ue_msgs::srv::GetBoolFromId_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_GetBoolFromId_Event_request(msg_);
  }

private:
  ::ue_msgs::srv::GetBoolFromId_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ue_msgs::srv::GetBoolFromId_Event>()
{
  return ue_msgs::srv::builder::Init_GetBoolFromId_Event_info();
}

}  // namespace ue_msgs

#endif  // UE_MSGS__SRV__DETAIL__GET_BOOL_FROM_ID__BUILDER_HPP_
