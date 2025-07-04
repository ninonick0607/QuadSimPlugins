// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from builtin_interfaces/msg/Time.msg - do not modify

#pragma once

// UE
#include "CoreMinimal.h"

// ROS
#include "builtin_interfaces/msg/time.h"

// rclUE
#include "Msgs/ROS2GenericMsg.h"
#include "rclcUtilities.h"

// Generated Msg/Srv/Action(can be empty)

// Generated
#include "ROS2Time.generated.h"

USTRUCT(Blueprintable)
struct RCLUE_API FROSTime
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Sec = 0;

    UPROPERTY(EditAnywhere)
    unsigned int Nanosec = 0;

    FROSTime()
    {
    }

    void SetFromROS2(const builtin_interfaces__msg__Time& in_ros_data)
    {
        Sec = in_ros_data.sec;

        Nanosec = in_ros_data.nanosec;
    }

    void SetROS2(builtin_interfaces__msg__Time& out_ros_data) const
    {
        out_ros_data.sec = Sec;

        out_ros_data.nanosec = Nanosec;
    }
};

UCLASS()
class RCLUE_API UROS2TimeMsg : public UROS2GenericMsg
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Fini() override;

    virtual const rosidl_message_type_support_t* GetTypeSupport() const override;

    UFUNCTION(BlueprintCallable)
    void SetMsg(const FROSTime& Input);

    UFUNCTION(BlueprintCallable)
    void GetMsg(FROSTime& Output) const;

    virtual void* Get() override;

private:
    virtual FString MsgToString() const override;

    builtin_interfaces__msg__Time time_msg;
};
