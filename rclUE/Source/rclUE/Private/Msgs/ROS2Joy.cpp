// Copyright 2021-2025 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from sensor_msgs/msg/Joy.msg - do not modify

#include "Msgs/ROS2Joy.h"

#include "Kismet/GameplayStatics.h"

void UROS2JoyMsg::Init()
{
    sensor_msgs__msg__Joy__init(&joy_msg);
}

void UROS2JoyMsg::Fini()
{
    sensor_msgs__msg__Joy__fini(&joy_msg);
}

const rosidl_message_type_support_t* UROS2JoyMsg::GetTypeSupport() const
{
    return ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Joy);
}

void UROS2JoyMsg::SetMsg(const FROSJoy& Inputs)
{
    Inputs.SetROS2(joy_msg);
}

void UROS2JoyMsg::GetMsg(FROSJoy& Outputs) const
{
    Outputs.SetFromROS2(joy_msg);
}

void* UROS2JoyMsg::Get()
{
    return &joy_msg;
}

FString UROS2JoyMsg::MsgToString() const
{
    /* TODO: Fill here */
    checkNoEntry();
    return FString();
}
