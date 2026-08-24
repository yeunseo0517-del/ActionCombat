# Troubleshooting

## 목차
* [Blood Field 모서리 구간 Sample 위치 보정](#Blood-Field-모서리-구간-Sample-위치-보정)
* [Skill System 설계 과정 트러블슈팅](#Skill-System-설계-과정-트러블슈팅)
* [Niagara Weapon Trail 위치 오프셋 문제](#Niagara-Weapon-Trail-위치-오프셋-문제)

---
## Blood Field 모서리 구간 Sample 위치 보정

### 문제

<img width="274" height="291" alt="image" src="https://github.com/user-attachments/assets/96ded2d3-d94b-4834-a3da-5a96d8c26967" />


급격한 Normal 변화가 있는 모서리에서 1차 Normal Trace가 실제 표면을 찾지 못해 Sample이 붕 뜨는 문제가 발생했습니다.
이를 보완하기 위해 Tangent/Bitangent 방향의 2차 Trace를 시도했지만 실제 인접 면의 방향과 일치하지 않아 안정적 표면 탐색에 실패했습니다.

### 해결

**① Normal Trace만으로 해결할 수 없었던 이유**

중심 충돌 지점의 Normal을 기준으로 각 Splat의 예상 위치를 계산한 뒤 Normal 방향으로 다시 Trace하면 평면이나 완만한 굴곡에서는 실제 표면으로 보정할 수 있었다. 하지만 Splat의 예상 위치가 급격한 모서리를 넘어간 경우에는 해당 Normal 방향에 더 이상 표면이 존재하지 않아 혈흔이 끊기거나, 긴 Trace가 다른 면을 잘못 잡는 문제가 발생했다.

**② 2차 Trace로 인접 면 탐색**

1차 Trace에서 유효한 표면을 찾지 못한 경우에는 해당 Splat이 모서리를 넘어갔을 가능성이 있다고 판단했다. 따라서 Center에서 Splat으로 향하는 `MoveDir`을 기준으로 추가 Trace를 수행하여 주변의 인접 표면을 탐색하고, 새로운 `ImpactPoint`와 `ImpactNormal`을 얻도록 했다.

**③ HitPoint가 아니라 Plane 정보로 Corner 추론**

2차 Trace의 HitPoint를 그대로 최종 위치로 사용할 경우, Trace 위치에 따라 패턴 간격이 압축되고 원래 Splat의 이동 거리가 보존되지 않았다. 따라서 HitPoint는 최종 위치가 아니라 **새 표면 위의 한 점**으로 사용하고, HitNormal과 함께 새로운 평면을 정의한 뒤 원래 `Center → Splat` 이동선과 이 평면의 교점을 계산하여 실제로 꺾이기 시작하는 Corner 위치를 추정했다.

**④ 남은 이동거리를 보존해 새 표면으로 Wrapping**

Corner 위치까지만 이동하고 끝내면 모서리를 넘어가야 했던 Splat의 원래 퍼짐 거리가 사라진다. 따라서 `전체 Offset 거리 - Corner까지 사용한 거리`를 남은 거리로 계산하고, 기존 Normal에서 새로운 Normal로 이동 방향을 회전시킨 뒤 남은 거리만큼 진행하도록 하여 모서리에서도 패턴의 확산 거리를 최대한 유지했다.

**⑤ 실제 메시의 Chamfer를 고려해 Normal Threshold 수정**

초기에는 2차 Trace에서 거의 90도에 가까운 Normal 변화만 급격한 면으로 인정하도록 조건을 설정했다. 하지만 실제 메시의 모서리에는 Chamfer나 라운딩이 적용되어 있어 Normal이 한 번에 90도로 변하지 않고 `Dot = 0.866`, `0.707`처럼 단계적으로 변화했으며, 이 때문에 정상적으로 찾은 인접 표면까지 무효 처리되는 문제가 발생했다. 이후 특정 각도 자체를 찾는 방식이 아니라 **기존 표면과 충분히 다른 Normal인가**를 판단하는 Threshold 방식으로 조건의 의미를 변경했다.

---

### 시도한 과정

#### 1. 예상 Splat 위치에서 Tangent / Bitangent Trace 시도

**시도**

1차 Normal Trace가 실패한 경우, 계산된 Splat 예상 위치를 기준으로 Tangent / Bitangent 방향 Trace를 추가했다.

**실패 원인**

1차 Trace가 실패한 Splat 위치는 이미 모서리를 넘어 표면 밖에 존재할 수 있었다.  
따라서 같은 위치에서 Trace 방향만 변경해도 Line 자체가 인접 면을 통과하지 못하는 경우가 발생했다.

**수정**

2차 Trace가 인접 면을 통과할 수 있도록 예상 Splat 위치가 아닌 **표면 근처의 별도 Probe 시작점**을 사용하도록 변경했다.


#### 2. Probe 시작점을 표면 안쪽으로 이동

**시도**

2차 Trace 시작점을 `Center - Normal * 10cm`로 이동시켜 인접 면을 탐색했다.

**실패 원인**

고정된 10cm는 Blood Radius나 Mesh 두께와 관계없는 값이므로, 얇은 Mesh에서는 Probe가 지나치게 내부로 들어가 다른 표면을 탐색하거나 Trace가 실패할 수 있었다.

`Offset.Size()`를 Probe 깊이로 사용하는 방법도 검토했지만, Offset은 **Splat이 중심에서 얼마나 퍼졌는지**를 의미하는 값이므로 표면 내부 탐색 깊이와 직접적인 관련이 없었다.

**수정**

2차 Trace의 목적은 최종 위치를 계산하는 것이 아니라 **새로운 표면 방향을 얻는 것**이므로, 표면과의 겹침만 피할 수 있도록 Normal 반대 방향으로 최소한의 Bias만 적용했다.

> **Insight**  
> Probe 위치와 Splat의 이동 거리는 서로 다른 목적의 값이므로 분리해서 처리해야 한다.

#### 3. 2차 Trace의 HitPoint를 최종 위치로 사용

**시도**

2차 Trace에서 얻은 `ImpactPoint`를 새로운 표면의 최종 Splat 위치로 사용했다.

**실패 원인**

2차 Trace는 표면 탐색을 위해 시작 위치를 별도로 보정했기 때문에 HitPoint 자체는 원래 Splat의 이동 경로 위에 존재하지 않는다.

따라서 HitPoint를 그대로 사용할 경우 **Splat 간격이 압축되거나 모서리 주변으로 과하게 튀는 현상**이 발생했다.

**수정**

`ImpactPoint + ImpactNormal`은 최종 위치가 아니라 **새로운 표면의 Plane 정보**로만 사용했다.

기존 `Center → Splat` 이동선과 새 Plane의 교점을 계산해 실제로 방향이 꺾이는 `CornerPoint`를 구한 뒤,

전체 이동거리 - Corner까지 사용한 거리만큼만 새로운 표면 방향으로 회전시켜 이동하도록 변경했다.

이를 통해 기존 Splat의 퍼짐 거리와 간격을 최대한 유지한 채 모서리를 따라 Wrapping할 수 있도록 했다.

#### 4. 단순 Hit 여부만으로 1차 Trace를 통과시킨 문제

**문제**

모서리 보정을 추가한 뒤에도 일부 Splat이 다른 위치로 튀는 현상이 남아 있었다.

**원인**

초기에는 1차 Normal Trace에서 Hit만 발생하면 정상적인 표면으로 판단했다.

하지만 Trace 길이가 길어 예상 위치와 관계없는 다른 모서리나 표면까지 맞는 경우가 있었다.

Collision Hit 여부와 알고리즘에서 실제로 사용할 수 있는 Hit 여부는 다르다.

**수정**

1차 Trace 결과에 대해 다음 두 조건을 추가로 확인했다.

예상 Splat 위치와 ImpactPoint 사이의 거리
기존 Normal과 ImpactNormal의 방향 차이

두 조건을 모두 만족하는 경우에만 기존 표면의 연장 또는 완만한 굴곡으로 판단했다.

유효하지 않은 Hit은 1차 Trace 성공으로 처리하지 않고 2차 Trace로 넘기도록 수정했다.

#### 5. 2차 Trace의 Normal 조건을 너무 강하게 설정

**문제**

2차 Trace 자체는 성공했지만 정상적으로 찾은 인접 면이 계속 Invalid 처리되는 문제가 발생했다.

**원인**

초기에는 기존 Normal과 거의 직각에 가까운 표면만 새로운 인접 면으로 인정했다.

하지만 실제 Mesh의 모서리에는 Chamfer / Round가 적용되어 있어 Normal이 한 번에 90도로 변화하지 않고 단계적으로 변화했다.

따라서 인접 면을 정상적으로 탐색했음에도 강한 각도 조건 때문에 유효한 결과가 제거되고 있었다.

**수정**

특정 각도에 가까운 표면을 찾는 방식에서

기존 표면과 충분히 다른 Normal인가?

를 판단하는 Threshold 방식으로 기준을 변경했다.

여러 Threshold를 비교하여 Chamfer 구간에서도 자연스럽게 새로운 표면으로 전환될 수 있도록 조정했다.

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
