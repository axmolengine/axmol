#!/usr/bin/python
# ----------------------------------------------------------------------------
# statistics: Statistics the user behaviors of cocos2d-console by google analytics
#
# Author: Bin Zhang
#
# License: MIT
# ----------------------------------------------------------------------------
'''
Statistics the user behaviors of cocos2d-console by google analytics
'''

import cocos
import uuid
import locale
import httplib
import urllib
import platform
import sys
import os
import json
import time
import socket
import hashlib
import datetime
import zlib

import multiprocessing

# GA related Constants

GA_HOST        = 'www.google-analytics.com'
GA_PATH        = '/collect'
GA_APIVERSION  = '1'
APPNAME     = 'CocosConcole'

TIMEOUT_VALUE = 0.5

# formal tracker ID
GA_TRACKERID = 'UA-60734607-3'

# debug tracker ID
# GA_TRACKERID = 'UA-60530469-4'

# BI related Constants
BI_HOST  = 'ark.cocounion.com'
BI_PATH  = '/as'
BI_APPID = '433748803'

GA_ENABLED = True
BI_ENABLED = False

class Fields(object):
    API_VERSION     = 'v'
    TRACKING_ID     = 'tid'
    HIT_TYPE        = 't'
    CLIENT_ID       = 'cid'
    EVENT_CATEGORY  = 'ec'
    EVENT_ACTION    = 'ea'
    EVENT_LABEL     = 'el'
    EVENT_VALUE     = 'ev'
    APP_NAME        = 'an'
    APP_VERSION     = 'av'
    USER_LANGUAGE   = 'ul'
    USER_AGENT      = 'ua'
    SCREEN_NAME     = "cd"
    SCREEN_RESOLUTION = "sr"


GA_CACHE_EVENTS_FILE = 'cache_events'
GA_CACHE_EVENTS_BAK_FILE = 'cache_event_bak'

local_cfg_path = os.path.expanduser('~/.cocos')
local_cfg_file = os.path.join(local_cfg_path, GA_CACHE_EVENTS_FILE)
local_cfg_bak_file = os.path.join(local_cfg_path, GA_CACHE_EVENTS_BAK_FILE)
file_in_use_lock = multiprocessing.Lock()
bak_file_in_use_lock = multiprocessing.Lock()


BI_CACHE_EVENTS_FILE = 'bi_cache_events'
bi_cfg_file = os.path.join(local_cfg_path, BI_CACHE_EVENTS_FILE)
bi_file_in_use_lock = multiprocessing.Lock()

def get_user_id():
    node = uuid.getnode()
    mac = uuid.UUID(int = node).hex[-12:]

    uid = hashlib.md5(mac).hexdigest()
    return uid

def get_language():
    lang, encoding = locale.getdefaultlocale()
    return lang

def get_user_agent():
    ret_str = None
    if cocos.os_is_win32():
        ver_info = sys.getwindowsversion()
        ver_str = '%d.%d' % (ver_info[0], ver_info[1])
        if cocos.os_is_32bit_windows():
            arch_str = "WOW32"
        else:
            arch_str = "WOW64"
        ret_str = "Mozilla/5.0 (Windows NT %s; %s) Chrome/33.0.1750.154 Safari/537.36" % (ver_str, arch_str)
    elif cocos.os_is_mac():
        ver_str = (platform.mac_ver()[0]).replace('.', '_')
        ret_str = "Mozilla/5.0 (Macintosh; Intel Mac OS X %s) Chrome/35.0.1916.153 Safari/537.36" % ver_str
    elif cocos.os_is_linux():
        arch_str = platform.machine()
        ret_str = "Mozilla/5.0 (X11; Linux %s) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1636.0 Safari/537.36" % arch_str

    return ret_str

