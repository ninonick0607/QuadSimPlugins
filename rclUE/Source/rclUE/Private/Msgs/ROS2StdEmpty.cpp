// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from std_msgs/msg/Empty.msg - do not modify

#include "Msgs/ROS2StdEmpty.h"

#include "Kismet/GameplayStatics.h"

void UROS2StdEmptyMsg::Init()
{
    std_msgs__msg__Empty__init(&empty_msg);
}

void UROS2StdEmptyMsg::Fini()
{
    std_msgs__msg__Empty__fini(&empty_msg);
}

const rosidl_message_type_support_t* UROS2StdEmptyMsg::GetTypeSupport() const
{
    return ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty);
}

void UROS2StdEmptyMsg::SetMsg(const FROSStdEmpty& Inputs)
{
    Inputs.SetROS2(empty_msg);
}

void UROS2StdEmptyMsg::GetMsg(FROSStdEmpty& Outputs) const
{
    Outputs.SetFromROS2(empty_msg);
}

void* UROS2StdEmptyMsg::Get()
{
    return &empty_msg;
}

FString UROS2StdEmptyMsg::MsgToString() const
{
    /* TODO: Fill here */
    checkNoEntry();
    return FString();
}
