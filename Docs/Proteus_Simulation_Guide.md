# Proteus仿真完整指南

## 目录
1. [仿真工程创建](#仿真工程创建)
2. [元件放置](#元件放置)
3. [接线连接](#接线连接)
4. [仿真配置](#仿真配置)
5. [运行仿真](#运行仿真)
6. [常见问题](#常见问题)

---

## 仿真工程创建

### 第1步：启动Proteus

```
打开 Proteus Design Suite 8.9+

File → New Project
```

### 第2步：创建新工程

**新工程对话框:**
```
┌─ New Project ──────────────────────────┐
│                                        │
│ Project Name:                          │
│   [SmartCar_STM32____________]         │
│                                        │
│ Project Path:                          │
│   [D:\smartcar\Proteus\______]        │
│                                        │
│ Create Project Template:               │
│   ○ Default Template                   │
│   ● Blank Project                      │
│                                        │
│              [OK]  [Cancel]            │
│                                        │
└────────────────────────────────────────┘
```

### 第3步：进入设计界面

```
Proteus自动打开原理图编辑器 (Schematic Capture)

工具栏:
   [P] - Pick Components (放置元件)
   [W] - Draw Wire (绘制导线)
   [T] - Text (添加文字标签)
```

---

## 元件放置

### 第4步：放置核心芯片 (STM32F103C8)

**打开元件库:**
```
方法1: 点击工具栏上的 "P" 按钮
方法2: 按 "P" 键
方法3: Library → Pick Devices
```

**搜索STM32:**
```
┌─ Pick Devices ──────────────────────────┐
│                                         │
│ Search for component:                   │
│   [STM32F103C8________________]         │
│                                         │
│ Results:                                │
│   ☑ STM32F103C8 (最常用)               │
│   ○ STM32F103C8T6                      │
│                                         │
│ 双击选择                                 │
│                                         │
└─────────────────────────────────────────┘
```

**放置芯片:**
```
1. 鼠标变十字形
2. 在原理图空白处点击
3. 按 Esc 结束
```

**配置STM32参数:**
```
双击放置的STM32芯片

┌─ Component Properties ──────────────────┐
│                                         │
│ Program File: [浏览] SmartCar.hex      │
│                                         │
│ Crystal Frequency: [8] MHz              │
│                                         │
│ ☑ Load Image (必须勾选)                 │
│                                         │
│            [OK]  [Apply]  [Cancel]     │
│                                         │
└─────────────────────────────────────────┘
```

---

### 第5步：放置电机驱动模块 (L298N)

**搜索L298N:**
```
按 "P" → 搜索 "L298N" 或 "L293D"

选择: L298N Dual H-Bridge Motor Driver
```

**放置位置:** STM32右侧

**L298N引脚定义:**
```
         ┌─────────────┐
    GND │1          16│ +5V
    IN1 │2          15│ GND
   OUT1 │3          14│ +12V
   OUT2 │4          13│ ENB
    GND │5          12│ ENA
   OUT3 │6          11│ GND
   OUT4 │7          10│ IN4
    IN3 │8           9│ IN2
         └─────────────┘

连接:
  IN1 → PA0    |  OUT1 → 左电机+
  IN2 → PA1    |  OUT2 → 左电机-
  IN3 → PA8    |  OUT3 → 右电机+
  IN4 → PB15   |  OUT4 → 右电机-
  ENA → PA6    |  ENB → PA7 (PWM速度控制)
```

---

### 第6步：放置液晶屏 (LCD1602)

**搜索LCD:**
```
按 "P" → 搜索 "LCD1602" 或 "LM016L"

选择: LM016L (液晶屏驱动)
```

**LCD1602引脚:**
```
连接:
  D4~D7 → PB8~PB11  (4位数据线)
  RS    → PB12       (寄存器选择)
  RW    → PB13       (读/写选择)
  E     → PB14       (使能信号)
  VSS   → GND        (接地)
  VDD   → +5V        (电源)
```

---

### 第7步：放置循迹传感器 (按钮模拟)

**搜索按钮:**
```
按 "P" → 搜索 "BUTTON" 或 "SWITCH"

选择: SWITCH-PUSHBUTTON
```

**放置5个按钮:**
```
代表5路循迹传感器:
  Button1 → PB0 (左边)
  Button2 → PB1 (左中)
  Button3 → PB2 (中间)
  Button4 → PB3 (右中)
  Button5 → PB4 (右边)
```

**按钮配置:**
```
双击每个按钮设置:
  Initial State: Open (未按下)
  Closed = 检测到黑线
```

---

### 第8步：放置超声波传感器 (HC-SR04)

**搜索超声波:**
```
按 "P" → 搜索 "HC-SR04" 或用按钮模拟

连接:
  TRIG → PB5 (输出,触发)
  ECHO → PB6 (输入,接收)
```

---

### 第9步：放置虚拟终端 (蓝牙模拟)

**搜索虚拟终端:**
```
按 "P" → 搜索 "VIRTUAL TERMINAL"

选择: VIRTUAL_TERMINAL
```

**配置波特率:**
```
双击虚拟终端:
  Port: USART2
  Baud Rate: 9600
  Data Bits: 8
  Stop Bits: 1
```

---

### 第10步：放置电源和GND

```
按 "P" → 搜索 "VDD", "GND"

放置:
  VDD (+3.3V) 在顶部
  GND (0V)   在底部
```

---

## 接线连接

### 第11步：绘制导线

**连线工具:**
```
按 "W" 键进入连线模式
```

**电机控制接线:**
```
STM32           L298N
────────────────────────
PA0 → IN1
PA1 → IN2
PA8 → IN3
PB15 → IN4
PA6 → ENA
PA7 → ENB
GND → GND (多个)
```

**循迹传感器接线:**
```
STM32           Button
───────────────────────────
PB0 → Button1
PB1 → Button2
PB2 → Button3
PB3 → Button4
PB4 → Button5
GND → GND (所有按钮)
```

**超声波接线:**
```
STM32           HC-SR04
──────────────────────────
PB5 → TRIG
PB6 ← ECHO
GND → GND
+5V → VCC
```

**液晶屏接线:**
```
STM32           LCD1602
──────────────────────────
PB8 → D4
PB9 → D5
PB10 → D6
PB11 → D7
PB12 → RS
PB13 → RW
PB14 → E
GND → VSS, K
+5V → VDD, A
```

**虚拟终端接线:**
```
STM32              VIRTUAL TERMINAL
─────────────────────────────────────
PA2 (TX) → RXD
PA3 (RX) ← TXD
GND → GND
```

---

## 仿真配置

### 第12步：设置仿真参数

```
Tools → Simulation Settings

┌─ Simulation Settings ──────────────────┐
│                                        │
│ Processor Frequency: [72] MHz          │
│                                        │
│ Animation Speed: [100] %               │
│                                        │
│ Gate Delay: [5] ns                    │
│                                        │
│ Real Time Simulation: ☑ (勾选)        │
│                                        │
│              [OK]  [Cancel]            │
│                                        │
└────────────────────────────────────────┘
```

### 第13步：配置STM32调试

```
右键STM32 → Edit Properties

设置:
  Program File: SmartCar.hex
  Crystal Frequency: 8 MHz
  ☑ Load Image
```

---

## 运行仿真

### 第14步：启动仿真

```
工具栏: [▶] Play 按钮

或按 Spacebar (空格键)
```

### 第15步：测试功能

#### **测试蓝牙命令**

```
1. 找到 VIRTUAL TERMINAL 窗口
2. 在输入框输入命令:
   F - 前进
   B - 后退
   L - 左转
   R - 右转
   S - 停止
   1 - 低速
   2 - 中速
   3 - 高速
   M - 切换模式
3. 观察电机和LED状态变化
```

#### **测试循迹功能**

```
1. 双击各Button
2. 设置为 "Closed" (检测到黑线)
3. 观察电机动作
4. 观察LCD显示
```

#### **测试超声波**

```
1. 双击 HC-SR04
2. 改变 Distance 参数
3. 观察避障行为
```

---

## 常见问题

### Q1: 仿真无反应

**原因:** hex文件未加载

**解决:**
1. 确保 SmartCar.hex 已编译生成
2. 双击STM32 → Program File 设置正确
3. 勾选 Load Image

### Q2: 虚拟终端无响应

**原因:** 波特率不匹配

**解决:**
1. 检查波特率: 9600
2. 检查数据位: 8
3. 检查连接: PA2(TX), PA3(RX)

### Q3: LED不闪烁

**原因:** PC13未连接LED

**解决:**
1. 添加LED元件
2. 连接 PC13 → LED → GND

---

**祝仿真成功!** 🎉
