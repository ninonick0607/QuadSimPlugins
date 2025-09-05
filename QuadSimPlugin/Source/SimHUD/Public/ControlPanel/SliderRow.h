// ControlPanel/SliderRow.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "Styles/SimControlLayout.h"     // defines EAxisChannel and FAxisSpec
#include "SliderRow.generated.h"

class UTextBlock;
class USlider;
class UEditableTextBox;

/** Dynamic so you can bind in Blueprints. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAxisChanged, EAxisChannel, Channel, float, Value);

/**
 * USliderRow: one labeled slider with an editable numeric box.
 * Expects a valid FAxisSpec (Channel, Label, Min/Max, Step, Default, bWrap[, UnitText]).
 */
UCLASS()
class USliderRow : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Broadcast whenever the value changes (dragging or text commit). */
	UPROPERTY(BlueprintAssignable, Category="SliderRow")
	FOnAxisChanged OnAxisChanged;

	/** Initialize with a spec once after CreateWidget (or call UpdateSpec later). */
	UFUNCTION(BlueprintCallable, Category="SliderRow")
	void Init(const FAxisSpec& InSpec);

	/** Update the spec in-place (optional reuse path). Calls RefreshDisplay but does not emit. */
	UFUNCTION(BlueprintCallable, Category="SliderRow")
	void UpdateSpec(const FAxisSpec& InSpec);

	/** Programmatic set; quantizes & clamps/wraps. Emits unless bSilent. */
	/* NOTE: UFUNCTIONs cannot have default arguments. */
	UFUNCTION(BlueprintCallable, Category="SliderRow", meta=(AdvancedDisplay="bSilent"))
	void SetValue(float V, bool bSilent);

	/** C++ convenience overload (not exposed to UHT/Blueprints). */
	FORCEINLINE void SetValue(float V) { SetValue(V, /*bSilent*/false); }

	UFUNCTION(BlueprintPure, Category="SliderRow")
	float GetValue() const { return Current; }

	UFUNCTION(BlueprintPure, Category="SliderRow")
	EAxisChannel GetChannel() const { return Spec.Channel; }

protected:
	virtual void NativeConstruct() override;

	// --- Bound widgets (names must match your WBP) ---
	UPROPERTY(meta=(BindWidget)) UTextBlock*       TxtLabel = nullptr;
	UPROPERTY(meta=(BindWidget)) USlider*          Slider   = nullptr;
	UPROPERTY(meta=(BindWidget)) UEditableTextBox* TxtValue = nullptr;

private:
	FAxisSpec Spec;
	float Current = 0.f;

	// --- internal helpers ---
	void ApplySpecToWidgets();             // label text, slider step, etc.
	void RefreshDisplay();                 // sync slider + text to Current
	void Emit(float V);                    // broadcast dynamic delegate

	int32 Decimals() const;                // based on Step
	FText  FormatValue(float V) const;     // trims trailing zeros
	float  Quantize(float V) const;
	float  ClampOrWrap(float V) const;
	float  ToSlider(float World) const;    // world → [0..1]
	float  FromSlider(float S) const;      // [0..1] → world

	UFUNCTION() void HandleSlider(float S);                                   // live drag
	UFUNCTION()
	void HandleTextCommitted(const FText& T, ETextCommit::Type CommitMethod);  // return / focus loss
};