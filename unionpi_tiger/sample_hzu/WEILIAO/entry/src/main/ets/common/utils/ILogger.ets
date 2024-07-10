/**
 * @author: Mr.Hong
 */

export interface ILogger {
  verbose(tag: string, msg: string): void;

  debug(tag: string, msg: string): void;

  info(tag: string, msg: string): void;

  warn(tag: string, msg: string): void;

  error(tag: string, msg: string): void;
}