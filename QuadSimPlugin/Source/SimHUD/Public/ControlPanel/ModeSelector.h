#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styles/SimControlLayout.h"   // EControlMode
#include "ModeSelector.generated.h"

class UWidget;
class UButton;
class UCheckBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModeChanged, EControlMode, NewMode);

UCLASS()
class SIMHUD_API UModeSelector : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable) EControlMode GetCurrentMode() const { return CurrentMode; }
    UFUNCTION(BlueprintCallable) void SetMode(EControlMode NewMode, bool bBroadcast = true);

    /** Toggle gamepad-only (hides Position/Velocity) */
    UFUNCTION(BlueprintCallable) void SetGamepadOnly(bool bOn, bool bBroadcast = true);
    UFUNCTION(BlueprintPure)     bool IsGamepadOnly() const { return bGamepadOnly; }

    UPROPERTY(BlueprintAssignable) FOnModeChanged OnModeChanged;

protected:
    /** These are your four WBP_IconButtonGeneral instances (wrapper widgets) */
    UPROPERTY(meta=(BindWidget)) UWidget* BtnPosition = nullptr;
    UPROPERTY(meta=(BindWidget)) UWidget* BtnVelocity = nullptr;
    UPROPERTY(meta=(BindWidget)) UWidget* BtnAngle    = nullptr;
    UPROPERTY(meta=(BindWidget)) UWidget* BtnAcro     = nullptr;

    /** Gamepad toggle lives in ModeSelector */
    UPROPERTY(meta=(BindWidgetOptional)) UCheckBox* ChkGamepad = nullptr;

    /** Optional simple tinting for selected/unselected */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style")
    FLinearColor SelectedTint   = FLinearColor(0.10f, 0.55f, 1.f, 1.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style")
    FLinearColor UnselectedTint = FLinearColor(0.25f, 0.25f, 0.25f, 1.f);

private:
    /** Current state */
    EControlMode CurrentMode = EControlMode::Position;
    bool bGamepadOnly = false;

    /** Cached real inner buttons resolved from each wrapper widget */
    UPROPERTY(Transient) UButton* PositionButton = nullptr;
    UPROPERTY(Transient) UButton* VelocityButton = nullptr;
    UPROPERTY(Transient) UButton* AngleButton    = nullptr;
    UPROPERTY(Transient) UButton* AcroButton     = nullptr;

    /** Helpers */
    UButton* FindFirstButtonDeep(UWidget* Root) const;
    void     BindButton(UWidget* Wrapper, UButton*& Cached, EControlMode Mode);
    void     RefreshVisibilityForGamepad();
    void     RefreshButtonStyles();

    /** Dynamic delegate targets (required for AddDynamic) */
    UFUNCTION() void OnPositionClicked();
    UFUNCTION() void OnVelocityClicked();
    UFUNCTION() void OnAngleClicked();
    UFUNCTION() void OnAcroClicked();
    UFUNCTION() void OnGamepadToggled(bool bChecked);
};
