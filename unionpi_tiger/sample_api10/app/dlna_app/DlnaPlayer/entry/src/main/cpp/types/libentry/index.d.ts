export const add: (a: number, b: number) => number;
export const startDmr: (ip: string, data:(string) => void) => number;
export const notifyPositionChanged:(position: number, duration: number) => number;