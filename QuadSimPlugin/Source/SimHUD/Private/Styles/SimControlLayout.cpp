#include "Styles/SimControlLayout.h"
#include "Internationalization/Text.h"
#include "UObject/UnrealType.h"

USimControlLayout::USimControlLayout()
{
    // ---- Position (meters, yaw in deg)
    {
        FModeLayout M;
        M.Axes = {
            // Channel,   Label,         Unit,    Min,    Max,  Default, Step,  bWrap, bShowBox
            { EAxisChannel::X,        FText::FromString("X"),        FText::FromString("m"),   -50.f,   50.f,   0.f,   0.1f,  false, true },
            { EAxisChannel::Y,        FText::FromString("Y"),        FText::FromString("m"),   -50.f,   50.f,   0.f,   0.1f,  false, true },
            { EAxisChannel::Z,        FText::FromString("Z"),        FText::FromString("m"),     0.f,   50.f,   0.f,   0.1f,  false, true },
            { EAxisChannel::Yaw,      FText::FromString("Yaw"),      FText::FromString("deg"),   0.f,  360.f,   0.f,   1.0f,  true,  true }
        };
        Layouts.Add(EControlMode::Position, M);
    }

    // ---- Velocity (m/s, yaw rate in deg/s)
    {
        FModeLayout M;
        M.Axes = {
            { EAxisChannel::X,       FText::FromString("VX"),        FText::FromString("m/s"), -10.f,   10.f,   0.f,   0.1f,  false, true },
            { EAxisChannel::Y,       FText::FromString("VY"),        FText::FromString("m/s"), -10.f,   10.f,   0.f,   0.1f,  false, true },
            { EAxisChannel::Z,       FText::FromString("VZ"),        FText::FromString("m/s"), -10.f,   10.f,   0.f,   0.1f,  false, true },
            { EAxisChannel::YawRate, FText::FromString("Yaw Rate"),  FText::FromString("deg/s"), -180.f, 180.f, 0.f,   1.0f,  false, true }
        };
        Layouts.Add(EControlMode::Velocity, M);
    }

    // ---- Angle (deg + throttle 0..1)
    {
        FModeLayout M;
        M.Axes = {
            { EAxisChannel::Roll,     FText::FromString("Roll"),     FText::FromString("deg"),   -60.f,   60.f,   0.f,   0.5f,  false, true },
            { EAxisChannel::Pitch,    FText::FromString("Pitch"),    FText::FromString("deg"),   -60.f,   60.f,   0.f,   0.5f,  false, true },
            { EAxisChannel::Yaw,      FText::FromString("Yaw"),      FText::FromString("deg"),     0.f,  360.f,   0.f,   1.0f,  true,  true },
            { EAxisChannel::Throttle, FText::FromString("Throttle"), FText::GetEmpty(),             0.f,    1.f,   0.f,   0.01f, false, true }
        };
        Layouts.Add(EControlMode::Angle, M);
        GamepadAngle = M; // gamepad angle same by default
    }

    // ---- Acro (deg/s + throttle)
    {
        FModeLayout M;
        M.Axes = {
            { EAxisChannel::Roll,     FText::FromString("Roll Rate"),    FText::FromString("deg/s"), -400.f,  400.f,  0.f,  5.f,   false, true },
            { EAxisChannel::Pitch,    FText::FromString("Pitch Rate"),   FText::FromString("deg/s"), -400.f,  400.f,  0.f,  5.f,   false, true },
            { EAxisChannel::YawRate,  FText::FromString("Yaw Rate"),     FText::FromString("deg/s"), -400.f,  400.f,  0.f,  5.f,   false, true },
            { EAxisChannel::Throttle, FText::FromString("Throttle"),     FText::GetEmpty(),             0.f,    1.f,  0.f,  0.01f, false, true }
        };
        Layouts.Add(EControlMode::Acro, M);
        GamepadAcro = M; // gamepad acro same by default
    }
}

#if WITH_EDITOR
void USimControlLayout::PostEditChangeProperty(FPropertyChangedEvent& E)
{
    Super::PostEditChangeProperty(E);

    // Basic safety: ensure Min <= Max; clamp Default inside range; nonzero Step
    auto Sanitize = [](FModeLayout& ML)
    {
        for (FAxisSpec& A : ML.Axes)
        {
            if (A.Max < A.Min) { Swap(A.Min, A.Max); }
            A.Default = FMath::Clamp(A.Default, A.Min, A.Max);
            A.Step    = FMath::Max(KINDA_SMALL_NUMBER, A.Step);
        }
    };

    for (auto& Pair : Layouts)
    {
        Sanitize(Pair.Value);
    }
    Sanitize(GamepadAngle);
    Sanitize(GamepadAcro);
}
#endif

const FModeLayout& USimControlLayout::GetLayout(EControlMode Mode, bool bGamepadOnly) const
{
    if (bGamepadOnly)
    {
        if (Mode == EControlMode::Angle) return GamepadAngle;
        if (Mode == EControlMode::Acro)  return GamepadAcro;
    }

    if (const FModeLayout* Found = Layouts.Find(Mode))
    {
        return *Found;
    }

    static const FModeLayout Empty;
    return Empty;
}