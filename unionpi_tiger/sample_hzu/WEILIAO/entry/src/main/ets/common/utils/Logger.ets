/**
 * @author: Mr.Hong
 */

import { ILogger } from './ILogger';
import { HiLogger } from './HiLogger';
import { ConsoleLogger } from './ConsoleLogger';

export class Logger implements ILogger {
  private static isAppDebugMode: boolean = true;
  private static appIdentifier: string = 'RDB';
  // if true, use a ConsoleLogger, or HiLogger otherwise
  private static useConsoleLogger: boolean = true;

  public static verbose(tag: string, msg: string): void {
    if (Logger.isAppDebugMode) {
      Logger.getInstance().verbose(tag, msg);
    }
  }

  public static debug(tag: string, msg: string): void {
    if (Logger.isAppDebugMode) {
      Logger.getInstance().debug(tag, msg);
    }
  }

  public static info(tag: string, msg: string): void {
    if (Logger.isAppDebugMode) {
      Logger.getInstance().info(tag, msg);
    }
  }

  public static warn(tag: string, msg: string): void {
    if (Logger.isAppDebugMode) {
      Logger.getInstance().warn(tag, msg);
    }
  }

  public static error(tag: string, msg: string): void {
    if (Logger.isAppDebugMode) {
      Logger.getInstance().error(tag, msg);
    }
  }

  private static logger: ILogger;

  private constructor() {
  }

  public static getInstance(): Logger {
    if (!Logger.logger) {
      Logger.logger = Logger.useConsoleLogger
        ? new ConsoleLogger(Logger.appIdentifier)
        : new HiLogger(Logger.appIdentifier);
    }

    return Logger.logger;
  }

  public verbose(tag: string, msg: string): void {
    Logger.logger.verbose(tag, msg);
  }

  public debug(tag: string, msg: string): void {
    Logger.debug(tag, msg);
  }

  public info(tag: string, msg: string): void {
    Logger.info(tag, msg);
  }

  public warn(tag: string, msg: string): void {
    Logger.warn(tag, msg);
  }

  public error(tag: string, msg: string): void {
    Logger.error(tag, msg);
  }
}