// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from rosflight_msgs:msg/PwmOutput.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "rosflight_msgs/msg/detail/pwm_output__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace rosflight_msgs
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void PwmOutput_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) rosflight_msgs::msg::PwmOutput(_init);
}

void PwmOutput_fini_function(void * message_memory)
{
  auto typed_message = static_cast<rosflight_msgs::msg::PwmOutput *>(message_memory);
  typed_message->~PwmOutput();
}

size_t size_function__PwmOutput__values(const void * untyped_member)
{
  (void)untyped_member;
  return 14;
}

const void * get_const_function__PwmOutput__values(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::array<uint16_t, 14> *>(untyped_member);
  return &member[index];
}

void * get_function__PwmOutput__values(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::array<uint16_t, 14> *>(untyped_member);
  return &member[index];
}

void fetch_function__PwmOutput__values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const uint16_t *>(
    get_const_function__PwmOutput__values(untyped_member, index));
  auto & value = *reinterpret_cast<uint16_t *>(untyped_value);
  value = item;
}

void assign_function__PwmOutput__values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<uint16_t *>(
    get_function__PwmOutput__values(untyped_member, index));
  const auto & value = *reinterpret_cast<const uint16_t *>(untyped_value);
  item = value;
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember PwmOutput_message_member_array[2] = {
  {
    "header",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<std_msgs::msg::Header>(),  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosflight_msgs::msg::PwmOutput, header),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "values",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    true,  // is array
    14,  // array size
    false,  // is upper bound
    offsetof(rosflight_msgs::msg::PwmOutput, values),  // bytes offset in struct
    nullptr,  // default value
    size_function__PwmOutput__values,  // size() function pointer
    get_const_function__PwmOutput__values,  // get_const(index) function pointer
    get_function__PwmOutput__values,  // get(index) function pointer
    fetch_function__PwmOutput__values,  // fetch(index, &value) function pointer
    assign_function__PwmOutput__values,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers PwmOutput_message_members = {
  "rosflight_msgs::msg",  // message namespace
  "PwmOutput",  // message name
  2,  // number of fields
  sizeof(rosflight_msgs::msg::PwmOutput),
  PwmOutput_message_member_array,  // message members
  PwmOutput_init_function,  // function to initialize message memory (memory has to be allocated)
  PwmOutput_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t PwmOutput_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &PwmOutput_message_members,
  get_message_typesupport_handle_function,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace rosflight_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<rosflight_msgs::msg::PwmOutput>()
{
  return &::rosflight_msgs::msg::rosidl_typesupport_introspection_cpp::PwmOutput_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, rosflight_msgs, msg, PwmOutput)() {
  return &::rosflight_msgs::msg::rosidl_typesupport_introspection_cpp::PwmOutput_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
