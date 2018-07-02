// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopWeaponLauncher.h"
#include "CoopProjectile.h"


void ACoopWeaponLauncher::Fire()
{
	AActor* PawnOwner = GetOwner();

	// try and fire a projectile
	if (ProjectileClass && PawnOwner)
	{
		FVector MuzzleLocation =	MeshComponent->GetSocketLocation(MuzzleSocketName);
		//FRotator MuzzleRotation =	MeshComponent->GetSocketRotation(MuzzleSocketName); // In the same direction as weapon

		// In the direction the pawn is looking
		FVector		EyeLocation;
		FRotator	EyeRotation;
		PawnOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		FRotator MuzzleRotation = EyeRotation;
		//

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.Owner = this;
		//ActorSpawnParams.Instigator = PawnOwner;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		//ActorSpawnParams.Instigator = PawnOwner;

		// spawn the projectile at the muzzle
		ACoopProjectile* Projectile= GetWorld()->SpawnActor<ACoopProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
		Projectile->SetDamageType(CoopWeaponDamage);
	}
}
