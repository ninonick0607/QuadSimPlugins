// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from rosflight_msgs/srv/ParamGet.srv - do not modify

#include "Srvs/ROS2ParamGet.h"





const rosidl_service_type_support_t* UROS2ParamGetSrv::GetTypeSupport() const
{
    return ROSIDL_GET_SRV_TYPE_SUPPORT(rosflight_msgs, srv, ParamGet);
}

void UROS2ParamGetSrv::Init()
{
    rosflight_msgs__srv__ParamGet_Request__init(&ParamGet_req);
    rosflight_msgs__srv__ParamGet_Response__init(&ParamGet_res);
}

void UROS2ParamGetSrv::Fini()
{
    rosflight_msgs__srv__ParamGet_Request__fini(&ParamGet_req);
    rosflight_msgs__srv__ParamGet_Response__fini(&ParamGet_res);
}

void UROS2ParamGetSrv::SetRequest(const FROSParamGetReq& Request)
{
    Request.SetROS2(ParamGet_req);
}

void UROS2ParamGetSrv::GetRequest(FROSParamGetReq& Request) const
{
    Request.SetFromROS2(ParamGet_req);
}

void UROS2ParamGetSrv::SetResponse(const FROSParamGetRes& Response)
{
    Response.SetROS2(ParamGet_res);
}

void UROS2ParamGetSrv::GetResponse(FROSParamGetRes& Response) const
{
    Response.SetFromROS2(ParamGet_res);
}

void* UROS2ParamGetSrv::GetRequest()
{
    return &ParamGet_req;
}

void* UROS2ParamGetSrv::GetResponse()
{
    return &ParamGet_res;
}

FString UROS2ParamGetSrv::SrvRequestToString() const
{
    /* TODO: Fill here */
	checkNoEntry();
    return FString();
}

FString UROS2ParamGetSrv::SrvResponseToString() const
{
    /* TODO: Fill here */
	checkNoEntry();
    return FString();
}