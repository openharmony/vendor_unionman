import { Hero, Enemy, Hullet, CBossEnemy, Enemya } from '../object/hero'
import { globalObject, GetDevice, ActiveApp } from '../pages/founction'
import { RankTable, Ranking } from '../common/relationIStore'
import promptAction from '@ohos.promptAction';
import { Callback } from '@ohos.base';
import { GlobalThis } from '../utils/globalThis'
import { Source } from '../entryability/EntryAbility'

let imgName = ['background.png', 'game_pause_nor.png', 'm1.png', 'start.png',
  // 敌机1
  ['Enemy1.png', 'Enemy1_down1.png', 'Enemy1_down2.png', 'Enemy1_down3.png', 'Enemy1_down4.png'],
  // 敌机2
  ['Enemy2.png', 'Enemy2_down1.png', 'Enemy2_down2.png', 'Enemy2_down3.png', 'Enemy2_down4.png'],
  // 敌机3
  ['Enemy3_n1.png', 'Enemy3_n2.png', 'Enemy3_hit.png', 'Enemy3_down1.png', 'Enemy3_down2.png', 'Enemy3_down3.png', 'Enemy3_down4.png', 'Enemy3_down5.png', 'Enemy3_down6.png',],
  // 游戏loading图
  ['game_loading1.png', 'game_loading2.png', 'game_loading3.png', 'game_loading4.png'],
  // 玩家飞机图
  ['hero1.png', 'hero2.png', 'hero_blowup_n1.png', 'hero_blowup_n2.png', 'hero_blowup_n3.png', 'hero_blowup_n4.png'],

  ['icon_000.png'], 'm.png', '排行.png'
];

let curPhase: number | undefined = 2;
const TAG = "FlyGame"
const PHASE_DOWNLOAD = 1;
const PHASE_READY = 2;
const PHASE_LOADING = 3;
const PHASE_PLAY = 4;
const PHASE_PAUSE = 5;
const PHASE_GAMEOVER = 6;
const csreen_width = 800;
const csreen_height = 400;

let generate = 0
let generateBig = 0
let index = 0
let Hullet1: Array<Hullet> = []
let Hullete: Array<Hullet> = []
let Enemy1: Array<Enemy> = []
let Enemy2: Array<Enemya> = []
let EnemyBoss: Array<CBossEnemy> = []
let hero: Hero | undefined = new Hero()
let settings: RenderingContextSettings = new RenderingContextSettings(true)
let context: CanvasRenderingContext2D = new CanvasRenderingContext2D(settings)
let offContext: OffscreenCanvasRenderingContext2D = new OffscreenCanvasRenderingContext2D(csreen_width, csreen_height, settings)
let score = 0
let count: number = 9
let hCout: number = 0
let mCout: number = 0
let qcout: number = 0
let mode: number = 0
let table: Array<Ranking> = []
let stringTable: Array<string> = []
let RankTable1 = new RankTable
// 设置随机数种子
let seed = 123; // 可以用任意数字来作为种子
let object = new Source()
// object = GlobalThis.getInstance().getObject('remoteObject') as Source

function seededRandom(seed: number, min: number, max: number) {
  let x = Math.sin(seed) * 10000;
  const result = min + (x - Math.floor(x)) * (max - min);
  return Math.floor(result);
}

function nIgm(scr: string | string[]) {
  let img = new ImageBitmap("/ytracom/" + scr)
  return img
}

function distr_link() {
  offContext.drawImage(nIgm('icon_000.png'), 0, 0)
}

function gameloading() {
  index % 1 == 0 && offContext.drawImage(nIgm(imgName[7][index]), 800 - 400, 80)
  index += 0.25
  if (index > 3) {
    curPhase = PHASE_PLAY;
    index = 0
  }
}

function paintLogo() {
  offContext.drawImage(nIgm(imgName[3]), 0, 0, csreen_width, csreen_height, 0, 0, csreen_width, csreen_height)
}

