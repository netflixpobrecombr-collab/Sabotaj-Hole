#include "SGTargetDummy.h"
#include "SGGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

ASGTargetDummy::ASGTargetDummy()
{
    PrimaryActorTick.bCanEverTick = false;
    TargetRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TargetRoot"));
    SetRootComponent(TargetRoot);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> TargetMark(TEXT("/Game/Singularity/Targets/M_SG_Training_Mark.M_SG_Training_Mark"));

    TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
    TargetMesh->SetupAttachment(TargetRoot);
    if (Cube.Succeeded()) { TargetMesh->SetStaticMesh(Cube.Object); }
    TargetMesh->SetRelativeScale3D(FVector(0.38f, 0.42f, 0.96f));
    TargetMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

    HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
    HeadMesh->SetupAttachment(TargetRoot);
    if (Sphere.Succeeded()) { HeadMesh->SetStaticMesh(Sphere.Object); }
    HeadMesh->SetRelativeScale3D(FVector(0.32f, 0.32f, 0.36f));
    HeadMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 65.0f));
    HeadMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

    LeftArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftArmMesh"));
    LeftArmMesh->SetupAttachment(TargetRoot);
    if (Cube.Succeeded()) { LeftArmMesh->SetStaticMesh(Cube.Object); }
    LeftArmMesh->SetRelativeScale3D(FVector(0.20f, 0.14f, 0.84f));
    LeftArmMesh->SetRelativeLocation(FVector(0.0f, -28.0f, -4.0f));
    LeftArmMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RightArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightArmMesh"));
    RightArmMesh->SetupAttachment(TargetRoot);
    if (Cube.Succeeded()) { RightArmMesh->SetStaticMesh(Cube.Object); }
    RightArmMesh->SetRelativeScale3D(FVector(0.20f, 0.14f, 0.84f));
    RightArmMesh->SetRelativeLocation(FVector(0.0f, 28.0f, -4.0f));
    RightArmMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    LeftLegMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftLegMesh"));
    LeftLegMesh->SetupAttachment(TargetRoot);
    if (Cube.Succeeded()) { LeftLegMesh->SetStaticMesh(Cube.Object); }
    LeftLegMesh->SetRelativeScale3D(FVector(0.20f,0.16f,0.58f));
    LeftLegMesh->SetRelativeLocation(FVector(0.0f,-12.0f,-76.5f));
    LeftLegMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RightLegMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightLegMesh"));
    RightLegMesh->SetupAttachment(TargetRoot);
    if (Cube.Succeeded()) { RightLegMesh->SetStaticMesh(Cube.Object); }
    RightLegMesh->SetRelativeScale3D(FVector(0.20f,0.16f,0.58f));
    RightLegMesh->SetRelativeLocation(FVector(0.0f,12.0f,-76.5f));
    RightLegMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ChestVerticalMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestVerticalMesh"));
    ChestVerticalMesh->SetupAttachment(TargetRoot);
    if (Cube.Succeeded()) ChestVerticalMesh->SetStaticMesh(Cube.Object);
    ChestVerticalMesh->SetRelativeScale3D(FVector(0.04f,0.08f,0.42f));
    ChestVerticalMesh->SetRelativeLocation(FVector(-20.0f,0.0f,3.0f));
    ChestVerticalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ChestVerticalMesh->SetCastShadow(false);
    if(TargetMark.Succeeded()) ChestVerticalMesh->SetMaterial(0,TargetMark.Object.Get());

    ChestHorizontalMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestHorizontalMesh"));
    ChestHorizontalMesh->SetupAttachment(TargetRoot);
    if (Cube.Succeeded()) ChestHorizontalMesh->SetStaticMesh(Cube.Object);
    ChestHorizontalMesh->SetRelativeScale3D(FVector(0.04f,0.28f,0.07f));
    ChestHorizontalMesh->SetRelativeLocation(FVector(-20.0f,0.0f,3.0f));
    ChestHorizontalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ChestHorizontalMesh->SetCastShadow(false);
    if(TargetMark.Succeeded()) ChestHorizontalMesh->SetMaterial(0,TargetMark.Object.Get());

    StandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StandMesh"));
    StandMesh->SetupAttachment(TargetRoot);
    if (Cube.Succeeded()) { StandMesh->SetStaticMesh(Cube.Object); }
    StandMesh->SetRelativeScale3D(FVector(0.45f, 0.78f, 0.08f));
    StandMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -108.0f));
    StandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASGTargetDummy::BeginPlay()
{
    Super::BeginPlay();
    Health = MaxHealth;
    // Reuse the neon material already assigned to each training target in the level.
    if (UMaterialInterface* SharedMaterial = TargetMesh->GetMaterial(0))
    {
        HeadMesh->SetMaterial(0, SharedMaterial);
        UMaterialInterface* Dark=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Singularity/Weapons/M_SG01_Graphite.M_SG01_Graphite"));
        if (!Dark) Dark=SharedMaterial;
        TargetMesh->SetMaterial(0, Dark);
        LeftArmMesh->SetMaterial(0, Dark);
        RightArmMesh->SetMaterial(0, Dark);
        LeftLegMesh->SetMaterial(0, Dark);
        RightLegMesh->SetMaterial(0, Dark);
        StandMesh->SetMaterial(0, Dark);
    }
}

void ASGTargetDummy::ResetTarget()
{
    Health = MaxHealth;
    for (UStaticMeshComponent* Part : {TargetMesh.Get(), HeadMesh.Get(), LeftArmMesh.Get(),
        RightArmMesh.Get(), LeftLegMesh.Get(), RightLegMesh.Get(),
        ChestVerticalMesh.Get(), ChestHorizontalMesh.Get(), StandMesh.Get()})
    {
        if (Part) { Part->SetVisibility(true, true); }
    }
    if (TargetMesh) TargetMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    if (HeadMesh) HeadMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    if (TargetMesh) TargetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    if (HeadMesh) HeadMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    UE_LOG(LogTemp, Display, TEXT("SG_TARGET_RESET %s HP=%.1f"), *GetName(), Health);
}

float ASGTargetDummy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (Health <= 0.0f) { return 0.0f; }
    const float Applied = FMath::Clamp(DamageAmount, 0.0f, Health);
    Health -= Applied;
    UE_LOG(LogTemp, Display, TEXT("SINGULARITY_TARGET_HIT %s Damage=%.1f Health=%.1f"),
        *GetName(), Applied, Health);
    if (ASGGameMode* Mode = Cast<ASGGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        Mode->RecordTrainingDamage(this, Applied, Health);
    }
    if (Health <= 0.0f)
    {
        for (UStaticMeshComponent* Part : {TargetMesh.Get(), HeadMesh.Get(), LeftArmMesh.Get(), RightArmMesh.Get(), LeftLegMesh.Get(), RightLegMesh.Get(), ChestVerticalMesh.Get(), ChestHorizontalMesh.Get(), StandMesh.Get()})
        {
            if (Part)
            {
                Part->SetVisibility(false, true);
                Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
        }
        UE_LOG(LogTemp, Display, TEXT("SINGULARITY_TARGET_DESTROYED %s"), *GetName());
    }
    return Applied;
}