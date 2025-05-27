// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from pcl_msgs:srv/UpdateFilename.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "pcl_msgs/srv/update_filename.hpp"


#ifndef PCL_MSGS__SRV__DETAIL__UPDATE_FILENAME__BUILDER_HPP_
#define PCL_MSGS__SRV__DETAIL__UPDATE_FILENAME__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "pcl_msgs/srv/detail/update_filename__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace pcl_msgs
{

namespace srv
{

namespace builder
{

class Init_UpdateFilename_Request_filename
{
public:
  Init_UpdateFilename_Request_filename()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::pcl_msgs::srv::UpdateFilename_Request filename(::pcl_msgs::srv::UpdateFilename_Request::_filename_type arg)
  {
    msg_.filename = std::move(arg);
    return std::move(msg_);
  }

private:
  ::pcl_msgs::srv::UpdateFilename_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::pcl_msgs::srv::UpdateFilename_Request>()
{
  return pcl_msgs::srv::builder::Init_UpdateFilename_Request_filename();
}

}  // namespace pcl_msgs


namespace pcl_msgs
{

namespace srv
{

namespace builder
{

class Init_UpdateFilename_Response_success
{
public:
  Init_UpdateFilename_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::pcl_msgs::srv::UpdateFilename_Response success(::pcl_msgs::srv::UpdateFilename_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::pcl_msgs::srv::UpdateFilename_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::pcl_msgs::srv::UpdateFilename_Response>()
{
  return pcl_msgs::srv::builder::Init_UpdateFilename_Response_success();
}

}  // namespace pcl_msgs


namespace pcl_msgs
{

namespace srv
{

namespace builder
{

class Init_UpdateFilename_Event_response
{
public:
  explicit Init_UpdateFilename_Event_response(::pcl_msgs::srv::UpdateFilename_Event & msg)
  : msg_(msg)
  {}
  ::pcl_msgs::srv::UpdateFilename_Event response(::pcl_msgs::srv::UpdateFilename_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::pcl_msgs::srv::UpdateFilename_Event msg_;
};

class Init_UpdateFilename_Event_request
{
public:
  explicit Init_UpdateFilename_Event_request(::pcl_msgs::srv::UpdateFilename_Event & msg)
  : msg_(msg)
  {}
  Init_UpdateFilename_Event_response request(::pcl_msgs::srv::UpdateFilename_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_UpdateFilename_Event_response(msg_);
  }

private:
  ::pcl_msgs::srv::UpdateFilename_Event msg_;
};

class Init_UpdateFilename_Event_info
{
public:
  Init_UpdateFilename_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_UpdateFilename_Event_request info(::pcl_msgs::srv::UpdateFilename_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_UpdateFilename_Event_request(msg_);
  }

private:
  ::pcl_msgs::srv::UpdateFilename_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::pcl_msgs::srv::UpdateFilename_Event>()
{
  return pcl_msgs::srv::builder::Init_UpdateFilename_Event_info();
}

}  // namespace pcl_msgs

#endif  // PCL_MSGS__SRV__DETAIL__UPDATE_FILENAME__BUILDER_HPP_
