// Copyright 2021-2025 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from ue_msgs/msg/OverlapEvent.msg - do not modify

#include "Msgs/ROS2OverlapEvent.h"

#include "Kismet/GameplayStatics.h"

void UROS2OverlapEventMsg::Init()
{
    ue_msgs__msg__OverlapEvent__init(&overlap_event_msg);
}

void UROS2OverlapEventMsg::Fini()
{
    ue_msgs__msg__OverlapEvent__fini(&overlap_event_msg);
}

const rosidl_message_type_support_t* UROS2OverlapEventMsg::GetTypeSupport() const
{
    return ROSIDL_GET_MSG_TYPE_SUPPORT(ue_msgs, msg, OverlapEvent);
}

void UROS2OverlapEventMsg::SetMsg(const FROSOverlapEvent& Inputs)
{
    Inputs.SetROS2(overlap_event_msg);
}

void UROS2OverlapEventMsg::GetMsg(FROSOverlapEvent& Outputs) const
{
    Outputs.SetFromROS2(overlap_event_msg);
}

void* UROS2OverlapEventMsg::Get()
{
    return &overlap_event_msg;
}

FString UROS2OverlapEventMsg::MsgToString() const
{
    /* TODO: Fill here */
    checkNoEntry();
    return FString();
}
