// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from example_interfaces/msg/WString.msg - do not modify

#include "Msgs/ROS2WStr.h"

#include "Kismet/GameplayStatics.h"

void UROS2WStrMsg::Init()
{
    example_interfaces__msg__WString__init(&w_string_msg);
}

void UROS2WStrMsg::Fini()
{
    example_interfaces__msg__WString__fini(&w_string_msg);
}

const rosidl_message_type_support_t* UROS2WStrMsg::GetTypeSupport() const
{
    return ROSIDL_GET_MSG_TYPE_SUPPORT(example_interfaces, msg, WString);
}

void UROS2WStrMsg::SetMsg(const FROSWStr& Inputs)
{
    Inputs.SetROS2(w_string_msg);
}

void UROS2WStrMsg::GetMsg(FROSWStr& Outputs) const
{
    Outputs.SetFromROS2(w_string_msg);
}

void* UROS2WStrMsg::Get()
{
    return &w_string_msg;
}

FString UROS2WStrMsg::MsgToString() const
{
    /* TODO: Fill here */
    checkNoEntry();
    return FString();
}
