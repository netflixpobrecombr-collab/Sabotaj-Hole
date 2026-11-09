#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "SGCombatSystem.generated.h"

UCLASS(Blueprintable)
class SINGULARITY_API ASGCombatSystem : public AActor
{
    GENERATED_BODY()
public:
    ASGCombatSystem();
    UFUNCTION(BlueprintCallable, Category="SINGULARITY|Combat")
    void FireTest();
    UFUNCTION(BlueprintCallable, Category="SINGULARITY|Combat")
    void Reload();
    void RestartTraining();
    void ResetForTraining();
    UFUNCTION(BlueprintPure, Category="SINGULARITY|Combat")
    int32 GetAmmoInMagazine() const { return AmmoInMagazine; }
    UFUNCTION(BlueprintPure, Category="SINGULARITY|Combat")
    bool HasRecentConfirmedTargetHit() const;
    UFUNCTION(BlueprintCallable, Category="SINGULARITY|Combat")
    void StartAim();
    UFUNCTION(BlueprintCallable, Category="SINGULARITY|Combat")
    void StopAim();
    UFUNCTION(BlueprintPure, Category="SINGULARITY|Combat")
    bool IsAiming() const { return bIsAiming; }
    UFUNCTION(BlueprintPure, Category="SINGULARITY|Combat")
    bool IsReloading() const { return bReloading; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SINGULARITY|Combat", meta=(ClampMin="1"))
    int32 MagazineCapacity = 30;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SINGULARITY|Combat", meta=(ClampMin="1.0"))
    float DamagePerHit = 34.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SINGULARITY|Combat", meta=(ClampMin="100.0"))
    float RangeInCm = 15000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SINGULARITY|Combat", meta=(ClampMin="0.01"))
    float MinimumShotInterval = 0.15f;

private:
    int32 AmmoInMagazine = 30;
    double LastShotTime = -100.0;
    double LastTargetHitTime = -100.0;
    bool bIsAiming = false;
    bool bReloading = false;
    FTimerHandle ReloadTimerHandle;
    void CompleteReload();
};