function drawGameOver() {
  promptAction.showToast({
    message: '游戏结束\n得分：' + score,
    duration: 2000
  });
  curPhase = PHASE_READY
  hero!.x = 676
  hero!.y = 142
  RankTable1.insertData("Handsome", score)
  score = 0
  hero!.index = 0
  Hullet1 = []
  Enemy1 = []
  Enemy2 = []
  if (GlobalThis.remote == 1) {
    object.hero = hero
    object.status = PHASE_READY
  } else {
    // globalObject.hero = hero
    object.status = PHASE_READY
  }
  seed = 123
}

function drawPause() {
  offContext.drawImage(nIgm(imgName[1]), 0, 0)
}

function drawhero() {
  let i = 0
  if (hero!.life > 0) {
    offContext.drawImage(nIgm(imgName[8][hero!.index]), hero!.x, hero!.y)
    mCout++
    if (mCout % 200 == 0) {
      EnemyBoss.push(new CBossEnemy(seededRandom(seed++, 20, 340)))
      mCout = 0
    }
    count++
    if (count % 3 == 0) {
      Hullet1.push(new Hullet(hero!.x, hero!.y, 1))
      Hullet1.push(new Hullet(hero!.x, hero!.y, 2))
      Hullet1.push(new Hullet(hero!.x, hero!.y, 3))
      count = 0
    }
    hCout++
    if (GlobalThis.remote == 1) {
      if (object.mode == 1) {
        if (hCout % 6 == 0) {
          Enemy1.push(new Enemy(seededRandom(seed++, 20, 340)))
        }
      } else if (object.generate == 1) {
        Enemy1.push(new Enemy(seededRandom(seed++, 20, 340)))
        object.generate = 0
      }
    } else if (mode == 1) {
      if (hCout % 6 == 0) {
        Enemy1.push(new Enemy(seededRandom(seed++, 20, 340)))
      }
    } else if (generate == 1) {
      Enemy1.push(new Enemy(seededRandom(seed++, 20, 340)))
      generate = 0
    }

    if (GlobalThis.remote == 1) {
      if (object.mode == 1) {
        if (hCout % 6 == 0) {
          Enemy2.push(new Enemya(seededRandom(seed++, 20, 340)))
        }
      } else if (object.generateBig == 1) {
        Enemy2.push(new Enemya(seededRandom(seed++, 20, 340)))
        object.generateBig = 0
      }
    } else if (mode == 1) {
      if (hCout % 6 == 0) {
        Enemy2.push(new Enemya(seededRandom(seed++, 20, 340)))
      }
    } else if (generateBig == 1) {
      Enemy2.push(new Enemya(seededRandom(seed++, 20, 340)))
      generateBig = 0
    }

  } else {
    curPhase = PHASE_GAMEOVER
    return
  }
  for (i = 0; i < Enemy1.length; i++) {
    let e = Enemy1[i]
    if (e.x + e.width >= hero!.x && hero!.x + hero!.width >= e.x && e.y + e.height >= hero!.y && hero!.y + hero!.height >= e.y) {

      e.life -= 1
      Enemy1[i].index += 1
      hero!.index += 1
      hero!.life -= 1
      if (hero!.life < 0) {
        hero!.life = 0
      }
      if (e.life < 0) {
        e.life = 0
      }
      if (Enemy1[i].index == 4) {
        Enemy1[i].index == 0
      }
      if (hero!.index == 4) {
        hero!.index = 0
      }
    }
  }
  for (i = 0; i < Enemy2.length; i++) {
    let e = Enemy2[i]
    if (e.x + e.width >= hero!.x && hero!.x + hero!.width >= e.x && e.y + e.height >= hero!.y && hero!.y + hero!.height >= e.y) {

      e.life -= 1
      Enemy2[i].index += 1
      hero!.index += 1
      hero!.life -= 1
      if (hero!.life < 0) {
        hero!.life = 0
      }
      if (e.life < 0) {
        e.life = 0
      }
      if (Enemy2[i].index == 4) {
        Enemy2[i].index == 0
      }
      if (hero!.index == 4) {
        hero!.index = 0
      }
    }
  }
}

