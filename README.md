# 遥控智能小车 - STM32版

**Smart Car with Bluetooth Control, Line Tracing & Obstacle Avoidance**

## 📋 项目概述

这是一个基于 **STM32F103C8T6** 微控制器的遥控智能小车完整解决方案，包含三种工作模式：

1. **蓝牙遥控模式** - 通过HC-05蓝牙模块接收手机命令
2. **自动循迹模式** - 使用TCRT5000五路红外传感器自动跟随黑线
3. **自动避障模式** - 使用HC-SR04超声波传感器自动避开障碍物

## 🛠 硬件配置

### 核心芯片
- **MCU**: STM32F103C8T6 (ARM Cortex-M3, 72MHz)
- **晶振**: 8MHz外部HSE
- **闪存**: 64KB
- **内存**: 20KB SRAM

### 外设模块
| 模块 | 芯片 | 连接端口 | 用途 |
|------|------|---------|------|
| 电机驱动 | L298N | PA0/PA1/PA8/PB15, PA6/PA7(PWM) | 双直流电机驱动 |
| 蓝牙通信 | HC-05 | USART2 (PA2-TX, PA3-RX) | 无线遥控 |
| 循迹传感器 | TCRT5000×5 | PB0~PB4 | 黑线识别 |
| 超声波测距 | HC-SR04 | PB5(TRIG), PB6(ECHO) | 距离测量 |
| 液晶显示 | LCD1602 | PB8~PB14 | 状态显示 |
| 状态指示灯 | LED | PC13 | 运行状态 |

### 电源配置
- 主电源: 7.4V锂电池
- 逻辑电源: AMS1117-3.3V稳压至3.3V
- 电机驱动电源: 直接来自电池(支持L298N)

## 📁 文件结构

```
smartcar/
├── main.c                     # 主程序 (15.7 KB)
├── stm32f10x_conf.h          # STM32配置文件
├── startup_stm32f10x_md.s    # 启动文件(汇编)
├── README.md                 # 本文件
├── Keil/
│   ├── SmartCar.uvprojx      # Keil5工程文件
│   └── SmartCar.uvoptx       # Keil5配置文件
└── Docs/
    └── 03-Proteus仿真教程.md  # 详细教程
```

## 🚀 快速开始

### 1. 编译环境设置

**所需软件:**
- Keil MDK-ARM V5.30+
- STM32F1xx DFP支持包
- STM32标准外设库(SPL)

**安装步骤:**
```bash
# 1. 下载STM32标准库
# https://www.st.com/en/embedded-software/stsw-stm32054.html

# 2. Keil中安装STM32F1xx支持包
# Pack Installer → STM32F1 → Install

# 3. 配置工程Include路径
# 添加标准库 inc/ 和 CMSIS/ 目录
```

### 2. Keil工程配置

```
Target Settings:
├── Device: STMicroelectronics > STM32F103C8
├── Crystal: 8.0 MHz
├── C/C++ Defines: STM32F10X_MD, USE_STDPERIPH_DRIVER
├── Include Paths: <库路径>/inc, <库路径>/CMSIS
├── MicroLIB: Enable (支持printf)
└── Output: Create HEX File
```

### 3. 编译和烧录

```bash
# 编译
F7 (Keil快捷键)

# 烧录方式1: ST-Link (推荐)
Download → ST-Link Debugger

# 烧录方式2: 串口ISP
BOOT0 = 1 → 复位 → 使用STM32 Flash Programmer
```

## 📱 蓝牙控制命令

### 基础控制
| 命令 | 功能 | 备注 |
|------|------|------|
| **F** | 前进 | Forward |
| **B** | 后退 | Backward |
| **L** | 左转 | Left Turn (左电机停止) |
| **R** | 右转 | Right Turn (右电机停止) |
| **S** | 停止 | Stop |

### 速度控制
| 命令 | 速度 | PWM占空比 |
|------|------|----------|
| **1** | 低速 | 25% |
| **2** | 中速 | 50% |
| **3** | 高速 | 95% |

### 模式切换
| 命令 | 模式 | 描述 |
|------|------|------|
| **M** | 循环切换 | 蓝牙模式 → 循迹模式 → 避障模式 → 蓝牙模式 |

**使用方法:**
- Android/iOS蓝牙串口助手发送单个字符
- 例如: 发送 "F" 前进, "2" 中速, "M" 切换模式

## 🧠 工作原理

### 蓝牙模式流程
```
USART2接收蓝牙数据 
    ↓
USART_IRQHandler中断处理
    ↓
Bluetooth_Control解析命令
    ↓
Motor_Control执行电机控制
    ↓
LCD_DisplayInfo更新显示
```

### 循迹模式流程
```
读取5路传感器(PB0-PB4)
    ↓
判断黑线位置
    ├→ 中间 → 前进
    ├→ 偏左 → 左转
    ├→ 偏右 → 右转
    └→ 全无 → 前进
    ↓
Motor_Control执行
    ↓
刷新LCD显示
```

