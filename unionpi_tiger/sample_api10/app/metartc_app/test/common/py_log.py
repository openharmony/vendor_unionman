import sys
import datetime
import logging

class PyLog(object):
    def __init__(self,logfile,debug_level=logging.debug):
        self._logfile = logfile
        self.debug_level = debug_level
        self._filehandler = None
        self.logger = self.setup()

    def __del__(self):
        if self._filehandler is not None:
            self._filehandler.close()

    def setup(self):
        logger = logging.getLogger("")
        logger.setLevel(logging.INFO)
        self._filehandler = logging.FileHandler(self._logfile,'a')
        formatter = logging.Formatter("%(asctime)s - %(levelname)s:%(message)s")
        self._filehandler.setFormatter(formatter)
        logger.addHandler(self._filehandler)
        ch = logging.StreamHandler(sys.stdout)
        ch.setFormatter(formatter)
        logger.addHandler(ch)
        return logger

    def clear_handlers(self):
        while self.logger.handlers:
            self.logger.handlers.clear()

def main():
    PyLog(datetime.datetime.now().strftime("%Y-%m-%d-%H-%M-%S") +".log")
    logging.debug('debug-log-8888888888888888')
    logging.info('info-log-999999999')
    return 0
if __name__ == "__main__":
    print("pyLog.py start")
    main()
