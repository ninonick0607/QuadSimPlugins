// QuadSimPlugin/Source/QuadSimCore/Public/Components/PX4Component.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include <atomic>
#include <memory>
#include <thread>

// MAVSDK includes
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>

#include "PX4Component.generated.h"

// Forward declarations
class UQuadDroneController;

UENUM(BlueprintType)
enum class EPX4ControlMode : uint8
{
    Disabled    UMETA(DisplayName = "Disabled"),
    Position    UMETA(DisplayName = "Position Control"),
    Velocity    UMETA(DisplayName = "Velocity Control"),
    Attitude    UMETA(DisplayName = "Attitude Control"),
    Manual      UMETA(DisplayName = "Manual Control")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class QUADSIMCORE_API UPX4Component : public UActorComponent
{
    GENERATED_BODY()

public:
    UPX4Component();
    virtual ~UPX4Component();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Connection")
    bool bUsePX4 = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Connection")
    FString ConnectionString = TEXT("tcp://127.0.0.1:4560");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Control")
    EPX4ControlMode ControlMode = EPX4ControlMode::Attitude;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Control")
    bool bEnableLockstep = false;

    // Status
    UPROPERTY(BlueprintReadOnly, Category = "PX4 Status")
    bool bConnectedToPX4 = false;

    UPROPERTY(BlueprintReadOnly, Category = "PX4 Status")
    float LastHeartbeatTime = 0.0f;

    // Control Functions
    UFUNCTION(BlueprintCallable, Category = "PX4")
    void SetPX4Active(bool bActive);

    UFUNCTION(BlueprintCallable, Category = "PX4")
    void ConnectToPX4();

    UFUNCTION(BlueprintCallable, Category = "PX4")
    void DisconnectFromPX4();

private:
    // MAVSDK objects
    std::unique_ptr<mavsdk::Mavsdk> MavSDK;
    std::shared_ptr<mavsdk::System> System;
    std::shared_ptr<mavsdk::Telemetry> Telemetry;
    std::shared_ptr<mavsdk::Action> Action;
    std::shared_ptr<mavsdk::MavlinkPassthrough> Passthrough;

    // Threading
    std::unique_ptr<std::thread> MAVSDKThread;
    std::atomic<bool> bShouldStop{ false };
    std::atomic<bool> bConnected{ false };

    // State management
    FCriticalSection StateMutex;
    uint64_t SimulationTimeUs = 0;
    uint64_t HILTimestamp = 10000; // Start at 10ms like PX4 expects

    // Current state (thread-safe copies)
    FVector CurrentPosition = FVector::ZeroVector;
    FVector CurrentVelocity = FVector::ZeroVector;
    FRotator CurrentRotation = FRotator::ZeroRotator;
    FVector CurrentAngularVelocity = FVector::ZeroVector;

    // QuadDroneController reference
    UPROPERTY()
    UQuadDroneController* QuadController = nullptr;

    // Timing
    float ConnectionTimeoutTimer = 0.0f;
    static constexpr float ConnectionTimeout = 10.0f;

    // Methods
    void RunMAVSDKThread();
    void SendHILSensor();
    void ProcessIncomingMessages();
    void UpdateStateFromGameThread();
    UQuadDroneController* FindQuadController();

    // Coordinate conversion
    void ConvertUnrealToPX4Coordinates(const FVector& UnrealPos, const FVector& UnrealVel,
        const FRotator& UnrealRot, const FVector& UnrealAngVel,
        float& OutX, float& OutY, float& OutZ,
        float& OutVx, float& OutVy, float& OutVz,
        float& OutQ0, float& OutQ1, float& OutQ2, float& OutQ3,
        float& OutRollRate, float& OutPitchRate, float& OutYawRate);
};