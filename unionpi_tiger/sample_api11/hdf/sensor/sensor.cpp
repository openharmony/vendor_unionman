/* Copyright 2023 Unionman Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "hdf_base.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "sensor_if.h"
#include "sensor_type.h"
#include <cstdio>
#include <unistd.h>

/* 创建回调函数 */
static int32_t SensorDataCallback(const struct SensorEvents *event) {
  if (event == nullptr) {
    return HDF_FAILURE;
  }

  float sensorData = *((float *)event->data);
  printf("sensor id [%d], data : %.2f\n", event->sensorId, sensorData);

  return HDF_SUCCESS;
}

static void SensorEntry(void) {
  int ret;
  struct SensorInformation *sensorInfo = nullptr;
  int32_t count = 0;
  int32_t sensorInterval = 200000000; /* 数据采样率设置200毫秒，单位纳秒 */
  int32_t reportInterval = 400000000; /* 数据上报间隔设置400毫秒，单位纳秒 */
  int32_t sleepTime = 1000000; /* 1秒，单位毫秒 */

  /* 1.创建传感器接口实例 */
  const struct SensorInterface *sensorDev = NewSensorInterfaceInstance();
  if (sensorDev == nullptr) {
    printf("NewSensorInterfaceInstance fail!\n");
    return;
  }

  printf("NewSensorInterfaceInstance success\n");

  /* 2.订阅者注册传感器数据回调处理函数 */
  ret = sensorDev->Register(TRADITIONAL_SENSOR_TYPE, SensorDataCallback);
  if (ret != HDF_SUCCESS) {
    return;
  }

  printf("Register success\n");

  /* 3.获取设备支持的Sensor列表 */
  ret = sensorDev->GetAllSensors(&sensorInfo, &count);
  if (ret != HDF_SUCCESS) {
    return;
  }

  printf("GetAllSensors count: %d\n", count);

  for (int i = 0; i < count; i++) {
    printf("sensor [%d] : sensorName:%s, vendorName:%s, sensorTypeId:%d, "
           "sensorId:%d\n",
           i, sensorInfo[i].sensorName, sensorInfo[i].vendorName,
           sensorInfo[i].sensorTypeId, sensorInfo[i].sensorId);
  }

  for (int i = 0; i < count; i++) {
    /* 4.设置传感器采样率 */
    ret = sensorDev->SetBatch(sensorInfo[i].sensorId, sensorInterval,
                              reportInterval);
    if (ret != HDF_SUCCESS) {
      printf("SetBatch failed,ret: %d\n", ret);
      continue;
    }

    printf("SetBatch success\n");

    /* 5.使能传感器 */
    ret = sensorDev->Enable(sensorInfo[i].sensorId);
    if (ret != HDF_SUCCESS) {
      continue;
    }

    printf("Enable success\n");

    /* 等待1秒钟数据回调 */
    usleep(sleepTime);

    /* 6.去使能传感器 */
    ret = sensorDev->Disable(sensorInfo[i].sensorId);
    if (ret != HDF_SUCCESS) {
      continue;
    }

    printf("Disable success\n");
  }

  /* 7.取消传感器数据订阅函数 */
  ret = sensorDev->Unregister(TRADITIONAL_SENSOR_TYPE, SensorDataCallback);
  if (ret != HDF_SUCCESS) {
    return;
  }

  printf("Unregister success\n");

  /* 8.释放传感器接口实例 */
  ret = FreeSensorInterfaceInstance();
  if (ret != HDF_SUCCESS) {
    return;
  }

  printf("FreeSensorInterfaceInstance success\n");
}

int main(int argc, char *argv[]) {
  SensorEntry();
  return HDF_SUCCESS;
}
