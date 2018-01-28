#include <bits/stdc++.h>

using namespace std;

string tostr(int x) {
    string s;
    if (x == 0) return "0";
    while (x) {
        s += '0' + (x % 10);
        x /= 10;
    }
    reverse(s.begin(), s.end());
    return s;
}

int main() {
    string files = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ?0123456789";
    
    for (char f : files) {
        ifstream myfile;
        string ff = ""; ff += f;
        myfile.open ("" + ff + ".txt");

        ofstream ofile; ofile.open(tostr(f) + ".txt");

        int i = 0;
        string tmp;
        while (!myfile.eof()) {
            getline(myfile, tmp);
            for (int j = 0; j < tmp.length(); j++)
                if (tmp[j] == '1')
                    ofile<<"put_pixel "<<j<<" "<<i<<endl;
            i++;
        }

        myfile.close();
        ofile.close();
    }

    return 0;
}