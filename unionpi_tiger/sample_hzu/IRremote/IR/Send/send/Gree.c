#include "Gree.h"


uint64_t transform(const struct INFO *info) {
    uint64_t data = 0;

    // Byte 0
    data |= (uint64_t)info->Mode;
    data |= (uint64_t)info->Power << 3;
    data |= (uint64_t)info->Fan << 4;
    data |= (uint64_t)info->SwingAuto << 6;
    data |= (uint64_t)info->Sleep << 7;

    // Byte 1
    data |= (uint64_t)info->Temp << 8;
    data |= (uint64_t)info->TimerHalfHr << 12;
    data |= (uint64_t)info->TimerTensHr << 13;
    data |= (uint64_t)info->TimerEnabled << 15;

    // Byte 2
    data |= (uint64_t)info->TimerHours << 16;
    data |= (uint64_t)info->Turbo << 20;
    data |= (uint64_t)info->Light << 21;
    data |= (uint64_t)info->ModelA << 22;
    data |= (uint64_t)info->Xfan << 23;

    // Byte 3
    data |= (uint64_t)info->TempExtraDegreeF << 24;
    data |= (uint64_t)info->UseFahrenheit << 25;
    data |= (uint64_t)info->unknown1 << 26;

    // Byte 4
    data |= (uint64_t)info->SwingV << 32;
    data |= (uint64_t)info->SwingH << 36;

    // Byte 5
    data |= (uint64_t)info->DisplayTemp << 40;
    data |= (uint64_t)info->IFeel << 42;
    data |= (uint64_t)info->unknown2 << 43;
    data |= (uint64_t)info->WiFi << 46;

    // Byte 6 is not used, so we skip it

    // Byte 7
    data |= (uint64_t)info->Econo << 56;
    data |= (uint64_t)info->Sum << 60;

    return data;
}


//用到Send里面的sendData、mark、space
void sendGree(const uint64_t data, const uint16_t nbits,const uint16_t repeat)
{
    if (nbits != kGreeBits)
        return;  // 位数不匹配，无法发送正确的消息

    // 根据重复次数 repeat 进行发送
  for (uint16_t r = 0; r <= repeat; r++) {
    // 发送消息头部
    mark(kGreeHdrMark);  // 发送头部的 Mark 信号
    space(kGreeHdrSpace);  // 发送头部的 Space 信号

    // 发送数据部分
    for (int16_t i = 8; i <= nbits; i += 8) {
      // 发送 8 位数据，按照一定的时序标记和空间信号发送每个字节
      sendData(kGreeBitMark, kGreeOneSpace, kGreeBitMark, kGreeZeroSpace,
               (data >> (nbits - i)) & 0xFF, 8, 0);  // 发送数据中的当前字节

      // 如果已经发送了半个消息的数据（即到达 nbits 的一半），则发送中间的 Footer
      if (i == nbits / 2) {
        // 发送中间的 Footer（例如用于分隔数据的控制信号）
        sendData(kGreeBitMark, kGreeOneSpace, kGreeBitMark, kGreeZeroSpace,
                 kGreeBlockFooter, kGreeBlockFooterBits , 0);  // 发送 3 位的 Footer（0b010）
        mark(kGreeBitMark);  // 发送一个标记信号
        space(kGreeMsgSpace);  // 发送消息的间隔信号
      }
    }

    // 发送消息的尾部
    mark(kGreeBitMark);  // 发送尾部的 Mark 信号
    space(kGreeMsgSpace);  // 发送尾部的 Space 信号
  }
}



