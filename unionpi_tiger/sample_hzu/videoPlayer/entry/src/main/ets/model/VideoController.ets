
/**
 * 获取当前播放时间
 *  当前播放时间    数字类型
 *  返回播放时间    字符串类型
 */
export function changeSliderTime(value: number): string{
  let second: number = value % 60;
  let min: number = Number.parseInt((value / 60).toString())
  let head = min < 10 ? `${0}${min}`: min;
  let end = second < 10 ? `${0}${second}`: second;
  let nowTime = `${head}:${end}`;
  return nowTime;
}