#include "SGSmokeTestActor.h"
#include "SGGameMode.h"
#include "SGCombatSystem.h"
#include "SGTargetDummy.h"
#include "SGFirstPersonWeapon.h"
#include "SGImpactFX.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/PlatformMisc.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"

ASGSmokeTestActor::ASGSmokeTestActor()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASGSmokeTestActor::Check(const TCHAR* Label, bool bPassed)
{
    if (bPassed) { ++Passed; }
    else { ++Failed; }
    UE_LOG(LogTemp, Display, TEXT("SG_SMOKE_CHECK %s %s"), bPassed ? TEXT("PASS") : TEXT("FAIL"), Label);
}

void ASGSmokeTestActor::Finish()
{
    UE_LOG(LogTemp, Display, TEXT("SG_SMOKE_END Passed=%d Failed=%d"), Passed, Failed);
    SetActorTickEnabled(false);
    FPlatformMisc::RequestExit(false);
}

void ASGSmokeTestActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    Elapsed += DeltaSeconds;
    if (Phase == 0 && Elapsed >= 1.0f)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        APawn* Pawn = PC ? PC->GetPawn() : nullptr;
        if (!Pawn)
        {
            if (Elapsed > 4.0f) { Check(TEXT("PLAYER_SPAWN"), false); Finish(); }
            return;
        }
        Controller = PC;
        Player = Pawn;
        for (TActorIterator<ASGCombatSystem> It(GetWorld()); It; ++It) { Combat = *It; break; }
        // Runtime builds do not include editor-only actor labels.
        for (TActorIterator<ASGTargetDummy> It(GetWorld()); It; ++It)
        {
            Target = *It;
            break;
        }
        InitialPosition = Pawn->GetActorLocation();
        {
            FVector Eye; FRotator ViewRot;
            PC->GetPlayerViewPoint(Eye,ViewRot);
            int32 TrainingTargets=0,VisibleTargets=0;
            for (TActorIterator<ASGTargetDummy> It(GetWorld()); It; ++It)
            {
                ++TrainingTargets;
                FHitResult Hit;
                FCollisionQueryParams LOS(SCENE_QUERY_STAT(SGThreeTargetsLOS),true,Pawn);
                const FVector Aim=It->GetActorLocation()+FVector(0,0,25);
                const bool bHit=GetWorld()->LineTraceSingleByChannel(Hit,Eye,Aim,ECC_Visibility,LOS);
                const bool bVisible=bHit && Hit.GetActor()==*It;
                if (bVisible) ++VisibleTargets;
                UE_LOG(LogTemp,Display,TEXT("SG_TRAINING_LOS Target=%s Visible=%d First=%s"),
                    *It->GetName(),bVisible?1:0,*GetNameSafe(Hit.GetActor()));
            }
            Check(TEXT("THREE_TRAINING_TARGETS"),TrainingTargets==3);
            Check(TEXT("ALL_THREE_TARGETS_VISIBLE"),TrainingTargets==3 && VisibleTargets==3);
        }
        if (FParse::Param(FCommandLine::Get(), TEXT("SGGroundProbe")))
        {
            FCollisionQueryParams GroundQuery(SCENE_QUERY_STAT(SGGroundProbe),true,Pawn);
            for (TActorIterator<ASGTargetDummy> It(GetWorld()); It; ++It) GroundQuery.AddIgnoredActor(*It);
            for (TActorIterator<AStaticMeshActor> It(GetWorld()); It; ++It)
            {
                if (It->GetActorScale3D().X > 100.0f) GroundQuery.AddIgnoredActor(*It);
            }
            for (const FVector2D& XY : {
                FVector2D(300,2530),FVector2D(850,2530),
                FVector2D(1050,2810),FVector2D(1050,2250),
                FVector2D(850,2380),FVector2D(850,2680),
                FVector2D(1250,2530),FVector2D(500,2530)
            })
            {
                FHitResult GroundHit;
                bool bHit=GetWorld()->LineTraceSingleByChannel(GroundHit,
                    FVector(XY.X,XY.Y,330),FVector(XY.X,XY.Y,-2500),
                    ECC_Visibility,GroundQuery);
                UE_LOG(LogTemp,Display,TEXT("SG_GROUND_PROBE X=%.0f Y=%.0f Hit=%d Actor=%s Z=%.1f"),
                    XY.X,XY.Y,bHit?1:0,*GetNameSafe(GroundHit.GetActor()),
                    bHit?GroundHit.ImpactPoint.Z:0.0f);
            }
        }
        if (FParse::Param(FCommandLine::Get(), TEXT("SGTargetSurvey")))
        {
            FVector Eye; FRotator CameraRotation;
            PC->GetPlayerViewPoint(Eye,CameraRotation);
            UE_LOG(LogTemp,Display,TEXT("SG_TARGET_SURVEY_CAMERA Eye=%s Rotation=%s"),*Eye.ToString(),*CameraRotation.ToString());
            FCollisionQueryParams SurveyParams(SCENE_QUERY_STAT(SGTargetLOS),true,Pawn);
            for (TActorIterator<ASGTargetDummy> It(GetWorld()); It; ++It)
                SurveyParams.AddIgnoredActor(*It);
            for (int32 X : {650,850,1050,1250,1450,1750,2050})
            {
                for (int32 Y : {2050,2250,2400,2530,2660,2810,3010})
                {
                    const FVector End(static_cast<float>(X),static_cast<float>(Y),490.f);
                    FHitResult SurveyHit;
                    const bool bBlocked=GetWorld()->LineTraceSingleByChannel(
                        SurveyHit,Eye,End,ECC_Visibility,SurveyParams);
                    if (!bBlocked)
                    {
                        UE_LOG(LogTemp,Display,TEXT("SG_TARGET_SURVEY_CLEAR X=%d Y=%d Z=490"),X,Y);
                    }
                    else
                    {
                        UE_LOG(LogTemp,Display,TEXT("SG_TARGET_SURVEY_BLOCKED X=%d Y=%d First=%s"),
                            X,Y,*GetNameSafe(SurveyHit.GetActor()));
                    }
                }
            }
        }
        ASGGameMode* Mode = Cast<ASGGameMode>(UGameplayStatics::GetGameMode(this));
        Check(TEXT("TRAINING_SESSION_ACTIVE"), Mode && Mode->IsTrainingActive());
        Check(TEXT("TRAINING_REGISTERED_TARGETS"), Mode && Mode->GetTrainingTargetCount() == 3);
        Check(TEXT("PLAYER_SPAWN"), true);
        Check(TEXT("PLAYER_AT_STREET_LEVEL"),InitialPosition.Z > -350.0f && InitialPosition.Z < 100.0f);
        Check(TEXT("TARGET_CHEST_MARK_PRESENT"),Target.IsValid() && Target->HasChestMark());
        Check(TEXT("TARGET_LEGS_PRESENT"),Target.IsValid() && Target->HasLegs());
        Weapon=GetWorld()->SpawnActor<ASGFirstPersonWeapon>();
        Check(TEXT("SG01_VIEWMODEL_PRESENT"), Weapon.IsValid());
        if (FParse::Param(FCommandLine::Get(),TEXT("SGReferenceAK74")))
        {
            Check(TEXT("AK74_REFERENCE_LOADED"),Weapon.IsValid() && Weapon->HasRecoveredAK74());
            Check(TEXT("AK74_REFERENCE_SAFE_COLLISION"),Weapon.IsValid() && Weapon->IsRecoveredAK74CollisionDisabled());
            Check(TEXT("AK74_REFERENCE_HANDS_LOADED"),Weapon.IsValid() && Weapon->HasFirstPersonHands());
        }
        Check(TEXT("SG01_MK2_DETAIL_GEOMETRY"), Weapon.IsValid() && Weapon->GetCosmeticPartCount() >= 30);
        Check(TEXT("SG01_MK2_NO_WEAPON_COLLISION"), Weapon.IsValid() && Weapon->HasSafeCosmeticCollision());
        Check(TEXT("SG01_ORIGINAL_AUDIO_LOADED"), Weapon.IsValid() && Weapon->HasSoundAssetsLoaded());
        Check(TEXT("COMBAT_EXISTS"), Combat.IsValid());
        Check(TEXT("TARGET_EXISTS"), Target.IsValid());
        UE_LOG(LogTemp, Display, TEXT("SG_SMOKE_SPAWN X=%.1f Y=%.1f Z=%.1f"), InitialPosition.X, InitialPosition.Y, InitialPosition.Z);
        FHitResult Hit;
        FCollisionQueryParams Params(SCENE_QUERY_STAT(SGSmokeFloor), true, Pawn);
        bool bFloor = GetWorld()->LineTraceSingleByChannel(Hit, InitialPosition + FVector(0,0,80), InitialPosition - FVector(0,0,1000), ECC_Visibility, Params);
        UE_LOG(LogTemp, Display, TEXT("SG_SMOKE_FLOOR Hit=%d Actor=%s Z=%.1f"), bFloor?1:0, *GetNameSafe(Hit.GetActor()), bFloor?Hit.ImpactPoint.Z:0.0);
        Check(TEXT("FLOOR_TRACE"), bFloor);
        Phase = 1;
    }
    if (Phase == 1 && Elapsed >= 1.25f)
    {
        if (Controller.IsValid() && Target.IsValid())
        {
            FVector Eye; FRotator CameraRot;
            Controller->GetPlayerViewPoint(Eye, CameraRot);
            const FVector AimAt = Target->GetActorLocation() + FVector(0,0,20);
            Controller->SetControlRotation((AimAt-Eye).Rotation());
            FHitResult Hit;
            FCollisionQueryParams Params(SCENE_QUERY_STAT(SGSmokeShotLOS), true, Player.Get());
            bool bLOS = GetWorld()->LineTraceSingleByChannel(Hit, Eye, AimAt, ECC_Visibility, Params);
            UE_LOG(LogTemp, Display, TEXT("SG_SMOKE_LOS Hit=%d Actor=%s"), bLOS?1:0,*GetNameSafe(Hit.GetActor()));
        }
        Phase = 2;
    }
    if (Phase == 2 && Elapsed >= 1.65f)
    {
        if (Combat.IsValid() && Target.IsValid())
        {
            Combat->StartAim();
            Check(TEXT("ADS_ENTER"), Combat->IsAiming());
            Combat->StopAim();
            Check(TEXT("ADS_EXIT"), !Combat->IsAiming());
            const int32 BeforeAmmo = Combat->GetAmmoInMagazine();
            const float BeforeHP = Target->GetHealth();
            Combat->FireTest();
            const int32 AfterAmmo = Combat->GetAmmoInMagazine();
            const float AfterHP = Target->GetHealth();
            UE_LOG(LogTemp, Display, TEXT("SG_SMOKE_FIRE Ammo=%d->%d Health=%.1f->%.1f"), BeforeAmmo, AfterAmmo, BeforeHP, AfterHP);
            Check(TEXT("SHOT_CONSUMES_AMMO"), AfterAmmo == BeforeAmmo-1);
            Check(TEXT("SG01_MUZZLE_FLASH_TRIGGERED"), Weapon.IsValid() && Weapon->IsMuzzleFlashActive());
            Check(TEXT("SHOT_HITS_TARGET"), AfterHP < BeforeHP);
            ASGGameMode* Mode = Cast<ASGGameMode>(UGameplayStatics::GetGameMode(this));
            Check(TEXT("TRAINING_SCORES_HIT"), Mode && Mode->GetTrainingScore() >= 25);
            ASGImpactFX* LatestImpact=nullptr;
            for (TActorIterator<ASGImpactFX> It(GetWorld()); It; ++It) { LatestImpact=*It;break; }
            Check(TEXT("IMPACT_FX_SPAWNED"), LatestImpact!=nullptr);
            Check(TEXT("IMPACT_FX_TARGET_CLASSIFIED"), LatestImpact && LatestImpact->IsTargetImpact());
            Check(TEXT("IMPACT_FX_NO_COLLISION"), LatestImpact && LatestImpact->IsCollisionDisabled());
            Check(TEXT("HITMARKER_CONFIRMED"), Combat->HasRecentConfirmedTargetHit());
            Combat->Reload();
            Check(TEXT("RELOAD_STARTED"), Combat->IsReloading());
            Check(TEXT("SG01_RELOAD_ANIM_TRIGGERED"), Weapon.IsValid() && Weapon->IsReloadAnimating());
            Combat->FireTest();
            Check(TEXT("RELOAD_BLOCKS_FIRE"), Combat->GetAmmoInMagazine() == AfterAmmo);
            const float HpDirect = Target->GetHealth();
            UGameplayStatics::ApplyDamage(Target.Get(), 34.0f, Controller.Get(), this, nullptr);
            Check(TEXT("DAMAGE_HANDLER"), Target->GetHealth() < HpDirect);
            UGameplayStatics::ApplyDamage(Target.Get(), 34.0f, Controller.Get(), this, nullptr);
            Check(TEXT("TARGET_ELIMINATED"), Target->GetHealth() <= 0.0f);
            Check(TEXT("TRAINING_COUNTS_ELIMINATION"), Mode && Mode->GetTrainingEliminations() == 1);
            Check(TEXT("TRAINING_KILL_BONUS"), Mode && Mode->GetTrainingScore() >= 175);
            UStaticMeshComponent* TargetMesh = Target->FindComponentByClass<UStaticMeshComponent>();
            Check(TEXT("DESTROYED_TARGET_COLLISION_OFF"), TargetMesh && TargetMesh->GetCollisionEnabled() == ECollisionEnabled::NoCollision);
        }
        else { Check(TEXT("COMBAT_TEST_SETUP"), false); }
        Phase = 3;
    }
    if (Phase == 3 && Elapsed >= 1.8f)
    {
        if (Player.IsValid())
        {
            Player->AddMovementInput(FVector(0.0f, 1.0f, 0.0f), 1.0f);
        }
        if (Elapsed >= 2.9f) { Phase = 4; }
    }
    if (Phase == 4 && Elapsed >= 3.5f)
    {
        if (Combat.IsValid())
        {
            Check(TEXT("RELOAD_COMPLETED"), !Combat->IsReloading());
            Check(TEXT("RELOAD_RESTORES_AMMO"), Combat->GetAmmoInMagazine() == 30);
        }
        Check(TEXT("SG01_RELOAD_ANIM_COMPLETE"), Weapon.IsValid() && !Weapon->IsReloadAnimating());
        if (Player.IsValid())
        {
            const FVector FinalPosition = Player->GetActorLocation();
            const float HorizontalTravel = FVector::Dist2D(FinalPosition, InitialPosition);
            UE_LOG(LogTemp, Display, TEXT("SG_SMOKE_MOVEMENT Travel2D=%.1f Zstart=%.1f Zend=%.1f"), HorizontalTravel, InitialPosition.Z, FinalPosition.Z);
            Check(TEXT("PLAYER_MOVES"), HorizontalTravel > 20.0f);
            Check(TEXT("PLAYER_NOT_FALLING_AWAY"), FinalPosition.Z > InitialPosition.Z-400.0f);
            // A new nearby target exercises an actual camera-origin raycast after the pawn has moved.
            // It is spawned only in opt-in smoke mode, never in regular gameplay.
            if (Combat.IsValid() && Controller.IsValid())
            {
                FVector Eye; FRotator ViewRotation;
                Controller->GetPlayerViewPoint(Eye,ViewRotation);
                const FVector TestPosition=Eye+ViewRotation.Vector()*220.0f;
                FActorSpawnParameters Options;
                Options.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                ASGTargetDummy* MovingTarget=GetWorld()->SpawnActor<ASGTargetDummy>(TestPosition,FRotator::ZeroRotator,Options);
                Check(TEXT("ADS_MOVEMENT_TARGET_SPAWN"),MovingTarget!=nullptr);
                if (MovingTarget)
                {
                    // Keep the camera center on the live target's torso.
                    Controller->SetControlRotation((MovingTarget->GetActorLocation()-Eye).Rotation());
                    Combat->StartAim();
                    const float HealthBefore=MovingTarget->GetHealth();
                    const int32 AmmoBefore=Combat->GetAmmoInMagazine();
                    Combat->FireTest();
                    Check(TEXT("ADS_AFTER_MOVEMENT_HIT"),MovingTarget->GetHealth()<HealthBefore);
                    Check(TEXT("ADS_AFTER_MOVEMENT_AMMO"),Combat->GetAmmoInMagazine()==AmmoBefore-1);
                    Check(TEXT("ADS_AFTER_MOVEMENT_HITMARKER"),Combat->HasRecentConfirmedTargetHit());
                    Combat->StopAim();
                }
            }
        }
        if (ASGGameMode* Mode = Cast<ASGGameMode>(UGameplayStatics::GetGameMode(this)))
        {
            Mode->RestartTraining();
            Check(TEXT("TRAINING_RESTART_REVIVES_TARGET"), Target.IsValid() && Target->GetHealth() >= 100.0f);
            Check(TEXT("TRAINING_RESTART_RESETS_SCORE"), Mode->GetTrainingScore() == 0 &&
                Mode->GetTrainingEliminations() == 0);
            Check(TEXT("TRAINING_RESTART_RESETS_AMMO"), Combat.IsValid() &&
                Combat->GetAmmoInMagazine() == 30);
            Check(TEXT("TRAINING_RESTART_RESETS_CLOCK"), Mode->IsTrainingActive() &&
                Mode->GetTrainingTimeRemaining() > 80.0f);
        }
        Finish();
        Phase = 5;
    }
}