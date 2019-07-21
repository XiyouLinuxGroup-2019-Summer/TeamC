#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

class Cow {
    public:
        Cow(int n_,vector <int> &v_) : n(n_),v(v_),sum(0) {}
        int compute(int n_,vector <int> &v) {
            sort(v.begin(),v.end());
            if(n % 2 == 0) {
                int i = v.size() - 1;
                while(i >= 1) {
                    sum += v[i] - v[i-1];
                    i -= 2;
                }
            }
            else {
                int i = v.size() -1;
                while(i > 0) {
                    sum += v[i] - v[i-1];
                    i -= 2;
                }
            }
            sum += v[0];
            return sum;
        }
    private:
        int n;
        vector <int> v;
        int sum;
}; 
int main (void) {
    vector <int> v(10);
    int n,t;
    cin >> n;
    for(int i = 0;i < n;i++) {
        cin >> t;
        v.push_back(t);
    }
    Cow c(n,v);
    cout << c.compute(n,v) << endl;
    return 0;
}