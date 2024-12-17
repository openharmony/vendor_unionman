import os
import datetime
import subprocess
import logging as logger
import time
from threading import Thread
from common.utils import exec_cmd

COMMAND_TIMEOUT = 20

class CrashThread(Thread):

    INTERNAL_TIME = 30

    def __init__(self,device_sn,hdc_key,log_path):
        super(CrashThread,self).__init__(name = "CrashThread")
        self.crash_list = set()
        self.hdc_shell = "%s -t %s shell" % (hdc_key,device_sn)
        self._is_stop = False
        self.log_path = log_path
        self.catch_log = CatchLog(log_path,device_sn,hdc_key)

    def run(self):
        logger.info("start Crash Thread")
        self.catch_log.get_ps_data()
        number = 0
        while not self._is_stop:
            logger.info("sleep 30s")
            time.sleep(CrashThread.INTERNAL_TIME)
            log_array = self.get_carsh_log()
            if len(log_array) == 0 :
                continue
            data_list = set(log_array).difference(self.crash_list)
            if len(data_list) == 0 :
                continue
            number += 1
            logger.info("data_list = "+ str(data_list) + "length = "+str(len(data_list)))
            temp_path = os.path.join(self.log_path,str(number))
            for log_name in data_list :
                log_name = log_name.strip()
                self.catch_log.get_cur_crash_log(temp_path,log_name)
                self.crash_list.add(log_name)
            self.catch_log.get_cur_hilog_data(temp_path)
            self.catch_log.get_current_phone_status(temp_path)
            self.catch_log.get_hilog_buffer_data(temp_path)
            logger.info( " check end next cycles")
        logger.info("crashThread run end， and is_stop is "+ str(self._is_stop))

    def get_carsh_log(self):
        log_array = list()
        native_crash_cmd = '%s "ls /data/log/faultlog/faultlogger | grep cppcrash"' % self.hdc_shell
        js_crash_cmd = '%s "ls /data/log/faultlog/faultlogger | grep jscrash"' % self.hdc_shell
        app_crash_cmd = '%s "ls /data/log/faultlog/faultlogger | grep appfreeze"' % self.hdc_shell
        block_crash_cmd = '%s "ls /data/log/faultlog/faultlogger | grep sysfreeze"' % self.hdc_shell

        log_array.extend(self._get_log(native_crash_cmd,"cppcrash"))
        log_array.extend(self._get_log(js_crash_cmd, "jscrash"))
        log_array.extend(self._get_log(app_crash_cmd, "appfreeze"))
        log_array.extend(self._get_log(block_crash_cmd, "sysfreeze"))

        return log_array
    def _get_log(self,log_cmd,*params):
        data_list = list()
        log_name_array = list()
        log_result = exec_cmd(log_cmd,wait_out=True)
        if log_result is not None and len(log_result) != 0 :
            log_name_array = log_result.strip().replace("\r","").split("\n")
        for log_name in log_name_array :
            log_name = log_name.strip()
            if self.filter_by_name(log_name,params):
                data_list.append(log_name)
        logger.info("_get_log loc_cmd = %s ,length = %d " % (log_cmd,len(log_name_array)))
        return data_list

    def filter_by_name(self,name,args):
        for starts_name in args :
            if name.startswith(starts_name):
                return True
        return False

    def filter_exception_data(self,log_result):
        temp_array = log_result.strip().replace("\r","").split("\n")
        logger.info("filter_exception_data = %s ",len(temp_array))
        log_array = filter(lambda name:"Not support std mode " not in name,temp_array)
        return list(log_array)

    def stop_catch(self):
        self._is_stop = True
        self.catch_log.get_fault_debug_log()
        self.catch_log.hdc_execute_log()
        self.catch_log.get_uptime_log()


