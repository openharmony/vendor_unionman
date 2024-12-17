import os
import subprocess
import logging as logger
from common.catch_log import CrashThread

class WukongBase(object):
    def __init__(self,device_sn,hdc_path,log_path):
        self.device_sn = device_sn
        self.hdc_path = hdc_path
        self.log_path = log_path
        self.hdc_shell = "%s -t %s shell" % (hdc_path,device_sn)

    def _send_data_to_phone(self,test_type):
        r_path = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        wukong_path = os.path.join(r_path,"utils",test_type,"wukong")
        logger.info("wukong_path = %s" % wukong_path)
        cmd_array = [
            "%s mount -o rw,remount /" % self.hdc_shell,
            "%s -t %s file send %s /bin/" % (self.hdc_path,self.device_sn,wukong_path),
            "%s chmod a+x /bin/wukong" % self.hdc_shell,
        ]
        for command in cmd_array:
            self.execute_command(command)

    def execute_command(self,command):
        try:
            code = subprocess.check_call(command,shell=True,stdout=subprocess.PIPE)
            logger.info("execute_command command = %s code = %d" % (command,code))
        except subprocess.CalledProcessError as exception:
            logger.info("execute_command exception stderr = %s" % exception.stderr)

    def start_log_thread(self):
        catch_thread = CrashThread(self.device_sn,self.hdc_path,self.log_path)
        #catch_thread.setDaemon(True)
        catch_thread.start()
        return catch_thread

if __name__ == '__main__':
    WukongBase("7001005458323933328a260ed7ee3900","hdc_std","D:\\temp\\catch_log_test").start_log_thread()