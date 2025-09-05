#include "ControlPanel/ControlPanelWidget.h"

#include "ControlPanel/ModeSelector.h"     // UModeSelector
#include "ControlPanel/SliderRow.h"        // USliderRow

#include "Components/CheckBox.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/SizeBox.h"
#include "Components/PanelWidget.h"
#include "Blueprint/UserWidget.h"

void UControlPanelWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 1) Find the embedded ModeSelector (wherever it lives in your Control Panel)
    ModeSelector = FindFirstModeSelector();
    if (ModeSelector)
    {
        // Read its current mode as our starting point
        CurrentMode = ModeSelector->GetCurrentMode();

        // Listen for changes from it
        ModeSelector->OnModeChanged.AddDynamic(this, &UControlPanelWidget::HandleModeChanged);
    }

    // 2) Hook the other controls owned by this panel
    if (ChkGamepad)
    {
        ChkGamepad->OnCheckStateChanged.AddDynamic(this, &UControlPanelWidget::OnGamepadChanged);
    }

    if (SizeBox_BodyLimiter)
    {
        SizeBox_BodyLimiter->SetMaxDesiredHeight(380.f);
    }

    if (Scroll_Sliders)
    {
        Scroll_Sliders->SetScrollbarThickness(FVector2D(4.f, 4.f));
    }

    // 3) Initial state
    ApplyGamepadFilterToButtons();
    RebuildSliderList();
}

// ---------------- Public API ----------------

void UControlPanelWidget::SetMode(EControlMode NewMode)
{
    // Respect the gamepad filter
    if (bGamepadOnly && (NewMode == EControlMode::Position || NewMode == EControlMode::Velocity))
    {
        NewMode = EControlMode::Angle;
    }

    if (CurrentMode == NewMode) return;

    CurrentMode = NewMode;

    // Keep the embedded ModeSelector UI in sync (without rebroadcasting)
    if (bSyncModeSelectorUI && ModeSelector)
    {
        ModeSelector->SetMode(NewMode, /*bBroadcast=*/false);
    }

    RebuildSliderList();
}

void UControlPanelWidget::SetGamepadOnly(bool bOn)
{
    if (bGamepadOnly == bOn) return;
    bGamepadOnly = bOn;

    ApplyGamepadFilterToButtons(); // may clamp CurrentMode
    RebuildSliderList();

    // You may also want the ModeSelector to visually hide modes it doesn’t allow.
    // If you add a SetGamepadOnly(bool) API on UModeSelector, call it here.
}

void UControlPanelWidget::SetChannelValue(EAxisChannel Channel, float Value, bool bSilent)
{
    auto& MapRef = ValuesFor(CurrentMode);
    MapRef.Add(Channel, Value);

    if (VBox_Sliders)
    {
        const int32 N = VBox_Sliders->GetChildrenCount();
        for (int32 i=0; i<N; ++i)
        {
            if (USliderRow* Row = Cast<USliderRow>(VBox_Sliders->GetChildAt(i)))
            {
                if (Row->GetChannel() == Channel)
                {
                    Row->SetValue(Value, bSilent);
                    break;
                }
            }
        }
    }

    if (!bSilent)
    {
        OnControlParamChanged.Broadcast(CurrentMode, Channel, Value);
    }
}

// ---------------- Helpers ----------------

TMap<EAxisChannel,float>& UControlPanelWidget::ValuesFor(EControlMode Mode)
{
    switch (Mode)
    {
    case EControlMode::Position: return Values_Position;
    case EControlMode::Velocity: return Values_Velocity;
    case EControlMode::Angle:    return Values_Angle;
    default:                     return Values_Acro;
    }
}

const TMap<EAxisChannel,float>& UControlPanelWidget::ValuesFor(EControlMode Mode) const
{
    switch (Mode)
    {
    case EControlMode::Position: return Values_Position;
    case EControlMode::Velocity: return Values_Velocity;
    case EControlMode::Angle:    return Values_Angle;
    default:                     return Values_Acro;
    }
}

void UControlPanelWidget::ApplyGamepadFilterToButtons()
{
    // We no longer own the four buttons; we just enforce a valid CurrentMode here.
    if (bGamepadOnly && (CurrentMode == EControlMode::Position || CurrentMode == EControlMode::Velocity))
    {
        CurrentMode = EControlMode::Angle;

        if (bSyncModeSelectorUI && ModeSelector)
        {
            ModeSelector->SetMode(CurrentMode, /*bBroadcast=*/false);
        }
    }
}

void UControlPanelWidget::RebuildSliderList()
{
    if (!VBox_Sliders || !SliderRowClass) return;

    VBox_Sliders->ClearChildren();

    const FModeLayout* ModeLayoutPtr = nullptr;
    static const FModeLayout Empty;

    if (Layout)
    {
        ModeLayoutPtr = &Layout->GetLayout(CurrentMode, bGamepadOnly);
    }
    const FModeLayout& ModeLayout = ModeLayoutPtr ? *ModeLayoutPtr : Empty;

    auto& MapRef = ValuesFor(CurrentMode);

    for (const FAxisSpec& Spec : ModeLayout.Axes)
    {
        USliderRow* Row = CreateWidget<USliderRow>(this, SliderRowClass);
        if (!Row) continue;

        Row->Init(Spec);

        if (float* Saved = MapRef.Find(Spec.Channel))
        {
            Row->SetValue(*Saved, /*bSilent=*/true);
        }
        else
        {
            MapRef.Add(Spec.Channel, Spec.Default);
            Row->SetValue(Spec.Default, /*bSilent=*/true);
        }

        Row->OnAxisChanged.AddDynamic(this, &UControlPanelWidget::HandleRowChanged);
        VBox_Sliders->AddChild(Row);
    }
}

// ---------------- UI events ----------------

void UControlPanelWidget::OnGamepadChanged(bool bChecked)
{
    SetGamepadOnly(bChecked);
}

void UControlPanelWidget::HandleRowChanged(EAxisChannel Channel, float Value)
{
    auto& MapRef = ValuesFor(CurrentMode);
    MapRef.Add(Channel, Value);
    OnControlParamChanged.Broadcast(CurrentMode, Channel, Value);
}

void UControlPanelWidget::HandleModeChanged(EControlMode NewMode)
{
    // The embedded ModeSelector told us the user picked a mode.
    SetMode(NewMode);
}

// ---------------- Find ModeSelector recursively ----------------

UModeSelector* UControlPanelWidget::FindFirstModeSelector() const
{
    // Defensive: search anywhere under our root; covers cases like "Body (WBP_ModeSelector)" with spaces
    TFunction<UModeSelector*(UWidget*)> FindDeep = [&](UWidget* W) -> UModeSelector*
    {
        if (!W) return nullptr;

        if (UModeSelector* AsMS = Cast<UModeSelector>(W))
            return AsMS;

        if (UUserWidget* AsUW = Cast<UUserWidget>(W))
        {
            if (UWidget* Root = AsUW->GetRootWidget())
                if (auto* Found = FindDeep(Root)) return Found;
        }

        if (UPanelWidget* Panel = Cast<UPanelWidget>(W))
        {
            const int32 Num = Panel->GetChildrenCount();
            for (int32 i=0; i<Num; ++i)
                if (auto* Found = FindDeep(Panel->GetChildAt(i))) return Found;
        }
        return nullptr;
    };

    return FindDeep(GetRootWidget());
}
