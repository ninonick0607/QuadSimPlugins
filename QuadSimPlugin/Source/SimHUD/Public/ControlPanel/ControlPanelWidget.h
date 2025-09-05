#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styles/SimControlLayout.h"     // EControlMode, EAxisChannel, FModeLayout, FAxisSpec
#include "ControlPanelWidget.generated.h"

class UCheckBox;
class UScrollBox;
class UVerticalBox;
class USizeBox;
class USliderRow;
class UModeSelector;   // forward (defined in ControlPanel/ModeSelector.h)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnControlParamChanged, EControlMode, Mode, EAxisChannel, Channel, float, Value);

/**
 * Parent class for WBP_ControlPanel.
 * - Hosts a UModeSelector child and reacts to its OnModeChanged.
 * - Builds slider rows for the currently selected mode using USimControlLayout.
 */
UCLASS()
class SIMHUD_API UControlPanelWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** External consumers (sim/manager) can listen for parameter changes */
    UPROPERTY(BlueprintAssignable, Category="ControlPanel")
    FOnControlParamChanged OnControlParamChanged;

    /** Layout asset to drive the slider specs */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ControlPanel")
    USimControlLayout* Layout = nullptr;

    /** Class used for each slider row */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ControlPanel")
    TSubclassOf<USliderRow> SliderRowClass;

    /** Current mode + filter */
    UPROPERTY(BlueprintReadOnly, Category="ControlPanel")
    EControlMode CurrentMode = EControlMode::Position;

    UPROPERTY(BlueprintReadOnly, Category="ControlPanel")
    bool bGamepadOnly = false;

    /** Programmatic control */
    UFUNCTION(BlueprintCallable, Category="ControlPanel")
    void SetMode(EControlMode NewMode);

    UFUNCTION(BlueprintCallable, Category="ControlPanel")
    void SetGamepadOnly(bool bOn);

    UFUNCTION(BlueprintCallable, Category="ControlPanel")
    void SetChannelValue(EAxisChannel Channel, float Value, bool bSilent=false);

protected:
    virtual void NativeConstruct() override;

    // ---- BindWidget for the *panel* parts that belong to ControlPanel (names must match in WBP_ControlPanel) ----
    UPROPERTY(meta=(BindWidgetOptional))   UScrollBox*  Scroll_Sliders = nullptr;
    UPROPERTY(meta=(BindWidget))           UVerticalBox* VBox_Sliders   = nullptr;
    UPROPERTY(meta=(BindWidgetOptional))   USizeBox*     SizeBox_BodyLimiter = nullptr;
    UPROPERTY(meta=(BindWidgetOptional))   UCheckBox*    ChkGamepad = nullptr;

private:
    // We do not bind four buttons anymore. We host a ModeSelector widget somewhere inside the panel.
    UPROPERTY(Transient)
    UModeSelector* ModeSelector = nullptr;

    // If true, when SetMode() is called here we also push it back to ModeSelector (without rebroadcast).
    UPROPERTY(EditAnywhere, Category="ControlPanel")
    bool bSyncModeSelectorUI = true;

    // Value caches per mode
    TMap<EAxisChannel, float> Values_Position;
    TMap<EAxisChannel, float> Values_Velocity;
    TMap<EAxisChannel, float> Values_Angle;
    TMap<EAxisChannel, float> Values_Acro;

private:
    // --- helpers
    TMap<EAxisChannel,float>& ValuesFor(EControlMode Mode);
    const TMap<EAxisChannel,float>& ValuesFor(EControlMode Mode) const;

    void RebuildSliderList();
    void ApplyGamepadFilterToButtons(); // now informs only our own state; ModeSelector handles its visuals

    // locate ModeSelector inside this widget tree (works even if the child is named "WBP Mode Selector")
    UModeSelector* FindFirstModeSelector() const;

    // --- UI handlers
    UFUNCTION() void OnGamepadChanged(bool bChecked);
    UFUNCTION() void HandleRowChanged(EAxisChannel Channel, float Value);
    UFUNCTION() void HandleModeChanged(EControlMode NewMode);   // comes from UModeSelector
};
