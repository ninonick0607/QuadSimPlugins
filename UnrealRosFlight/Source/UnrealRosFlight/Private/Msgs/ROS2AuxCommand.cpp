// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from rosflight_msgs/msg/AuxCommand.msg - do not modify

#include "Msgs/ROS2AuxCommand.h"

#include "Kismet/GameplayStatics.h"



void UROS2AuxCommandMsg::Init()
{
	rosflight_msgs__msg__AuxCommand__init(&aux_command_msg);
}

void UROS2AuxCommandMsg::Fini()
{
	rosflight_msgs__msg__AuxCommand__fini(&aux_command_msg);
}

const rosidl_message_type_support_t* UROS2AuxCommandMsg::GetTypeSupport() const
{
	return ROSIDL_GET_MSG_TYPE_SUPPORT(rosflight_msgs, msg, AuxCommand);
}

void UROS2AuxCommandMsg::SetMsg(const FROSAuxCommand& Inputs)
{
    Inputs.SetROS2(aux_command_msg);
}

void UROS2AuxCommandMsg::GetMsg(FROSAuxCommand& Outputs) const
{
    Outputs.SetFromROS2(aux_command_msg);
}

void* UROS2AuxCommandMsg::Get()
{
	return &aux_command_msg;
}

FString UROS2AuxCommandMsg::MsgToString() const
{
    /* TODO: Fill here */
	checkNoEntry();
	return FString();
}