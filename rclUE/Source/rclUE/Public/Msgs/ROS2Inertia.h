// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from geometry_msgs/msg/Inertia.msg - do not modify

#pragma once

// UE
#include "CoreMinimal.h"

// ROS
#include "geometry_msgs/msg/inertia.h"

// rclUE
#include "Msgs/ROS2GenericMsg.h"
#include "rclcUtilities.h"

// Generated Msg/Srv/Action(can be empty)

// Generated
#include "ROS2Inertia.generated.h"

USTRUCT(Blueprintable)
struct RCLUE_API FROSInertia
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double M = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Com = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double Ixx = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double Ixy = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double Ixz = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double Iyy = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double Iyz = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double Izz = 0.f;

    FROSInertia()
    {
    }

    void SetFromROS2(const geometry_msgs__msg__Inertia& in_ros_data)
    {
        M = in_ros_data.m;

        Com = UROS2Utils::VectorROSToUE<geometry_msgs__msg__Vector3>(in_ros_data.com);

        Ixx = in_ros_data.ixx;

        Ixy = in_ros_data.ixy;

        Ixz = in_ros_data.ixz;

        Iyy = in_ros_data.iyy;

        Iyz = in_ros_data.iyz;

        Izz = in_ros_data.izz;
    }

    void SetROS2(geometry_msgs__msg__Inertia& out_ros_data) const
    {
        out_ros_data.m = M;

        out_ros_data.com = UROS2Utils::VectorUEToROS<geometry_msgs__msg__Vector3>(Com);

        out_ros_data.ixx = Ixx;

        out_ros_data.ixy = Ixy;

        out_ros_data.ixz = Ixz;

        out_ros_data.iyy = Iyy;

        out_ros_data.iyz = Iyz;

        out_ros_data.izz = Izz;
    }
};

UCLASS()
class RCLUE_API UROS2InertiaMsg : public UROS2GenericMsg
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Fini() override;

    virtual const rosidl_message_type_support_t* GetTypeSupport() const override;

    UFUNCTION(BlueprintCallable)
    void SetMsg(const FROSInertia& Input);

    UFUNCTION(BlueprintCallable)
    void GetMsg(FROSInertia& Output) const;

    virtual void* Get() override;

private:
    virtual FString MsgToString() const override;

    geometry_msgs__msg__Inertia inertia_msg;
};
