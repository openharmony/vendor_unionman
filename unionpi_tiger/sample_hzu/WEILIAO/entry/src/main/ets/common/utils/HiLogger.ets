/**
 * @author: Mr.Hong
 */

import hilog from '@ohos.hilog';
import { ILogger } from './ILogger';

export class HiLogger implements ILogger {
  private appIdentifier: string;
  private appDomain: number = 0x0001;

  constructor(appIdentifier: string) {
    this.appIdentifier = appIdentifier;
  }

  public verbose(tag: string, msg: string): void {
    hilog.debug(this.appDomain, tag, `[${this.appIdentifier}] [verbose] tag:${tag} msg:${msg}`);
  }

  public debug(tag: string, msg: string): void {
    hilog.debug(this.appDomain, tag, `[${this.appIdentifier}] [debug] tag:${tag} msg:${msg}`);
  }

  public info(tag: string, msg: string): void {
    hilog.info(this.appDomain, tag, `[${this.appIdentifier}] [info] tag:${tag} msg:${msg}`);
  }

  public warn(tag: string, msg: string): void {
    hilog.warn(this.appDomain, tag, `[${this.appIdentifier}] [warn] tag:${tag} msg:${msg}`);
  }

  public error(tag: string, msg: string): void {
    hilog.error(this.appDomain, tag, `[${this.appIdentifier}] [error] tag:${tag} msg:${msg}`);
  }
}