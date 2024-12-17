import os
import sys
from datetime import datetime
from common.py_log import PyLog

class LogManage(object):
    def __init__(self,device_sn):
        self.device_sn = device_sn
        if 'python.exe' in sys.executable:
            root_path = os.path.dirname(os.path.dirname(__file__))
        else:
            root_path = os.path.dirname(sys.executable)
        self.log_dir = os.path.join(root_path,"log")

    def init_logging(self,log_path):
        pyLog = PyLog(logfile=self.get_test_log_name(log_path))
        return pyLog

    def get_log_dir(self,log_name):
        log_dir = os.path.join(self.log_dir,log_name)
        if not os.path.exists(log_dir):
            os.makedirs(log_dir)
        return log_dir

    def _get_device_name(self):
        name = self.device_sn
        if len(self.device_sn) > 16 :
            name = self.device_sn[len(self.device_sn) - 16 : len(self.device_sn)]
        return name

    def get_monkey_dir_name(self):
        device_name = self._get_device_name()
        dir_name = "monkey_test_%s_%s" % ('{:%Y_%m_%d_%H_%M_%S}'.format(datetime.now()),device_name)
        return dir_name

    def get_reboot_dir_name(self):
        device_name = self._get_device_name()
        dir_name = "reboot_test_%s_%s" % ('{:%Y_%m_%d_%H_%M_%S}'.format(datetime.now()),device_name)
        return dir_name

    def get_sleep_wakeup_dir_name(self):
        device_name = self._get_device_name()
        dir_name = "sleep_wakeup_test_%s_%s" % ('{:%Y_%m_%d_%H_%M_%S}'.format(datetime.now()),device_name)
        return dir_name

    def get_hap_dir_name(self):
        device_name = self._get_device_name()
        dir_name = "hap_test_%s_%s" % ('{:%Y_%m_%d_%H_%M_%S}'.format(datetime.now()),device_name)
        return dir_name
    def get_test_log_name(self,log_path=None):
        return os.path.join(self.log_dir if log_path is None else log_path,"test_log.log")
def main():
    log_manage = LogManage("7001005458323933328a260ed7ee3900")
    print("log_mange = ",log_manage.get_reboot_dir_name())

if __name__ == '__main__':
    main()