def get_system_info():
    if cocos.os_is_win32():
        ret_str = "windows"
        ret_str += "_%s" % platform.release()
        if cocos.os_is_32bit_windows():
            ret_str += "_%s" % "32bit"
        else:
            ret_str += "_%s" % "64bit"
    elif cocos.os_is_mac():
        ret_str = "mac_%s" % (platform.mac_ver()[0]).replace('.', '_')
    elif cocos.os_is_linux():
        ret_str = "linux_%s" % platform.linux_distribution()[0]
    else:
        ret_str = "unknown"

    return ret_str

def get_python_version():
    return "python_%s" % platform.python_version()

def get_time_stamp():
    utc_dt = datetime.datetime.utcnow()
    local_dt = utc_dt + datetime.timedelta(hours=8)
    epoch = datetime.datetime(1970,1,1)
    local_ts = (local_dt - epoch).total_seconds()
    ret = '%d' % int(local_ts)

    return ret

def get_static_params(engine_version):
    static_params = {
        Fields.API_VERSION: GA_APIVERSION,
        Fields.TRACKING_ID: GA_TRACKERID,
        Fields.CLIENT_ID: get_user_id(),
        Fields.APP_NAME: APPNAME,
        Fields.HIT_TYPE: "event",
        Fields.USER_LANGUAGE: get_language(),
        Fields.APP_VERSION: engine_version,
        Fields.SCREEN_NAME: get_system_info(),
        Fields.SCREEN_RESOLUTION: get_python_version()
    }
    agent_str = get_user_agent()
    if agent_str is not None:
        static_params[Fields.USER_AGENT] = agent_str

    return static_params

def gen_bi_event(event, event_value):
    time_stamp = get_time_stamp()
    if event_value == 0:
        is_cache_event = '1'
    else:
        is_cache_event = '0'

    category = event[0]
    action = event[1]
    label = event[2]

    event_name = category
    params = {
        'cached_event' : is_cache_event
    }
    if category == 'cocos':
        if action == 'start':
            event_name = 'cocos_invoked'
        elif action == 'running_command':
            event_name = 'running_command'
            params['command'] = label
        else:
            params['category'] = category
            params['action'] = action
            params['label'] = label
    elif category == 'new':
        event_name = 'new_project'
        params['language'] = action
        params['template'] = label
    elif category == 'new_engine_ver':
        event_name = 'engine_info'
        params['version'] = action
        params['engine_type'] = label
    elif category == 'compile':
        params['language'] = action
        params['target_platform'] = label
    else:
        params['category'] = category
        params['action'] = action
        params['label'] = label

    if len(event) >= 4:
        appear_time = event[3]
    else:
        appear_time = time_stamp
    ret = {
        'u' : {
            '28' : get_user_id(),
            '34' : get_python_version()
        },
        'p' : params,
        's' : time_stamp,
        'e' : event_name,
        't' : appear_time
    }

    return ret

def get_bi_params(events, event_value, multi_events=False, engine_versio=''):
    if cocos.os_is_win32():
        system_str = 'windows'
        ver_info = sys.getwindowsversion()
        ver_str = '%d.%d' % (ver_info[0], ver_info[1])
        if cocos.os_is_32bit_windows():
            arch_str = "_32bit"
        else:
            arch_str = "_64bit"
        system_ver = '%s%s' % (ver_str, arch_str)
    elif cocos.os_is_mac():
        system_str = 'mac'
        system_ver = (platform.mac_ver()[0])
    elif cocos.os_is_linux():
        system_str = 'linux'
        system_ver = platform.machine()
    else:
        system_str = 'unknown'
        system_ver = 'unknown'

    events_param = []
    if multi_events:
        for e in events:
            events_param.append(gen_bi_event(e, event_value))
    else:
        events_param.append(gen_bi_event(events, event_value))

    params = {
        'device': {
            '10' : system_ver,
            '11' : system_str
        },
        'app': {
            '7' : BI_APPID,
            '8' : engine_version,
            '9' : get_language()
        },
        'time' : get_time_stamp(),
        'events' : events_param
    }

    return params

