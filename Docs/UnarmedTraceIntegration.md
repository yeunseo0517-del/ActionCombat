# Weapon Trace Pipeline 재사용 구조

> 맨손 전투까지 확장한 Weapon Trace Pipeline 재사용 구조

## 목차

* [설계 배경](#설계-배경-및-결정)
* [핵심 구현](#핵심-구현)
  * [Override 구조](#Override-구조)
    * [의사결정 과정](#의사결정-과정)
* [관련 코드](#관련-코드)

---

## 설계 배경

| 항목        | 별도 Unarmed 시스템 | Weapon 구조 재사용 (선택) |
| --------- | -------------- | ------------------ |
| Trace 로직  | 별도 구현 필요       | 기존 구조 재사용          |
| Combat 연동 | 별도 처리          | 기존 처리 사용           |
| 유지보수      | 시스템 증가         | 기존 구조 유지           |
| 확장 비용     | 기능별 추가 구현      | Weapon 구조 활용       |


맨몸 공격과 무기가 Character Mesh에 포함된 경우를 캐릭터 기반 공격으로 분류했습니다.

캐릭터 기반 공격은 Projectile이나 Area Attack과 달리, **Weapon Mesh만 없는 근접 공격**에 가깝다고 판단했습니다.

근접 Trace 로직을 담당하는 객체인 Weapon을 이용해 Dummy Weapon Actor를 통해 **기존 Weapon Trace Pipeline을 재사용**하도록 구성했습니다.
별도 파이프라인을 만들기보다 기존 MeleeWeapon 구조를 그대로 재사용하는 것이 중복을 줄일 수 있다고 판단했습니다.

---

## 핵심 구현

### Dummy Actor 방식

캐릭터 기반 공격은 Mesh가 없는 **Dummy Actor(Unarmed Weapon)** 를 Weapon으로 사용합니다.

맨손 공격의 Socket 조회 방식 → [Data-driven Trace System - Socket 조회 위치 결정](https://github.com/yeunseo0517-del/ActionCombat/blob/187a0821ffb25bff47c20c13409936f2bf39454c/Docs/DataDrivenTrace.md?plain=1#L161)

---

### Override 구조

`Unarmed Weapon`은 캐릭터 기반 공격을 처리하는 공통 Weapon으로 여러 캐릭터가 공유합니다. 캐릭터마다 다른 Trace Data가 필요한 경우 Override로 처리합니다.

`Unarmed Weapon`의 기본 Trace Data는 Slash Character 기준으로 `WeaponData`에 정의되어 있습니다. 다른 Trace Data가 필요한 캐릭터는 `CombatComponent`의 `OverrideCombatData`를 에디터에서 Data Asset을 세팅해 덮어씌웁니다.
공격이 시작될 때 CombatComponent가 OverrideCombatData를 넘기고 Weapon은 값이 있으면 해당 Data Asset을 사용하고 없으면 기본 WeaponData에서 Tag 기준으로 조회합니다.

```cpp
void AMeleeWeapon::SetCombatTraceData(const FCombatTraceData* NewData, const FGameplayTag& Tag)
{
    if (NewData)
        CurrentTraceData = NewData;                          // Override DA 사용
    else
        CurrentTraceData = WeaponData->AttackSet.Find(Tag); // 기본 DA 사용
}
```

공격이 시작될 때마다 호출되며, Override DA가 세팅된 캐릭터는 자동으로 해당 데이터를 우선 사용합니다.

---

### 의사결정 과정

### 기존 구조 - Weapon 소유

초기에는 Weapon 중심 전투 시스템이었기 때문에 Weapon이 Trace Data를 소유하는 구조였습니다. 로직이 단순하고 대부분의 케이스를 자연스럽게 처리할 수 있었습니다.

### 문제 인식

Unarmed Weapon처럼 여러 캐릭터가 공유하는 Weapon이 생기면서 문제가 발생했습니다. Weapon에 Trace Data를 고정하면 특정 캐릭터의 Trace에 종속될 수 있었습니다.

실제로 플레이어 캐릭터의 맨손 공격은 Sphere Trace를 사용하지만 무기가 Character Mesh에 포함된 미니언 캐릭터는 Box Trace가 필요했습니다.

### 중간 시도 - Character(Combat Component) 소유로 전환

#### 장점

캐릭터별로 유연하게 설정할 수 있었습니다.

#### 문제점

하지만 실제로는 Weapon Type에 따라 Trace가 결정되는 경우가 대부분이었습니다. 예외가 없는 일반 케이스에서도 매번 Character쪽에서 설정해야 해서 설정 비용이 불필요하게 높아졌습니다.


### 해결 - Override 구조

기존 Weapon 소유 구조를 유지하면서 예외 상황에서만 Data Asset을 Override하는 방식을 채택했습니다. 예외 케이스가 적으니 일반 케이스를 단순하게 유지하고 따로 처리하는 것이 현실적이라고 판단했습니다.

### Insight

모든 상황을 하나의 구조로 일반화하려는 시도가 오히려 복잡도를 높일 수 있다는 점을 깨달았습니다.

---

## 관련 코드

- [UCombatComponent::ExecuteAttackSequence()](https://github.com/yeunseo0517-del/ActionCombat/blob/187a0821ffb25bff47c20c13409936f2bf39454c/Source/ActionCombact/Private/Components/Combat/CombatComponent.cpp#L59)

- [UCombatComponent::SetCombatTraceData()](https://github.com/yeunseo0517-del/ActionCombat/blob/187a0821ffb25bff47c20c13409936f2bf39454c/Source/ActionCombact/Private/Components/Combat/CombatComponent.cpp#L295)

- [AMeleeWeapon::SetCombatTraceData()](https://github.com/yeunseo0517-del/ActionCombat/blob/187a0821ffb25bff47c20c13409936f2bf39454c/Source/ActionCombact/Private/Items/Weapons/MeleeWeapon.cpp#L202)
