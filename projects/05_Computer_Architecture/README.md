## 🎯 05. 컴퓨터 아키텍처 (Computer Architecture)

### 1. 목표

 이전 장들에서 구현한 ALU와 메모리 칩들을 부품으로 활용하여 중앙 처리 장치를 설계하고, 이를 명령어 메모리, 데이터 메모리와 결합하여 완벽하게 작동하는 16비트 컴퓨터, 'Hack'을 완성했다. 이 과정을 통해 컴퓨터가 명령어를 인출하고, 해석하여, 실행하는 기본적인 동작 원리를 이해했다..

---

### 2. CPU 구현

![enter image description here](https://i.sstatic.net/iBTc6.png)

전장에서 아키텍처에 대해 설명을 간단히 했으니 이번장에선 어떻게 구현했는지에 집중한다.

![assembly - What does the 'a' means in Nand2Tetris C-instruction comp? -  Stack Overflow](https://i.sstatic.net/Bwpui.png)

------

### 2. 구현 칩 명세

#### CPU



- **기능**: 16비트 명령어를 해석하고 실행하여 Hack 아키텍처의 모든 연산과 제어 흐름을 총괄한다.

- **명세 코드**:

  코드 스니펫

  ```
  IN  inM[16],         // Memory[A]의 값
      instruction[16], // 실행할 명령어
      reset;           // 리셋 신호
  OUT outM[16],        // Memory[A]에 기록될 값
      writeM,          // 메모리 쓰기 신호
      addressM[15],    // Memory[A]의 주소
      pc[15];          // 다음 명령어의 주소
  ```

- **구현**:

  1. **명령어 해석**: `instruction[15]` 비트를 확인하여 A-명령어와 C-명령어를 구분한다. 이 비트는 CPU 전체 데이터 흐름을 제어하는 핵심 Mux들의 선택 신호로 사용된다.
  2. **A 레지스터**: A-명령어일 경우 `instruction` 값을, C-명령어일 경우 `ALU`의 출력 값을 입력으로 받는다. `load` 신호는 `A-명령어`이거나 `C-명령어의 dest 비트 중 d1이 1`일 때 활성화된다.
  3. **D 레지스터**: `ALU`의 출력을 입력으로 받으며, `C-명령어의 dest 비트 중 d2가 1`일 때만 `load` 신호가 활성화된다.
  4. **ALU**: `x` 입력은 항상 `D` 레지스터의 출력을 받고, `y` 입력은 `instruction[12]`(a비트)에 따라 `A` 레지스터 또는 `inM` 값 중 하나를 Mux를 통해 선택받는다. `comp` 비트(`instruction[6..11]`)는 ALU의 6개 제어 핀에 직접 연결된다.
  5. **PC 제어**: `jump` 비트(`instruction[0..2]`)와 ALU의 상태 플래그(`zr`, `ng`)를 조합한 논리 회로를 통해 PC의 `load` 신호를 생성한다. 점프 시에는 A 레지스터의 값을, 아닐 시에는 `PC+1`의 값을 다음 주소로 결정한다.

------

#### Memory

- **기능**: 컴퓨터의 전체 주소 공간을 관리한다. RAM, 스크린, 키보드 세 장치를 하나의 주소 공간에 매핑하여 CPU가 일관된 방식으로 접근할 수 있게 한다.

- **명세 코드**:

  코드 스니펫

  ```
  IN  in[16],      // 기록할 값
      load,        // 쓰기 신호
      address[15]; // 접근할 주소
  OUT out[16];     // 읽어온 값
  ```

- **구현**:

  1. **주소 디코딩**: `address`의 상위 비트들(`address[14]`, `address[13]`)을 논리 게이트로 조합하여 현재 주소가 RAM, 스크린, 키보드 중 어느 영역에 해당하는지 판별한다.
  2. **쓰기 제어**: 주소 디코딩 결과와 `Memory` 칩의 마스터 `load` 신호를 `AND` 연산하여, 선택된 장치(`RAM16K` 또는 `Screen`)에만 쓰기 신호를 전달한다.
  3. **읽기 제어**: 세 장치(`RAM16K`, `Screen`, `Keyboard`)의 출력을 `Mux` 칩들을 이용해 하나로 합친다. 주소 비트를 선택 신호로 사용하여 현재 주소에 해당하는 장치의 출력값만이 `Memory` 칩의 최종 `out`으로 나가도록 한다.

------

#### Computer

- **기능**: CPU, 명령어 메모리(ROM), 데이터 메모리(Memory) 세 주요 부품을 결합하여 완성된 컴퓨터이다.

- **명세 코드**:

  코드 스니펫

  ```
  IN reset;
  ```

- 구현:

  세 부품을 데이터 흐름에 맞게 연결했다.

  1. CPU의 `pc` 출력을 `ROM32K`의 `address` 입력으로 연결하여 명령어를 인출(Fetch)한다.

  2. `ROM32K`의 출력을 CPU의 `instruction` 입력으로 연결하여 명령어를 전달한다.

  3. CPU의 메모리 관련 출력들(`outM`, `writeM`, `addressM`)을 `Memory` 칩의 해당 입력들(`in`, `load`, `address`)에 연결한다.

  4. Memory 칩의 출력을 CPU의 inM 입력으로 연결하여 메모리 값을 CPU로 전달한다.

     이 배선을 통해 컴퓨터의 **인출-실행 사이클(Fetch-Execute Cycle)**이 완성된다.