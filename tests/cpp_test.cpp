#include "jrep.h"

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

int get_stream_size(ifstream& stream) {
    int start_pos = stream.tellg();
    stream.seekg(0, stream.end);
    int length = stream.tellg();
    stream.seekg(start_pos);
    return length;
}

void parse_files(int argc, char** argv) {
    int i;
    for (i = 1; i < argc; i++) {
        char* buf;
        JParser parser;
        ifstream stream(argv[i]);
        int ssize = get_stream_size(stream);
        buf = new char[ssize + 2];
        buf[ssize] = '\0';
        buf[ssize+1] = '\0';
        stream.read(buf, ssize);
        //cout << buf << endl;
        parser.scan_string(buf);
        JPtr jrep;
        int ret = parser.parse(jrep);
        if (ret == 0){
            jrep->fix_depth(0);
            cout << jrep->representation() << endl;
        } else {
            cout << "unable to parse: " << argv[i] << endl;
        }
        delete[] buf;
    }
}

void parse_stdin() {

        JParser parser;
        JPtr jrep;
        int ret = parser.parse(jrep);
        if (ret == 0){
            jrep->fix_depth(0);
            cout << jrep->representation() << endl;
        }
}

int main(int argc, char** argv) {
    if (argc > 1) {
        parse_files(argc, argv);
    } else {
        parse_stdin();
    }
    return 0;
}
