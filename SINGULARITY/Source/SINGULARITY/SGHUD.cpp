#include "SGHUD.h"
#include "SGGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "SGCombatSystem.h"
#include "SGFirstPersonWeapon.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

void ASGHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas || !GetWorld()) { return; }

    const float X = Canvas->ClipX * 0.5f;
    const float Y = Canvas->ClipY * 0.5f;
    const FLinearColor Crosshair(0.98f, 0.98f, 0.98f, 1.0f);

    DrawLine(X - 11, Y, X - 4, Y, Crosshair, 1.5f);
    DrawLine(X + 4, Y, X + 11, Y, Crosshair, 1.5f);
    DrawLine(X, Y - 11, X, Y - 4, Crosshair, 1.5f);
    DrawLine(X, Y + 4, X, Y + 11, Crosshair, 1.5f);
    DrawRect(Crosshair, X - 1.0f, Y - 1.0f, 2.0f, 2.0f);

    UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;
    if (!Font) { return; }
    DrawText(TEXT("SINGULARITY  |  BLACK HOLE STUDIO"), FLinearColor::White, 20, 18, Font, 1.0f, false);

    if (ASGGameMode* Training = Cast<ASGGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        const float Right = Canvas->ClipX - 238.0f;
        DrawRect(FLinearColor(0.015f, 0.035f, 0.050f, 0.70f), Right - 12.0f, 14.0f, 234.0f, 115.0f);
        DrawText(TEXT("TRAINING  /  GALATA"), FLinearColor(0.21f, 0.93f, 0.96f, 1.0f), Right, 22.0f, Font, 1.10f, false);
        DrawText(FString::Printf(TEXT("SCORE   %04d"), Training->GetTrainingScore()),
            FLinearColor::White, Right, 47.0f, Font, 1.05f, false);
        DrawText(FString::Printf(TEXT("TARGETS  %d / %d"),
            Training->GetTrainingEliminations(), Training->GetTrainingTargetCount()),
            FLinearColor::White, Right, 70.0f, Font, 1.05f, false);
        const int32 Seconds = FMath::CeilToInt(Training->GetTrainingTimeRemaining());
        const FString Timer = FString::Printf(TEXT("TIME   %02d:%02d"), Seconds / 60, Seconds % 60);
        DrawText(Timer, Seconds <= 15 ? FLinearColor(1.0f, 0.45f, 0.35f, 1.0f) :
            FLinearColor(0.21f, 0.93f, 0.96f, 1.0f), Right, 94.0f, Font, 1.05f, false);
        if (Training->IsTrainingComplete() || Training->IsTrainingTimedOut())
        {
            const FString Result = Training->IsTrainingComplete() ? TEXT("TRAINING COMPLETE!") : TEXT("TIME UP!");
            DrawText(Result, FLinearColor(0.21f, 0.93f, 0.96f, 1.0f), X - 90.0f, Y - 72.0f, Font, 1.2f, false);
        }
    }

    int32 Ammo = -1;
    ASGCombatSystem* Combat = nullptr;
    for (TActorIterator<ASGCombatSystem> It(GetWorld()); It; ++It)
    {
        Combat = *It;
        Ammo = Combat->GetAmmoInMagazine();
        break;
    }
    if (Ammo >= 0)
    {
        DrawText(FString::Printf(TEXT("AMMO  %02d / 30"), Ammo), FLinearColor::White,
            Canvas->ClipX - 160.0f, Canvas->ClipY - 65.0f, Font, 1.3f, false);
        bool bReference=false;
        for (TActorIterator<ASGFirstPersonWeapon> Weapon(GetWorld()); Weapon; ++Weapon)
        {
            bReference=Weapon->HasRecoveredAK74();
            break;
        }
        DrawText(bReference ?
            TEXT("AK74 - INTERNAL REFERENCE  |  LMB: FIRE  RMB: AIM  R: RELOAD  T: RETRY") :
            TEXT("SG-01   |   LMB: FIRE   RMB: AIM   R: RELOAD   T: RETRY"),
            FLinearColor(0.75f,0.85f,0.86f,1.0f),
            20, Canvas->ClipY - 48.0f, Font, 0.9f, false);
    }
    if (Combat && Combat->IsReloading())
    {
        DrawText(TEXT("RELOADING..."), FLinearColor(0.18f,0.86f,0.98f,1.0f),
            Canvas->ClipX - 180.0f, Canvas->ClipY - 92.0f, Font, 1.0f, false);
    }
    if (Combat && Combat->IsAiming())
    {
        const FLinearColor ADSLabel(0.20f,0.95f,0.98f,1.0f);
        DrawText(TEXT("ADS"), ADSLabel, X - 12, Y + 43, Font, 0.85f, false);
    }
    if (Combat && Combat->HasRecentConfirmedTargetHit())
    {
        const FLinearColor Confirm(0.12f, 0.95f, 0.97f, 1.0f);
        DrawLine(X - 17, Y - 17, X - 9, Y - 9, Confirm, 2.2f);
        DrawLine(X + 17, Y - 17, X + 9, Y - 9, Confirm, 2.2f);
        DrawLine(X - 17, Y + 17, X - 9, Y + 9, Confirm, 2.2f);
        DrawLine(X + 17, Y + 17, X + 9, Y + 9, Confirm, 2.2f);
        DrawText(TEXT("HIT"), Confirm, X - 11, Y + 25, Font, 0.85f, false);
    }
}