def cache_event(event, is_ga=True, multi_events=False):
    if is_ga:
        cache_ga_event(event)
    else:
        cache_bi_event(event, multi_events)

# BI cache events related methods
def cache_bi_event(event, multi_events=False):
    bi_file_in_use_lock.acquire()

    outFile = None
    try:
        # get current cached events
        cache_events = get_bi_cached_events(need_lock=False)

        if multi_events:
            need_cache_size = len(event)
        else:
            need_cache_size = 1

        # delete the oldest events if there are too many events.
        events_size = len(cache_events)
        if events_size >= Statistic.MAX_CACHE_EVENTS:
            start_idx = events_size - (Statistic.MAX_CACHE_EVENTS - need_cache_size)
            cache_events = cache_events[start_idx:]

        # cache the new event
        if multi_events:
            for e in event:
                cache_events.append(e)
        else:
            cache_events.append(event)

        # write file
        outFile = open(bi_cfg_file, 'w')
        json.dump(cache_events, outFile)
        outFile.close()
    except:
        if outFile is not None:
            outFile.close()
    finally:
        bi_file_in_use_lock.release()

def get_bi_cached_events(need_lock=True):
    if not os.path.isfile(bi_cfg_file):
        cached_events = []
    else:
        f = None
        try:
            if need_lock:
                bi_file_in_use_lock.acquire()

            f = open(bi_cfg_file)
            cached_events = json.load(f)
            f.close()

            if not isinstance(cached_events, list):
                cached_events = []
        except:
            cached_events = []
        finally:
            if f is not None:
                f.close()

            if need_lock:
                bi_file_in_use_lock.release()

    return cached_events

# GA cache events related methods
def get_ga_cached_events(is_bak=False, need_lock=True):
    if is_bak:
        cfg_file = local_cfg_bak_file
        lock = bak_file_in_use_lock
    else:
        cfg_file = local_cfg_file
        lock = file_in_use_lock

    if not os.path.isfile(cfg_file):
        cached_events = []
    else:
        f = None
        try:
            if need_lock:
                lock.acquire()

            f = open(cfg_file)
            cached_events = json.load(f)
            f.close()

            if not isinstance(cached_events, list):
                cached_events = []
        except:
            cached_events = []
        finally:
            if f is not None:
                f.close()
            if need_lock:
                lock.release()

    return cached_events

def cache_ga_event(event):
    file_in_use_lock.acquire()

    outFile = None
    try:
        # get current cached events
        cache_events = get_ga_cached_events(is_bak=False, need_lock=False)

        # delete the oldest events if there are too many events.
        events_size = len(cache_events)
        if events_size >= Statistic.MAX_CACHE_EVENTS:
            start_idx = events_size - (Statistic.MAX_CACHE_EVENTS - 1)
            cache_events = cache_events[start_idx:]

        # cache the new event
        cache_events.append(event)

        # write file
        outFile = open(local_cfg_file, 'w')
        json.dump(cache_events, outFile)
        outFile.close()
    except:
        if outFile is not None:
            outFile.close()
    finally:
        file_in_use_lock.release()

def pop_bak_ga_cached_event():
    bak_file_in_use_lock.acquire()
    events = get_ga_cached_events(is_bak=True, need_lock=False)

    if len(events) > 0:
        e = events[0]
        events = events[1:]
        outFile = None
        try:
            outFile = open(local_cfg_bak_file, 'w')
            json.dump(events, outFile)
            outFile.close()
        except:
            if outFile:
                outFile.close()
    else:
        e = None

    bak_file_in_use_lock.release()

    return e

def do_send_ga_cached_event(engine_version):
    e = pop_bak_ga_cached_event()
    while(e is not None):
        do_send(e, 0, is_ga=True, multi_events=False, engine_version=engine_version)
        e = pop_bak_ga_cached_event()

