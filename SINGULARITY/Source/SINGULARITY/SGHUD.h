#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SGHUD.generated.h"

UCLASS()
class SINGULARITY_API ASGHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
};