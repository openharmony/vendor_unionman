export class Hero {
  x: number = 676
  y: number = 142
  width: number = 124
  height: number = 99
  index: number = 0
  eCout: number = 0
  n: number = 0
  removable: number = 0
  life: number = 6
}

export class Enemy {
  x: number = 0
  y: number = 0
  index: number = 0
  width: number = 51
  height: number = 51
  life: number = 5

  constructor(y: number) {
    this.y = y
  }
}

export class Enemya {
  x: number = 0
  y: number = 0
  index: number = 0
  width: number = 95
  height: number = 69
  life: number = 10

  constructor(y: number) {
    this.y = y
  }
}

export class CBossEnemy {
  x: number = 0
  y: number = 0
  index: number = 0
  width: number = 258
  height: number = 169
  life: number = 200

  constructor(y: number) {
    this.y = y
  }
}

export class Hullet {
  x: number = 0
  y: number = 0
  width: number = 0
  height: number = 0
  n: number = 0
  removable: number = 0

  constructor(x: number, y: number, n: number) {
    this.x = x
    this.y = y + (99 - 8) / 2
    this.n = n
  }
}
