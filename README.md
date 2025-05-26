# CPU 스케줄링 시뮬레이터

운영체제의 CPU 스케줄링 알고리즘을 시뮬레이션하는 프로그램입니다.

## 📚 필수 개념 키워드

### 1. 프로세스 관리
- **프로세스 상태 (Process State)**
  - New: 프로세스가 생성된 상태
  - Ready: CPU 할당을 기다리는 상태
  - Running: CPU를 할당받아 실행 중인 상태
  - Waiting: I/O 작업 등을 기다리는 상태
  - Terminated: 프로세스가 종료된 상태

- **프로세스 특성**
  - CPU Burst Time: CPU 사용 시간
  - I/O Burst Time: I/O 작업 시간
  - Arrival Time: 프로세스가 시스템에 도착한 시간
  - Priority: 프로세스의 우선순위

### 2. CPU 스케줄링 알고리즘
- **FCFS (First Come First Served)**
  - 비선점형 알고리즘
  - 먼저 도착한 프로세스가 먼저 처리됨
  - Convoy Effect 발생 가능

- **SJF (Shortest Job First)**
  - 비선점형/선점형 버전 존재
  - CPU 버스트 시간이 가장 짧은 프로세스 우선 처리
  - Starvation 문제 발생 가능

- **Priority Scheduling**
  - 비선점형/선점형 버전 존재
  - 우선순위가 높은 프로세스 우선 처리
  - Aging 기법으로 Starvation 해결 가능

- **Round Robin**
  - 선점형 알고리즘
  - Time Quantum 기반의 시간 할당
  - 공정한 CPU 시간 분배

### 3. 성능 평가 지표
- **Waiting Time**: 프로세스가 Ready Queue에서 대기한 시간
- **Turnaround Time**: 프로세스 생성부터 종료까지의 총 시간
- **Response Time**: 프로세스가 처음 CPU를 할당받기까지의 시간
- **CPU Utilization**: CPU 사용률
- **Throughput**: 단위 시간당 처리된 프로세스 수

## 🛠️ 프로젝트 구조

```
.
├── include/
│   ├── process.h     # 프로세스 관련 구조체 및 함수 선언
│   ├── scheduler.h   # 스케줄러 관련 구조체 및 함수 선언
│   └── utils.h       # 유틸리티 함수 선언
├── src/
│   ├── process.c     # 프로세스 관련 함수 구현
│   ├── scheduler.c   # 스케줄러 관련 함수 구현
│   └── utils.c       # 유틸리티 함수 구현
├── Makefile          # 빌드 설정
└── README.md         # 프로젝트 설명
```

## 📋 구현 태스크 목록

### 1. 프로젝트 기본 구조 설정 
- [x] 프로젝트 디렉토리 구조 설계
- [x] Makefile 생성
- [x] 기본 헤더 파일 구조 설계

### 2. 프로세스 관리 모듈 
- [x] Process 구조체 정의
- [x] 프로세스 상태 열거형 정의
- [x] 프로세스 생성/삭제/출력 함수 구현

### 3. 스케줄러 모듈 구현 (진행 중)
- [x] 스케줄러 기본 구조 설계
- [x] 스케줄링 알고리즘 구현
- [ ] 간트 차트 생성 및 출력

### 4. 유틸리티 모듈 구현
- [x] I/O 작업 처리
- [x] 큐 조작 함수

### 5. 메인 프로그램 구현
- [x] 사용자 인터페이스
- [x] 시뮬레이션 실행
- [x] 결과 출력

## 🚀 빌드 및 실행 방법

```bash
# 빌드
make

# 실행
./scheduler

# 정리
make clean
```

## 📊 성능 평가 방법

각 스케줄링 알고리즘별 성능 측정
- 평균 대기 시간
- 평균 반환 시간

## 구현된 스케줄링 알고리즘
1. FCFS (First Come First Served)
2. SJF-NP (Shortest Job First - Non Preemptive)
3. SJF-P (Shortest Job First - Preemptive)
4. Priority-NP (Priority - Non Preemptive)
5. Priority-P (Priority - Preemptive)
6. Round Robin (구현 예정)

## 성능 측정 지표
각 스케줄링 알고리즘의 성능을 다음 지표들을 통해 측정합니다:
- Response Time: 프로세스가 처음 실행되기까지 걸린 시간
- Waiting Time: 프로세스가 대기 큐에서 기다린 총 시간
- Turnaround Time: 프로세스가 완료되기까지 걸린 총 시간 (대기 시간 + 실행 시간)
- Completion Time: 프로세스가 완료된 시점

## 입력 형식
1. 시뮬레이션할 프로세스 개수
2. 사용할 스케줄링 알고리즘 선택
3. 타임 퀀텀 (Round Robin 알고리즘 선택 시)
4. 각 프로세스별 정보:
   - 도착 시간
   - CPU 실행 시간
   - 우선순위

## 출력 형식
- 각 프로세스의 실행 상태 변화
- 현재 시뮬레이션 시간
- 프로세스 종료 메시지
- 성능 측정 결과:
  - 평균 Waiting Time
  - 평균 Turnaround Time


