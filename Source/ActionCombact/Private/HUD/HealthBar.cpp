// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBar::SetHealthPercent(float Percent)
{
	HealthBar->SetPercent(Percent);
}