#ifndef GREE_H
#define GREE_H

#include "Send.h"

// 空调状态结构体
struct INFO {
    uint8_t Mode      :3;
    uint8_t Power     :1;
    uint8_t Fan       :2;
    uint8_t SwingAuto :1;
    uint8_t Sleep     :1;
    uint8_t Temp      :4;
    uint8_t TimerHalfHr :1;
    uint8_t TimerTensHr :2;
    uint8_t TimerEnabled:1;
    uint8_t TimerHours :4;
    uint8_t Turbo     :1;
    uint8_t Light     :1;
    uint8_t ModelA    :1;
    uint8_t Xfan      :1;
    uint8_t TempExtraDegreeF :1;
    uint8_t UseFahrenheit   :1;
    uint8_t unknown1        :4;
    uint8_t SwingV      :4;
    uint8_t SwingH      :3;
    uint8_t DisplayTemp :2;
    uint8_t IFeel       :1;
    uint8_t unknown2    :3;
    uint8_t WiFi        :1;
    uint8_t Econo       :1;
    uint8_t Sum         :4;
};

// 格力协议常量声明
extern const uint8_t kGreeMinTempC;           ///< 最低温度（摄氏度）
extern const uint8_t kGreeMaxTempC;           ///< 最高温度（摄氏度）

extern const uint16_t kGreeStateLength;       ///< 状态长度
extern const uint16_t kGreeBits;              ///< 位数
extern const uint16_t kGreeDefaultRepeat;     ///< 默认重复次数
extern const uint16_t kGreeHdrMark;           ///< Header Mark
extern const uint16_t kGreeHdrSpace;          ///< Header Space
extern const uint16_t kGreeBitMark;           ///< Bit Mark
extern const uint16_t kGreeOneSpace;          ///< 1 位时长
extern const uint16_t kGreeZeroSpace;         ///< 0 位时长
extern const uint16_t kGreeMsgSpace;          ///< 消息间隔
extern const uint8_t kGreeBlockFooter;        ///< 块的尾部
extern const uint8_t kGreeBlockFooterBits;    ///< 块尾部的位数

// 函数声明
uint64_t transform(const struct INFO *info);
void sendGree(const uint64_t data, const uint16_t nbits, const uint16_t repeat);

#endif  // GREE_H
