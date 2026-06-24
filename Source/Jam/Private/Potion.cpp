// Fill out your copyright notice in the Description page of Project Settings.


#include "Potion.h"
#include "StatusComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Jam/JamCharacter.h"
#include "TimerManager.h"

// Sets default values
APotion::APotion()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Trigger"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HealthRestoreAmount = 50.0f;
	ManaRestoreAmount = 30.0f;
	RespawnTime = 60.0f;

}

// Called when the game starts or when spawned
void APotion::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APotion::OnOverlapBegin);
	
}

void APotion::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
    {
        // 1. 부딪힌 액터가 AJamCharacter인지 검사 (캐스팅)
        AJamCharacter* JamPlayer = Cast<AJamCharacter>(OtherActor);

        // 플레이어가 맞다면 실행
        if (JamPlayer) 
        {
            // 2. 플레이어 내부에 있는 StatusComponent 가져오기
            // ※ AJamCharacter 내부에 선언된 컴포넌트 변수명이나 Getter 함수에 맞게 수정하세요.
            // 예시 1: JamPlayer->GetStatusComponent()
            // 예시 2: JamPlayer->StatusComponent
            UStatusComponent* StatusComp = JamPlayer->FindComponentByClass<UStatusComponent>();
            
            if (StatusComp)
            {
                // [디버그] 먹기 전 수치 확인
                float HealthBefore = StatusComp->GetCurrentHealth();
                float ManaBefore = StatusComp->GetCurrentMana();

                UE_LOG(LogTemp, Log, TEXT("[Potion] Before -> Health: %f, Mana: %f"), HealthBefore, ManaBefore);
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Yellow, 
                        FString::Printf(TEXT("[Before] Health: %.1f / Mana: %.1f"), HealthBefore, ManaBefore));
                }

                // 3. 플레이어의 스태터스 컴포넌트를 통해 회복 함수 호출
                StatusComp->IncreaseHealth(HealthRestoreAmount);
                StatusComp->IncreaseMana(ManaRestoreAmount);

            	if (HealthRestoreAmount > 0.0f) JamPlayer->SpawnEffect(JamPlayer->HealthHealEffect, JamPlayer->GetActorLocation());
            	if (ManaRestoreAmount > 0.0f) JamPlayer->SpawnEffect(JamPlayer->ManaHealEffect, JamPlayer->GetActorLocation());

                // [디버그] 먹은 후 수치 확인
                float HealthAfter = StatusComp->GetCurrentHealth();
                float ManaAfter = StatusComp->GetCurrentMana();

                UE_LOG(LogTemp, Warning, TEXT("[Potion] After -> Health: %f, Mana: %f"), HealthAfter, ManaAfter);
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, 
                        FString::Printf(TEXT("[After] Health: %.1f (+%.1f) / Mana: %.1f (+%.1f)"), 
                            HealthAfter, HealthRestoreAmount, ManaAfter, ManaRestoreAmount));
                }

                // 4. 포션 획득 처리 (숨기기 및 콜리전 끄기)
                MeshComponent->SetVisibility(false);
                CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

                // 5. 60초 리젠 타이머 작동
                GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &APotion::RespawnItem, RespawnTime, false);
            }
            else
            {
                // AJamCharacter는 맞는데 내부에 StatusComponent가 없을 때 에러 로그
                UE_LOG(LogTemp, Error, TEXT("[Potion] ERROR: JamCharacter has NO StatusComponent!"));
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, TEXT("Error: JamCharacter has no StatusComponent!"));
                }
            }
        }
        else
        {
            // 플레이어가 아닌 다른 액터(배경 오브젝트, 적 등)가 닿았을 때 무시
            UE_LOG(LogTemp, Log, TEXT("[Potion] Overlapped with non-player actor: %s"), *OtherActor->GetName());
        }
    }
}

void APotion::RespawnItem()
{
	MeshComponent->SetVisibility(true);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Potion Respawned!"));
	}
}


