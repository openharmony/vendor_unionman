
/**
 * 水平图片类。
 */
@Observed
export class HorizontalVideoItem{
  public id: number;
  public image: Resource;
  public name: string;

  constructor(id: number, image: Resource, name: string) {
    this.id = id;
    this.image = image;
    this.name = name
  }
}