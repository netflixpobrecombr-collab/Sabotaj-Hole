#include "SGFirstPersonWeapon.h"
#include "SGCombatSystem.h"
#include "EngineUtils.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimSequence.h"
#include "Components/SkinnedMeshComponent.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Components/PointLightComponent.h"
#include "Sound/SoundWave.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

ASGFirstPersonWeapon::ASGFirstPersonWeapon()
{
    PrimaryActorTick.bCanEverTick=true;
    PrimaryActorTick.TickGroup=TG_PostUpdateWork;
    SetActorEnableCollision(false);
    WeaponRoot=CreateDefaultSubobject<USceneComponent>(TEXT("SG01_Root"));
    SetRootComponent(WeaponRoot);
    RecoveredAK74Mesh=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RecoveredAK74_InternalOnly"));
    RecoveredAK74Mesh->SetupAttachment(WeaponRoot);
    RecoveredAK74Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RecoveredAK74Mesh->SetGenerateOverlapEvents(false);
    RecoveredAK74Mesh->SetCastShadow(false);
    // Import long axis is local +Y. Rotate it to forward +X in the viewmodel.
    RecoveredAK74Mesh->SetRelativeRotation(FRotator(0.0f,-90.0f,0.0f));
    RecoveredAK74Mesh->SetRelativeLocation(FVector(-12.0f,0.0f,-4.0f));
    RecoveredAK74Mesh->SetRelativeScale3D(FVector(0.94f));
    RecoveredAK74Mesh->SetVisibility(false);
    FirstPersonArmsMesh=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPerson_RifleHands"));
    FirstPersonArmsMesh->SetupAttachment(WeaponRoot);
    FirstPersonArmsMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    FirstPersonArmsMesh->SetGenerateOverlapEvents(false);
    FirstPersonArmsMesh->SetCastShadow(false);
    FirstPersonArmsMesh->SetRelativeLocation(FVector(-23.5f,-4.0f,-132.0f));
    FirstPersonArmsMesh->SetRelativeScale3D(FVector(0.92f));
    FirstPersonArmsMesh->SetRelativeRotation(FRotator(0.0f,-90.0f,0.0f));
    FirstPersonArmsMesh->SetVisibility(false);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cylinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> Graphite(TEXT("/Game/Singularity/Weapons/M_SG01_Graphite.M_SG01_Graphite"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> Cyan(TEXT("/Game/Singularity/Weapons/M_SG01_Cyan.M_SG01_Cyan"));
    static ConstructorHelpers::FObjectFinder<USoundWave> FireClip(TEXT("/Game/Singularity/Audio/S_SG01_Fire.S_SG01_Fire"));
    static ConstructorHelpers::FObjectFinder<USoundWave> ReloadClip(TEXT("/Game/Singularity/Audio/S_SG01_Reload.S_SG01_Reload"));
    if (FireClip.Succeeded()) FireSound=FireClip.Object;
    if (ReloadClip.Succeeded()) ReloadSound=ReloadClip.Object;

    auto Part=[this](const TCHAR* Name,UStaticMesh* Mesh,const FVector& Position,const FVector& Scale,
                     const FRotator& Rotation,UMaterialInterface* Material)
    {
        UStaticMeshComponent* Component=CreateDefaultSubobject<UStaticMeshComponent>(Name);
        Component->SetupAttachment(WeaponRoot);
        Component->SetStaticMesh(Mesh);
        Component->SetRelativeLocation(Position);
        Component->SetRelativeRotation(Rotation);
        Component->SetRelativeScale3D(Scale);
        Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Component->SetGenerateOverlapEvents(false);
        Component->SetCastShadow(false);
        if(Material) Component->SetMaterial(0,Material);
        return Component;
    };

    UStaticMesh* C=Cube.Succeeded()?Cube.Object.Get():nullptr;
    UStaticMesh* Tube=Cylinder.Succeeded()?Cylinder.Object.Get():C;
    UMaterialInterface* Dark=Graphite.Succeeded()?Graphite.Object.Get():nullptr;
    UMaterialInterface* Accent=Cyan.Succeeded()?Cyan.Object.Get():Dark;
    // SG-01 / Mk II: an original modular carbine silhouette assembled from
    // lightweight engine primitives. Cosmetic, non-colliding, no third-party mesh.
    // Local X is forward, Y is horizontal, Z vertical.
    Part(TEXT("Receiver"),C,FVector(0,0,0),FVector(0.35,0.105,0.125),FRotator::ZeroRotator,Dark);
    Part(TEXT("ReceiverTop"),C,FVector(-3,0,9),FVector(0.265,0.082,0.028),FRotator::ZeroRotator,Dark);
    Part(TEXT("UpperFront"),C,FVector(25,0,3),FVector(0.24,0.103,0.091),FRotator::ZeroRotator,Dark);
    Part(TEXT("Handguard"),C,FVector(35,0,0),FVector(0.24,0.093,0.083),FRotator::ZeroRotator,Dark);
    Part(TEXT("HandguardLower"),C,FVector(37,0,-8),FVector(0.20,0.079,0.020),FRotator::ZeroRotator,Dark);
    Part(TEXT("StockBuffer"),Tube,FVector(-31,0,-2),FVector(0.06,0.06,0.20),FRotator(90,0,0),Dark);
    Part(TEXT("StockCheekRest"),C,FVector(-44,0,4),FVector(0.21,0.095,0.055),FRotator::ZeroRotator,Dark);
    Part(TEXT("StockLowerBrace"),C,FVector(-42,0,-11),FVector(0.175,0.055,0.043),FRotator(12,0,0),Dark);
    Part(TEXT("StockButtPad"),C,FVector(-64,0,-4),FVector(0.038,0.125,0.17),FRotator::ZeroRotator,Dark);
    Part(TEXT("PistolGrip"),C,FVector(-14,0,-17),FVector(0.085,0.073,0.20),FRotator(-14,0,0),Dark);
    Part(TEXT("TriggerGuardLower"),C,FVector(-3,0,-20),FVector(0.13,0.043,0.023),FRotator::ZeroRotator,Dark);
    MagazineMesh=Part(TEXT("Magazine"),C,FVector(9,0,-21),FVector(0.103,0.084,0.26),FRotator(12,0,0),Dark);
    Part(TEXT("MagazineBase"),C,FVector(4,0,-47),FVector(0.12,0.105,0.022),FRotator(12,0,0),Dark);
    Part(TEXT("MagazineAccent"),C,FVector(16,9,-25),FVector(0.018,0.009,0.145),FRotator(12,0,0),Accent);
    Part(TEXT("BarrelCore"),Tube,FVector(56,0,3),FVector(0.034,0.034,0.31),FRotator(90,0,0),Dark);
    Part(TEXT("BarrelSleeve"),Tube,FVector(63,0,3),FVector(0.050,0.050,0.15),FRotator(90,0,0),Dark);
    Part(TEXT("MuzzleBrake"),Tube,FVector(81,0,3),FVector(0.053,0.053,0.049),FRotator(90,0,0),Dark);
    Part(TEXT("MuzzleRing"),Tube,FVector(87,0,3),FVector(0.054,0.054,0.015),FRotator(90,0,0),Accent);
    Part(TEXT("TopRail"),C,FVector(12,0,12),FVector(0.42,0.046,0.018),FRotator::ZeroRotator,Dark);
    // Low-profile holo optic; avoids a large block over the reticle.
    Part(TEXT("OpticBase"),C,FVector(6,0,16),FVector(0.12,0.062,0.024),FRotator::ZeroRotator,Dark);
    Part(TEXT("OpticHousingLeft"),C,FVector(8,-5,20),FVector(0.042,0.018,0.054),FRotator::ZeroRotator,Dark);
    Part(TEXT("OpticHousingRight"),C,FVector(8,5,20),FVector(0.042,0.018,0.054),FRotator::ZeroRotator,Dark);
    Part(TEXT("OpticHousingUpper"),C,FVector(8,0,25),FVector(0.039,0.067,0.012),FRotator::ZeroRotator,Dark);
    Part(TEXT("OpticActiveDot"),C,FVector(13,0,21),FVector(0.012,0.010,0.010),FRotator::ZeroRotator,Accent);
    Part(TEXT("FrontSight"),C,FVector(50,0,13),FVector(0.021,0.025,0.039),FRotator::ZeroRotator,Dark);
    Part(TEXT("FrontSightDot"),C,FVector(51,0,17),FVector(0.013,0.012,0.010),FRotator::ZeroRotator,Accent);
    // Thin signature cyan lines, not a continuous emissive bar.
    Part(TEXT("ReceiverLogoSide"),C,FVector(-9,10.9,3),FVector(0.075,0.008,0.014),FRotator::ZeroRotator,Accent);
    Part(TEXT("ReceiverBadgeSide"),C,FVector(4,10.9,-6),FVector(0.038,0.008,0.014),FRotator::ZeroRotator,Accent);
    Part(TEXT("HandguardStripe"),C,FVector(39,9.65,3),FVector(0.105,0.008,0.009),FRotator::ZeroRotator,Accent);
    Part(TEXT("StockAccent"),C,FVector(-47,9.7,6),FVector(0.085,0.007,0.009),FRotator::ZeroRotator,Accent);
    for (int32 Index=0;Index<4;++Index)
    {
        // Names must be stable FNames across construction.
        const FName RailName(*FString::Printf(TEXT("TopRailTooth_%02d"),Index));
        Part(*RailName.ToString(),C,FVector(24+Index*8,0,14),FVector(0.021,0.048,0.016),FRotator::ZeroRotator,Dark);
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    MuzzleFlashMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MuzzleFlashSphere"));
    MuzzleFlashMesh->SetupAttachment(WeaponRoot);
    if (Sphere.Succeeded()) MuzzleFlashMesh->SetStaticMesh(Sphere.Object);
    MuzzleFlashMesh->SetRelativeLocation(FVector(94,0,3));
    MuzzleFlashMesh->SetRelativeScale3D(FVector(0.12f,0.12f,0.24f));
    MuzzleFlashMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MuzzleFlashMesh->SetCastShadow(false);
    if (Accent) MuzzleFlashMesh->SetMaterial(0,Accent);
    MuzzleFlashMesh->SetVisibility(false);

    MuzzleFlashLight=CreateDefaultSubobject<UPointLightComponent>(TEXT("MuzzleFlashLight"));
    MuzzleFlashLight->SetupAttachment(WeaponRoot);
    MuzzleFlashLight->SetRelativeLocation(FVector(92,0,3));
    MuzzleFlashLight->SetIntensity(1200.0f);
    MuzzleFlashLight->SetAttenuationRadius(220.0f);
    MuzzleFlashLight->SetLightColor(FLinearColor(0.05f,0.65f,1.0f));
    MuzzleFlashLight->SetCastShadows(false);
    MuzzleFlashLight->SetVisibility(false);
}

int32 ASGFirstPersonWeapon::GetCosmeticPartCount() const
{
    TArray<UStaticMeshComponent*> Parts;
    GetComponents<UStaticMeshComponent>(Parts);
    return Parts.Num();
}

bool ASGFirstPersonWeapon::HasSafeCosmeticCollision() const
{
    TArray<UStaticMeshComponent*> Parts;
    GetComponents<UStaticMeshComponent>(Parts);
    for (const UStaticMeshComponent* PartMesh : Parts)
    {
        if (PartMesh && PartMesh->GetCollisionEnabled() != ECollisionEnabled::NoCollision) return false;
    }
    return Parts.Num() >= 30;
}

bool ASGFirstPersonWeapon::IsRecoveredAK74CollisionDisabled() const
{
    return RecoveredAK74Mesh &&
        RecoveredAK74Mesh->GetCollisionEnabled() == ECollisionEnabled::NoCollision;
}

void ASGFirstPersonWeapon::BeginPlay()
{
    Super::BeginPlay();
    // Opt-in PRIVATE reference evaluation: never use this model in a public release.
    if (FParse::Param(FCommandLine::Get(),TEXT("SGReferenceAK74")))
    {
        USkeletalMesh* Source=LoadObject<USkeletalMesh>(nullptr,
            TEXT("/Game/LegacyReview/AK74/SK_AK74_v03.SK_AK74_v03"));
        if (Source && RecoveredAK74Mesh)
        {
            // Move the entire first-person rig closer to the camera;
            // the original AK74 looked tiny at 96cm forward.
            ViewOffset=FVector(55.0f,23.0f,-23.0f);
            ADSOffset=FVector(56.0f,0.0f,-12.0f);
            AimedFOV=74.0f;
            RecoveredAK74Mesh->SetRelativeScale3D(FVector(1.12f));
            USkeletalMesh* Manny=LoadObject<USkeletalMesh>(nullptr,
                TEXT("/Game/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
            RifleIdleAnimation=LoadObject<UAnimSequence>(nullptr,
                TEXT("/Game/Mannequins/Anims/Rifle/MF_Rifle_Idle_ADS.MF_Rifle_Idle_ADS"));
            if (Manny && RifleIdleAnimation && FirstPersonArmsMesh)
            {
                FirstPersonArmsMesh->SetSkeletalMesh(Manny);
                FirstPersonArmsMesh->PlayAnimation(RifleIdleAnimation,true);
                // Hide head and legs so only the upper-body rifle stance is
                // available to the first-person camera.
                FirstPersonArmsMesh->HideBoneByName(TEXT("neck_01"),EPhysBodyOp::PBO_None);
                FirstPersonArmsMesh->HideBoneByName(TEXT("thigh_l"),EPhysBodyOp::PBO_None);
                FirstPersonArmsMesh->HideBoneByName(TEXT("thigh_r"),EPhysBodyOp::PBO_None);
                FirstPersonArmsMesh->SetVisibility(true);
                bFirstPersonHandsActive=true;
                UE_LOG(LogTemp,Display,TEXT("SG_REFERENCE_HANDS_READY Manny=%s Animation=%s"),
                    *GetNameSafe(Manny),*GetNameSafe(RifleIdleAnimation));
            }
            else UE_LOG(LogTemp,Warning,TEXT("SG_REFERENCE_HANDS_MISSING; AK74 remains visible"));
            RecoveredAK74Mesh->SetSkeletalMesh(Source);
            RecoveredAK74Mesh->SetVisibility(true);
            bRecoveredAK74Active=true;
            MuzzleFlashMesh->SetRelativeLocation(FVector(57,0,-3));
            MuzzleFlashLight->SetRelativeLocation(FVector(55,0,-3));
            TArray<UStaticMeshComponent*> Parts;
            GetComponents<UStaticMeshComponent>(Parts);
            for (UStaticMeshComponent* PartMesh : Parts)
            {
                if (PartMesh) PartMesh->SetVisibility(false);
            }
            UE_LOG(LogTemp,Display,TEXT("SG_REFERENCE_AK74_READY Mesh=%s Material=%s Bounds=%s"),
                *GetNameSafe(Source),*GetNameSafe(RecoveredAK74Mesh->GetMaterial(0)),
                *Source->GetBounds().GetBox().ToString());
        }
        else
        {
            UE_LOG(LogTemp,Error,TEXT("SG_REFERENCE_AK74_MISSING fallback to original SG-01"));
        }
    }
    UE_LOG(LogTemp,Display,TEXT("SG01_VIEWMODEL_SPAWNED ReferenceAK74=%d"),bRecoveredAK74Active?1:0);
}

void ASGFirstPersonWeapon::NotifyShot()
{
    Recoil=FMath::Min(4.0f,Recoil+1.7f);
    FlashSecondsRemaining=0.08f;
    if (MuzzleFlashMesh) MuzzleFlashMesh->SetVisibility(true);
    if (MuzzleFlashLight) MuzzleFlashLight->SetVisibility(true);
    if (FireSound) UGameplayStatics::PlaySound2D(this,FireSound,0.46f);
    UE_LOG(LogTemp,Display,TEXT("SG01_FIRE_FEEDBACK Flash=1 Sound=%d"),FireSound ? 1 : 0);
}

void ASGFirstPersonWeapon::NotifyReload()
{
    bReloadAnimating=true;
    ReloadElapsed=0.0f;
    if (ReloadSound) UGameplayStatics::PlaySound2D(this,ReloadSound,0.48f);
    UE_LOG(LogTemp,Display,TEXT("SG01_RELOAD_ANIMATION_BEGIN Sound=%d"),ReloadSound ? 1 : 0);
}

void ASGFirstPersonWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Restore camera state even if the actor is removed while the player is aiming.
    if (bFOVOverrideActive && CameraManager.IsValid())
    {
        CameraManager->UnlockFOV();
    }
    Super::EndPlay(EndPlayReason);
}

void ASGFirstPersonWeapon::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    APlayerController* PC=UGameplayStatics::GetPlayerController(this,0);
    if(!PC || !PC->GetPawn()) return;

    if (!Combat.IsValid())
    {
        for (TActorIterator<ASGCombatSystem> It(GetWorld()); It; ++It)
        {
            Combat = *It;
            break;
        }
    }
    if (PC->PlayerCameraManager && CameraManager.Get() != PC->PlayerCameraManager)
    {
        if (bFOVOverrideActive && CameraManager.IsValid()) CameraManager->UnlockFOV();
        CameraManager = PC->PlayerCameraManager;
        HipFOV = CameraManager->GetFOVAngle();
        bSavedHipFOV = true;
        bFOVOverrideActive = false;
    }

    const bool bAiming=Combat.IsValid() && Combat->IsAiming();
    ADSAlpha=FMath::FInterpTo(ADSAlpha,bAiming ? 1.0f : 0.0f,DeltaSeconds,12.0f);
    if (bSavedHipFOV && CameraManager.IsValid())
    {
        if (bAiming || ADSAlpha > 0.005f)
        {
            CameraManager->SetFOV(FMath::Lerp(HipFOV,AimedFOV,ADSAlpha));
            bFOVOverrideActive = true;
        }
        else if (bFOVOverrideActive)
        {
            CameraManager->UnlockFOV();
            bFOVOverrideActive = false;
        }
    }

    FVector Eye; FRotator ViewRot;
    PC->GetPlayerViewPoint(Eye,ViewRot);
    Recoil=FMath::FInterpTo(Recoil,0.0f,DeltaSeconds,11.0f);
    if (FlashSecondsRemaining > 0.0f)
    {
        FlashSecondsRemaining=FMath::Max(0.0f,FlashSecondsRemaining-DeltaSeconds);
        if (FlashSecondsRemaining <= 0.0f)
        {
            if (MuzzleFlashMesh) MuzzleFlashMesh->SetVisibility(false);
            if (MuzzleFlashLight) MuzzleFlashLight->SetVisibility(false);
        }
    }

    float ReloadDip=0.0f;
    if (bReloadAnimating)
    {
        ReloadElapsed+=DeltaSeconds;
        const float Phase=FMath::Clamp(ReloadElapsed/1.25f,0.0f,1.0f);
        ReloadDip=FMath::Sin(Phase*PI);
        if (ReloadElapsed>=1.25f)
        {
            bReloadAnimating=false;
            ReloadDip=0.0f;
            UE_LOG(LogTemp,Display,TEXT("SG01_RELOAD_ANIMATION_END"));
        }
    }
    if (MagazineMesh)
    {
        MagazineMesh->SetRelativeLocation(FVector(8,0,-20.0f-18.0f*ReloadDip));
    }
    if (bRecoveredAK74Active && RecoveredAK74Mesh)
    {
        // Lightweight pose response for an unanimated reference mesh.
        // Full skeleton/magazine hand animation requires a licensed production rig.
        RecoveredAK74Mesh->SetRelativeRotation(FRotator(-9.0f*ReloadDip,-90.0f,10.0f*ReloadDip));
        RecoveredAK74Mesh->SetRelativeLocation(FVector(-12.0f,0.0f,-4.0f-8.0f*ReloadDip));
    }

    const FVector ADSPosition=FMath::Lerp(ViewOffset,ADSOffset,ADSAlpha);
    if (bFirstPersonHandsActive && FirstPersonArmsMesh)
    {
        // Compact ADS hand pose: preserve both grips without covering the sight.
        const FVector HipArms(-23.5f,-4.0f,-132.0f);
        const FVector AimArms(-17.0f,-2.0f,-94.0f);
        const float ArmsScale=FMath::Lerp(0.92f,0.65f,ADSAlpha);
        FirstPersonArmsMesh->SetRelativeScale3D(FVector(ArmsScale));
        FirstPersonArmsMesh->SetRelativeLocation(
            FMath::Lerp(HipArms,AimArms,ADSAlpha)+FVector(0.0f,0.0f,-6.0f*ReloadDip));
        FirstPersonArmsMesh->SetRelativeRotation(FRotator(-3.0f*ReloadDip,-90.0f,0.0f));
    }
    const FVector Offset=ADSPosition+FVector(-Recoil*1.8f-8.0f*ReloadDip,
                                             5.0f*ReloadDip,-Recoil*1.1f-14.0f*ReloadDip);
    SetActorLocationAndRotation(Eye+ViewRot.RotateVector(Offset),
        ViewRot+FRotator(-Recoil*0.6f,0,-10.0f*ReloadDip),false,nullptr,ETeleportType::TeleportPhysics);
    if(!bReadyLogged)
    {
        UE_LOG(LogTemp,Display,TEXT("SG01_VIEWMODEL_READY Position=%s"),*GetActorLocation().ToString());
        if (bFirstPersonHandsActive && FirstPersonArmsMesh)
        {
            const FTransform Right=FirstPersonArmsMesh->GetSocketTransform(TEXT("hand_r"),RTS_Component);
            const FTransform Left=FirstPersonArmsMesh->GetSocketTransform(TEXT("hand_l"),RTS_Component);
            UE_LOG(LogTemp,Display,TEXT("SG_REFERENCE_HAND_BONES Right=%s Left=%s Arms=%s Weapon=%s"),
                *Right.GetLocation().ToString(),*Left.GetLocation().ToString(),
                *FirstPersonArmsMesh->GetRelativeLocation().ToString(),
                *RecoveredAK74Mesh->GetRelativeLocation().ToString());
        }
        bReadyLogged=true;
    }
}