function drawEnemy1() {
  let i = 0
  let j = 0
  for (i; i < Enemy1.length; i++) {
    Enemy1[i].x += 6

    if (Enemy1[i].x < 0 || Enemy1[i].x > 800 || Enemy1[i].y < 0 || Enemy1[i].y > 480) {
      Enemy1[i].life = 0
    }
    if (Enemy1[i].life == 0) {
      score += 1
      Enemy1.splice(i, 1)
    }
  }
  for (i = 0; i < Enemy1.length; i++) {
    offContext.drawImage(nIgm(imgName[4][Enemy1[i].index]), Enemy1[i].x, Enemy1[i].y)
  }
  for (i = 0; i < Enemy1.length; i++) {
    for (j = 0; j < Hullet1.length; j++) {
      let h = Hullet1[j]
      let e = Enemy1[i]
      if (e.x + e.width >= h.x && h.x + h.width >= e.x && e.y + e.height >= h.y && h.y + h.height >= e.y) {
        e.life -= 1
        Enemy1[i].index += 1
        if (e.life < 0) {
          e.life = 0
        }
        if (Enemy1[i].index == 4) {
          Enemy1[i].index == 0
        }
      }
    }
  }
}

function drawEnemy2() {
  let i = 0
  let j = 0
  for (i; i < Enemy2.length; i++) {
    Enemy2[i].x += 8

    if (Enemy2[i].x < 0 || Enemy2[i].x > 800 || Enemy2[i].y < 0 || Enemy2[i].y > 480) {
      Enemy2[i].life = 0
    }
    if (Enemy2[i].life == 0) {
      score += 1
      Enemy2.splice(i, 1)
    }
  }
  for (i = 0; i < Enemy2.length; i++) {
    offContext.drawImage(nIgm(imgName[5][Enemy2[i].index]), Enemy2[i].x, Enemy2[i].y)
  }
  for (i = 0; i < Enemy2.length; i++) {
    for (j = 0; j < Hullet1.length; j++) {
      let h = Hullet1[j]
      let e = Enemy2[i]
      if (e.x + e.width >= h.x && h.x + h.width >= e.x && e.y + e.height >= h.y && h.y + h.height >= e.y) {
        e.life -= 1
        qcout++
        if (qcout % 2 == 0) {
          Enemy2[i].index += 1
        }
        if (e.life < 0) {
          e.life = 0
        }
        if (Enemy2[i].index == 4) {
          Enemy2[i].index == 0
        }
      }
    }
  }
}

function drawEnemyBoss() {
  let i = 0
  let j = 0
  for (i; i < EnemyBoss.length; i++) {
    qcout++
    if (qcout % 3 == 0) {
      Hullete.push(new Hullet(EnemyBoss[i].x, EnemyBoss[i].y, 1))
      Hullete.push(new Hullet(EnemyBoss[i].x, EnemyBoss[i].y, 2))
      Hullete.push(new Hullet(EnemyBoss[i].x, EnemyBoss[i].y, 3))
      qcout = 0
    }
    if (EnemyBoss[i].x < 0 || EnemyBoss[i].x > 800 || EnemyBoss[i].y < 0 || EnemyBoss[i].y > 480) {
      EnemyBoss[i].life = 0
    }
    if (EnemyBoss[i].life == 0) {
      score += 1
      EnemyBoss.splice(i, 1)
    }
  }
  for (i = 0; i < EnemyBoss.length; i++) {
    offContext.drawImage(nIgm(imgName[6][EnemyBoss[i].index]), EnemyBoss[i].x, EnemyBoss[i].y)
  }
  for (i = 0; i < EnemyBoss.length; i++) {
    for (j = 0; j < Hullet1.length; j++) {
      let h = Hullet1[j]
      let e = EnemyBoss[i]
      if (e.x + e.width >= h.x && h.x + h.width >= e.x && e.y + e.height >= h.y && h.y + h.height >= e.y) {
        e.life -= 1
        console.info(TAG + e.life)
        EnemyBoss[i].index += 1
        if (e.life < 0) {
          e.life = 0
        }
        if (EnemyBoss[i].index == 8) {
          EnemyBoss[i].index = 0
        }
      }
    }
  }
}