def get_params_str(event, event_value, is_ga=True, multi_events=False, engine_version=''):
    if is_ga:
        params = get_static_params(engine_version)
        params[Fields.EVENT_CATEGORY] = '2dx-' + event[0]
        params[Fields.EVENT_ACTION]   = event[1]
        params[Fields.EVENT_LABEL]    = event[2]
        params[Fields.EVENT_VALUE]    = '%d' % event_value
        params_str = urllib.urlencode(params)
    else:
        params = get_bi_params(event, event_value, multi_events, engine_version)
        strParam = json.dumps(params)
        params_str = zlib.compress(strParam, 9)

    return params_str

def do_http_request(event, event_value, is_ga=True, multi_events=False, engine_version=''):
    ret = False
    conn = None
    try:
        params_str = get_params_str(event, event_value, is_ga, multi_events, engine_version)
        if is_ga:
            host_url = GA_HOST
            host_path = GA_PATH
        else:
            host_url = BI_HOST
            host_path = BI_PATH

        socket.setdefaulttimeout(TIMEOUT_VALUE)

        conn = httplib.HTTPConnection(host_url, timeout=TIMEOUT_VALUE)
        conn.request(method="POST", url=host_path, body=params_str)

        response = conn.getresponse()
        res = response.status
        if res >= 200 and res < 300:
            # status is 2xx mean the request is success.
            ret = True
        else:
            ret = False
    except:
        pass
    finally:
        if conn:
            conn.close()

    return ret

def do_send(event, event_value, is_ga=True, multi_events=False, engine_version=''):
    try:
        ret = do_http_request(event, event_value, is_ga, multi_events, engine_version)
        if not ret:
            # request failed, cache the event
            cache_event(event, is_ga, multi_events)
    except:
        pass

class Statistic(object):

    MAX_CACHE_EVENTS = 50
    MAX_CACHE_PROC = 5

    def __init__(self, engine_version):
        self.process_pool = []
        self.engine_version = engine_version
        if cocos.os_is_win32():
            multiprocessing.freeze_support()

    def send_cached_events(self):
        try:
            # send GA cached events
            if GA_ENABLED:
                events = get_ga_cached_events()
                event_size = len(events)
                if event_size == 0:
                    return

                # rename the file
                if os.path.isfile(local_cfg_bak_file):
                    os.remove(local_cfg_bak_file)
                os.rename(local_cfg_file, local_cfg_bak_file)

                # create processes to handle the events
                proc_num = min(event_size, Statistic.MAX_CACHE_PROC)
                for i in range(proc_num):
                    p = multiprocessing.Process(target=do_send_ga_cached_event, args=(self.engine_version,))
                    p.start()
                    self.process_pool.append(p)

            # send BI cached events
            if BI_ENABLED:
                events = get_bi_cached_events()
                event_size = len(events)
                if event_size == 0:
                    return

                # remove the cached events file
                if os.path.isfile(bi_cfg_file):
                    os.remove(bi_cfg_file)

                p = multiprocessing.Process(target=do_send, args=(events, 0, False, True, self.engine_version,))
                p.start()
                self.process_pool.append(p)
        except:
            pass

    def send_event(self, category, action, label):
        try:
            event = [ category, action, label ]

            # send event to GA
            if GA_ENABLED:
                p = multiprocessing.Process(target=do_send, args=(event, 1, True, False, self.engine_version,))
                p.start()
                self.process_pool.append(p)

            # send event to BI
            if BI_ENABLED:
                # add timestamp
                event.append(get_time_stamp())
                p = multiprocessing.Process(target=do_send, args=(event, 1, False, False, self.engine_version,))
                p.start()
                self.process_pool.append(p)
        except:
            pass

    def terminate_stat(self):
        # terminate sub-processes
        if len(self.process_pool) > 0:
            alive_count = 0
            for p in self.process_pool:
                if p.is_alive():
                    alive_count += 1

            if alive_count > 0:
                time.sleep(1)
                for p in self.process_pool:
                    if p.is_alive():
                        p.terminate()

        # remove the backup file
        if os.path.isfile(local_cfg_bak_file):
            os.remove(local_cfg_bak_file)
