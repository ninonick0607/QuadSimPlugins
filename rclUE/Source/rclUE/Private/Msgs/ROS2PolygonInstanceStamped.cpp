// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from geometry_msgs/msg/PolygonInstanceStamped.msg - do not modify

#include "Msgs/ROS2PolygonInstanceStamped.h"

#include "Kismet/GameplayStatics.h"



void UROS2PolygonInstanceStampedMsg::Init()
{
	geometry_msgs__msg__PolygonInstanceStamped__init(&polygon_instance_stamped_msg);
}

void UROS2PolygonInstanceStampedMsg::Fini()
{
	geometry_msgs__msg__PolygonInstanceStamped__fini(&polygon_instance_stamped_msg);
}

const rosidl_message_type_support_t* UROS2PolygonInstanceStampedMsg::GetTypeSupport() const
{
	return ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, PolygonInstanceStamped);
}

void UROS2PolygonInstanceStampedMsg::SetMsg(const FROSPolygonInstanceStamped& Inputs)
{
    Inputs.SetROS2(polygon_instance_stamped_msg);
}

void UROS2PolygonInstanceStampedMsg::GetMsg(FROSPolygonInstanceStamped& Outputs) const
{
    Outputs.SetFromROS2(polygon_instance_stamped_msg);
}

void* UROS2PolygonInstanceStampedMsg::Get()
{
	return &polygon_instance_stamped_msg;
}

FString UROS2PolygonInstanceStampedMsg::MsgToString() const
{
    /* TODO: Fill here */
	checkNoEntry();
	return FString();
}