function drawHullet1() {
  let i = 0
  for (i; i < Hullet1.length; i++) {
    switch (Hullet1[i].n) {
      case 1:
        Hullet1[i].y -= 1
        Hullet1[i].x -= 40
        break
      case 2:
        Hullet1[i].y += 1
        Hullet1[i].x -= 40
        break
      case 3:
        Hullet1[i].x -= 40
        break
      default:
        console.log("invalid n")
        break
    }
    if (Hullet1[i].x < 0 || Hullet1[i].x >= 800 || Hullet1[i].y < 0 || Hullet1[i].y >= 480) {
      Hullet1[i].removable = 1
    }
    if (Hullet1[i].removable == 1) {
      Hullet1.splice(i, 1)
    }
  }
  for (i = 0; i < Hullet1.length; i++) {
    offContext.drawImage(nIgm(imgName[2]), Hullet1[i].x, Hullet1[i].y)
  }
}

function drawHullete() {
  let i = 0
  for (i; i < Hullete.length; i++) {
    switch (Hullete[i].n) {
      case 1:
        Hullete[i].y -= 1
        Hullete[i].x += 40
        break
      case 2:
        Hullete[i].y += 1
        Hullete[i].x += 40
        break
      case 3:
        Hullete[i].x += 40
        break
      default:
        console.log("invalid n")
        break
    }
    if (Hullete[i].x < 0 || Hullete[i].x >= 800 || Hullete[i].y < 0 || Hullete[i].y >= 480) {
      Hullete[i].removable = 1
    }
    if (Hullete[i].removable == 1) {
      Hullet1.splice(i, 1)
    }
  }
  for (i = 0; i < Hullete.length; i++) {
    offContext.drawImage(nIgm(imgName[10]), Hullete[i].x, Hullete[i].y)
  }
}

function drawsocre() {
  if (GlobalThis.remote == 1) {
    offContext.strokeText('分数：' + score, 350, 20)
  } else {
    if (mode == 1) {
      offContext.strokeText('分数：' + score, 350, 20)
    } else {
      offContext.strokeText('点击派遣飞机=>', 350, 20)
    }
  }

}

function getrank(callback: Callback<void>) {
  let i = 0
  RankTable1.queryAll((result) => {
    console.info(TAG + "ranking list" + JSON.stringify(result))
    table = result
    table.sort((a, b) => b.score - a.score)
    console.info(TAG + "sort ranking list" + JSON.stringify(result))
    if (table != undefined) {
      if (table.length < 10) {
        for (i = 0; i < table.length; i++) {
          console.info(TAG + "   " + i + "   " + table.length)
          stringTable.push("第" + (i + 1) + "名" + table[i].name + "    得分" + table[i].score)
        }
      } else {
        console.info(TAG + "大于10" + "  " + table.length)
        for (i = 0; i < 10; i++) {
          stringTable.push("第" + (i + 1) + "名" + table[i].name + "    得分" + table[i].score)
        }
      }
    }
    callback();
  })
}

@Component
export struct PlaneGame {
  @State score: number = 0
  @State select: number | number[] = 0
  @State x: number = 676
  @State y: number = 142
  private select1: number | number[] = 0
  private device: string[] = ['本机器']

