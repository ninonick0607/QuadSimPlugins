// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from visualization_msgs:msg/InteractiveMarkerUpdate.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "visualization_msgs/msg/detail/interactive_marker_update__functions.h"
#include "visualization_msgs/msg/detail/interactive_marker_update__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace visualization_msgs
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void InteractiveMarkerUpdate_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) visualization_msgs::msg::InteractiveMarkerUpdate(_init);
}

void InteractiveMarkerUpdate_fini_function(void * message_memory)
{
  auto typed_message = static_cast<visualization_msgs::msg::InteractiveMarkerUpdate *>(message_memory);
  typed_message->~InteractiveMarkerUpdate();
}

size_t size_function__InteractiveMarkerUpdate__markers(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<visualization_msgs::msg::InteractiveMarker> *>(untyped_member);
  return member->size();
}

const void * get_const_function__InteractiveMarkerUpdate__markers(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<visualization_msgs::msg::InteractiveMarker> *>(untyped_member);
  return &member[index];
}

void * get_function__InteractiveMarkerUpdate__markers(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<visualization_msgs::msg::InteractiveMarker> *>(untyped_member);
  return &member[index];
}

void fetch_function__InteractiveMarkerUpdate__markers(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const visualization_msgs::msg::InteractiveMarker *>(
    get_const_function__InteractiveMarkerUpdate__markers(untyped_member, index));
  auto & value = *reinterpret_cast<visualization_msgs::msg::InteractiveMarker *>(untyped_value);
  value = item;
}

void assign_function__InteractiveMarkerUpdate__markers(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<visualization_msgs::msg::InteractiveMarker *>(
    get_function__InteractiveMarkerUpdate__markers(untyped_member, index));
  const auto & value = *reinterpret_cast<const visualization_msgs::msg::InteractiveMarker *>(untyped_value);
  item = value;
}

void resize_function__InteractiveMarkerUpdate__markers(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<visualization_msgs::msg::InteractiveMarker> *>(untyped_member);
  member->resize(size);
}

size_t size_function__InteractiveMarkerUpdate__poses(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<visualization_msgs::msg::InteractiveMarkerPose> *>(untyped_member);
  return member->size();
}

const void * get_const_function__InteractiveMarkerUpdate__poses(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<visualization_msgs::msg::InteractiveMarkerPose> *>(untyped_member);
  return &member[index];
}

void * get_function__InteractiveMarkerUpdate__poses(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<visualization_msgs::msg::InteractiveMarkerPose> *>(untyped_member);
  return &member[index];
}

void fetch_function__InteractiveMarkerUpdate__poses(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const visualization_msgs::msg::InteractiveMarkerPose *>(
    get_const_function__InteractiveMarkerUpdate__poses(untyped_member, index));
  auto & value = *reinterpret_cast<visualization_msgs::msg::InteractiveMarkerPose *>(untyped_value);
  value = item;
}

void assign_function__InteractiveMarkerUpdate__poses(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<visualization_msgs::msg::InteractiveMarkerPose *>(
    get_function__InteractiveMarkerUpdate__poses(untyped_member, index));
  const auto & value = *reinterpret_cast<const visualization_msgs::msg::InteractiveMarkerPose *>(untyped_value);
  item = value;
}

void resize_function__InteractiveMarkerUpdate__poses(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<visualization_msgs::msg::InteractiveMarkerPose> *>(untyped_member);
  member->resize(size);
}

size_t size_function__InteractiveMarkerUpdate__erases(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<std::string> *>(untyped_member);
  return member->size();
}

const void * get_const_function__InteractiveMarkerUpdate__erases(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<std::string> *>(untyped_member);
  return &member[index];
}

void * get_function__InteractiveMarkerUpdate__erases(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<std::string> *>(untyped_member);
  return &member[index];
}

void fetch_function__InteractiveMarkerUpdate__erases(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const std::string *>(
    get_const_function__InteractiveMarkerUpdate__erases(untyped_member, index));
  auto & value = *reinterpret_cast<std::string *>(untyped_value);
  value = item;
}

