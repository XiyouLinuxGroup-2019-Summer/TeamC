#include <iostream>
using namespace std;

class Chocolate {
    public:
        Chocolate(int m_,int day_) :m(m_),day(day_) {}
        int Sum(int m_,int day_) {
            int sum = 0;
            for(int i = 1;i <= day_;i++) {
                sum += m_;
                m = (m % 2 == 0) ? (m/2):(m/2+1);
            }
            return sum;
        }
        int Find(int max,int day_) {
            int l =1;
            int r = max;
            int sum = 0;
            int t = r - l;
            t = (t % 2 == 0) ? t/2 : t/2 +1;
            int mid = l + t;
            if(day_ == max)
                return 1;
            while(l <= r) {
                sum = Sum(mid,day_);
                if(sum < max)
                    l = mid + 1;
                else if(sum == max)
                    return mid;
                else 
                    r = mid - 1;
                t = r - l;
                t = (t % 2 == 0) ? t/2 : t/2 +1;
                mid = l + t;
            }
            return mid;      
        }
    private:
        int m;
        int day;
};
int main () {
    int m,day;
    cin >> m >> day;
    Chocolate s(m,day);
    cout << s.Find(m,day) << endl;
    return 0;
}