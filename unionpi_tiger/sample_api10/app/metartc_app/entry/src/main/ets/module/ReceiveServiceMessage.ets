export class ReceiveMessageBase {
  constructor() {
    this.type = "";
    this.source = "";
  }
  type: string
  source: string
}

export class ReceiveRequestMsg extends ReceiveMessageBase {
  constructor() {
    super();
    this.type = "";
    this.url = "";
  }
  url: string
}
export class ReceiveRequestAckMsg extends ReceiveMessageBase{
  constructor() {
    super();
    this.type = "";
    this.opinion= "";
  }
  opinion:string
}
export class ReceiveRequestAckAgreeMsg extends ReceiveRequestAckMsg {
  constructor() {
    super();
    this.url = "";
  }
  url: string
}

export class ReceiveRequestAckDeniedMsg extends ReceiveRequestAckMsg {
  constructor() {
    super();
    this.deniedReason = "";
  }
  deniedReason: string
}

export class CloseMsg extends ReceiveMessageBase {
  constructor() {
    super();
    this.closeReason = "";
  }
  closeReason: string
}