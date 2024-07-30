
import { HorizontalVideoItem } from './HorizontalVideoItem';
import { SwiperVideoItem } from './SwiperVideoItem'

export const SWIPER_VIDEOS: SwiperVideoItem[] = [
  new SwiperVideoItem($r('app.media.banner1')),
  new SwiperVideoItem($r('app.media.banner2')),
  new SwiperVideoItem($r('app.media.banner3'))
];

export const HORIZONTAL_VIDEOS: HorizontalVideoItem[] = [
  new HorizontalVideoItem(1, $r('app.media.video_list0'), '视频1'),
  new HorizontalVideoItem(1, $r('app.media.video_list1'), '视频2'),
  new HorizontalVideoItem(1, $r('app.media.video_list2'), '视频3')
];