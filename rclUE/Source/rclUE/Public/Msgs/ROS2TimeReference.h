// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from sensor_msgs/msg/TimeReference.msg - do not modify

#pragma once

// UE
#include "CoreMinimal.h"

// ROS
#include "sensor_msgs/msg/time_reference.h"

// rclUE
#include "Msgs/ROS2GenericMsg.h"
#include "rclcUtilities.h"

// Generated Msg/Srv/Action(can be empty)
#include "Msgs/ROS2Header.h"
#include "Msgs/ROS2Time.h"

// Generated
#include "ROS2TimeReference.generated.h"

USTRUCT(Blueprintable)
struct RCLUE_API FROSTimeReference
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FROSHeader Header;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FROSTime TimeRef;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Source;

    FROSTimeReference()
    {
    }

    void SetFromROS2(const sensor_msgs__msg__TimeReference& in_ros_data)
    {
        Header.SetFromROS2(in_ros_data.header);

        TimeRef.SetFromROS2(in_ros_data.time_ref);

        Source = UROS2Utils::StringROSToUE<rosidl_runtime_c__String>(in_ros_data.source);
    }

    void SetROS2(sensor_msgs__msg__TimeReference& out_ros_data) const
    {
        Header.SetROS2(out_ros_data.header);

        TimeRef.SetROS2(out_ros_data.time_ref);

        UROS2Utils::StringUEToROS(Source, out_ros_data.source);
    }
};

UCLASS()
class RCLUE_API UROS2TimeReferenceMsg : public UROS2GenericMsg
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Fini() override;

    virtual const rosidl_message_type_support_t* GetTypeSupport() const override;

    UFUNCTION(BlueprintCallable)
    void SetMsg(const FROSTimeReference& Input);

    UFUNCTION(BlueprintCallable)
    void GetMsg(FROSTimeReference& Output) const;

    virtual void* Get() override;

private:
    virtual FString MsgToString() const override;

    sensor_msgs__msg__TimeReference time_reference_msg;
};
