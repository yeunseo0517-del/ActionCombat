# Troubleshooting

## 목차
* [Skill System 설계 과정 트러블슈팅](#Skill-System-설계-과정-트러블슈팅)
* [Niagara Weapon Trail 위치 오프셋 문제](#Niagara-Weapon-Trail-위치-오프셋-문제)

---

## Niagara Weapon Trail 위치 오프셋 문제

![Before](./Images/Before_VFXProblem.gif)
### 문제

공격 애니메이션마다 기준 Socket 정보를 별도로 지정해야 하는 반복 작업을 줄이기 위해 Trail용 Custom AnimNotify State를 제작했습니다.

검의 시작점과 끝점을 나타내는 Start / End Socket을 공격 Trace와 Trail이 함께 사용하도록 구성했습니다. Trail에서 따로 Socket 정보를 등록하지 않아도 기존 공격용 Socket 정보를 자동으로 읽어 Niagara Trail의 위치와 길이를 자동 계산하는 방식이었습니다.

검 궤적 Trail이 실제 검 위치가 아닌 엉뚱한 위치에 생성되는 문제가 발생했습니다.

### 시도한 과정

1. Trail Socket 정보 전달 성공 여부 확인
UE_LOG로 Trail Socket 전달 여부를 체크했습니다. 정상적으로 전달되고 있었습니다.

2. StartPosition / WeaponLength 값 확인
UE_LOG로 값을 출력하고 Niagara에서 DrawDebug로 변수가 올바르게 넘어오는지 확인했습니다. 값 자체는 정상이었습니다.

3. 축 계산 오류 의심
WeaponLength만큼 X축, Y축으로 각각 이동시켜봤지만 위치가 여전히 맞지 않았습니다.

### 해결

다운로드한 Sword Mesh 리소스가 Y축 기준 -90도 회전된 상태였습니다. SpawnSystemAttached() 시 Sword Mesh에 붙어 계산되기 때문에 UpVector가 Y축으로 바뀌면서 WeaponLength만큼 오른쪽에서 이펙트가 실행되었습니다.

Unreal Engine Modeling 모드를 이용해 Sword Mesh의 UpVector가 Z축이 되도록 Bake Transform 적용 후 Socket, Trace Start/End 위치를 재조정했습니다.

### 결과

![After](./Images/After_VFXProblem.gif)

---

## Skill System 설계 과정 트러블슈팅

Player와 Enemy 모두 동일한 Skill 시스템을 사용합니다. 에디터에서 설정한 Skill Class Data`FSkillEntry`를 바탕으로 Runtime 중 Skill 객체를 생성해 관리하는 구조입니다.

Player는 입력 슬롯(Q/E/R), Enemy는 교전 거리(Far/Mid)를 기준으로 Skill을 관리합니다.

Skill 시스템 구조 -> [Runtime-based Skill System](./RuntimeSkill.md)

### 문제

Player와 Enemy가 서로 다른 기준(ESkillSlot / ESkillRange)으로 스킬을 분류하면서 Skill 데이터가 두 역할을 동시에 포함해야 하는 구조가 발생했습니다. 이로 인해 사용하지 않는 불필요한 데이터가 포함되고 Skill이 자신의 호출 주체를 알아야 하는 문제가 있었습니다.

### 해결

Skill 데이터를 순수 실행 데이터로 분리하고, 분류 책임을 외부 컨테이너(TMap)에서 관리하도록 변경했습니다. 실행 단계에서는 Enum 대신 공통 int32 Key 기반으로 처리하도록 통합했습니다.

```cpp
Player : TMap<ESkillSlot, FSkillEntry>   // 입력 슬롯 기반
Enemy  : TMap<ESkillRange, TArray<FSkillEntry>>  // 교전 거리 기반
```

### 결과

Player / Enemy 공통 Skill 시스템 구축할 수 있었습니다. Skill 이 호출 주체를 알 필요가 없어졌으며 불필요한 데이터를 제거하고 구조를 단순화시킬 수 있었습니다. 또한 쉽게 구조 확장이 가능해졌습니다.

### Insight

공통 시스템을 설계할 때는 실행 주체가 아니라 실제 책임과 역할 기준으로 구조를 분리하는 것이 중요한 것을 깨달았습니다.
