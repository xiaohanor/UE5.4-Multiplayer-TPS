#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // 初始化时尝试绑定 BlasterCharacter
    BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    //  如果 BlasterCharacter 为空，尝试重新绑定
    if (BlasterCharacter == nullptr)
    {
        BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
    }
    if (BlasterCharacter == nullptr) return;

    // 获取角色的速度
    FVector Velocity = BlasterCharacter->GetVelocity();
    Velocity.Z = 0;
    Speed = Velocity.Size();

    // 动画状态标志
    bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
    bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
    bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
    EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
    bIsCrouched = BlasterCharacter->bIsCrouched;
    bIsAiming = BlasterCharacter->IsAiming();
    TurningInPlace = BlasterCharacter->GetTurningInPlace();
    bRotateRootBone = BlasterCharacter->ShouldRotateRootBone();
    bElimmed = BlasterCharacter->IsElimmed();

    // 计算 Yaw 偏移
    if (Velocity.SizeSquared() > 0) // 仅在有移动时计算
    {
        FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
        FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Velocity);
        FRotator DeltaDot = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, MovementRotation);
        DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaDot, DeltaSeconds, 6.f);
        YawOffset = DeltaRotation.Yaw;
    }

    // 计算角色的倾斜度
    CharacterRotationLastFrame = CharacterRotation;
    CharacterRotation = BlasterCharacter->GetActorRotation();
    const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
    const float Target = Delta.Yaw / DeltaSeconds;
    const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
    Lean = FMath::Clamp(Interp, -90.f, 90.f);

    // 动画偏移角度
    AO_Yaw = BlasterCharacter->GetAOYaw();
    AO_Pitch = BlasterCharacter->GetAOPitch();


    if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->WeaponMeshGetter() && BlasterCharacter->GetMesh())
    {
        // 左手插槽位置
        LeftHandTransform = EquippedWeapon->WeaponMeshGetter()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
        FVector OutPosition;
        FRotator OutRotation;
        BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
        LeftHandTransform.SetLocation(OutPosition);
        LeftHandTransform.SetRotation(FQuat(OutRotation));

        // 右手插槽位置
        if (BlasterCharacter->IsLocallyControlled())
        {
            bLocallyControlled = true;
			FTransform RightHandTransform = BlasterCharacter->GetMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
            FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));
            RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 30.f);
        }
    }

    bUseFABRIK = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;
}
