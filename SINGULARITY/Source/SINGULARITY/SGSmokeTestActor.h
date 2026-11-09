#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGSmokeTestActor.generated.h"

class APlayerController;
class APawn;
class ASGCombatSystem;
class ASGTargetDummy;
class ASGFirstPersonWeapon;

// Opt-in, command-line-only functional smoke test. Never spawn in normal gameplay.
UCLASS(NotBlueprintable)
class SINGULARITY_API ASGSmokeTestActor : public AActor
{
    GENERATED_BODY()
public:
    ASGSmokeTestActor();
    virtual void Tick(float DeltaSeconds) override;
private:
    float Elapsed = 0.0f;
    int32 Phase = 0;
    int32 Passed = 0;
    int32 Failed = 0;
    FVector InitialPosition = FVector::ZeroVector;
    TWeakObjectPtr<APlayerController> Controller;
    TWeakObjectPtr<APawn> Player;
    TWeakObjectPtr<ASGCombatSystem> Combat;
    TWeakObjectPtr<ASGTargetDummy> Target;
    TWeakObjectPtr<ASGFirstPersonWeapon> Weapon;
    void Check(const TCHAR* Label, bool bPassed);
    void Finish();
};