// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from geometry_msgs:msg/PolygonInstance.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/polygon_instance.hpp"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__POLYGON_INSTANCE__TRAITS_HPP_
#define GEOMETRY_MSGS__MSG__DETAIL__POLYGON_INSTANCE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "geometry_msgs/msg/detail/polygon_instance__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'polygon'
#include "geometry_msgs/msg/detail/polygon__traits.hpp"

namespace geometry_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const PolygonInstance & msg,
  std::ostream & out)
{
  out << "{";
  // member: polygon
  {
    out << "polygon: ";
    to_flow_style_yaml(msg.polygon, out);
    out << ", ";
  }

  // member: id
  {
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const PolygonInstance & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: polygon
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "polygon:\n";
    to_block_style_yaml(msg.polygon, out, indentation + 2);
  }

  // member: id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const PolygonInstance & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace geometry_msgs

namespace rosidl_generator_traits
{

[[deprecated("use geometry_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const geometry_msgs::msg::PolygonInstance & msg,
  std::ostream & out, size_t indentation = 0)
{
  geometry_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use geometry_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const geometry_msgs::msg::PolygonInstance & msg)
{
  return geometry_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<geometry_msgs::msg::PolygonInstance>()
{
  return "geometry_msgs::msg::PolygonInstance";
}

template<>
inline const char * name<geometry_msgs::msg::PolygonInstance>()
{
  return "geometry_msgs/msg/PolygonInstance";
}

template<>
struct has_fixed_size<geometry_msgs::msg::PolygonInstance>
  : std::integral_constant<bool, has_fixed_size<geometry_msgs::msg::Polygon>::value> {};

template<>
struct has_bounded_size<geometry_msgs::msg::PolygonInstance>
  : std::integral_constant<bool, has_bounded_size<geometry_msgs::msg::Polygon>::value> {};

template<>
struct is_message<geometry_msgs::msg::PolygonInstance>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // GEOMETRY_MSGS__MSG__DETAIL__POLYGON_INSTANCE__TRAITS_HPP_
