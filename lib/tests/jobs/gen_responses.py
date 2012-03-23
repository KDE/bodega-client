#!/usr/bin/env python

import ConfigParser
import getopt
import glob
import os
import re
import string
import sys

from mechanize import Browser
from mechanize import HTTPError
from mechanize import URLError

#SERVER="bodega.coherenttheory.com"
SERVER="localhost:3000"
perform = False

def handle_section(config, section):
    if string.find(section, '/') >= 0:
        splits = string.split(section, '/')
        group = splits[0]
        idx = splits[1]
        query = config.get(section, 'url')
        query = string.strip(query, "\"")
        filename = "responses/" + group + idx + ".txt"
        url = 'http://' + SERVER + query
        if perform:
            print 'Working on ' + filename + ': ' + url
            try:
                res = browser.open(url)
            except HTTPError, e:
                print 'The server couldn\'t fulfill the request for ' + url
                print 'Error code: ', e.code
            except URLError, e:
                    print 'We failed to reach a server.'
                    print 'Reason: ', e.reason
            else:
                data = res.read()
                f = open(filename, 'w')
                f.write(data)
                #print "data %s" % data
        else:
            print filename + ': ' + url

def process_queries(path):
    # not ideal but python config parser does not reatain the ordering
    #  of sections in the file so we need another way of making sure that
    #  the signon tests run first
    config = ConfigParser.RawConfigParser()
    config.read(path)
    for section in sorted(config.sections()):
        handle_section(config, section)

try:
    opts, args = getopt.getopt(sys.argv[1:], "p", ["perform"])
except getopt.GetoptError, err:
    # print help information and exit:
    print str(err) # will print something like "option -a not recognized"
    usage()
    sys.exit(2)

for o, a in opts:
    if o in ("-p", "--perform"):
        perform = True
        browser = Browser()

for file in sorted(glob.glob('queries/*.conf')):
    process_queries(file)

