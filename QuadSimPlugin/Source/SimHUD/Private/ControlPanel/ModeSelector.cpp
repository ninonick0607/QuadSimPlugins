#include "ControlPanel/ModeSelector.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/PanelWidget.h"
#include "Components/Border.h"
#include "Blueprint/UserWidget.h"

void UModeSelector::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind the four wrappers (WBP_IconButtonGeneral). We’ll drill down to their inner Button.
    BindButton(BtnPosition, PositionButton, EControlMode::Position);
    BindButton(BtnVelocity, VelocityButton, EControlMode::Velocity);
    BindButton(BtnAngle,    AngleButton,    EControlMode::Angle);
    BindButton(BtnAcro,     AcroButton,     EControlMode::Acro);

    // Gamepad toggle
    if (ChkGamepad)
    {
        ChkGamepad->OnCheckStateChanged.Clear();
        ChkGamepad->OnCheckStateChanged.AddDynamic(this, &UModeSelector::OnGamepadToggled);
        bGamepadOnly = ChkGamepad->IsChecked();
    }

    RefreshVisibilityForGamepad();
    RefreshButtonStyles();
}

void UModeSelector::SetMode(EControlMode NewMode, bool bBroadcast /*=true*/)
{
    if (bGamepadOnly && (NewMode == EControlMode::Position || NewMode == EControlMode::Velocity))
    {
        NewMode = EControlMode::Angle; // coerce when gamepad-only
    }
    if (CurrentMode == NewMode) return;

    CurrentMode = NewMode;
    RefreshButtonStyles();
    if (bBroadcast) OnModeChanged.Broadcast(CurrentMode);
}

void UModeSelector::SetGamepadOnly(bool bOn, bool /*bBroadcast*/)
{
    if (bGamepadOnly == bOn) return;
    bGamepadOnly = bOn;

    if (ChkGamepad && ChkGamepad->IsChecked() != bGamepadOnly)
        ChkGamepad->SetIsChecked(bGamepadOnly);

    if (bGamepadOnly && (CurrentMode == EControlMode::Position || CurrentMode == EControlMode::Velocity))
    {
        CurrentMode = EControlMode::Angle;
        OnModeChanged.Broadcast(CurrentMode);
    }

    RefreshVisibilityForGamepad();
    RefreshButtonStyles();
}

void UModeSelector::OnGamepadToggled(bool bChecked)
{
    SetGamepadOnly(bChecked, /*bBroadcast=*/true);
}

void UModeSelector::OnPositionClicked() { SetMode(EControlMode::Position, true); }
void UModeSelector::OnVelocityClicked() { SetMode(EControlMode::Velocity, true); }
void UModeSelector::OnAngleClicked()    { SetMode(EControlMode::Angle,    true); }
void UModeSelector::OnAcroClicked()     { SetMode(EControlMode::Acro,     true); }

UButton* UModeSelector::FindFirstButtonDeep(UWidget* Root) const
{
    if (!Root) return nullptr;

    if (UButton* AsButton = Cast<UButton>(Root))
        return AsButton;

    if (UUserWidget* AsUW = Cast<UUserWidget>(Root))
    {
        if (UWidget* Inner = AsUW->GetRootWidget())
            if (UButton* B = FindFirstButtonDeep(Inner)) return B;
    }

    if (UPanelWidget* Panel = Cast<UPanelWidget>(Root))
    {
        const int32 Num = Panel->GetChildrenCount();
        for (int32 i = 0; i < Num; ++i)
            if (UWidget* Child = Panel->GetChildAt(i))
                if (UButton* B = FindFirstButtonDeep(Child)) return B;
    }

    if (UBorder* Border = Cast<UBorder>(Root))
    {
        if (UWidget* Cont = Border->GetContent())
            if (UButton* B = FindFirstButtonDeep(Cont)) return B;
    }

    return nullptr;
}

void UModeSelector::BindButton(UWidget* Wrapper, UButton*& Cached, EControlMode Mode)
{
    if (!Wrapper)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ModeSelector] Wrapper for %s is null"), *UEnum::GetValueAsString(Mode));
        return;
    }

    if (!Cached)
    {
        // WBP_IconButtonGeneral is a UUserWidget; find its inner UButton
        Cached = FindFirstButtonDeep(Wrapper);
        if (!Cached)
        {
            // If the wrapper itself happens to be a Button (unlikely), use it
            Cached = Cast<UButton>(Wrapper);
        }
    }

    if (!Cached)
    {
        UE_LOG(LogTemp, Error, TEXT("[ModeSelector] Could not find a UButton under '%s' (mode %s)."),
               *Wrapper->GetName(), *UEnum::GetValueAsString(Mode));
        return;
    }

    Cached->OnClicked.Clear();
    switch (Mode)
    {
        case EControlMode::Position: Cached->OnClicked.AddDynamic(this, &UModeSelector::OnPositionClicked); break;
        case EControlMode::Velocity: Cached->OnClicked.AddDynamic(this, &UModeSelector::OnVelocityClicked); break;
        case EControlMode::Angle:    Cached->OnClicked.AddDynamic(this, &UModeSelector::OnAngleClicked);    break;
        case EControlMode::Acro:     Cached->OnClicked.AddDynamic(this, &UModeSelector::OnAcroClicked);     break;
    }

    UE_LOG(LogTemp, Log, TEXT("[ModeSelector] Bound %s to inner button '%s'"),
           *UEnum::GetValueAsString(Mode), *Cached->GetName());
}

void UModeSelector::RefreshVisibilityForGamepad()
{
    auto SetVis = [](UWidget* W, bool bShow)
    {
        if (!W) return;
        W->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    };

    // In gamepad-only, hide Pos/Vel; Angle/Acro always visible
    SetVis(BtnPosition, !bGamepadOnly);
    SetVis(BtnVelocity, !bGamepadOnly);
    SetVis(BtnAngle,    true);
    SetVis(BtnAcro,     true);
}

void UModeSelector::RefreshButtonStyles()
{
    auto Tint = [&](UButton* Btn, bool bSelected)
    {
        if (!Btn) return;
        const FLinearColor C = bSelected ? SelectedTint : UnselectedTint;

        // Apply a light tint to the UButton’s style (works even if WBP_IconButtonGeneral drives visuals)
        FButtonStyle S = Btn->WidgetStyle;
        S.Normal.TintColor  = FSlateColor(C);
        S.Hovered.TintColor = FSlateColor(C);
        S.Pressed.TintColor = FSlateColor(C * 0.9f);
        Btn->SetStyle(S);
    };

    Tint(PositionButton, CurrentMode == EControlMode::Position);
    Tint(VelocityButton, CurrentMode == EControlMode::Velocity);
    Tint(AngleButton,    CurrentMode == EControlMode::Angle);
    Tint(AcroButton,     CurrentMode == EControlMode::Acro);
}
