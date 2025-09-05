// Copyright (c) Nicolas N.
// SPDX-License-Identifier: MIT
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SimControlLayout.generated.h"

// ---- Control modes shown in the top bar
UENUM(BlueprintType, meta=(UnderlyingType="uint8"))
enum class EControlMode : uint8
{
    Position UMETA(DisplayName="Position"),
    Velocity UMETA(DisplayName="Velocity"),
    Angle    UMETA(DisplayName="Angle"),
    Acro     UMETA(DisplayName="Acro")
};

// ---- Axes/channels a slider row can drive
UENUM(BlueprintType, meta=(UnderlyingType="uint8"))
enum class EAxisChannel : uint8
{
    X         UMETA(DisplayName="X"),
    Y         UMETA(DisplayName="Y"),
    Z         UMETA(DisplayName="Z"),
    Roll      UMETA(DisplayName="Roll"),
    Pitch     UMETA(DisplayName="Pitch"),
    Yaw       UMETA(DisplayName="Yaw"),
    YawRate   UMETA(DisplayName="Yaw Rate"),
    Throttle  UMETA(DisplayName="Throttle")
};

// ---- Per-slider specification
USTRUCT(BlueprintType)
struct FAxisSpec
{
    GENERATED_BODY()

    /** Which variable this row controls. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
    EAxisChannel Channel = EAxisChannel::X;

    /** UI label, e.g., "X", "Yaw rate", "Throttle" (no units here). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
    FText Label = FText::FromString(TEXT("X"));

    /** Optional unit label to be shown alongside Label, e.g., "m", "deg". */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
    FText UnitText = FText::GetEmpty();

    /** Range and default. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
    float Min = -1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
    float Max = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
    float Default = 0.f;

    /** Slider step and whether to wrap (useful for yaw 0..360). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
    float Step = 0.01f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
    bool bWrap = false;

    /** Show the numeric box to the right. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
    bool bShowBox = true;
};

// ---- A mode is just a list of sliders in order
USTRUCT(BlueprintType)
struct FModeLayout
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Layout")
    TArray<FAxisSpec> Axes;
};

// ---- Data asset you will author in the editor
UCLASS(BlueprintType)
class USimControlLayout : public UDataAsset
{
    GENERATED_BODY()
public:
    /** Normal (keyboard/mouse) layouts. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layouts")
    TMap<EControlMode, FModeLayout> Layouts;

    /** Gamepad overrides for Angle/Acro when the UI is in "Gamepad" mode. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layouts|Gamepad")
    FModeLayout GamepadAngle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layouts|Gamepad")
    FModeLayout GamepadAcro;

    /** Existence check. */
    UFUNCTION(BlueprintPure, Category="Layout")
    bool HasMode(EControlMode Mode) const { return Layouts.Contains(Mode); }

    /** Returns the correct layout for Mode. If bGamepadOnly and Mode is Angle/Acro, returns Gamepad overrides. */
    UFUNCTION(BlueprintPure, Category="Layout")
    const FModeLayout& GetLayout(EControlMode Mode, bool bGamepadOnly) const;

#if WITH_EDITOR
    // Optional: after edits, clamp bad ranges
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    // Constructor seeds sensible defaults so you can just tweak in the editor
    USimControlLayout();
};