// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from sensor_msgs/msg/PointCloud2.msg - do not modify

#include "Msgs/ROS2PointCloud2.h"

#include "Kismet/GameplayStatics.h"

void UROS2PointCloud2Msg::Init()
{
    sensor_msgs__msg__PointCloud2__init(&point_cloud2_msg);
}

void UROS2PointCloud2Msg::Fini()
{
    sensor_msgs__msg__PointCloud2__fini(&point_cloud2_msg);
}

const rosidl_message_type_support_t* UROS2PointCloud2Msg::GetTypeSupport() const
{
    return ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, PointCloud2);
}

void UROS2PointCloud2Msg::SetMsg(const FROSPointCloud2& Inputs)
{
    Inputs.SetROS2(point_cloud2_msg);
}

void UROS2PointCloud2Msg::GetMsg(FROSPointCloud2& Outputs) const
{
    Outputs.SetFromROS2(point_cloud2_msg);
}

void* UROS2PointCloud2Msg::Get()
{
    return &point_cloud2_msg;
}

FString UROS2PointCloud2Msg::MsgToString() const
{
    /* TODO: Fill here */
    checkNoEntry();
    return FString();
}
