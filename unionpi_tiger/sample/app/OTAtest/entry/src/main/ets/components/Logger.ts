class Logger {
  constructor(private prefix: string) {
  }

  public log(...args: any[]) {
    console.log(`[${this.prefix}]: ${args.join('')}`)
  }

  public info(...args: any[]) {
    console.info(`[${this.prefix}]: ${args.join('')}`)
  }

  public debug(...args: any[]) {
    console.debug(`[${this.prefix}]: ${args.join('')}`)
  }

  public warn(...args: any[]) {
    console.warn(`[${this.prefix}]: ${args.join('')}`)
  }

  public error(...args: any[]) {
    console.error(`[${this.prefix}]: ${args.join('')}`)
  }
}

export default new Logger('ServiceExtAbility')