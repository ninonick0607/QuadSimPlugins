#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "QuadSimPlayerController.generated.h"

UCLASS()
class AQuadSimPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    /** This is the correct function to override for binding inputs. */
    virtual void SetupInputComponent() override;

    /** The function that will execute when the input is pressed. */
    void ToggleImguiInput();
};