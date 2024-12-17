import math
import time
import threading
import subprocess
import optparse
import logging as logger
from datetime import datetime
from common.utils import *
from common.log_manage import LogManage
from common.wukong_base import WukongBase

class MonkeyPressTest(WukongBase):
    def __init__(self, devices_sn, hdc_path, log_path, interval, time):
        super(MonkeyPressTest, self).__init__(devices_sn, hdc_path, log_path)
        self.interval = interval
        self.time = time
        self.execute_time = self.time * 60
        logger.info("wukong execute time = " + str(self.execute_time))
        self._stop = False

    def execute_command(self,command):
        process = None
        try:
            process = subprocess.run(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=30)
        except subprocess.CalledProcessError as e:
            logger.info("execute_command exception = %s" % e)
        finally:
            if process is not None:
                result = process.stdout.decode().strip()
                return result
            return ""


    def _kill_wukong_process(self):
        data_line = self.execute_command("%s \"ps -ef | grep wukong | grep -v grep \"" % self.hdc_shell)
        pid = self._get_process_pid(data_line)
        cmd_rm = '{} \"kill -9 {}\""'.format(self.hdc_shell,pid)
        self.execute_command(cmd_rm)

    def start_timer(self):
        execute_time = self.time * 60
        logger.info(" execute_time = %s" % execute_time)
        def set_test_status():
            self._stop = True
        timer = threading.Timer(execute_time,set_test_status)
        timer.start()
        return timer

    def run(self):
        self._kill_wukong_process()
        cath_thread = self.start_log_thread()
        logger.info("MonkeyPressTest start()")
        command = '{} "wukong exec -a 0.05 -t 0.05 -C 0.65 -S 0.1 -m 0.05 -k 0.05 -i {} -T {} &"' .format(self.hdc_shell,self.interval,self.time)
        process = None
        logger.info("monkey  test command %s" % command)
        timer = self.start_timer()
        start_time = datetime.now()
        exception_path = ""
        s_time = int(time.time())
        try:
            process = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
            while not self._stop:
                if process.poll() is None:
                    line = process.stdout.readline().strip()
                    temp_str = line.decode("utf-8","ignore")
                    logger.info("wukong line = %s" % temp_str)
                    if ("/data/local/tmp/wukong/report/" in temp_str and "/exception" in temp_str):
                        exception_path = temp_str.split("(")[2].split(")")[0]
                    end_time = int(time.time())
                    if ((end_time -s_time) % 30 == 0 and exception_path != ""):
                        cmd_rm = '{} \"rm -rf {}*\""'.format(self.hdc_shell,exception_path)
                        self.execute_command(cmd_rm)
                    if (end_time -s_time > 43200):
                        s_time = int(time.time())
                        self._kill_wukong_process()
                    continue
                if self.check_devices():
                    data_line = self.execute_command("%s \"ps -ef | grep wukong | grep -v grep\"" % self.hdc_shell)
                    pid = self._get_process_pid(data_line)
                    logger.info("the %s is reconnected, wukong pid is %s" % (self.device_sn,pid))
                    if (pid == None):
                        if process is not None:
                            process.kill()
                            process = None
                        logger.info("-----------wukong restart-----------")
                        process = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
                        continue
                else:
                    logger.info("the device is disconnected")
                time.sleep(30)
        except KeyboardInterrupt as exception:
            logger.info("run exception = %s " % str(exception))
        finally:
            timer.cancel()
            cath_thread.stop_catch()
            self.check_execute_status(start_time)
            self._kill_wukong_process()
            if process is not None:
                process.kill()

    def _get_process_pid(self,data_line):
        if data_line is None or data_line == "":
            return None
        pid_str = " ".join(data_line.split())
        data_array = pid_str.split(" ")
        if len(data_array) < 1 :
            return None
        return data_array[1]
    def check_devices(self):
        result = self.execute_command('%s list targets' % self.hdc_path)
        return result.find(self.device_sn) != -1

    def check_execute_status(self,start_time):
        end_time = datetime.now()
        dur_time = (end_time - start_time).seconds
        if dur_time >= self.execute_time :
            logger.info("wukong execute success")
            return
        if not self.check_devices():
            logger.info("%s not find the hdc is not connet or the phone battery is exhausted." % self.device_sn)
            return
        result = self.execute_command('%s "ps -ef | grep wukong | grep -v grep"' % self.hdc_shell)
        logger.info("wukong process info = " + result)

def parse_option():
    parser = optparse.OptionParser()
    parser.add_option("-d","--devices",dest="devices",type="str",default=None,help='hdc list targets')
    parser.add_option("-p", "--hdc path", dest="hdc_path", type="str", default='hdc', help='hdc absolute path')
    parser.add_option("-i", "--interval", dest="interval", type="int", default='400', help='')
    parser.add_option("-t", "--time", dest="time", type="int", default='60', help='')
    ret_option,remainder = parser.parse_args()
    if ret_option.devices is None :
        raise ValueError(" please add -d argument (sn)")
    return ret_option
def main():
    option = parse_option()
    log_manage = LogManage(option.devices)
    pc_log_path = log_manage.get_log_dir(log_manage.get_monkey_dir_name())
    my_log = log_manage.init_logging(pc_log_path)
    set_env("%s -t %s shell" % (option.hdc_path,option.devices))
    press_test = MonkeyPressTest(option.devices,option.hdc_path,pc_log_path,option.interval,option.time)
    press_test.run()
    my_log.clear_handlers()

if __name__ == '__main__':
    main()