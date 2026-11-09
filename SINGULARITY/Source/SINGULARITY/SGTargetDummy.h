#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGTargetDummy.generated.h"

class USceneComponent;
class UStaticMeshComponent;

// Modular practice target: an original, primitive-based human silhouette.
// Keeps damage and collision on the actor; no licensed character assets required.
UCLASS(Blueprintable)
class SINGULARITY_API ASGTargetDummy : public AActor
{
    GENERATED_BODY()
public:
    ASGTargetDummy();
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, class AActor* DamageCauser) override;

    UFUNCTION(BlueprintPure, Category="SINGULARITY|Combat")
    float GetHealth() const { return Health; }
    void ResetTarget();
    bool HasChestMark() const { return ChestVerticalMesh != nullptr && ChestHorizontalMesh != nullptr; }
    bool HasLegs() const { return LeftLegMesh != nullptr && RightLegMesh != nullptr; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SINGULARITY|Target")
    TObjectPtr<USceneComponent> TargetRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SINGULARITY|Target")
    TObjectPtr<UStaticMeshComponent> TargetMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SINGULARITY|Target")
    TObjectPtr<UStaticMeshComponent> HeadMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SINGULARITY|Target")
    TObjectPtr<UStaticMeshComponent> LeftArmMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SINGULARITY|Target")
    TObjectPtr<UStaticMeshComponent> RightArmMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SINGULARITY|Target")
    TObjectPtr<UStaticMeshComponent> LeftLegMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SINGULARITY|Target")
    TObjectPtr<UStaticMeshComponent> RightLegMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SINGULARITY|Target")
    TObjectPtr<UStaticMeshComponent> ChestVerticalMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SINGULARITY|Target")
    TObjectPtr<UStaticMeshComponent> ChestHorizontalMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SINGULARITY|Target")
    TObjectPtr<UStaticMeshComponent> StandMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SINGULARITY|Target", meta=(ClampMin="1.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SINGULARITY|Target")
    float Health = 100.0f;

    virtual void BeginPlay() override;
};