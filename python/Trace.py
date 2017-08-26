import os, sys, re
import fcntl
import argparse
import logging
import serial
import datetime

import TraceDecoder

_CSV_HEADER = 'timestamp, counter, epoch [ms], message, raw hex'

parser = argparse.ArgumentParser(description='trace - trace decoder application')
parser.add_argument('-i', '--input', choices=['file', 'serial'], default='serial')
parser.add_argument('path', nargs='?', default='/dev/ttyUSB0:576000:8N1',
                   help='path to file or serial port identifier')
parser.add_argument('-d', '--debug', default=False, action='store_true')
args = parser.parse_args()

if args.debug:
    lv = logging.DEBUG
else:
    lv = logging.INFO

logging.basicConfig(level=lv,
                    format='%(asctime)s.%(msecs)03d %(levelname)-8s %(message)s',
                    datefmt='%m-%d %H:%M:%S',
                    filemode='w')

try:
    if args.input == 'serial':
        logging.info('Trying to open serial port: ' + args.path)
        rs = re.match('([^:]+):([0-9]+):([5678])([NOE])([12])', args.path)
        if rs:
            breader = serial.Serial(rs.group(1), rs.group(2), timeout=None, bytesize=int(rs.group(3)),
                                parity=rs.group(4), stopbits=int(rs.group(5)))
            fcntl.flock(breader.fileno(), fcntl.LOCK_EX | fcntl.LOCK_NB)
            reader = TraceDecoder.SerialTraceReader(breader, 1)
        else:
            raise ValueError('Wrong serial port format: ' + args.path)
    else:
        logging.info('Trying to open file: ' + args.path)
        breader = open(args.path, 'rb')
        reader = TraceDecoder.FileTraceReader(breader, 16)

    try:
        ts_str = datetime.datetime.now().strftime("%Y-%m-%d-%H:%M:%S")
        wname = os.path.basename(args.path) + '.csv'
        logging.info('Opening output file: ' + wname)
        bwriter = open(wname, 'wb')
        exporter = TraceDecoder.CSVTraceExporter(bwriter, _CSV_HEADER)
        try:
            decoder = TraceDecoder.TraceDecoder(reader, exporter)
            decoder.decode()
        finally:
            logging.debug('Closing the writer')
            bwriter.close()
            os.rename(bwriter.name, ts_str + '-' + os.path.basename(bwriter.name))
    finally:
        logging.debug('Closing the reader')
        breader.close()

except KeyboardInterrupt:
    logging.info('Exiting due to user keyboard press')
except (ValueError, IOError, serial.SerialException):
    logging.error(sys.exc_info()[1])
