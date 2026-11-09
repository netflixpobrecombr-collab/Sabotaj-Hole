#include "SGGameMode.h"
#include "SGSmokeTestActor.h"
#include "SGWalkTestActor.h"
#include "SGFirstPersonWeapon.h"
#include "SGCombatSystem.h"
#include "SGTargetDummy.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "SGHUD.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ASGGameMode::ASGGameMode()
{
    HUDClass = ASGHUD::StaticClass();

    static ConstructorHelpers::FClassFinder<APawn> PawnBP(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
    if (PawnBP.Succeeded()) { DefaultPawnClass = PawnBP.Class; }

    static ConstructorHelpers::FClassFinder<APlayerController> ControllerBP(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonPlayerController"));
    if (ControllerBP.Succeeded()) { PlayerControllerClass = ControllerBP.Class; }
}

void ASGGameMode::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Display, TEXT("SINGULARITY_GAMEMODE_READY HUD=%s Pawn=%s"),
        *GetNameSafe(HUDClass), *GetNameSafe(DefaultPawnClass));
    // Opt-in smoke test; normal gameplay remains untouched.
    if (FParse::Param(FCommandLine::Get(), TEXT("SGSmokeTest")) && GetWorld())
    {
        GetWorld()->SpawnActor<ASGSmokeTestActor>();
        UE_LOG(LogTemp, Display, TEXT("SG_SMOKE_BEGIN"));
    }
    if (FParse::Param(FCommandLine::Get(), TEXT("SGWalkTest")) && GetWorld())
    {
        GetWorld()->SpawnActor<ASGWalkTestActor>();
        UE_LOG(LogTemp, Display, TEXT("SG_WALK_TEST_BEGIN"));
    }
    // Original, low-poly training viewmodel. Disabled for headless regression tests.
    if (GetWorld() && !FParse::Param(FCommandLine::Get(), TEXT("SGSmokeTest")) &&
        !FParse::Param(FCommandLine::Get(), TEXT("SGWalkTest")))
    {
        GetWorld()->SpawnActor<ASGFirstPersonWeapon>();
    }
    if (bEnableTrainingFallRecovery && GetWorld())
    {
        GetWorldTimerManager().SetTimer(FallRecoveryTimer, this, &ASGGameMode::RecoverFallenPlayer, 0.25f, true);
    }
    if (GetWorld())
    {
        StartTraining();
        GetWorldTimerManager().SetTimer(TrainingClockTimer, this, &ASGGameMode::UpdateTrainingClock, 0.1f, true);
    }
}

void ASGGameMode::StartTraining()
{
    if (!GetWorld()) return;
    TrainingTargets.Reset();
    // Snapshot the authored three targets. Smoke-test-only spawned actors never count.
    for (TActorIterator<ASGTargetDummy> It(GetWorld()); It; ++It)
    {
        TrainingTargets.Add(*It);
    }
    TrainingScore = 0;
    TrainingEliminations = 0;
    TrainingStartTime = GetWorld()->GetTimeSeconds();
    TrainingFrozenTime = TrainingDurationSeconds;
    bTrainingActive = TrainingTargets.Num() > 0;
    bTrainingComplete = false;
    bTrainingTimedOut = false;
    UE_LOG(LogTemp, Display, TEXT("SG_TRAINING_START Targets=%d Duration=%.0f"),
        TrainingTargets.Num(), TrainingDurationSeconds);
}

float ASGGameMode::GetTrainingTimeRemaining() const
{
    if (!bTrainingActive || !GetWorld()) return TrainingFrozenTime;
    return FMath::Max(0.0f, TrainingDurationSeconds -
        static_cast<float>(GetWorld()->GetTimeSeconds() - TrainingStartTime));
}

void ASGGameMode::UpdateTrainingClock()
{
    if (bTrainingActive && GetTrainingTimeRemaining() <= 0.0f)
    {
        bTrainingActive = false;
        bTrainingTimedOut = true;
        TrainingFrozenTime = 0.0f;
        UE_LOG(LogTemp, Display, TEXT("SG_TRAINING_TIMEOUT Score=%d Eliminations=%d/%d"),
            TrainingScore, TrainingEliminations, TrainingTargets.Num());
    }
}

void ASGGameMode::RecordTrainingDamage(ASGTargetDummy* Target, float AppliedDamage, float RemainingHealth)
{
    if (!bTrainingActive || !Target || AppliedDamage <= 0.0f) return;
    const bool bRegistered = TrainingTargets.ContainsByPredicate(
        [Target](const TWeakObjectPtr<ASGTargetDummy>& Candidate) { return Candidate.Get() == Target; });
    if (!bRegistered) return;

    TrainingScore += 25;
    if (RemainingHealth <= 0.0f)
    {
        ++TrainingEliminations;
        TrainingScore += 100;
        if (TrainingEliminations >= TrainingTargets.Num())
        {
            TrainingFrozenTime = GetTrainingTimeRemaining();
            bTrainingActive = false;
            bTrainingComplete = true;
            UE_LOG(LogTemp, Display, TEXT("SG_TRAINING_COMPLETE Score=%d Remaining=%.1f"),
                TrainingScore, TrainingFrozenTime);
        }
    }
    UE_LOG(LogTemp, Display, TEXT("SG_TRAINING_SCORE Score=%d Eliminations=%d/%d HP=%.1f"),
        TrainingScore, TrainingEliminations, TrainingTargets.Num(), RemainingHealth);
}

void ASGGameMode::RestartTraining()
{
    for (const TWeakObjectPtr<ASGTargetDummy>& Target : TrainingTargets)
    {
        if (Target.IsValid()) Target->ResetTarget();
    }
    if (GetWorld())
    {
        for (TActorIterator<ASGCombatSystem> It(GetWorld()); It; ++It)
        {
            It->ResetForTraining();
        }
        StartTraining();
        UE_LOG(LogTemp, Display, TEXT("SG_TRAINING_RESTART"));
    }
}

void ASGGameMode::RecoverFallenPlayer()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    APawn* Pawn = PC ? PC->GetPawn() : nullptr;
    if (!Pawn || Pawn->GetActorLocation().Z > -500.0f) return;

    AActor* Start = UGameplayStatics::GetActorOfClass(this, APlayerStart::StaticClass());
    const FVector SafePosition = Start ? Start->GetActorLocation() + FVector(0, 0, 80) : FVector(300, 2530, 580);
    const FVector OldPosition = Pawn->GetActorLocation();
    if (ACharacter* Character = Cast<ACharacter>(Pawn))
    {
        if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
        {
            Movement->StopMovementImmediately();
        }
    }
    const bool bTeleported = Pawn->SetActorLocation(SafePosition, false, nullptr, ETeleportType::TeleportPhysics);
    UE_LOG(LogTemp, Warning, TEXT("SG_FALL_RECOVERY Teleported=%d OldZ=%.1f NewZ=%.1f"),
        bTeleported ? 1 : 0, OldPosition.Z, Pawn->GetActorLocation().Z);
}