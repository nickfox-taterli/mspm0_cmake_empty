#include <stdint.h>
#include <ti/devices/msp/m0p/mspm0g350x.h>
#include <ti/driverlib/dl_gpio.h>
#include <ti/driverlib/driverlib.h>

int main(void)
{
    // 设置欠压复位(BOR)阈值为级别0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    // 设置系统振荡器频率为基频
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    // 禁用高频晶振(HFXT)
    DL_SYSCTL_disableHFXT();
    // 禁用系统PLL
    DL_SYSCTL_disableSYSPLL();
    // 设置ULP时钟分频为1(不分频)
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1);
    // 设置主时钟(MCLK)不分频
    DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE);

    // 重置GPIOB外设
    DL_GPIO_reset(GPIOB);
    // 启用GPIOB电源
    DL_GPIO_enablePower(GPIOB);

    // 短暂延迟，确保GPIO稳定
    delay_cycles(16);

    // 初始化数字输出特性:
    // - 使用IOMUX_PINCM50引脚配置
    // - 禁用输出反转
    // - 下拉电阻
    // - 低驱动强度
    // - 禁用高阻抗模式
    DL_GPIO_initDigitalOutputFeatures(IOMUX_PINCM50,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
        DL_GPIO_DRIVE_STRENGTH_LOW, DL_GPIO_HIZ_DISABLE);

    // 清除GPIOB的22号引脚(初始设为低电平)
    DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_22);
    // 使能GPIOB的22号引脚为输出模式
    DL_GPIO_enableOutput(GPIOB, DL_GPIO_PIN_22);
    // 设置GPIOB的22号引脚为高电平
    DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_22);

    while (1) {
        // 简单的延时循环
        for (volatile uint32_t i = 0; i < 160000; ++i) {
            __asm__ volatile("nop");  // 执行空操作指令，用于精确延时
        }
        // 切换GPIOB的22号引脚状态(高变低或低变高)
        DL_GPIO_togglePins(GPIOB, DL_GPIO_PIN_22);
    }
}