// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from example_interfaces/msg/UInt16.msg - do not modify

#include "Msgs/ROS2UInt16.h"

#include "Kismet/GameplayStatics.h"

void UROS2UInt16Msg::Init()
{
    example_interfaces__msg__UInt16__init(&u_int16_msg);
}

void UROS2UInt16Msg::Fini()
{
    example_interfaces__msg__UInt16__fini(&u_int16_msg);
}

const rosidl_message_type_support_t* UROS2UInt16Msg::GetTypeSupport() const
{
    return ROSIDL_GET_MSG_TYPE_SUPPORT(example_interfaces, msg, UInt16);
}

void UROS2UInt16Msg::SetMsg(const FROSUInt16& Inputs)
{
    Inputs.SetROS2(u_int16_msg);
}

void UROS2UInt16Msg::GetMsg(FROSUInt16& Outputs) const
{
    Outputs.SetFromROS2(u_int16_msg);
}

void* UROS2UInt16Msg::Get()
{
    return &u_int16_msg;
}

FString UROS2UInt16Msg::MsgToString() const
{
    /* TODO: Fill here */
    checkNoEntry();
    return FString();
}