### 避障模式流程
```
触发HC-SR04(PB5)
    ↓
等待ECHO信号(PB6)
    ↓
计算距离 = 时间/58
    ↓
├→ 距离 > 20cm → 前进
└→ 距离 ≤ 20cm → 后退 → 转向
    ↓
刷新显示
```

## 🔧 主要函数说明

### 初始化函数
| 函数 | 功能 |
|------|------|
| `RCC_Configuration()` | 配置72MHz系统时钟 |
| `GPIO_Configuration()` | 配置所有GPIO引脚 |
| `TIM_Configuration()` | 配置TIM3为10kHz PWM |
| `USART_Configuration()` | 配置9600bps USART2 |
| `NVIC_Configuration()` | 配置中断优先级 |

### 控制函数
| 函数 | 功能 | 参数 |
|------|------|------|
| `Motor_Control()` | 电机控制 | direction(0-4), speed(0-3) |
| `Sonic_Trigger()` | 触发超声波 | 无 |
| `Sonic_GetDistance()` | 获取距离 | 返回cm |
| `Bluetooth_Control()` | 蓝牙命令处理 | cmd(字符) |
| `LCD_DisplayInfo()` | 液晶显示更新 | 无 |

### 工具函数
| 函数 | 功能 |
|------|------|
| `Delay_ms()` | 毫秒延时 |
| `Delay_us()` | 微秒延时 |
| `USART_SendChar()` | 发送单字符 |
| `USART_SendString()` | 发送字符串 |

## 🐛 常见问题排查

### 问题1: 编译报错 "undefined reference to `SystemInit`"
**原因**: 缺少system_stm32f10x.c或CMSIS配置

**解决**:
```
1. 从标准库复制 Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c
2. 添加到Keil工程的CMSIS分组
3. 重新编译
```

### 问题2: 电机不转
**检查清单**:
- [ ] Keil编译成功，.hex文件已生成
- [ ] STM32已正确烧录(LED闪烁确认程序运行)
- [ ] 电池电压 > 6V
- [ ] GPIO是否输出(用万用表量引脚电压)
- [ ] PWM频率是否正确(示波器看PA6/PA7)
- [ ] L298N使能脚(ENA/ENB)是否有PWM

### 问题3: 蓝牙无法接收命令
**检查清单**:
- [ ] HC-05已配对(手机蓝牙列表中显示)
- [ ] USART2波特率是否为9600
- [ ] PA2(TX)和PA3(RX)接线是否正确
- [ ] 用串口监测工具验证USART是否工作

### 问题4: 循迹传感器无反应
**检查清单**:
- [ ] TCRT5000 5路传感器连接(PB0-PB4, GND, 5V)
- [ ] 黑线宽度 15-20mm, 对比度高(黑色纸+白背景)
- [ ] 传感器距地面 5-8mm
- [ ] 用LED指示灯测试是否有输出

### 问题5: 液晶显示乱码
**原因**: 初始化时序不正确或晶振频率不匹配

**解决**:
1. 确认Keil配置中 Xtal = 8.0 MHz
2. 调整 `Delay_ms()` 中的延时倍数
3. 检查LCD连接是否松动

## 📊 性能指标

| 指标 | 数值 |
|------|------|
| 系统时钟 | 72 MHz |
| PWM频率 | 10 kHz |
| USART波特率 | 9600 bps |
| 电机最大速度 | ~50 cm/s (95%PWM) |
| 超声波测距范围 | 2cm - 400cm |
| LCD刷新频率 | 20 Hz |

## 🔌 接线图

```
┌─────────────────────────────────────┐
│      STM32F103C8T6 (蓝色药丸板)       │
│                                     │
│ PA0 ─────────→ L298N IN1            │
│ PA1 ─────────→ L298N IN2            │
│ PA8 ─────────→ L298N IN3            │
│ PB15 ────────→ L298N IN4            │
│ PA6(PWM) ────→ L298N ENA            │
│ PA7(PWM) ────→ L298N ENB            │
│                                     │
│ PB0-PB4 ──────→ TCRT5000 ×5 (传感器) │
│ PB5 ──────────→ HC-SR04 TRIG        │
│ PB6 ◀─────────── HC-SR04 ECHO       │
│                                     │
│ PA2(TX) ──────→ HC-05 RXD           │
│ PA3(RX) ◀────── HC-05 TXD           │
│                                     │
│ PB8-PB14 ─────→ LCD1602 (D4-D7等)   │
│ PC13 ─────────→ LED (状态指示)      │
│ GND ──────────→ 公共地              │
│ +3.3V ────────→ VDD                 │
└─────────────────────────────────────┘
```

## 📚 参考资源

- **STM32F103C8数据手册**: [ST官网](https://www.st.com/)
- **标准库文档**: `STM32F10x_StdPeriph_Driver/html/index.html`
- **Proteus仿真教程**: 见 `Docs/03-Proteus仿真教程.md`

## 📝 许可证

开源项目，可自由使用和修改。

## 👨‍💻 作者

Smart Car Development Team

---

**最后更新**: 2026年6月11日

**支持的硬件版本**: STM32F103C8T6 (Medium Density)

**编译工具**: Keil MDK-ARM v5.30+

如有问题，欢迎提Issue！