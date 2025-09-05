#include "ControlPanel/SliderRow.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/EditableTextBox.h"
#include "Internationalization/Internationalization.h"

void USliderRow::NativeConstruct()
{
	Super::NativeConstruct();

	if (Slider)
	{
		Slider->OnValueChanged.Clear();
		Slider->OnValueChanged.AddDynamic(this, &USliderRow::HandleSlider);

		// Make sure slider’s domain is 0..1 (we map ourselves)
#if ENGINE_MAJOR_VERSION >= 5
		// UE5 sliders default to 0..1; keeping explicit for clarity.
#endif
	}

	if (TxtValue)
	{
		TxtValue->OnTextCommitted.Clear();
		TxtValue->OnTextCommitted.AddDynamic(this, &USliderRow::HandleTextCommitted);
		// Optional: keep user edits numeric-only visually (no unit suffix here).
	}

	RefreshDisplay(); // shows whatever Current is
}

void USliderRow::Init(const FAxisSpec& InSpec)
{
	Spec = InSpec;
	Current = FMath::Clamp(Spec.Default, Spec.Min, Spec.Max);
	ApplySpecToWidgets();
	RefreshDisplay();
}

void USliderRow::UpdateSpec(const FAxisSpec& InSpec)
{
	Spec = InSpec;
	Current = FMath::Clamp(Current, Spec.Min, Spec.Max); // keep current if reasonable
	ApplySpecToWidgets();
	RefreshDisplay();
}

void USliderRow::ApplySpecToWidgets()
{
	if (TxtLabel)
	{
		// If your FAxisSpec has UnitText, append; if not, Label alone is fine.
		// Assuming FAxisSpec has optional FText UnitText; if not, just use Label.
		if (Spec.UnitText.IsEmpty())
		{
			TxtLabel->SetText(Spec.Label);
		}
		else
		{
			// "Yaw (deg)" style
			FText LabelWithUnit = FText::Format(
				NSLOCTEXT("SliderRow", "LabelUnitFmt", "{0} ({1})"),
				Spec.Label, Spec.UnitText);
			TxtLabel->SetText(LabelWithUnit);
		}
	}

	if (Slider)
	{
		// Normalize step for [0..1] domain
		const float Range = FMath::Max(0.000001f, Spec.Max - Spec.Min);
		const float StepN = FMath::Clamp(Spec.Step / Range, 0.0f, 1.0f);
		Slider->SetStepSize(StepN > 0.f ? StepN : 0.f);
	}

	// TxtValue formatting handled in RefreshDisplay()
}

int32 USliderRow::Decimals() const
{
	if (Spec.Step <= KINDA_SMALL_NUMBER) return 2;
	const float S = FMath::Abs(Spec.Step);
	if (S >= 1.f) return 0;
	int32 dp = 0; float t = S;
	while (t < 1.f && dp < 6) { t *= 10.f; ++dp; }
	return dp;
}

FText USliderRow::FormatValue(float V) const
{
	// Pretty number: 0 decimals if integer-ish, else up to Decimals().
	const int32 dp = Decimals();
	FNumberFormattingOptions Opts;
	Opts.MinimumIntegralDigits = 1;
	Opts.MinimumFractionalDigits = 0;
	Opts.MaximumFractionalDigits = dp;
	return FText::AsNumber(V, &Opts);
}

float USliderRow::Quantize(float V) const
{
	if (Spec.Step <= KINDA_SMALL_NUMBER) return V;
	const float q = Spec.Step;
	return FMath::RoundToFloat(V / q) * q;
}

float USliderRow::ClampOrWrap(float V) const
{
	if (Spec.bWrap)
	{
		const float Range = Spec.Max - Spec.Min;
		if (Range <= 0.f) return Spec.Min;
		float x = FMath::Fmod(V - Spec.Min, Range);
		if (x < 0) x += Range;
		return Spec.Min + x;
	}
	return FMath::Clamp(V, Spec.Min, Spec.Max);
}

float USliderRow::ToSlider(float World) const
{
	const float Den = (Spec.Max - Spec.Min);
	return Den > 0 ? (World - Spec.Min) / Den : 0.f;
}

float USliderRow::FromSlider(float S) const
{
	return Spec.Min + S * (Spec.Max - Spec.Min);
}

void USliderRow::SetValue(float V, bool bSilent)
{
	Current = ClampOrWrap(Quantize(V));
	RefreshDisplay();
	if (!bSilent) Emit(Current);
}

void USliderRow::RefreshDisplay()
{
	if (Slider)
	{
		// Avoid feedback loop: set normalized value derived from Current
		Slider->SetValue(ToSlider(Current));
	}

	if (TxtValue)
	{
		TxtValue->SetText(FormatValue(Current)); // numeric only
		// If you want a visible unit, put it in TxtLabel or add a separate unit text.
	}
}

void USliderRow::Emit(float V)
{
	// Dynamic delegate → bindable in Blueprint.
	OnAxisChanged.Broadcast(Spec.Channel, V);
}

void USliderRow::HandleSlider(float S)
{
	const float V = ClampOrWrap(Quantize(FromSlider(S)));
	Current = V;
	RefreshDisplay();  // keep text in sync during drags
	Emit(V);
}

void USliderRow::HandleTextCommitted(const FText& T, ETextCommit::Type /*CommitMethod*/)
{
	// Parse numeric (TxtValue does not contain unit)
	const float V = ClampOrWrap(Quantize(FCString::Atof(*T.ToString())));
	SetValue(V); // refresh + emit
}