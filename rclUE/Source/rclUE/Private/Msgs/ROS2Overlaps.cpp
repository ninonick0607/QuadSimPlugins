// Copyright 2021-2025 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from ue_msgs/msg/Overlaps.msg - do not modify

#include "Msgs/ROS2Overlaps.h"

#include "Kismet/GameplayStatics.h"

void UROS2OverlapsMsg::Init()
{
    ue_msgs__msg__Overlaps__init(&overlaps_msg);
}

void UROS2OverlapsMsg::Fini()
{
    ue_msgs__msg__Overlaps__fini(&overlaps_msg);
}

const rosidl_message_type_support_t* UROS2OverlapsMsg::GetTypeSupport() const
{
    return ROSIDL_GET_MSG_TYPE_SUPPORT(ue_msgs, msg, Overlaps);
}

void UROS2OverlapsMsg::SetMsg(const FROSOverlaps& Inputs)
{
    Inputs.SetROS2(overlaps_msg);
}

void UROS2OverlapsMsg::GetMsg(FROSOverlaps& Outputs) const
{
    Outputs.SetFromROS2(overlaps_msg);
}

void* UROS2OverlapsMsg::Get()
{
    return &overlaps_msg;
}

FString UROS2OverlapsMsg::MsgToString() const
{
    /* TODO: Fill here */
    checkNoEntry();
    return FString();
}
