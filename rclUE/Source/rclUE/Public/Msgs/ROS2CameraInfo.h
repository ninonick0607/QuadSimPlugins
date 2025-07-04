// Copyright 2021-2024 Rapyuta Robotics Co., Ltd.
// This code has been autogenerated from sensor_msgs/msg/CameraInfo.msg - do not modify

#pragma once

// UE
#include "CoreMinimal.h"

// ROS
#include "sensor_msgs/msg/camera_info.h"

// rclUE
#include "Msgs/ROS2GenericMsg.h"
#include "rclcUtilities.h"

// Generated Msg/Srv/Action(can be empty)
#include "Msgs/ROS2Header.h"
#include "Msgs/ROS2RegionOfInterest.h"
#include "rosidl_runtime_c/primitives_sequence_functions.h"

// Generated
#include "ROS2CameraInfo.generated.h"

USTRUCT(Blueprintable)
struct RCLUE_API FROSCameraInfo
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FROSHeader Header;

    UPROPERTY(EditAnywhere)
    unsigned int Height = 0;

    UPROPERTY(EditAnywhere)
    unsigned int Width = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DistortionModel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<double> D;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<double> K;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<double> R;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<double> P;

    UPROPERTY(EditAnywhere)
    unsigned int BinningX = 0;

    UPROPERTY(EditAnywhere)
    unsigned int BinningY = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FROSRegionOfInterest Roi;

    FROSCameraInfo()
    {
        K.SetNumZeroed(9);

        R.SetNumZeroed(9);

        P.SetNumZeroed(12);
    }

    void SetFromROS2(const sensor_msgs__msg__CameraInfo& in_ros_data)
    {
        Header.SetFromROS2(in_ros_data.header);

        Height = in_ros_data.height;

        Width = in_ros_data.width;

        DistortionModel = UROS2Utils::StringROSToUE<rosidl_runtime_c__String>(in_ros_data.distortion_model);

        UROS2Utils::SequenceROSToUEArray<double, double>(in_ros_data.d.data, D, in_ros_data.d.size);

        UROS2Utils::SequenceROSToUEArray<double, double>(in_ros_data.k, K, 9);

        UROS2Utils::SequenceROSToUEArray<double, double>(in_ros_data.r, R, 9);

        UROS2Utils::SequenceROSToUEArray<double, double>(in_ros_data.p, P, 12);

        BinningX = in_ros_data.binning_x;

        BinningY = in_ros_data.binning_y;

        Roi.SetFromROS2(in_ros_data.roi);
    }

    void SetROS2(sensor_msgs__msg__CameraInfo& out_ros_data) const
    {
        Header.SetROS2(out_ros_data.header);

        out_ros_data.height = Height;

        out_ros_data.width = Width;

        UROS2Utils::StringUEToROS(DistortionModel, out_ros_data.distortion_model);

        if (out_ros_data.d.data)
        {
            rosidl_runtime_c__float64__Sequence__fini(&out_ros_data.d);
        }
        if (!rosidl_runtime_c__float64__Sequence__init(&out_ros_data.d, D.Num()))
        {
            UE_LOG_WITH_INFO(LogTemp, Error, TEXT("failed to create array for field out_ros_data.d  "));
        }
        UROS2Utils::ArrayUEToROSSequence<double, double>(D, out_ros_data.d.data, D.Num());

        UROS2Utils::ArrayUEToROSSequence<double, double>(K, out_ros_data.k, 9);

        UROS2Utils::ArrayUEToROSSequence<double, double>(R, out_ros_data.r, 9);

        UROS2Utils::ArrayUEToROSSequence<double, double>(P, out_ros_data.p, 12);

        out_ros_data.binning_x = BinningX;

        out_ros_data.binning_y = BinningY;

        Roi.SetROS2(out_ros_data.roi);
    }
};

UCLASS()
class RCLUE_API UROS2CameraInfoMsg : public UROS2GenericMsg
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Fini() override;

    virtual const rosidl_message_type_support_t* GetTypeSupport() const override;

    UFUNCTION(BlueprintCallable)
    void SetMsg(const FROSCameraInfo& Input);

    UFUNCTION(BlueprintCallable)
    void GetMsg(FROSCameraInfo& Output) const;

    virtual void* Get() override;

private:
    virtual FString MsgToString() const override;

    sensor_msgs__msg__CameraInfo camera_info_msg;
};
