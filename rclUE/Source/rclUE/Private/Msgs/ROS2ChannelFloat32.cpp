// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from sensor_msgs/msg/ChannelFloat32.msg - do not modify

#include "Msgs/ROS2ChannelFloat32.h"

#include "Kismet/GameplayStatics.h"

void UROS2ChannelFloat32Msg::Init()
{
    sensor_msgs__msg__ChannelFloat32__init(&channel_float32_msg);
}

void UROS2ChannelFloat32Msg::Fini()
{
    sensor_msgs__msg__ChannelFloat32__fini(&channel_float32_msg);
}

const rosidl_message_type_support_t* UROS2ChannelFloat32Msg::GetTypeSupport() const
{
    return ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, ChannelFloat32);
}

void UROS2ChannelFloat32Msg::SetMsg(const FROSChannelFloat32& Inputs)
{
    Inputs.SetROS2(channel_float32_msg);
}

void UROS2ChannelFloat32Msg::GetMsg(FROSChannelFloat32& Outputs) const
{
    Outputs.SetFromROS2(channel_float32_msg);
}

void* UROS2ChannelFloat32Msg::Get()
{
    return &channel_float32_msg;
}

FString UROS2ChannelFloat32Msg::MsgToString() const
{
    /* TODO: Fill here */
    checkNoEntry();
    return FString();
}
