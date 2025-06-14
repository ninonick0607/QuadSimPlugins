// Copyright 2021-2025 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from geometry_msgs/msg/VelocityStamped.msg - do not modify

#include "Msgs/ROS2VelocityStamped.h"

#include "Kismet/GameplayStatics.h"

void UROS2VelocityStampedMsg::Init()
{
    geometry_msgs__msg__VelocityStamped__init(&velocity_stamped_msg);
}

void UROS2VelocityStampedMsg::Fini()
{
    geometry_msgs__msg__VelocityStamped__fini(&velocity_stamped_msg);
}

const rosidl_message_type_support_t* UROS2VelocityStampedMsg::GetTypeSupport() const
{
    return ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, VelocityStamped);
}

void UROS2VelocityStampedMsg::SetMsg(const FROSVelocityStamped& Inputs)
{
    Inputs.SetROS2(velocity_stamped_msg);
}

void UROS2VelocityStampedMsg::GetMsg(FROSVelocityStamped& Outputs) const
{
    Outputs.SetFromROS2(velocity_stamped_msg);
}

void* UROS2VelocityStampedMsg::Get()
{
    return &velocity_stamped_msg;
}

FString UROS2VelocityStampedMsg::MsgToString() const
{
    /* TODO: Fill here */
    checkNoEntry();
    return FString();
}
