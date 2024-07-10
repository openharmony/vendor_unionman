/**
 * @author: Mr.Hong
 */

import { ILogger } from './ILogger';

export class ConsoleLogger implements ILogger {
  private appIdentifier: string;

  constructor(appIdentifier: string) {
    this.appIdentifier = appIdentifier;
  }

  public verbose(tag: string, msg: string): void {
    console.log(`[${this.appIdentifier}] [verbose] tag:${tag} msg:${msg}`);
  }

  public debug(tag: string, msg: string): void {
    console.debug(`[${this.appIdentifier}] [debug] tag:${tag} msg:${msg}`);
  }

  public info(tag: string, msg: string): void {
    console.info(`[${this.appIdentifier}] [info] tag:${tag} msg:${msg}`);
  }

  public warn(tag: string, msg: string): void {
    console.warn(`[${this.appIdentifier}] [warn] tag:${tag} msg:${msg}`);
  }

  public error(tag: string, msg: string): void {
    console.error(`[${this.appIdentifier}] [error] tag:${tag} msg:${msg}`);
  }
}