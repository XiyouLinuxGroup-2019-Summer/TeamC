#include <iostream>
using namespace std;
const int MAX = 1000+100;
int main()
{
    int ans = 0;
    int arr[MAX];
    int n,a,b,c,t;
    cin >> n >> a >> b >> c >> t;
    for(int i = 0;i<n;i++)
        cin >> arr[i];

    if(b >= c)
        ans = n*a;
    else 
        for(int i = 0;i<n;i++)
            ans += (t-arr[i])*c + (a - (t-arr[i])*b);

    cout << ans << endl;
    return 0;
}


