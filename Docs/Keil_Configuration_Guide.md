# Keil5工程完整配置指南

## 目录
1. [环境准备](#环境准备)
2. [Keil工程创建](#keil工程创建)
3. [工程配置](#工程配置)
4. [编译设置](#编译设置)
5. [烧录配置](#烧录配置)
6. [故障排查](#故障排查)

---

## 环境准备

### 软件要求
- **Keil MDK-ARM V5.30+** (最新版本推荐V5.38)
- **STM32F1xx DFP** 支持包
- **STM32标准外设库** (SPL)

### 第1步：安装Keil MDK-ARM

```
1. 下载: https://www.keil.com/download/
2. 选择 MDK535.exe (最新版本)
3. 运行安装程序
4. 选择安装路径: C:\Keil_v5\ (推荐)
5. 安装完成后需要激活许可证
```

### 第2步：安装STM32F1xx支持包

**方法A: 通过Pack Installer (推荐)**

```
1. 打开Keil → Tools → Pack Installer

┌─ Pack Installer ────────────────────┐
│                                     │
│ Search: [STM32F1______]  [Search]  │
│                                     │
│ Results:                            │
│ ☑ Keil.STM32F1xx_DFP v2.1.0       │
│   (Device Family Pack)              │
│                                     │
│ 右键选择 → Install                  │
│                                     │
│ 等待下载并安装完成...               │
│                                     │
└─────────────────────────────────────┘

2. 等待安装完成 (约2-5分钟)
3. 安装成功提示: "Installation Complete"
```

**方法B: 离线安装**

```
1. 从 https://www.keil.com/dd2/pack/ 下载:
   Keil.STM32F1xx_DFP.x.x.x.pack

2. 双击.pack文件进行安装

3. Keil会自动识别并安装
```

### 第3步：下载STM32标准库

```
1. 访问: https://www.st.com/en/embedded-software/stsw-stm32054.html

2. 下载: STM32F10x_StdPeriph_Lib_v3.5.0.zip

3. 解压到本地:
   C:\STM32_Libraries\STM32F10x_StdPeriph_Lib_v3.5.0\

文件结构:
├── Libraries/
│   ├── CMSIS/
│   │   ├── CM3/
│   │   │   ├── CoreSupport/
│   │   │   │   ├── core_cm3.c
│   │   │   │   └── core_cm3.h
│   │   │   └── DeviceSupport/
│   │   │       └── ST/STM32F10x/
│   │   │           ├── stm32f10x.h
│   │   │           ├── system_stm32f10x.c
│   │   │           ├── system_stm32f10x.h
│   │   │           └── startup/
│   │   │               └── arm/
│   │   │                   └── startup_stm32f10x_md.s
│   │
│   └── STM32F10x_StdPeriph_Driver/
│       ├── inc/  (所有.h头文件)
│       └── src/  (所有.c源文件)
│
└── Project/
    └── STM32F10x_StdPeriph_Template/
        └── (参考工程)
```

---

## Keil工程创建

### 第4步：创建新工程

```
1. 打开 Keil μVision5

2. File → New μVision Project

┌─ Save As ────────────────────────────┐
│                                      │
│ 文件名: SmartCar                     │
│                                      │
│ 保存位置: C:\smartcar\Keil\         │
│           (创建此文件夹)             │
│                                      │
│ 文件类型: Keil μVision Project       │
│          (*.uvproj)                 │
│                                      │
│         [Save]  [Cancel]            │
│                                      │
└──────────────────────────────────────┘

3. 点击 [Save]
```

### 第5步：选择目标芯片

```
┌─ Select Device ──────────────────────┐
│                                      │
│ Vendor: STMicroelectronics           │
│ Device: STM32F1 Series               │
│ Part Number: STM32F103C8             │
│                                      │
│ 完整路径:
│ STMicroelectronics → STM32F1 Series →
│ STM32F103 Devices → STM32F103C8     │
│                                      │
│ 选中后显示:
│ ┌────────────────────────────────┐  │
│ │ Device Information:            │  │
│ │ STM32F103C8T6 (LQFP48)        │  │
│ │ Flash: 64 KB                   │  │
│ │ RAM: 20 KB                     │  │
│ │ Frequency: 72 MHz              │  │
│ └────────────────────────────────┘  │
│                                      │
│          [OK]  [Cancel]             │
│                                      │
└──────────────────────────────────────┘
```

### 第6步：配置运行时环境 (RTE)

```
┌─ Manage Run-Time Environment ────────┐
│                                      │
│ Device (CMSIS):                      │
│  ☑ CORE (Cortex-M3内核)             │
│                                      │
│ Device (STM32 Startup):              │
│  ☑ Startup (启动代码)               │
│                                      │
│ Compiler:                            │
│  ○ GCC                               │
│  ● ARMCC (推荐,Keil自带)            │
│                                      │
│ 不勾选:                              │
│  ☐ HAL (我们使用标准库,不用HAL)    │
│  ☐ RTOS (暂不使用)                  │
│                                      │
│          [OK]  [Cancel]             │
│                                      │
└──────────────────────────────────────┘
```

---

## 工程配置

### 第7步：配置工程结构

**项目面板布局:**

```
┌─ Project (SmartCar) ──────┐
│ ├─ Target 1               │
│ │  ├─ Source Group 1      │
│ │  │  ├─ main.c          │
│ │  │  ├─ system_stm32...  │
│ │  │  └─ stm32f10x_...   │ (驱动程序)
│ │  ├─ Startup            │
│ │  │  └─ startup_stm...  │ (汇编启动)
│ │  └─ STM32 Device       │
│ │     ├─ core_cm3        │ (CMSIS)
│ └─ Headers               │
│    ├─ stm32f10x_conf.h   │
│    └─ stm32f10x.h        │
└──────────────────────────┘
```

### 第8步：添加源文件

**步骤1: 创建分组**

```
1. 在Project面板中右键 "Target 1" → Add Group

2. 新建以下分组:
   ✓ Startup
   ✓ CMSIS
   ✓ StdPeriph
   ✓ User
```

**步骤2: 添加启动文件**

```
分组: Startup
文件: startup_stm32f10x_md.s
路径: C:\STM32_Libraries\...\startup_stm32f10x_md.s

右键 Startup → Add Files to Group 'Startup'
选择 startup_stm32f10x_md.s
```

**步骤3: 添加CMSIS文件**

```
分组: CMSIS
文件:
  1. system_stm32f10x.c
     来源: C:\STM32_Libraries\...\system_stm32f10x.c
  
  2. core_cm3.c (可选)
     来源: C:\STM32_Libraries\...\core_cm3.c
```

**步骤4: 添加标准库驱动**

```
分组: StdPeriph
文件 (从 STM32F10x_StdPeriph_Driver/src/ ):
  ☑ stm32f10x_adc.c
  ☑ stm32f10x_bkp.c
  ☑ stm32f10x_can.c
  ☑ stm32f10x_crc.c
  ☑ stm32f10x_dac.c
  ☑ stm32f10x_dbgmcu.c
  ☑ stm32f10x_dma.c
  ☑ stm32f10x_exti.c
  ☑ stm32f10x_flash.c
  ☑ stm32f10x_fsmc.c
  ☑ stm32f10x_gpio.c        (必需)
  ☑ stm32f10x_i2c.c
  ☑ stm32f10x_iwdg.c
  ☑ stm32f10x_pwr.c
  ☑ stm32f10x_rcc.c         (必需)
  ☑ stm32f10x_rtc.c
  ☑ stm32f10x_sdio.c
  ☑ stm32f10x_spi.c
  ☑ stm32f10x_tim.c         (必需)
  ☑ stm32f10x_usart.c       (必需)
  ☑ stm32f10x_wwdg.c
  ☑ misc.c                  (必需)
```

**步骤5: 添加用户文件**

```
分组: User
文件:
  1. main.c (项目根目录)
  2. stm32f10x_conf.h (放在同目录)
```

### 第9步：配置包含路径

```
1. 右键 Target 1 → Options for Target 'Target 1'

2. 选择 "C/C++" 选项卡

3. 在 "Include Paths" 中添加:
   (使用分号分隔多个路径)

   C:\STM32_Libraries\STM32F10x_StdPeriph_Lib_v3.5.0\Libraries\CMSIS\CM3\CoreSupport;
   C:\STM32_Libraries\STM32F10x_StdPeriph_Lib_v3.5.0\Libraries\CMSIS\CM3\DeviceSupport\ST\STM32F10x;
   C:\STM32_Libraries\STM32F10x_StdPeriph_Lib_v3.5.0\Libraries\STM32F10x_StdPeriph_Driver\inc;
   .\;
   
   简化写法 (假设标准库路径为 C:\STM32F10x_Lib):
   C:\STM32F10x_Lib\Libraries\CMSIS\CM3\CoreSupport;
   C:\STM32F10x_Lib\Libraries\CMSIS\CM3\DeviceSupport\ST\STM32F10x;
   C:\STM32F10x_Lib\Libraries\STM32F10x_StdPeriph_Driver\inc;
   .\;
```

---

## 编译设置

### 第10步：配置编译器

```
右键 Target 1 → Options for Target 'Target 1'

选择 "C/C++" 选项卡
```

**编译器选项:**

```
┌─ C/C++ Tab ──────────────────────────────┐
│                                          │
│ Optimization:                            │
│   ○ Level 0 (关闭优化)                   │
│   ○ Level 1 (-O1)                       │
│   ● Level 2 (-O2, 推荐)                 │
│   ○ Level 3 (-O3)                       │
│                                          │
│ Warnings:                                │
│   ○ No Warnings                          │
│   ● Warnings (推荐)                     │
│   ○ Errors                               │
│                                          │
│ Define:                                  │
│   STM32F10X_MD, USE_STDPERIPH_DRIVER    │
│                                          │
│ Include Paths:                           │
│   (按上面的配置)                         │
│                                          │
│ Misc Controls:                           │
│   (留空)                                 │
│                                          │
│ C99 Mode:                                │
│   ☑ (勾选)                               │
│                                          │
└──────────────────────────────────────────┘
```

**宏定义 (Define) 详解:**

```
定义: STM32F10X_MD, USE_STDPERIPH_DRIVER

解释:
  STM32F10X_MD     - 选择中等密度型STM32F10x
                     (MD = Medium Density)
                     其他选项: LD, HD, LD_VL, MD_VL, HD_VL
  
  USE_STDPERIPH_DRIVER - 使用标准外设库
                         (不使用HAL库)
```

### 第11步：配置链接器

```
右键 Target 1 → Options for Target 'Target 1'

选择 "Linker" 选项卡
```

**链接器选项:**

```
┌─ Linker Tab ──────────────────────────────┐
│                                           │
│ Scatter File:                             │
│   (留空 - 使用Keil默认)                   │
│                                           │
│ Memory Layout:                            │
│   ☑ Use Default  (推荐)                   │
│                                           │
│ Memory Pools:                             │
│   Heap Size:  [0x1000]  (4KB)            │
│   Stack Size: [0x400]   (1KB)            │
│                                           │
│ Misc Controls:                            │
│   (留空)                                  │
│                                           │
└───────────────────────────────────────────┘
```

**STM32F103C8内存分配:**

```
总体:         64 KB Flash + 20 KB RAM

Flash分配:
  - 0x0000_0000 - 0x0000_FFFF: 64 KB (程序代码)

RAM分配:
  - 0x2000_0000 - 0x2000_4FFF: 20 KB (全部)
    其中:
    - 全局变量、静态变量
    - 堆(Heap): 4 KB
    - 栈(Stack): 1 KB
```

### 第12步：配置启动选项

```
右键 Target 1 → Options for Target 'Target 1'

选择 "Target" 选项卡
```

**Target选项:**

```
┌─ Target Tab ──────────────────────────────┐
│                                           │
│ System Clock:                             │
│   External Clock: [8.0] MHz  (重要!)     │
│                                           │
│ Memory Initialization:                    │
│   ☑ Run from Flash                       │
│                                           │
│ Library Configuration:                    │
│   ☑ Use MicroLIB  (勾选 - 支持printf)   │
│                                           │
│ RO Base: 0x0000_0000 (代码起始地址)     │
│ RW Base: 0x2000_0000 (数据起始地址)     │
│                                           │
│ RTX Kernel:                               │
│   (不使用)                                │
│                                           │
└───────────────────────────────────────────┘
```

### 第13步：配置输出文件

```
右键 Target 1 → Options for Target 'Target 1'

选择 "Output" 选项卡
```

**Output选项:**

```
┌─ Output Tab ──────────────────────────────┐
│                                           │
│ Output File Name:                         │
│   SmartCar (自动生成.axf和.hex)          │
│                                           │
│ Print Execution Profile:                  │
│   ☐ (不勾选)                              │
│                                           │
│ Create HEX File:                          │
│   ☑ (勾选 - 生成.hex烧录文件)            │
│                                           │
│ Create Binary File:                       │
│   ☐ (可选)                                │
│                                           │
│ Bitwise AND with Mask:                    │
│   (留空)                                  │
│                                           │
└───────────────────────────────────────────┘
```

---

## 烧录配置

### 第14步：配置调试器

```
右键 Target 1 → Options for Target 'Target 1'

选择 "Debug" 选项卡
```

**Debug选项:**

```
┌─ Debug Tab ───────────────────────────────┐
│                                           │
│ Use Simulator:                            │
│   ○ (不选 - 用真实硬件烧录)              │
│                                           │
│ Use Device Driver:                        │
│   ● ST-Link Debugger (推荐)              │
│     或                                    │
│   ● J-Link Debugger                      │
│                                           │
│ Port Selection:                           │
│   SWD (推荐)                              │
│   (ST-Link使用SWD接口)                   │
│                                           │
│ Reset Strategy:                           │
│   Autodetect (推荐)                      │
│                                           │
│ [Settings...] 按钮:                      │
│   设置调试器具体参数                      │
│                                           │
└───────────────────────────────────────────┘
```

**ST-Link调试器设置:**

```
点击 [Settings...]:

┌─ ST-Link Debugger ────────────────────────┐
│                                           │
│ Port Selection:                           │
│   SWD                                     │
│                                           │
│ Device Name:                              │
│   (自动识别)                               │
│                                           │
│ Reset:                                    │
│   ○ Hardware Reset (推荐)                │
│   ○ Software Reset                       │
│   ○ Software SYSRESETREQ                 │
│                                           │
│ Frequency:                                │
│   [4 MHz] (默认)                          │
│                                           │
│ Other Settings:                           │
│   ☑ Halt after Download                  │
│   ☑ Verify Code Download                 │
│                                           │
└───────────────────────────────────────────┘
```

### 第15步：配置下载方式

```
右键 Target 1 → Options for Target 'Target 1'

选择 "Utilities" 选项卡
```

**Utilities (烧录):**

```
┌─ Utilities Tab ───────────────────────────┐
│                                           │
│ Use Debugger:                             │
│   ● ST-Link Debugger                     │
│                                           │
│ Download Function Calls:                  │
│   ☑ (勾选自动下载)                       │
│                                           │
│ Erase Sectors:                            │
│   ◉ Erase Full Chip                      │
│                                           │
│ Programming Algorithm:                    │
│   STM32F10x_128.FLM (自动选择)          │
│                                           │
│ Verify:                                   │
│   ☑ Verify Code Download                 │
│                                           │
└───────────────────────────────────────────┘
```

---

## 故障排查

### 常见编译错误

#### 错误1: "undefined reference to `SystemInit'"

**原因:** 缺少 system_stm32f10x.c

**解决:**
```
1. 在工程中添加 system_stm32f10x.c
   File → Open → 选择 system_stm32f10x.c
   
2. 右键添加到 CMSIS 分组
   
3. 重新编译
```

#### 错误2: "undefined reference to `Reset_Handler'"

**原因:** 缺少启动文件

**解决:**
```
1. 添加 startup_stm32f10x_md.s 文件
   
2. 确保在 Startup 分组中
   
3. 右键 → Compile
```

#### 错误3: "stm32f10x.h: No such file or directory"

**原因:** Include路径不正确

**解决:**
```
1. 检查 Options for Target → C/C++ → Include Paths
   
2. 确保包含:
   - CMSIS\CM3\DeviceSupport\ST\STM32F10x
   
3. 使用绝对路径不要相对路径
```

#### 错误4: "undefined reference to `stm32f10x_gpio_init'"

**原因:** 缺少驱动文件

**解决:**
```
1. 添加 stm32f10x_gpio.c 到 StdPeriph 分组
   
2. 添加其他需要的驱动文件
   
3. 重新编译
```

### 编译成功后

**生成文件:**
```
编译成功后,在 Objects/ 目录中生成:

✓ SmartCar.axf      (可执行文件)
✓ SmartCar.hex      (烧录文件 - 用于ST-Link)
✓ SmartCar.bin      (二进制)
✓ SmartCar.map      (内存映射)

烧录时使用: SmartCar.hex
```

### 烧录前检查

```
烧录检查清单:

□ ST-Link驱动已安装 (Windows)
□ STM32与ST-Link连接:
  - SWDIO (PA13)
  - SWCLK (PA14)
  - GND
  - 3.3V (可选)
  
□ STM32电源正常 (3.3V)
□ BOOT0 = 0 (正常模式)
□ 复位线(NRST)连接 (可选)
□ Keil中配置正确 (调试器, 算法)
```

### 快速编译和烧录流程

```
1. 编译:
   按 F7 或 Build → Build Target
   
   预期输出:
   ...
   Build completed.
   "...\SmartCar.axf" - 0 Error(s), 0 Warning(s).

2. 下载:
   按 F8 或 Debug → Start/Stop Debug Session
   
   或直接按工具栏的 [Download] 按钮
   
   预期输出:
   Connected to ST-Link
   Erasing flash...
   Programming flash...
   Download completed.

3. 运行:
   按 Ctrl+F5 或 Debug → Run
   程序开始执行
```

---

## 完整配置总结表

| 设置项 | 值 | 说明 |
|--------|-----|------|
| 芯片 | STM32F103C8 | 中等密度型 |
| 外晶振 | 8.0 MHz | 必须与硬件匹配 |
| 系统时钟 | 72 MHz | PLL倍频 |
| 优化级别 | -O2 | 平衡速度和大小 |
| 宏定义 | STM32F10X_MD, USE_STDPERIPH_DRIVER | 标准库 |
| MicroLIB | 启用 | 支持printf |
| 调试器 | ST-Link | SWD接口 |
| 输出格式 | HEX | 烧录用 |
| 堆大小 | 4KB | 动态内存 |
| 栈大小 | 1KB | 函数调用栈 |

---

**配置完成！现在可以编译和烧录程序了。** 🎉
