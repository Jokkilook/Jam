// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DeathScreen.h"
#include "GameFramework/Character.h"
#include "NiagaraSystem.h"
#include "JamCharacter.generated.h"

class UPlayerHUD;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDebuffChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDebuffActivated, int8, DebuffIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuffChanged);

UCLASS(Blueprintable)
class AJamCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	class UStatusComponent* StatusComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* IMC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Movement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* TeleportAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ManaBulletAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireBallAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IceStormAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EarthQuakeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BindingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	UPlayerHUD* PlayerHUD;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	float MovementEffector = 1.0f;

	//텔레포트 스킬 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Teleport")
	float TeleportDistance = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Teleport")
	float TeleportCoolTime = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Teleport")
	float TeleportManaUse = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Teleport")
	bool CanTeleport = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|Teleport")
	USoundBase* TeleportSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|Teleport|Effect")
	UNiagaraSystem* TeleportEffect;
	
	//마나샷 스킬 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|ManaBullet")
	float ManaBulletCoolTime = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|ManaBullet")
	float ManaBulletManaUse = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|ManaBullet")
	float ManaBulletDamage = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|ManaBullet")
	bool CanManaBullet = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|ManaBullet")
	TSubclassOf<class ASkillProjectile> ManaBulletProjectileClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|ManaBullet")
	USoundBase* ManaBulletSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|ManaBullet|Effect")
	UNiagaraSystem* ManaBulletEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|ManaBullet")
	TSubclassOf<class ASkillProjectile> FireBallProjectileClass;
	
	//파이어볼 스킬 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|FireBall")
	float FireBallCoolTime = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|FireBall")
	float FireBallManaUse = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|FireBall")
	float FireBallDamage = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|FireBall")
	bool CanFireBall = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|FireBall")
	USoundBase* FireBallSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|FireBall|Effect")
	UNiagaraSystem* FireBallEffect;
	
	//아이스스톰 스킬 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|IceStorm")
	float IceStormCoolTime = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|IceStorm")
	float IceStormManaUse = 25.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|IceStorm")
	float IceStormDamage = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|IceStorm")
	bool CanIceStorm = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|IceStorm")
	float IceStormAttackRadius = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|IceStorm")
	float IceStormMaxRangeRadius = 800.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|IceStorm")
	USoundBase* IceStormSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|IceStorm|Effect")
	UNiagaraSystem* IceStormEffect;
	
	//지진 스킬 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|EarthQuake")
	float EarthQuakeCoolTime = 25.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|EarthQuake")
	float EarthQuakeManaUse = 35.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|EarthQuake")
	float EarthQuakeRadius = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|EarthQuake")
	float EarthQuakeDamage = 70.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|EarthQuake")
	bool CanEarthQuake = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|EarthQuake")
	USoundBase* EarthQuakeSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|EarthQuake|Effect")
	UNiagaraSystem* EarthQuakeEffect;
	
	//바인딩 스킬 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Binding")
	float BindingCoolTime = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Binding")
	float BindingManaUse = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Binding")
	float BindingDamage = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Binding")
	bool CanBinding = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Binding")
	float BindingDuration = 5.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|Binding")
	USoundBase* BindingSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|Binding")
	USoundBase* BindingHitSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|Binding")
	class UNiagaraSystem* BindingEffect;

	//동료스킬 무적 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|God")
	float GodCoolTime = 120.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|God")
	float GodRemainingTime = 3.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|God")
	USoundBase* GodSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skills|God|Effect")
	UNiagaraSystem* GodEffect;	
	
	//동료스킬 스킬쿨제로 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|CoolZero")
	float CoolZeroCoolTime = 240.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|CoolZero")
	float CoolZeroRemainingTime = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skills|CoolZero")
	bool IsCoolZero = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skills|CoolZero")
	USoundBase* CoolZeroSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skills|CoolZero|Effect")
	UNiagaraSystem* CoolZeroEffect;

	//레벨업 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
	UNiagaraSystem* LevelUpEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundBase* LevelUpSound;

	//물약 회복 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
	UNiagaraSystem* HealthHealEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundBase* HealthHealSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
	UNiagaraSystem* ManaHealEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundBase* ManaHealSound;

	//스킬 쿨타임 타이머스~
	FTimerHandle TeleportCoolTimer;
	FTimerHandle ManaBulletCoolTimer;
	FTimerHandle FireBallCoolTimer;
	FTimerHandle IceStormCoolTimer;
	FTimerHandle EarthQuakeCoolTimer;
	FTimerHandle BindingCoolTimer;
	FTimerHandle BindingDurationTimer;
	FTimerHandle CoolZeroCoolTimer;
	FTimerHandle CoolZeroRemainingTimer;
	FTimerHandle GodCoolTimer;
	FTimerHandle GodRemainingTimer;
	
	AJamCharacter();

	virtual void BeginPlay() override;
	
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;	
	
	UFUNCTION(BlueprintCallable)
	void Move(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void Teleport();

	UFUNCTION(BlueprintCallable)
	void ManaBullet();

	UFUNCTION(BlueprintCallable)
	void FireBall();

	UFUNCTION(BlueprintCallable)
	void IceStorm();

	UFUNCTION(BlueprintCallable)
	void EarthQuake();

	UFUNCTION(BlueprintCallable)
	void Binding();

	UFUNCTION(BlueprintCallable)
	void GodMode();

	UFUNCTION(BlueprintCallable)
	void GodModeOff();
	
	UFUNCTION(BlueprintCallable)
	void StartGod();

	UFUNCTION(BlueprintCallable)
	void CoolZero();

	UFUNCTION(BlueprintCallable)
	void CoolZeroOff();
	
	UFUNCTION(BlueprintCallable)
	void StartCoolZero();

	UFUNCTION(BlueprintCallable)
	void LevelUp();

	UFUNCTION(BlueprintCallable)
	void OnDeath();

	UFUNCTION(BlueprintCallable)
	void Respawn();

	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* DeathAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDead = false;

	FTimerHandle RespawnTimer;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDeathScreen> DeathScreenClass;

	UFUNCTION(BlueprintCallable)
	void SpawnEffect(UNiagaraSystem* Effect, FVector SpawnLocation);

	//버어프
	FOnDebuffChanged OnDebuffChanged;
	FOnDebuffActivated OnDebuffActivated;
	FOnBuffChanged OnBuffChanged;
	
	//스택
	UPROPERTY(BlueprintReadOnly)
	uint8 DiscordStack = 0;
	
	//디버프 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debuff")
	float DebuffRemainingTime = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debuff")
	float HealthDebuffAmount = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debuff")
	float ManaDebuffAmount = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debuff")
	float SpeedDebuffRate = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debuff")
	float AttackDebuffRate = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debuff")
	float DamageDebuffRate = 60.0f;
	
	//버프 타이머
	FTimerHandle SpeedDebuffTimer;
	FTimerHandle AttackDebuffTimer;
	FTimerHandle DamageDebuffTimer;

	//디버프 리스트
	TArray<uint8> DebuffList = {0,1,2,3,4};

	//랜덤 버프 적용
	void ApplyRandomDebuff();

	//받는 피해, 공격 영향
	//공격력 반영 비율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Setting")
	float AttackRate = 1.0f;
	//받는 피해 반영 비율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Setting")
	float DamageRate = 1.0f;
	
	//체력감소
	void HealthDebuff();
	//마나 감소
	void ManaDebuff();
	//이속 감소
	void SpeedDebuff();
	//대미지 감소
	void AttackDebuff();
	//받는 피해 증가
	void DamageDebuff();

	//이속 정상화
	void RestoreSpeed();
	//대미지 정상화
	void RestoreAttack();
	//받는 피해 정상화
	void RestoreDamage();

	//디스코드 스택 쌓기
	void StackDiscord();
	
	// 타격 플래시 머티리얼
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitResponse")
	UMaterialInterface* HitMaterial;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	TArray<UMaterialInterface*> OriginalMaterials;
	FTimerHandle HitFlashTimerHandle;
	void RestoreOriginalMaterials();
};