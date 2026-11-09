#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "SGGameMode.generated.h"

class ASGTargetDummy;

UCLASS()
class SINGULARITY_API ASGGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ASGGameMode();
    virtual void BeginPlay() override;

    // Local training challenge: three preplaced targets, 90-second clock.
    void RecordTrainingDamage(ASGTargetDummy* Target, float AppliedDamage, float RemainingHealth);
    void RestartTraining();
    int32 GetTrainingScore() const { return TrainingScore; }
    int32 GetTrainingEliminations() const { return TrainingEliminations; }
    int32 GetTrainingTargetCount() const { return TrainingTargets.Num(); }
    bool IsTrainingActive() const { return bTrainingActive; }
    bool IsTrainingComplete() const { return bTrainingComplete; }
    bool IsTrainingTimedOut() const { return bTrainingTimedOut; }
    float GetTrainingTimeRemaining() const;

private:
    UPROPERTY(EditDefaultsOnly, Category="SINGULARITY|Training")
    bool bEnableTrainingFallRecovery = true;
    UPROPERTY(EditDefaultsOnly, Category="SINGULARITY|Training", meta=(ClampMin="10.0"))
    float TrainingDurationSeconds = 90.0f;

    FTimerHandle FallRecoveryTimer;
    FTimerHandle TrainingClockTimer;
    TArray<TWeakObjectPtr<ASGTargetDummy>> TrainingTargets;
    int32 TrainingScore = 0;
    int32 TrainingEliminations = 0;
    double TrainingStartTime = 0.0;
    float TrainingFrozenTime = 90.0f;
    bool bTrainingActive = false;
    bool bTrainingComplete = false;
    bool bTrainingTimedOut = false;

    void RecoverFallenPlayer();
    void StartTraining();
    void UpdateTrainingClock();
};