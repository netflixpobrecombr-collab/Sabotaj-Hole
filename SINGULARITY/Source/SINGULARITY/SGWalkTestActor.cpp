#include "SGWalkTestActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "HAL/PlatformMisc.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

ASGWalkTestActor::ASGWalkTestActor()
{
    PrimaryActorTick.bCanEverTick=true;
}
void ASGWalkTestActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if(bFinished) return;
    if(!Player.IsValid())
    {
        Player=UGameplayStatics::GetPlayerPawn(this,0);
        if(!Player.IsValid()) return;
        StartLocation=Player->GetActorLocation();
        UE_LOG(LogTemp,Display,TEXT("SG_WALK_START X=%.1f Y=%.1f Z=%.1f"),StartLocation.X,StartLocation.Y,StartLocation.Z);
    }
    Elapsed+=DeltaSeconds;
    FVector Direction(1,0,0);
    if (FParse::Param(FCommandLine::Get(),TEXT("SGWalkY"))) Direction=FVector(0,1,0);
    if (FParse::Param(FCommandLine::Get(),TEXT("SGWalkNegY"))) Direction=FVector(0,-1,0);
    if (FParse::Param(FCommandLine::Get(),TEXT("SGWalkBack"))) Direction=FVector(-1,0,0);
    if(Elapsed<8.0f) Player->AddMovementInput(Direction,1.0f);
    const bool bForceFall=FParse::Param(FCommandLine::Get(),TEXT("SGForceFall"));
    if (bForceFall && Elapsed>=2.5f && !bInjectedFall)
    {
        FVector ForcedPosition=Player->GetActorLocation();
        ForcedPosition.Z=-1800.0f;
        bInjectedFall=Player->SetActorLocation(ForcedPosition,false,nullptr,ETeleportType::TeleportPhysics);
        UE_LOG(LogTemp,Warning,TEXT("SG_WALK_FORCE_FALL Injected=%d Z=%.1f"),bInjectedFall?1:0,Player->GetActorLocation().Z);
    }
    const FVector P=Player->GetActorLocation();
    if(bForceFall && bInjectedFall && Elapsed>3.0f && P.Z>300.0f) bRecoveredFall=true;
    if(Elapsed>=NextReportSecond)
    {
        UE_LOG(LogTemp,Display,TEXT("SG_WALK_SAMPLE T=%.1f X=%.1f Y=%.1f Z=%.1f Dist2D=%.1f"),Elapsed,P.X,P.Y,P.Z,FVector::Dist2D(P,StartLocation));
        ++NextReportSecond;
    }
    if(P.Z<StartLocation.Z-400.f && !bFell && !bForceFall)
    {
        bFell=true;
        UE_LOG(LogTemp,Error,TEXT("SG_WALK_FALL T=%.2f X=%.1f Y=%.1f Z=%.1f"),Elapsed,P.X,P.Y,P.Z);
    }
    if(Elapsed>=9.0f)
    {
        const bool bPass=bForceFall ? (bInjectedFall && bRecoveredFall && P.Z>300.f) : (!bFell && P.Z>StartLocation.Z-400.0f && FVector::Dist2D(P,StartLocation)>200.0f);
        UE_LOG(LogTemp,Display,TEXT("SG_WALK_END Passed=%d Fell=%d Recovered=%d X=%.1f Y=%.1f Z=%.1f Travel=%.1f"),bPass?1:0,bFell?1:0,bRecoveredFall?1:0,P.X,P.Y,P.Z,FVector::Dist2D(P,StartLocation));
        bFinished=true;
        FPlatformMisc::RequestExit(false);
    }
}