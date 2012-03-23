#!/usr/bin/env python

"""A sql to c++ compiler.

Usage: sql2cpp.py file.sql -o filename.cpp

generates file.cpp with types parsed from file.sql
by default it outputs to out.h, use -o to change it

"""

import sqlparse, sys, os, string
from optparse import OptionParser

OUTPUT = "out.h"

def to_cpp_type(sqltype):
    if sqltype == "int":
        return "int"
    elif sqltype == "text":
        return "QString"
    elif sqltype == "timestamp":
        return "QDateTime"
    elif sqltype == "char(5)":
        return "QByteArray"
    else:
        print "Unknown type %s" % sqltype
        return "int"

def parse_create(statement, cpp):
    create_type = statement.token_next(2)
    create_type = create_type.to_unicode()
    if create_type == "table":
        token = statement.token_next(4)
        tablename = string.split(token.to_unicode())[0]
        cpp.write("struct %s\n" % string.capitalize(tablename))
        cpp.write("{\n");

        token = token.token_next(0);

        idx = 0;
        while token.token_next(idx) and not token.token_next(idx).match(sqlparse.tokens.Punctuation, ')'):
            token_name = token.token_next(idx).to_unicode()
            idx += 3;
            token_type = token.token_next(idx).to_unicode()
            while token.token_next(idx) and not token.token_next(idx).match(sqlparse.tokens.Punctuation, ','):
                idx += 1
            idx += 1
#            print "token = %s (%s)" %(token_name, token_type);
            cpp.write("    %s %s;\n" %(to_cpp_type(token_type), token_name))

        cpp.write("};\n\n");

def sql2cpp(filename, output, verbose):
    f = open(filename, 'r')
    sql = f.read();
    f.close()
    print "output is %s " % output
    cpp = open(output, 'w')
    header = string.replace(output, ".", "_")
    header = string.upper(header)
    cpp.write("#ifndef %s\n" % header)
    cpp.write("#define %s\n\n" % header);
    parsed = sqlparse.parse(sql)

    for statement in parsed:
        if statement.get_type() == "CREATE":
            parse_create(statement, cpp)

    cpp.write("\n#endif /* %s */\n" % header);
    cpp.close();

def main():
    usage = "usage: %prog [options] arg"
    parser = OptionParser(__doc__)
    parser.add_option("-o", "--output", dest="output", default=OUTPUT,
                      help="write data to FILENAME")
    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose")
    (options, args) = parser.parse_args(sys.argv[1:])

    if len(args) != 1:
        parser.print_help("missing sql script to compile")
        return 1

    filename = args[0]
    if options.verbose:
        print "Reading %s..." % filename
    return sql2cpp(filename, options.output, options.verbose)

if __name__ == "__main__":
    main()



