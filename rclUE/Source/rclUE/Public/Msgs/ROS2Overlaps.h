// Copyright 2021-2025 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from ue_msgs/msg/Overlaps.msg - do not modify

#pragma once

// UE
#include "CoreMinimal.h"

// ROS
#include "ue_msgs/msg/overlaps.h"

// rclUE
#include "Msgs/ROS2GenericMsg.h"
#include "rclcUtilities.h"

// Generated Msg/Srv/Action(can be empty)
#include "Msgs/ROS2OverlappingObjects.h"
#include "rosidl_runtime_c/string_functions.h"
#include "ue_msgs/msg/detail/overlapping_objects__functions.h"

// Generated
#include "ROS2Overlaps.generated.h"

USTRUCT(Blueprintable)
struct RCLUE_API FROSOverlaps
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Targets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FROSOverlappingObjects> Overlaps;

    FROSOverlaps()
    {
    }

    void SetFromROS2(const ue_msgs__msg__Overlaps& in_ros_data)
    {
        UROS2Utils::StringSequenceROSToUEArray<rosidl_runtime_c__String>(
            in_ros_data.targets.data, Targets, in_ros_data.targets.size);

        UROS2Utils::SequenceROSToUEArray<ue_msgs__msg__OverlappingObjects, FROSOverlappingObjects>(
            in_ros_data.overlaps.data, Overlaps, in_ros_data.overlaps.size);
    }

    void SetROS2(ue_msgs__msg__Overlaps& out_ros_data) const
    {
        if (out_ros_data.targets.data)
        {
            rosidl_runtime_c__String__Sequence__fini(&out_ros_data.targets);
        }
        if (!rosidl_runtime_c__String__Sequence__init(&out_ros_data.targets, Targets.Num()))
        {
            UE_LOG_WITH_INFO(LogTemp, Error, TEXT("failed to create array for field out_ros_data.targets  "));
        }
        UROS2Utils::StringArrayUEToROSSequence(Targets, out_ros_data.targets.data, Targets.Num());

        if (out_ros_data.overlaps.data)
        {
            ue_msgs__msg__OverlappingObjects__Sequence__fini(&out_ros_data.overlaps);
        }
        if (!ue_msgs__msg__OverlappingObjects__Sequence__init(&out_ros_data.overlaps, Overlaps.Num()))
        {
            UE_LOG_WITH_INFO(LogTemp, Error, TEXT("failed to create array for field out_ros_data.overlaps  "));
        }
        UROS2Utils::ArrayUEToROSSequence<ue_msgs__msg__OverlappingObjects, FROSOverlappingObjects>(
            Overlaps, out_ros_data.overlaps.data, Overlaps.Num());
    }
};

UCLASS()
class RCLUE_API UROS2OverlapsMsg : public UROS2GenericMsg
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Fini() override;

    virtual const rosidl_message_type_support_t* GetTypeSupport() const override;

    UFUNCTION(BlueprintCallable)
    void SetMsg(const FROSOverlaps& Input);

    UFUNCTION(BlueprintCallable)
    void GetMsg(FROSOverlaps& Output) const;

    virtual void* Get() override;

private:
    virtual FString MsgToString() const override;

    ue_msgs__msg__Overlaps overlaps_msg;
};
