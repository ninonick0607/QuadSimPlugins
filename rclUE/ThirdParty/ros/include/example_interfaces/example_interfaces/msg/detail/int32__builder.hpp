// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from example_interfaces:msg/Int32.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "example_interfaces/msg/int32.hpp"


#ifndef EXAMPLE_INTERFACES__MSG__DETAIL__INT32__BUILDER_HPP_
#define EXAMPLE_INTERFACES__MSG__DETAIL__INT32__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "example_interfaces/msg/detail/int32__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace example_interfaces
{

namespace msg
{

namespace builder
{

class Init_Int32_data
{
public:
  Init_Int32_data()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::example_interfaces::msg::Int32 data(::example_interfaces::msg::Int32::_data_type arg)
  {
    msg_.data = std::move(arg);
    return std::move(msg_);
  }

private:
  ::example_interfaces::msg::Int32 msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::example_interfaces::msg::Int32>()
{
  return example_interfaces::msg::builder::Init_Int32_data();
}

}  // namespace example_interfaces

#endif  // EXAMPLE_INTERFACES__MSG__DETAIL__INT32__BUILDER_HPP_
