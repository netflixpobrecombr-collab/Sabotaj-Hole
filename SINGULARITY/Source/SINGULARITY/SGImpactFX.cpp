#include "SGImpactFX.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ASGImpactFX::ASGImpactFX()
{
    PrimaryActorTick.bCanEverTick=true;
    SetActorEnableCollision(false);
    MarkMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SG_ImpactMark"));
    SetRootComponent(MarkMesh);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> Cyan(TEXT("/Game/Singularity/Weapons/M_SG01_Cyan.M_SG01_Cyan"));
    if (Sphere.Succeeded()) MarkMesh->SetStaticMesh(Sphere.Object);
    if (Cyan.Succeeded()) MarkMesh->SetMaterial(0,Cyan.Object.Get());
    MarkMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MarkMesh->SetGenerateOverlapEvents(false);
    MarkMesh->SetCastShadow(false);
    MarkMesh->SetRelativeScale3D(FVector(0.06f));

    Glow=CreateDefaultSubobject<UPointLightComponent>(TEXT("SG_ImpactGlow"));
    Glow->SetupAttachment(MarkMesh);
    Glow->SetIntensity(450.0f);
    Glow->SetAttenuationRadius(125.0f);
    Glow->SetLightColor(FLinearColor(0.04f,0.65f,1.0f));
    Glow->SetCastShadows(false);
    InitialLifeSpan=0.32f;
}
void ASGImpactFX::InitializeImpact(bool bTargetImpact, const FVector& SurfaceNormal)
{
    bHitTarget=bTargetImpact;
    SetActorRotation(SurfaceNormal.Rotation());
    UE_LOG(LogTemp,Display,TEXT("SG_IMPACT_FX Kind=%s Position=%s Collision=Off"),
        bHitTarget ? TEXT("Target") : TEXT("Surface"),*GetActorLocation().ToString());
}
bool ASGImpactFX::IsCollisionDisabled() const
{
    return MarkMesh && MarkMesh->GetCollisionEnabled()==ECollisionEnabled::NoCollision;
}
void ASGImpactFX::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    AgeSeconds+=DeltaSeconds;
    const float Alpha=FMath::Clamp(1.0f-AgeSeconds/0.32f,0.0f,1.0f);
    MarkMesh->SetRelativeScale3D(FVector(0.06f*FMath::Max(Alpha,0.01f)));
    if(Glow) Glow->SetIntensity(450.0f*Alpha);
}