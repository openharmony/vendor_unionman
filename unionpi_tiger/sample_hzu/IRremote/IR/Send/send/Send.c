#include "Send.h"


uint16_t onTimePeriod;
uint16_t offTimePeriod;

uint32_t getTimeInMicros() {
    struct timeval tv;
    gettimeofday(&tv, NULL);  // 获取当前时间
    return (uint32_t)(tv.tv_sec * 1000000 + tv.tv_usec);  // 返回微秒数
}

/// 关闭 IR LED。
void ledOff() {
  set_pwm_enable(pwmChannel,PWM_NOT_ENABLED); // 设置引脚输出为关闭状态（低电平或高电平，取决于是否反转）
}

/// 打开 IR LED。
void ledOn() {
  set_pwm_enable(pwmChannel,PWM_IS_ENABLED);  // 设置引脚输出为开启状态（高电平或低电平，取决于是否反转）
}

/// 调制红外 LED，持续给定的时间周期（微秒），并按照设定的占空比进行调制。
/// @param[in] usec 调制红外 LED 的时间（微秒）。
/// @return 实际发送的脉冲数量。
/// @note
///   ESP8266 并没有很好的硬件 PWM 支持，因此我们需要用软件来实现它。
///   有一个 hack 方法可以使用第二个串口 TX 引脚来实现较为精准的硬件 PWM，
///   但它只适用于特定的 GPIO 引脚，且仅在某些模块上可用，例如 ESP-01 模块上不可用。
///   因此，为了更好的兼容性和选择性，我们没有使用该方法。
///   参考链接：
///   https://www.analysir.com/blog/2017/01/29/updated-esp8266-nodemcu-backdoor-upwm-hack-for-ir-signals/
uint16_t mark(uint16_t usec) {
    uint16_t counter = 0;  // 计数器，用于记录发送的脉冲数

    // 获取当前时间
    uint32_t start_time = getTimeInMicros();  // 获取当前时间（微秒）
    uint32_t elapsed = 0;  // 已消耗的时间，单位：微秒

    while (elapsed < usec) {  // 循环直到已消耗的时间达到或超过要求的时间
        ledOn();  // 打开 LED
        // 计算当前需要打开 LED 的时间
        usleep((usec - elapsed) < onTimePeriod ? (usec - elapsed) : onTimePeriod);  // 微秒延时
        ledOff();  // 关闭 LED
        counter++;  // 增加脉冲计数

        // 更新消耗的时间
        elapsed = getTimeInMicros() - start_time;

        // 如果已经消耗的时间加上本次打开 LED 的时间已经超过了所要求的时间，返回已发送的脉冲数量
        if (elapsed + onTimePeriod >= usec) {
            return counter;  // LED 已关闭并且已超过所请求的时间
        }

        // 等待剩余时间，最小值是剩余 duty cycle 时间，或剩余所需时间
        usleep((usec - elapsed - onTimePeriod) < offTimePeriod ? (usec - elapsed - onTimePeriod) : offTimePeriod);  // 微秒延时
        elapsed = getTimeInMicros() - start_time;  // 更新已消耗的时间
    }

    return counter;  // 返回已发送的脉冲数量
}

/// 将引脚（LED）关闭给定的时间，发送指定时间（微秒）的红外空间信号。
/// 空间是指没有输出，PWM 输出被禁用。
/// @param[in] time 关闭 LED 的时间（微秒）。
void space(uint32_t time) {
    ledOff();  // 关闭 LED
    if (time == 0) return;  // 如果时间为 0，不做任何操作

    usleep(time);  // 延时指定的时间，单位为微秒
}

/// 通用方法，用于发送数据，适用于大多数协议。
/// 如果 `nbits` 大于数据的位数，将发送前导或尾随的 0。
/// @param[in] onemark 发送 '1' 位时 LED 闪烁的微秒数。
/// @param[in] onespace 发送 '1' 位时 LED 关闭的微秒数。
/// @param[in] zeromark 发送 '0' 位时 LED 闪烁的微秒数。
/// @param[in] zerospace 发送 '0' 位时 LED 关闭的微秒数。
/// @param[in] data 要传输的数据。
/// @param[in] nbits 要发送的数据位数。
/// @param[in] MSBfirst 位的传输顺序标志。默认为 MSB->LSB（从最高有效位到最低有效位）顺序。
void sendData(uint16_t onemark, uint32_t onespace, uint16_t zeromark,uint32_t zerospace, uint64_t data, uint16_t nbits,bool MSBfirst)
{
  // 如果要求发送的位数为 0，直接返回
  if (nbits == 0)  
    return;

  if (MSBfirst) {  // 如果是 MSB (最高有效位) 优先顺序发送
    // 如果数据的位数小于需要发送的位数，发送前导 0 直到满足位数要求
    while (nbits > sizeof(data) * 8) {
      mark(zeromark);   // 发送 0 时 LED 闪烁的时间
      space(zerospace); // 发送 0 时 LED 关闭的时间
      nbits--;          // 减少剩余需要发送的位数
    }

    // 从最高有效位 (MSB) 开始逐位发送数据
    for (uint64_t mask = 1ULL << (nbits - 1); mask; mask >>= 1) {
      if (data & mask) {  // 如果当前位是 1
        mark(onemark);     // 发送 '1' 时 LED 闪烁的时间
        space(onespace);   // 发送 '1' 时 LED 关闭的时间
      } else {  // 如果当前位是 0
        mark(zeromark);    // 发送 '0' 时 LED 闪烁的时间
        space(zerospace);  // 发送 '0' 时 LED 关闭的时间
      }
    }

  } else {  // 如果是 LSB (最低有效位) 优先顺序发送
    // 从最低有效位 (LSB) 开始逐位发送数据
    for (uint16_t bit = 0; bit < nbits; bit++, data >>= 1) {
      if (data & 1) {  // 如果当前位是 1
        mark(onemark);     // 发送 '1' 时 LED 闪烁的时间
        space(onespace);   // 发送 '1' 时 LED 关闭的时间
      } else {  // 如果当前位是 0
        mark(zeromark);    // 发送 '0' 时 LED 闪烁的时间
        space(zerospace);  // 发送 '0' 时 LED 关闭的时间
      }
    }
  }
}