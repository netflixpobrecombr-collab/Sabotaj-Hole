#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGImpactFX.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;

// Original, transient collision-free impact feedback (no bullet or damage logic).
UCLASS(NotBlueprintable)
class SINGULARITY_API ASGImpactFX : public AActor
{
    GENERATED_BODY()
public:
    ASGImpactFX();
    virtual void Tick(float DeltaSeconds) override;
    void InitializeImpact(bool bTargetImpact, const FVector& SurfaceNormal);
    bool IsTargetImpact() const { return bHitTarget; }
    bool IsCollisionDisabled() const;
private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> MarkMesh;
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UPointLightComponent> Glow;
    float AgeSeconds=0.0f;
    bool bHitTarget=false;
};