void assign_function__InteractiveMarkerUpdate__erases(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<std::string *>(
    get_function__InteractiveMarkerUpdate__erases(untyped_member, index));
  const auto & value = *reinterpret_cast<const std::string *>(untyped_value);
  item = value;
}

void resize_function__InteractiveMarkerUpdate__erases(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<std::string> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember InteractiveMarkerUpdate_message_member_array[6] = {
  {
    "server_id",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs::msg::InteractiveMarkerUpdate, server_id),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "seq_num",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs::msg::InteractiveMarkerUpdate, seq_num),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "type",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs::msg::InteractiveMarkerUpdate, type),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "markers",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<visualization_msgs::msg::InteractiveMarker>(),  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs::msg::InteractiveMarkerUpdate, markers),  // bytes offset in struct
    nullptr,  // default value
    size_function__InteractiveMarkerUpdate__markers,  // size() function pointer
    get_const_function__InteractiveMarkerUpdate__markers,  // get_const(index) function pointer
    get_function__InteractiveMarkerUpdate__markers,  // get(index) function pointer
    fetch_function__InteractiveMarkerUpdate__markers,  // fetch(index, &value) function pointer
    assign_function__InteractiveMarkerUpdate__markers,  // assign(index, value) function pointer
    resize_function__InteractiveMarkerUpdate__markers  // resize(index) function pointer
  },
  {
    "poses",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<visualization_msgs::msg::InteractiveMarkerPose>(),  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs::msg::InteractiveMarkerUpdate, poses),  // bytes offset in struct
    nullptr,  // default value
    size_function__InteractiveMarkerUpdate__poses,  // size() function pointer
    get_const_function__InteractiveMarkerUpdate__poses,  // get_const(index) function pointer
    get_function__InteractiveMarkerUpdate__poses,  // get(index) function pointer
    fetch_function__InteractiveMarkerUpdate__poses,  // fetch(index, &value) function pointer
    assign_function__InteractiveMarkerUpdate__poses,  // assign(index, value) function pointer
    resize_function__InteractiveMarkerUpdate__poses  // resize(index) function pointer
  },
  {
    "erases",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs::msg::InteractiveMarkerUpdate, erases),  // bytes offset in struct
    nullptr,  // default value
    size_function__InteractiveMarkerUpdate__erases,  // size() function pointer
    get_const_function__InteractiveMarkerUpdate__erases,  // get_const(index) function pointer
    get_function__InteractiveMarkerUpdate__erases,  // get(index) function pointer
    fetch_function__InteractiveMarkerUpdate__erases,  // fetch(index, &value) function pointer
    assign_function__InteractiveMarkerUpdate__erases,  // assign(index, value) function pointer
    resize_function__InteractiveMarkerUpdate__erases  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers InteractiveMarkerUpdate_message_members = {
  "visualization_msgs::msg",  // message namespace
  "InteractiveMarkerUpdate",  // message name
  6,  // number of fields
  sizeof(visualization_msgs::msg::InteractiveMarkerUpdate),
  false,  // has_any_key_member_
  InteractiveMarkerUpdate_message_member_array,  // message members
  InteractiveMarkerUpdate_init_function,  // function to initialize message memory (memory has to be allocated)
  InteractiveMarkerUpdate_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t InteractiveMarkerUpdate_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &InteractiveMarkerUpdate_message_members,
  get_message_typesupport_handle_function,
  &visualization_msgs__msg__InteractiveMarkerUpdate__get_type_hash,
  &visualization_msgs__msg__InteractiveMarkerUpdate__get_type_description,
  &visualization_msgs__msg__InteractiveMarkerUpdate__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace visualization_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<visualization_msgs::msg::InteractiveMarkerUpdate>()
{
  return &::visualization_msgs::msg::rosidl_typesupport_introspection_cpp::InteractiveMarkerUpdate_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, visualization_msgs, msg, InteractiveMarkerUpdate)() {
  return &::visualization_msgs::msg::rosidl_typesupport_introspection_cpp::InteractiveMarkerUpdate_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
