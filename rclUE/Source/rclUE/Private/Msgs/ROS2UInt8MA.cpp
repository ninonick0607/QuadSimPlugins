// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from example_interfaces/msg/UInt8MultiArray.msg - do not modify

#include "Msgs/ROS2UInt8MA.h"

#include "Kismet/GameplayStatics.h"

void UROS2UInt8MAMsg::Init()
{
    example_interfaces__msg__UInt8MultiArray__init(&u_int8_multi_array_msg);
}

void UROS2UInt8MAMsg::Fini()
{
    example_interfaces__msg__UInt8MultiArray__fini(&u_int8_multi_array_msg);
}

const rosidl_message_type_support_t* UROS2UInt8MAMsg::GetTypeSupport() const
{
    return ROSIDL_GET_MSG_TYPE_SUPPORT(example_interfaces, msg, UInt8MultiArray);
}

void UROS2UInt8MAMsg::SetMsg(const FROSUInt8MA& Inputs)
{
    Inputs.SetROS2(u_int8_multi_array_msg);
}

void UROS2UInt8MAMsg::GetMsg(FROSUInt8MA& Outputs) const
{
    Outputs.SetFromROS2(u_int8_multi_array_msg);
}

void* UROS2UInt8MAMsg::Get()
{
    return &u_int8_multi_array_msg;
}

FString UROS2UInt8MAMsg::MsgToString() const
{
    /* TODO: Fill here */
    checkNoEntry();
    return FString();
}
