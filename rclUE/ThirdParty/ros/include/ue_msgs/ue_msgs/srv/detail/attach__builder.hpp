// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from ue_msgs:srv/Attach.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "ue_msgs/srv/attach.hpp"


#ifndef UE_MSGS__SRV__DETAIL__ATTACH__BUILDER_HPP_
#define UE_MSGS__SRV__DETAIL__ATTACH__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "ue_msgs/srv/detail/attach__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace ue_msgs
{

namespace srv
{

namespace builder
{

class Init_Attach_Request_name2
{
public:
  explicit Init_Attach_Request_name2(::ue_msgs::srv::Attach_Request & msg)
  : msg_(msg)
  {}
  ::ue_msgs::srv::Attach_Request name2(::ue_msgs::srv::Attach_Request::_name2_type arg)
  {
    msg_.name2 = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ue_msgs::srv::Attach_Request msg_;
};

class Init_Attach_Request_name1
{
public:
  Init_Attach_Request_name1()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Attach_Request_name2 name1(::ue_msgs::srv::Attach_Request::_name1_type arg)
  {
    msg_.name1 = std::move(arg);
    return Init_Attach_Request_name2(msg_);
  }

private:
  ::ue_msgs::srv::Attach_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ue_msgs::srv::Attach_Request>()
{
  return ue_msgs::srv::builder::Init_Attach_Request_name1();
}

}  // namespace ue_msgs


namespace ue_msgs
{

namespace srv
{

namespace builder
{

class Init_Attach_Response_success
{
public:
  Init_Attach_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::ue_msgs::srv::Attach_Response success(::ue_msgs::srv::Attach_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ue_msgs::srv::Attach_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ue_msgs::srv::Attach_Response>()
{
  return ue_msgs::srv::builder::Init_Attach_Response_success();
}

}  // namespace ue_msgs


namespace ue_msgs
{

namespace srv
{

namespace builder
{

class Init_Attach_Event_response
{
public:
  explicit Init_Attach_Event_response(::ue_msgs::srv::Attach_Event & msg)
  : msg_(msg)
  {}
  ::ue_msgs::srv::Attach_Event response(::ue_msgs::srv::Attach_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ue_msgs::srv::Attach_Event msg_;
};

class Init_Attach_Event_request
{
public:
  explicit Init_Attach_Event_request(::ue_msgs::srv::Attach_Event & msg)
  : msg_(msg)
  {}
  Init_Attach_Event_response request(::ue_msgs::srv::Attach_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_Attach_Event_response(msg_);
  }

private:
  ::ue_msgs::srv::Attach_Event msg_;
};

class Init_Attach_Event_info
{
public:
  Init_Attach_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Attach_Event_request info(::ue_msgs::srv::Attach_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_Attach_Event_request(msg_);
  }

private:
  ::ue_msgs::srv::Attach_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ue_msgs::srv::Attach_Event>()
{
  return ue_msgs::srv::builder::Init_Attach_Event_info();
}

}  // namespace ue_msgs

#endif  // UE_MSGS__SRV__DETAIL__ATTACH__BUILDER_HPP_
