#include "SGCombatSystem.h"
#include "SGGameMode.h"
#include "SGTargetDummy.h"
#include "SGFirstPersonWeapon.h"
#include "SGImpactFX.h"
#include "EngineUtils.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "InputCoreTypes.h"

ASGCombatSystem::ASGCombatSystem()
{
    PrimaryActorTick.bCanEverTick = false;
    AutoReceiveInput = EAutoReceiveInput::Player0;
    InputPriority = 10;
}

void ASGCombatSystem::BeginPlay()
{
    Super::BeginPlay();
    AmmoInMagazine = MagazineCapacity;
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        EnableInput(PC);
        if (InputComponent)
        {
            InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ASGCombatSystem::FireTest);
            InputComponent->BindKey(EKeys::R, IE_Pressed, this, &ASGCombatSystem::Reload);
            InputComponent->BindKey(EKeys::T, IE_Pressed, this, &ASGCombatSystem::RestartTraining);
            InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ASGCombatSystem::StartAim);
            InputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &ASGCombatSystem::StopAim);
        }
    }
    UE_LOG(LogTemp, Display, TEXT("SINGULARITY_COMBAT_READY Ammo=%d"), AmmoInMagazine);
}

void ASGCombatSystem::FireTest()
{
    UWorld* World = GetWorld();
    if (!World) { return; }
    if (bReloading)
    {
        UE_LOG(LogTemp, Display, TEXT("SG_RELOAD_BLOCKED_FIRE"));
        return;
    }
    const double TimeNow = World->GetTimeSeconds();
    if (TimeNow - LastShotTime < MinimumShotInterval) { return; }
    if (AmmoInMagazine <= 0)
    {
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("SINGULARITY - Press R to reload")); }
        return;
    }
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) { return; }
    LastShotTime = TimeNow;
    AmmoInMagazine--;
    for (TActorIterator<ASGFirstPersonWeapon> It(World); It; ++It)
    {
        It->NotifyShot();
        break;
    }

    FVector Origin;
    FRotator Rotation;
    PC->GetPlayerViewPoint(Origin, Rotation);
    FVector End = Origin + Rotation.Vector() * RangeInCm;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(SingularityHitscan), true, this);
    if (APawn* PlayerPawn = PC->GetPawn()) { Params.AddIgnoredActor(PlayerPawn); }
    FHitResult Hit;
    bool bHit = World->LineTraceSingleByChannel(Hit, Origin, End, ECC_Visibility, Params);
    if (bHit)
    {
        End = Hit.ImpactPoint;
        const bool bTrainingTarget=Cast<ASGTargetDummy>(Hit.GetActor())!=nullptr;
        const FVector FXPosition=Hit.ImpactPoint+Hit.ImpactNormal*6.0f;
        if (ASGImpactFX* Impact=World->SpawnActor<ASGImpactFX>(FXPosition,Hit.ImpactNormal.Rotation()))
        {
            Impact->InitializeImpact(bTrainingTarget,Hit.ImpactNormal);
        }
        if (AActor* Target = Hit.GetActor())
        {
            ASGTargetDummy* Dummy = Cast<ASGTargetDummy>(Target);
            const float BeforeHealth = Dummy ? Dummy->GetHealth() : 0.0f;
            UGameplayStatics::ApplyPointDamage(Target, DamagePerHit, Rotation.Vector(), Hit, PC, this, UDamageType::StaticClass());
            if (Dummy && Dummy->GetHealth() < BeforeHealth)
            {
                LastTargetHitTime = TimeNow;
                UE_LOG(LogTemp, Display, TEXT("SG_HIT_CONFIRM Target=%s RemainingHP=%.1f"), *Dummy->GetName(), Dummy->GetHealth());
            }
            UE_LOG(LogTemp, Display, TEXT("SINGULARITY_FIRE Hit=%s Ammo=%d"), *Target->GetName(), AmmoInMagazine);
        }
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("SINGULARITY_FIRE Miss Ammo=%d"), AmmoInMagazine);
    }
    DrawDebugLine(World, Origin, End, bHit ? FColor::Green : FColor::Yellow, false, 0.7f, 0, 1.5f);
    if (GEngine) { GEngine->AddOnScreenDebugMessage(1001, 1.2f, FColor::White,
        FString::Printf(TEXT("SINGULARITY  %d/%d"), AmmoInMagazine, MagazineCapacity)); }
}

void ASGCombatSystem::StartAim()
{
    if (!bIsAiming)
    {
        bIsAiming = true;
        UE_LOG(LogTemp, Display, TEXT("SG_ADS_STATE ENTER"));
    }
}

void ASGCombatSystem::StopAim()
{
    if (bIsAiming)
    {
        bIsAiming = false;
        UE_LOG(LogTemp, Display, TEXT("SG_ADS_STATE EXIT"));
    }
}

bool ASGCombatSystem::HasRecentConfirmedTargetHit() const
{
    return GetWorld() && LastTargetHitTime > 0.0 &&
        (GetWorld()->GetTimeSeconds() - LastTargetHitTime) <= 0.24;
}

void ASGCombatSystem::RestartTraining()
{
    if (ASGGameMode* Mode = Cast<ASGGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        Mode->RestartTraining();
    }
}

void ASGCombatSystem::ResetForTraining()
{
    GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
    bReloading = false;
    StopAim();
    AmmoInMagazine = MagazineCapacity;
    LastShotTime = -100.0;
    UE_LOG(LogTemp, Display, TEXT("SG_COMBAT_TRAINING_RESET Ammo=%d"), AmmoInMagazine);
}

void ASGCombatSystem::Reload()
{
    if (!GetWorld() || bReloading || AmmoInMagazine >= MagazineCapacity) return;
    bReloading=true;
    StopAim();
    UE_LOG(LogTemp, Display, TEXT("SG_RELOAD_START Ammo=%d"), AmmoInMagazine);
    for (TActorIterator<ASGFirstPersonWeapon> It(GetWorld()); It; ++It)
    {
        It->NotifyReload();
        break;
    }
    GetWorldTimerManager().SetTimer(ReloadTimerHandle,this,&ASGCombatSystem::CompleteReload,1.25f,false);
}

void ASGCombatSystem::CompleteReload()
{
    AmmoInMagazine=MagazineCapacity;
    bReloading=false;
    UE_LOG(LogTemp, Display, TEXT("SINGULARITY_RELOAD Ammo=%d"), AmmoInMagazine);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(1002, 1.2f, FColor::Cyan, TEXT("SINGULARITY  Reloaded"));
    }
}