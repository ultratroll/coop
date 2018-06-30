// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopWeapon.h"

// Sets default values
ACoopWeapon::ACoopWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void ACoopWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACoopWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

