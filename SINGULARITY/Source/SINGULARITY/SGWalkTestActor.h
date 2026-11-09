#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGWalkTestActor.generated.h"

class APawn;
UCLASS(NotBlueprintable)
class SINGULARITY_API ASGWalkTestActor : public AActor
{
    GENERATED_BODY()
public:
    ASGWalkTestActor();
    virtual void Tick(float DeltaSeconds) override;
private:
    TWeakObjectPtr<APawn> Player;
    FVector StartLocation=FVector::ZeroVector;
    float Elapsed=0.0f;
    int32 NextReportSecond=0;
    bool bFell=false;
    bool bInjectedFall=false;
    bool bRecoveredFall=false;
    bool bFinished=false;
};