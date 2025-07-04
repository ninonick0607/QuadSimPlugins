// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from pcl_msgs/msg/Vertices.msg - do not modify

#include "Msgs/ROS2Vertices.h"

#include "Kismet/GameplayStatics.h"

void UROS2VerticesMsg::Init()
{
    pcl_msgs__msg__Vertices__init(&vertices_msg);
}

void UROS2VerticesMsg::Fini()
{
    pcl_msgs__msg__Vertices__fini(&vertices_msg);
}

const rosidl_message_type_support_t* UROS2VerticesMsg::GetTypeSupport() const
{
    return ROSIDL_GET_MSG_TYPE_SUPPORT(pcl_msgs, msg, Vertices);
}

void UROS2VerticesMsg::SetMsg(const FROSVertices& Inputs)
{
    Inputs.SetROS2(vertices_msg);
}

void UROS2VerticesMsg::GetMsg(FROSVertices& Outputs) const
{
    Outputs.SetFromROS2(vertices_msg);
}

void* UROS2VerticesMsg::Get()
{
    return &vertices_msg;
}

FString UROS2VerticesMsg::MsgToString() const
{
    /* TODO: Fill here */
    checkNoEntry();
    return FString();
}
