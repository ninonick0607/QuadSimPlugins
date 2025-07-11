// QuadSimPlugin/Source/QuadSimCore/Public/Components/PX4Component.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"

// MavLinkCom includes
#include "MavLinkConnection.hpp"
#include "MavLinkMessages.hpp"
#include "MavLinkVehicle.hpp"
#include "VehicleState.hpp"

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

// Add this class declaration in your header file before UPX4Component:
class FPX4CommunicationThread : public FRunnable
{
public:
    FPX4CommunicationThread(UPX4Component* InPX4Component);
    virtual ~FPX4CommunicationThread();

    // FRunnable interface
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    virtual void Exit() override;

    void StartThread();
    void StopThread();

private:
    UPX4Component* PX4Component;
    FRunnableThread* Thread;
    FThreadSafeBool bStopRequested;
    
    static const int32 TARGET_FREQUENCY_HZ = 100; // 100Hz for HIL data as PX4 expects
    static const int32 TARGET_INTERVAL_MS = 1000 / TARGET_FREQUENCY_HZ; // 10ms
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class QUADSIMCORE_API UPX4Component : public UActorComponent
{
    GENERATED_BODY()

public:
    UPX4Component();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Connection")
    bool bUsePX4 = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Connection")
    FString PX4_IP = TEXT("127.0.0.1");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Connection")
    int32 PX4_Port = 4560;  // Standard PX4 simulator port (TCP)
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Connection")
    int32 ControlPortLocal = 14540;  // Local UDP port for MAVLink control
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Connection")
    int32 ControlPortRemote = 14580;  // Remote UDP port for MAVLink control

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Control")
    EPX4ControlMode ControlMode = EPX4ControlMode::Attitude;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Control")
    float StateUpdateRate = 100.0f; // Hz

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Control")
    float HeartbeatRate = 1.0f; // Hz

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PX4 Control")
    bool bEnableLockstep = true; // Enable lockstep synchronization

    // Status
    UPROPERTY(BlueprintReadOnly, Category = "PX4 Status")
    bool bConnectedToPX4 = false;

    UPROPERTY(BlueprintReadOnly, Category = "PX4 Status")
    float LastHeartbeatTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "PX4 Status")
    bool bLockstepActive = false;

    UPROPERTY(BlueprintReadOnly, Category = "PX4 Status")
    int32 LockstepResetCount = 0;

    // Control Functions
    UFUNCTION(BlueprintCallable, Category = "PX4")
    void SetPX4Active(bool bActive);

    UFUNCTION(BlueprintCallable, Category = "PX4")
    void ConnectToPX4();

    UFUNCTION(BlueprintCallable, Category = "PX4")
    void DisconnectFromPX4();
    void SendHILDataFromThread();
    void UpdateThreadSafeState();
    bool IsConnectedToPX4();
private:
    // MavLinkCom Connection
    std::shared_ptr<mavlinkcom::MavLinkConnection> MavLinkConn;
    std::shared_ptr<mavlinkcom::MavLinkVehicle> Vehicle;
    
    // Communication state
    bool bConnected = false;

    // MAVLink System IDs
    uint8 SystemID = 1;
    uint8 ComponentID = 1;
    uint8 TargetSystem = 1;
    uint8 TargetComponent = 1;
    uint64_t HILTimestamp = 0;
    const uint64_t HIL_INTERVAL_US = 10000; // 100Hz = 10ms = 10000 microseconds

    // Message sequence counter
    uint8 MessageSequence = 0;

    // Timing
    float HeartbeatTimer = 0.0f;
    float StateUpdateTimer = 0.0f;
    float ConnectionTimeoutTimer = 0.0f;
    static constexpr float ConnectionTimeout = 10.0f; // Use constexpr instead


    // State Storage
    FVector CurrentPosition = FVector::ZeroVector;
    FVector CurrentVelocity = FVector::ZeroVector;
    FRotator CurrentRotation = FRotator::ZeroRotator;
    FVector CurrentAngularVelocity = FVector::ZeroVector;

    // QuadDroneController reference
    UPROPERTY()
    UQuadDroneController* QuadController;

    FPX4CommunicationThread* CommunicationThread;
    FCriticalSection StateMutex; // Protects shared state access
    
    // Thread-safe state copies
    FVector ThreadSafePosition;
    FVector ThreadSafeVelocity;
    FRotator ThreadSafeRotation;
    FVector ThreadSafeAngularVelocity;
    bool bThreadSafeDataValid;
    
    // MavLinkCom Communication Functions
    void SetupMavLinkConnection();
    void CleanupConnection();
    void SendHeartbeat();
    void SendHILStateQuaternion();
    void SendHILSensor();
    void SendHILGPS();
    void SendHILRCInputs();
    void SendBasicHILData();
    
    // Message Handlers (called by MavLinkCom)
    void OnMessageReceived(std::shared_ptr<mavlinkcom::MavLinkMessage> message);
    void HandleHILActuatorControls(std::shared_ptr<mavlinkcom::MavLinkHilActuatorControls> message);
    void HandleHeartbeat(std::shared_ptr<mavlinkcom::MavLinkHeartbeat> message);
    
    // Helper Functions
    UQuadDroneController* FindQuadController();
    void UpdateConnectionStatus();
    FQuat RotatorToQuaternion(const FRotator& Rotator);
    void ConvertUnrealToPX4Coordinates(const FVector& UnrealPos, const FVector& UnrealVel, 
                                      const FRotator& UnrealRot, const FVector& UnrealAngVel,
                                      float& OutX, float& OutY, float& OutZ,
                                      float& OutVx, float& OutVy, float& OutVz,
                                      float& OutQ0, float& OutQ1, float& OutQ2, float& OutQ3,
                                      float& OutRollRate, float& OutPitchRate, float& OutYawRate);
    
    // Lockstep Functions
    void HandleLockstep();
    void ResetLockstep(const FString& Reason);
    bool ShouldSendHILData();
    
    // Lockstep State
    bool bReceivedActuatorControls = false;
    uint64 LastActuatorControlsTimestamp = 0;
    uint64 LastSentHILTimestamp = 0;
    double LastLockstepUpdateTime = 0.0;
    FCriticalSection LockstepMutex;
};