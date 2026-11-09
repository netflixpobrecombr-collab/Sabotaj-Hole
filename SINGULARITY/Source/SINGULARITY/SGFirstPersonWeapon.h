#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGFirstPersonWeapon.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class UAnimSequence;
class UPointLightComponent;
class USoundWave;
class ASGCombatSystem;
class APlayerCameraManager;

// Original low-poly SG-01 training carbine. Viewmodel, not a replicated weapon yet.
UCLASS(NotBlueprintable)
class SINGULARITY_API ASGFirstPersonWeapon : public AActor
{
    GENERATED_BODY()
public:
    ASGFirstPersonWeapon();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    void NotifyShot();
    void NotifyReload();
    bool IsMuzzleFlashActive() const { return FlashSecondsRemaining > 0.0f; }
    bool IsReloadAnimating() const { return bReloadAnimating; }
    bool HasSoundAssetsLoaded() const { return FireSound != nullptr && ReloadSound != nullptr; }
    int32 GetCosmeticPartCount() const;
    bool HasSafeCosmeticCollision() const;
    bool HasRecoveredAK74() const { return bRecoveredAK74Active; }
    bool IsRecoveredAK74CollisionDisabled() const;
    bool HasFirstPersonHands() const { return bFirstPersonHandsActive; }
protected:
    UPROPERTY(VisibleAnywhere, Category="SINGULARITY|Weapon")
    TObjectPtr<USceneComponent> WeaponRoot;
    UPROPERTY(EditDefaultsOnly, Category="SINGULARITY|Weapon")
    FVector ViewOffset=FVector(96.0f, 30.0f, -31.0f);
    UPROPERTY(EditDefaultsOnly, Category="SINGULARITY|Weapon")
    FVector ADSOffset=FVector(88.0f, 0.0f, -13.0f);
    UPROPERTY(EditDefaultsOnly, Category="SINGULARITY|Weapon", meta=(ClampMin="40.0", ClampMax="100.0"))
    float AimedFOV = 65.0f;
private:
    UPROPERTY(VisibleAnywhere,Category="SINGULARITY|Weapon")
    TObjectPtr<UStaticMeshComponent> MagazineMesh;
    UPROPERTY(VisibleAnywhere, Category="SINGULARITY|InternalReference")
    TObjectPtr<USkeletalMeshComponent> RecoveredAK74Mesh;
    UPROPERTY(VisibleAnywhere, Category="SINGULARITY|InternalReference")
    TObjectPtr<USkeletalMeshComponent> FirstPersonArmsMesh;
    UPROPERTY()
    TObjectPtr<UAnimSequence> RifleIdleAnimation;
    bool bRecoveredAK74Active=false;
    bool bFirstPersonHandsActive=false;
    UPROPERTY(VisibleAnywhere,Category="SINGULARITY|Weapon")
    TObjectPtr<UStaticMeshComponent> MuzzleFlashMesh;
    UPROPERTY(VisibleAnywhere,Category="SINGULARITY|Weapon")
    TObjectPtr<UPointLightComponent> MuzzleFlashLight;
    UPROPERTY()
    TObjectPtr<USoundWave> FireSound;
    UPROPERTY()
    TObjectPtr<USoundWave> ReloadSound;
    float FlashSecondsRemaining=0.0f;
    float ReloadElapsed=0.0f;
    bool bReloadAnimating=false;
    float Recoil=0.0f;
    float ADSAlpha=0.0f;
    float HipFOV=90.0f;
    bool bSavedHipFOV=false;
    bool bFOVOverrideActive=false;
    bool bReadyLogged=false;
    TWeakObjectPtr<ASGCombatSystem> Combat;
    TWeakObjectPtr<APlayerCameraManager> CameraManager;
};