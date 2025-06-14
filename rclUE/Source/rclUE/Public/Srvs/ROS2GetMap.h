// Copyright 2021-2025 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from nav_msgs/srv/GetMap.srv - do not modify

#pragma once

// UE
#include "CoreMinimal.h"

// ROS
#include "nav_msgs/srv/get_map.h"

// rclUE
#include "Srvs/ROS2GenericSrv.h"
#include "rclcUtilities.h"

// Generated Msg/Srv/Action(can be empty)

#include "Msgs/ROS2OccupancyGrid.h"

// Generated
#include "ROS2GetMap.generated.h"

// potential problem: if this struct is defined multiple times!
USTRUCT(Blueprintable)
struct RCLUE_API FROSGetMapReq
{
    GENERATED_BODY()

public:
    FROSGetMapReq()
    {
    }

    void SetFromROS2(const nav_msgs__srv__GetMap_Request& in_ros_data)
    {
    }

    void SetROS2(nav_msgs__srv__GetMap_Request& out_ros_data) const
    {
    }
};

USTRUCT(Blueprintable)
struct RCLUE_API FROSGetMapRes
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FROSOccupancyGrid Map;

    FROSGetMapRes()
    {
    }

    void SetFromROS2(const nav_msgs__srv__GetMap_Response& in_ros_data)
    {
        Map.SetFromROS2(in_ros_data.map);
    }

    void SetROS2(nav_msgs__srv__GetMap_Response& out_ros_data) const
    {
        Map.SetROS2(out_ros_data.map);
    }
};

UCLASS()
class RCLUE_API UROS2GetMapSrv : public UROS2GenericSrv
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual void Init() override;

    UFUNCTION(BlueprintCallable)
    virtual void Fini() override;

    virtual const rosidl_service_type_support_t* GetTypeSupport() const override;

    // used by client
    UFUNCTION(BlueprintCallable)
    void SetRequest(const FROSGetMapReq& Request);

    // used by service
    UFUNCTION(BlueprintCallable)
    void GetRequest(FROSGetMapReq& Request) const;

    // used by service
    UFUNCTION(BlueprintCallable)
    void SetResponse(const FROSGetMapRes& Response);

    // used by client
    UFUNCTION(BlueprintCallable)
    void GetResponse(FROSGetMapRes& Response) const;

    virtual void* GetRequest() override;
    virtual void* GetResponse() override;

private:
    virtual FString SrvRequestToString() const override;
    virtual FString SrvResponseToString() const override;

    nav_msgs__srv__GetMap_Request GetMap_req;
    nav_msgs__srv__GetMap_Response GetMap_res;
};