class CatchLog(object):
    DEBUG_LOG_PATH = "/data/log/faultlog/debug/"
    NATIVE_CRASH_PATH = "/data/log/faultlog/temp"
    JS_CRASH_PATH = "/data/log/faultlog/faultlogger"
    ROOT_PATH = "/data/log/faultlog"
    HILOG_PATH = "/data/log/hilog/"

    def __init__(self,log_path,device_sn,hdc_path):
        self.device_sn = device_sn
        self.log_path = log_path
        self.hdc_shell = "%s -t %s shell" % (hdc_path,device_sn)
        self.hdc_cmd = "%s -t %s"%(hdc_path,device_sn)
        self.log_name_map = {'cppcrash':CatchLog.JS_CRASH_PATH,
                             'jscrash':CatchLog.JS_CRASH_PATH,
                             'sysfreeze': CatchLog.JS_CRASH_PATH,
                             'appfreeze': CatchLog.JS_CRASH_PATH,

        }

    def get_cur_crash_log(self,crash_path,log_name):
        if not os.path.exists(crash_path):
            os.makedirs(crash_path)
        if "Not support std mode" in log_name:
            return
        log_path = self.get_log_path(log_name)
        temp_path = "%s/%s" % (log_path,log_name)
        recv_command = "%s file recv %s %s" % (self.hdc_cmd,temp_path,crash_path)
        logger.info("file recv_command :" + recv_command)
        cmd_ret = exec_cmd(recv_command,wait_out=True)
        logger.info("file recv result :" + cmd_ret)

    def get_log_path(self,logname):
        name_array = logname.split("-")
        if len(name_array) <= 1 :
            return CatchLog.ROOT_PATH
        return self.log_name_map.get(name_array[0])

    def get_cur_hilog_data(self,crash_path):
        hilog_cmd = '%s "ls %s" ' % (self.hdc_shell,CatchLog.HILOG_PATH)
        result = exec_cmd(hilog_cmd,wait_out=True)
        if result is None:
            logger.info("hilog not open or hilog not catch crash log")
            return
        log_array = result.strip().split("\n")
        if log_array is None or len(log_array) == 0:
            logger.info("hilog not open or hilog not catch crash log")
            return
        for file_name in log_array:
            if "Not support std mode " in file_name:
                continue
            temp_path = "%s%s" % (CatchLog.HILOG_PATH,file_name)
            recv_command = "%s file recv %s %s" % (self.hdc_cmd,temp_path,crash_path)
            execute_result = exec_cmd(recv_command,wait_out=True)
            logger.info("get_cur_hilg_data command = %s \nresult = %s" % (recv_command,execute_result))

    def get_hilog_buffer_data(self,crash_path):
        hilog_buf_fd = open(os.path.join(crash_path,"hilog_buffer_data.log"),'a')
        buf_command = "%s hilog -x" % self.hdc_shell
        process = None
        try:
            process = subprocess.Popen(buf_command,stdout=hilog_buf_fd,bufsize=1,shell=True)
            process.communicate(timeout=10)
        except subprocess.TimeoutExpired as excepte:
            logger.info("hilog buffer time out expection = "+str(excepte))
        finally:
            if process is not None:
                if process.returncode:
                    hilog_buf_fd.flush()
                    hilog_buf_fd.close()
                process.kill()
            self._kill_hilog_process()
            logger.info("get_hilog_buffer_data end ")

    def _kill_hilog_process(self):
        command = "%s \"ps -ef | grep 'hilog -x' | grep -v grep \"" % self.hdc_shell
        pid_data = subprocess.check_output(command)
        pid_data = pid_data.strip().decode()
        logger.info("pid_data = "+ pid_data)
        if pid_data is None or pid_data == "":
            return
        pid_array = pid_data.replace("\r","").split("\n")
        for pid_str in pid_array :
            logger.info("pid_str = "+pid_str)
            pid_str = " ".join(pid_str.split())
            hilog_pid = pid_str.split(" ")[1]
            if not hilog_pid.isdigit():
                continue
            kill_command = "%s kill -9 %s" % (self.hdc_shell,hilog_pid)
            re_code = subprocess.call(kill_command)
            logger.info("kill_command = %s ret_code = %s " % (kill_command,re_code))

    def get_current_phone_status(self,crash_path):
        self.get_ps_data(crash_path)
        self._get_dump_data(crash_path)
        self._get_meminfo_data(crash_path)
        self.get_process_heap_info(crash_path)
        self.get_hidumper_mem(crash_path)
        logger.info("get_current_phone_status end ")

    def _get_dump_data(self,crash_path=None):
        dump_fd_out = open(os.path.join(self.log_path if crash_path is None else crash_path,"phone_dump_data.log"),'a')
        dump_command = '%s "aa dump -a"' % self.hdc_shell
        p = subprocess.Popen(dump_command,stdout=dump_fd_out,bufsize=1,shell=True)
        p.communicate(timeout=COMMAND_TIMEOUT)
        dump_fd_out.close()
        p.kill()
        logger.info("_get_dump_data end ")

    def get_ps_data(self,crash_path=None):
        ps_fd_out = open(os.path.join(self.log_path if crash_path is None else crash_path, "phone_ps_data.log"),'a')
        ps_command = '%s "ps -efw -ORES,Virt"' % self.hdc_shell
        logger.info(ps_command)
        p = subprocess.Popen(ps_command,stdout=ps_fd_out,bufsize=1,shell=True)
        p.communicate(timeout=COMMAND_TIMEOUT)
        ps_fd_out.close()
        p.kill()
        logger.info("get_ps_data end ")

    def _get_meminfo_data(self, crash_path=None):
        meminfo_fd_out = open(os.path.join(self.log_path if crash_path is None else crash_path, "phone_meminfo_data.log"), 'a')
        meminfo_command = '%s "cat /proc/meminfo"' % self.hdc_shell
        logger.info(meminfo_command)
        p = subprocess.Popen(meminfo_command, stdout=meminfo_fd_out, bufsize=1, shell=True)
        p.communicate(timeout=COMMAND_TIMEOUT)
        meminfo_fd_out.close()
        p.kill()
        logger.info("get_meminfo_data end ")

    def get_process_heap_info(self, crash_path=None):
        process_heap_info_out = open(
            os.path.join(self.log_path if crash_path is None else crash_path, "phone_process_heap_info.log"), 'a')
        dump_command = '%s "cat /d/ion/heaps/process_heap_info"' % self.hdc_shell
        p = subprocess.Popen(dump_command, stdout=process_heap_info_out, bufsize=1, shell=True)
        p.communicate(timeout=COMMAND_TIMEOUT)
        process_heap_info_out.close()
        p.kill()
        logger.info("get_process_heap_info end ")

    def get_hidumper_mem(self, crash_path=None):
        hidumper_mem_out = open(
            os.path.join(self.log_path if crash_path is None else crash_path, "phone_hidumper_mem.log"), 'a')
        hidump_command = '%s "hidumper --mem"' % self.hdc_shell
        logger.info(hidump_command)
        p = subprocess.Popen(hidump_command, stdout=hidumper_mem_out, bufsize=1, shell=True)
        try:
            p.communicate(timeout=COMMAND_TIMEOUT)
        except subprocess.TimeoutExpired as e:
            logger.info("get_hidumper_mem TimeoutExpired"+str(e))
        finally:
            hidumper_mem_out.close()
            p.kill()
            logger.info("hidumper_mem_out.close end ")
        logger.info("get_hidumper_mem end ")

    def hdc_execute_log(self):
        hdc_log_path = "/data/local/tmp/hdc.log"
        recv_command = "%s file recv %s %s" % (self.hdc_cmd,hdc_log_path,self.log_path)
        exec_cmd(recv_command,wait_out=True)

    def get_uptime_log(self, crash_path=None):
        uptime_fd = open(
            os.path.join(self.log_path if crash_path is None else crash_path, "uptime.log"), 'a')
        uptime_command = '%s "uptime -p"' % self.hdc_shell
        logger.info(uptime_command)
        p = subprocess.Popen(uptime_command, stdout=uptime_fd, bufsize=1, shell=True)
        p.communicate(timeout=COMMAND_TIMEOUT)
        uptime_fd.close()
        p.kill()
        logger.info("get_uptime_log end ")

    def get_fault_debug_log(self):
        debug_log_path = os.path.join(self.log_path,"faulog_debug")
        if not os.path.exists(debug_log_path):
            os.makedirs(debug_log_path)
        debug_command = "%s ls /data/log/faultlog/debug" % self.hdc_shell
        completed_process = subprocess.run(debug_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print(completed_process.returncode)
        if completed_process.returncode is None:
            return
        data = completed_process.stdout.decode().strip()
        debug_log_array = data.replace("\r","").split("\n")
        for log_name in debug_log_array:
            phone_log_path = os.path.join(CatchLog.DEBUG_LOG_PATH,log_name)
            pull_cmd = "%s file recv %s %s" % (self.hdc_cmd,phone_log_path,debug_log_path)
            exec_cmd(pull_cmd,wait_out=True)
        logger.info("get_fault_debug_log end ")

if __name__ == '__main__':
    r_path = r"D:\temp\catch_log_test"
    if not os.path.exists(r_path):
        os.makedirs(r_path)
    catch_log = CatchLog(r_path,"7001005458323933328a260ed7ee3900","hdc_std")
    catch_log.get_fault_debug_log()
    catch_log.get_uptime_log()
    catch_log.hdc_execute_log()
    catch_log.get_hidumper_mem()
    catch_log.get_process_heap_info()
    catch_log._get_meminfo_data()
    catch_log.get_ps_data(r_path)
    catch_log.get_current_phone_status(r_path)
    catch_log._kill_hilog_process()
    catch_log.get_hilog_buffer_data(r_path)
    catch_log.get_cur_hilog_data(r_path)
    #catch_log.get_cur_crash_log(r_path,test1)



