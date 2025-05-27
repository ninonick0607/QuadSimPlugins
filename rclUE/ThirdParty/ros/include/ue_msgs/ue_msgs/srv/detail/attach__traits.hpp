// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from ue_msgs:srv/Attach.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "ue_msgs/srv/attach.hpp"


#ifndef UE_MSGS__SRV__DETAIL__ATTACH__TRAITS_HPP_
#define UE_MSGS__SRV__DETAIL__ATTACH__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "ue_msgs/srv/detail/attach__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace ue_msgs
{

namespace srv
{

inline void to_flow_style_yaml(
  const Attach_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: name1
  {
    out << "name1: ";
    rosidl_generator_traits::value_to_yaml(msg.name1, out);
    out << ", ";
  }

  // member: name2
  {
    out << "name2: ";
    rosidl_generator_traits::value_to_yaml(msg.name2, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Attach_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: name1
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "name1: ";
    rosidl_generator_traits::value_to_yaml(msg.name1, out);
    out << "\n";
  }

  // member: name2
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "name2: ";
    rosidl_generator_traits::value_to_yaml(msg.name2, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Attach_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace ue_msgs

namespace rosidl_generator_traits
{

[[deprecated("use ue_msgs::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const ue_msgs::srv::Attach_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  ue_msgs::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use ue_msgs::srv::to_yaml() instead")]]
inline std::string to_yaml(const ue_msgs::srv::Attach_Request & msg)
{
  return ue_msgs::srv::to_yaml(msg);
}

template<>
inline const char * data_type<ue_msgs::srv::Attach_Request>()
{
  return "ue_msgs::srv::Attach_Request";
}

template<>
inline const char * name<ue_msgs::srv::Attach_Request>()
{
  return "ue_msgs/srv/Attach_Request";
}

template<>
struct has_fixed_size<ue_msgs::srv::Attach_Request>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<ue_msgs::srv::Attach_Request>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<ue_msgs::srv::Attach_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace ue_msgs
{

namespace srv
{

inline void to_flow_style_yaml(
  const Attach_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: success
  {
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Attach_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: success
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Attach_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace ue_msgs

namespace rosidl_generator_traits
{

[[deprecated("use ue_msgs::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const ue_msgs::srv::Attach_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  ue_msgs::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use ue_msgs::srv::to_yaml() instead")]]
inline std::string to_yaml(const ue_msgs::srv::Attach_Response & msg)
{
  return ue_msgs::srv::to_yaml(msg);
}

template<>
inline const char * data_type<ue_msgs::srv::Attach_Response>()
{
  return "ue_msgs::srv::Attach_Response";
}

template<>
inline const char * name<ue_msgs::srv::Attach_Response>()
{
  return "ue_msgs/srv/Attach_Response";
}

template<>
struct has_fixed_size<ue_msgs::srv::Attach_Response>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<ue_msgs::srv::Attach_Response>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<ue_msgs::srv::Attach_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__traits.hpp"

namespace ue_msgs
{

namespace srv
{

inline void to_flow_style_yaml(
  const Attach_Event & msg,
  std::ostream & out)
{
  out << "{";
  // member: info
  {
    out << "info: ";
    to_flow_style_yaml(msg.info, out);
    out << ", ";
  }

  // member: request
  {
    if (msg.request.size() == 0) {
      out << "request: []";
    } else {
      out << "request: [";
      size_t pending_items = msg.request.size();
      for (auto item : msg.request) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: response
  {
    if (msg.response.size() == 0) {
      out << "response: []";
    } else {
      out << "response: [";
      size_t pending_items = msg.response.size();
      for (auto item : msg.response) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Attach_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: info
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "info:\n";
    to_block_style_yaml(msg.info, out, indentation + 2);
  }

  // member: request
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.request.size() == 0) {
      out << "request: []\n";
    } else {
      out << "request:\n";
      for (auto item : msg.request) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: response
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.response.size() == 0) {
      out << "response: []\n";
    } else {
      out << "response:\n";
      for (auto item : msg.response) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Attach_Event & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace ue_msgs

namespace rosidl_generator_traits
{

[[deprecated("use ue_msgs::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const ue_msgs::srv::Attach_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  ue_msgs::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use ue_msgs::srv::to_yaml() instead")]]
inline std::string to_yaml(const ue_msgs::srv::Attach_Event & msg)
{
  return ue_msgs::srv::to_yaml(msg);
}

template<>
inline const char * data_type<ue_msgs::srv::Attach_Event>()
{
  return "ue_msgs::srv::Attach_Event";
}

template<>
inline const char * name<ue_msgs::srv::Attach_Event>()
{
  return "ue_msgs/srv/Attach_Event";
}

template<>
struct has_fixed_size<ue_msgs::srv::Attach_Event>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<ue_msgs::srv::Attach_Event>
  : std::integral_constant<bool, has_bounded_size<service_msgs::msg::ServiceEventInfo>::value && has_bounded_size<ue_msgs::srv::Attach_Request>::value && has_bounded_size<ue_msgs::srv::Attach_Response>::value> {};

template<>
struct is_message<ue_msgs::srv::Attach_Event>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<ue_msgs::srv::Attach>()
{
  return "ue_msgs::srv::Attach";
}

template<>
inline const char * name<ue_msgs::srv::Attach>()
{
  return "ue_msgs/srv/Attach";
}

template<>
struct has_fixed_size<ue_msgs::srv::Attach>
  : std::integral_constant<
    bool,
    has_fixed_size<ue_msgs::srv::Attach_Request>::value &&
    has_fixed_size<ue_msgs::srv::Attach_Response>::value
  >
{
};

template<>
struct has_bounded_size<ue_msgs::srv::Attach>
  : std::integral_constant<
    bool,
    has_bounded_size<ue_msgs::srv::Attach_Request>::value &&
    has_bounded_size<ue_msgs::srv::Attach_Response>::value
  >
{
};

template<>
struct is_service<ue_msgs::srv::Attach>
  : std::true_type
{
};

template<>
struct is_service_request<ue_msgs::srv::Attach_Request>
  : std::true_type
{
};

template<>
struct is_service_response<ue_msgs::srv::Attach_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // UE_MSGS__SRV__DETAIL__ATTACH__TRAITS_HPP_
