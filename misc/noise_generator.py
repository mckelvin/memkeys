# coding: utf-8
import sys
import itertools
import time
import libmc


def main(argv):
    if len(argv) != 2:
        print >> sys.stderr, "Usage: python %s hostname:11211" % argv[0]
        return 1

    s = argv[1]
    if s.isdigit():
        server_addr = "%s:%s" % ('127.0.0.1', s)
    else:
        server_addr = s

    mc = libmc.Client([server_addr])
    noise_data = []
    for key_prefix in ('foo', 'bar', 'baz'):
        for value_size in (1, 10, 100, 1000, 10000):
            key = '%s_with_value_size_%s' % (key_prefix, value_size)
            val = '1' * value_size
            mc.set(key, val)
            noise_data.append((key, val))

    for (key, val) in itertools.cycle(noise_data):
        val2 = mc.get(key)
        assert val2 == val
        mc.set(key, val)
        time.sleep(0.5)


if __name__ == '__main__':
    sys.exit(main(sys.argv))
