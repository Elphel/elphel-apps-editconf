#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <string>
#include <map>
#include <fstream>

using namespace std;

void load(string fn, map<string, string> &c) {
	int i;
	char *z;
	string s;
	ifstream ifile(fn.c_str());
	while(!ifile.eof()) {
		ifile >> s;
		z = const_cast<char *>(s.c_str());
		for(i = 0; z[i]; i++)
			if(z[i] == '=') {
				z[i] = '\0';
				c[&z[0]] = &z[i + 1];
				break;
			}
	}
	ifile.close();
/*
	for(map<string, string>::iterator it = c.begin(); it != c.end(); it++)
		syslog(LOG_ALERT, "|%s|==|%s|\r\n", (*it).first.c_str(), (*it).second.c_str());
syslog(LOG_ALERT, "//||\\\\--------------------------- %s\r\n", fn.c_str());
*/
}

void save(string fn, map<string, string> &c) {
	string s;
	ofstream of(fn.c_str());
/*
	for(map<string, string>::iterator it = c.begin(); it != c.end(); it++)
		syslog(LOG_ALERT, "|%s|==|%s|\r\n", (*it).first.c_str(), (*it).second.c_str());
*/
	for(map<string, string>::iterator it = c.begin(); it != c.end(); it++) {
		s = (*it).first;
		s += "=";
		s += (*it).second;
		of << s;
		of << endl;
	}
	of.close();
}

int main() {
	const char *uri;
	const char *method;
	const char *query;
//	map<string, string> in
//	string in;
//	string out;
	map<string, string> params;
	map<string, string> in;
	map<string, string> out;

	method = getenv("REQUEST_METHOD");
	query = getenv("QUERY_STRING");
	uri = getenv("REQUEST_URI");
/*
syslog(LOG_ALERT, "REQUEST_METHOD=|%s|\r\n", method);
syslog(LOG_ALERT, "REQUEST_STRING=|%s|\r\n", query);
syslog(LOG_ALERT, "REQUEST_URI=|%s|\r\n", uri);

syslog(LOG_ALERT, "strconf!\r\n");
*/
	// parse GET params
	string p_s = query;
	char *p = const_cast<char *>(p_s.c_str());
//syslog(LOG_ALERT, "string: |%s|\r\n", p);
	int i, j, k;
	k = 0;
	bool f = false, to_break = false;
	for(i = 0; ; i++) {
		if(p[i] == '\'')
			f = !f;
		if((p[i] == '&' || p[i] == '\0') && !f) {
			if(p[i] == '\0')
				to_break = true;
			else
				p[i] = '\0';
			for(j = k; p[j]; j++) {
				if(p[j] == '=') {
					p[j] = '\0';
					params[&p[k]] = &p[j + 1];
					break;
				}
			}
			k = i + 1;
			f = false;
		}
		if(to_break)
			break;
	}
	// load 'in' and 'out' file;
	load(params["in"], in);
	load(params["out"], out);
///*
	in["init_1"] = out["init_1"];
	in["init_2"] = out["init_2"];
///*
	if(params["init_1"] != "''")
		in["init_1"] = params["init_1"];
	if(params["init_2"] != "''")
		in["init_2"] = params["init_2"];
//*/
	save(params["out"], in);
//*/
//	for(map<string, string>::iterator it = params.begin(); it != params.end(); it++) {
//		syslog(LOG_ALERT, "param: |%s|==|%s|\r\n", (*it).first.c_str(), (*it).second.c_str());
//	}

	return 0;
}