  build() {
    Column() {
      Stack({ alignContent: Alignment.TopEnd }) {
        Canvas(context)
          .backgroundImage('/ytracom/background.png')
          .onReady(() => {
            RankTable1.getStore()
            console.info('object:' + JSON.stringify(object)); // 调试输出，查看object的值
            console.info(TAG + " refresh the page")
            setInterval(() => {
              // 每次刷新画布数据时同步两边的状态，当一方进入了加载状态时，另一方跟着进入
              if (GlobalThis.remote == 1 && curPhase == 2 && curPhase != object.status) {
                offContext.clearRect(0, 0, csreen_width - 5, csreen_height - 5)
                curPhase = object.status
              } else if (curPhase == 2 && curPhase != object.status) {
                offContext.clearRect(0, 0, csreen_width - 5, csreen_height - 5)
                curPhase = object.status
              }
              // 通过变量curPhase来判断当前的游戏转态
              switch (curPhase) {
                case PHASE_READY:
                  offContext.clearRect(0, 0, csreen_width - 5, csreen_height - 5)
                  offContext.drawImage(nIgm(imgName[11]), 5, 290)
                  if (GlobalThis.remote == 1) {
                    paintLogo()
                  } else {
                    paintLogo()
                    distr_link()
                  }
                  break
                case PHASE_LOADING:
                  gameloading()
                  hero!.life = 6
                  break
                case PHASE_PLAY:
                  if (GlobalThis.remote == 1) {
                    if (object.mode == 1) {

                      hero = object.hero
                    }
                  } else {
                    hero = object.hero
                  }
                  offContext.clearRect(0, 0, csreen_width - 1, csreen_height - 1)
                  if (GlobalThis.remote != 1 && mode != 1) {
                    offContext.drawImage(nIgm(imgName[4][0]), 500, 0)
                    offContext.drawImage(nIgm(imgName[5][0]), 600, 0)
                  }
                  drawsocre()
                  drawEnemy1()
                  drawEnemy2()
                  drawHullet1()
                  drawhero()
                  break
                case PHASE_PAUSE:
                  drawPause()
                  break
                case PHASE_GAMEOVER:
                  if (GlobalThis.remote != 1) {
                    object.mode = 0
                  }
                  mode = 0
                  drawGameOver()
                  break
              }
              let image = offContext.transferToImageBitmap();
              context.transferFromImageBitmap(image);
              this.score = score
            }, 100)
            // 每隔500ms提交一次数据，当我把提交数据写入上面的100ms一次刷新时，会导致分布式对象有数据丢失现象，导致游戏很卡
            setInterval(() => {
              if (GlobalThis.remote == 1) {
                object.hero = hero
              } else {
                if (mode == 1) {
                  object.hero = hero
                }

              }
            }, 500)
          })
          .onTouch((event) => {

            if (curPhase == PHASE_PLAY) {
              console.info(TAG + event.touches[0].x + "  " + event.touches[0].y)
              // 判断触摸是否在飞机附近，只有在飞机附件的触摸是有效触摸
              if (event.touches[0].x > hero!.x && event.touches[0].x < hero!.x + 200 && event.touches[0].y > hero!.y - 20 && event.touches[0].y < hero!.y + 150) {
                hero!.x = event.touches[0].x - 124
                hero!.y = event.touches[0].y - 50
              }
              // GlobalThis.remote表示拉起被控时拉起的对象存在，用于判断当前的触控后通过哪个对象更新hero数据
              if (GlobalThis.remote == 1) {
                object.hero = hero
              } else {
                // 只有模式为同步模式，才在控制端更新hero值
                if (mode == 1) {
                  object.hero = hero
                }
              }
            }
          })
          .onClick((event) => {
            // 在对战模式下点击飞机图案生成飞机
            if (curPhase == PHASE_PLAY && event.x > 500 && event.y > 0 && event.x < 550 && event.y < 60 && GlobalThis.remote != 1) {
              object.generate = 1
              generate = 1
            }

            if (curPhase == PHASE_PLAY && event.x > 600 && event.y > 0 && event.x < 700 && event.y < 100 && GlobalThis.remote != 1) {
              object.generateBig = 1
              generateBig = 1
            }
            // 在准备阶段点击非标志区域则弹出游戏模式选择
            if (curPhase == 2 && event.x > 96 && event.y > 96 && GlobalThis.remote != 1) {
              TextPickerDialog.show({
                range: ["同步模式", "对抗模式"],
                selected: this.select1,
                onAccept: (value: TextPickerResult) => {
                  console.info("TextPickerDialog:onAccept()" + JSON.stringify(value))
                  this.select1 = value.index
                  if (this.select1 == 0) {
                    mode = 1
                    object.mode = 1
                    offContext.clearRect(0, 0, csreen_width - 1, csreen_height - 1)
                    if (GlobalThis.remote == 1) {
                      object.status = PHASE_LOADING
                    } else {
                      object.status = PHASE_LOADING
                    }
                    curPhase = PHASE_LOADING
                  } else {
                    offContext.clearRect(0, 0, csreen_width - 1, csreen_height - 1)
                    if (GlobalThis.remote == 1) {
                      object.status = PHASE_LOADING
                    } else {
                      object.status = PHASE_LOADING
                    }
                    curPhase = PHASE_LOADING

                  }
                },
                onCancel: () => {
                  console.info("TextPickerDialog:onCancel()")
                },
                onChange: (value: TextPickerResult) => {
                  console.info("TextPickerDialog:onChange()" + JSON.stringify(value))
                }
              })
            }
            // 在准备阶段点击排行榜标志则弹出排行榜
            if (event.x < 70 && event.y > 290 && event.y < 350 && curPhase == 2) {
              getrank(() => {
                TextPickerDialog.show({
                  range: stringTable,
                  selected: this.select,
                  onAccept: (value: TextPickerResult) => {
                    stringTable = []
                    // 设置select为按下确定按钮时候的选中项index，这样当弹窗再次弹出时显示选中的是上一次确定的选项
                    console.info("TextPickerDialog:onAccept()" + JSON.stringify(value))
                  },
                  onCancel: () => {
                    stringTable = []
                    console.info("TextPickerDialog:onCancel()")
                  },
                  onChange: (value: TextPickerResult) => {
                    console.info("TextPickerDialog:onChange()" + JSON.stringify(value))
                  }
                })
              })
            }
            // 在准备阶段按到连接标志则弹出连接框
            if (event.x < 96 && event.y < 96 && curPhase == 2 && GlobalThis.remote != 1) {
              let deviceList = GetDevice()
              if (deviceList != undefined) {
                console.info("deviceList" + JSON.stringify(deviceList))
                if (deviceList[0] != undefined) {
                  this.device.push(deviceList[0].deviceName)
                }
              }
              TextPickerDialog.show({
                range: this.device,
                selected: this.select,
                onAccept: (value: TextPickerResult) => {
                  if (deviceList != undefined) {
                    let Coordinate: Array<number>
                    if (deviceList[0].deviceId != null) {
                      Coordinate = ActiveApp(deviceList[0].deviceId)
                      this.x = Coordinate[0]
                      this.y = Coordinate[1]
                    }
                  }
                  this.select = value.index
                  console.info("TextPickerDialog:onAccept()" + JSON.stringify(value))
                },
                onCancel: () => {
                  console.info("TextPickerDialog:onCancel()")
                },
                onChange: (value: TextPickerResult) => {
                  console.info("TextPickerDialog:onChange()" + JSON.stringify(value))
                }
              })
              this.device = ["本机器"]
            }
          })
        // 画布使用onKeyEvent时无法得到响应，通过弹性布局加入不占空间的按键实现监测键盘输入
        Button()// .borderColor(Color.Black)
          .onKeyEvent((event: KeyEvent) => {
            let space = 20
            if (event.type === KeyType.Down) {
              // 判断键盘输入是否为方向键位，是则进行相应的位移
              if (event.keyCode == 2012) {
                hero!.x -= space
              }
              if (event.keyCode == 2013) {
                hero!.x += space
              }
              if (event.keyCode == 2014) {
                hero!.y += space
              }
              if (event.keyCode == 2015) {
                hero!.y -= space
              }
              //给飞机设置边界不能飞出屏幕
              if (hero!.x > 676) {
                hero!.x = 676
              }
              if (hero!.x < 0) {
                hero!.x = 0
              }
              if (hero!.y < 0) {
                hero!.y = 0
              }
              if (hero!.y > 292) {
                hero!.y = 292
              }
            }
          })
      }
    }
  }
}