export class SendMessageBase {
  constructor() {
    this.type = "";
    this.target = "";
  }
  type: string
  target: string
}
export class SendRequestMsg extends SendMessageBase {
  constructor() {
    super();
    this.type = "request"
    this.url = "";
  }
  url: string
}
export class SendRequestAckMsg extends SendMessageBase{
  constructor() {
    super();
    this.type = "requestAck"
    this.opinion = "";
  }
  opinion:string
}

export class SendCloseMsg extends SendMessageBase{
  constructor() {
    super();
    this.type = "close";
  }
}