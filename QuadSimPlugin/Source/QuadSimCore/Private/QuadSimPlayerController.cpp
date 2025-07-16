#include "QuadSimPlayerController.h" // Always include the header for the class you are implementing
#include "Components/InputComponent.h"

void AQuadSimPlayerController::SetupInputComponent()
{
	// Always call the parent's function first!
	Super::SetupInputComponent();

	// The InputComponent is guaranteed to be valid here.
	if (InputComponent)
	{
		InputComponent->BindAction("ToggleImGui", IE_Pressed, this, &AQuadSimPlayerController::ToggleImguiInput);
	}
}

void AQuadSimPlayerController::ToggleImguiInput()
{
	// Execute the console command
	this->ConsoleCommand("ImGui.ToggleInput");
}