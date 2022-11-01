#include<bits/stdc++.h>
using namespace std;

/* constants */
const int INF = 0x3f3f3f3f;
const int maxnI = 105, maxnJ = 35, maxnK = 35, nPeriod = 25;

class Schedule{
public:
    const int &nI, &nJ, &nK;
    int targetFunction;
    const vector<vector<int>>& shiftType;
    vector<vector<int>> demand;
    vector<vector<int>> shiftContribution, assignedSchedule;

    Schedule(int& _nI, int& _nJ, int& _nK, vector<vector<int>>& _shiftType, vector<vector<int>> _demand): nI(_nI), nJ(_nJ), nK(_nK), shiftType(_shiftType), demand(_demand){
        targetFunction = 0;
        shiftContribution.resize(nJ);
        for(vector<int>& v:shiftContribution) v.resize(nK + 1, 0);
        assignedSchedule.resize(nI);
        for(vector<int>& v:assignedSchedule) v.resize(nJ, 0);

        for(vector<int>& v:demand) for(int& i:v) targetFunction += i;
        for(int i = 0; i < nJ; i++)
            for(int j = 1; j <= nK; j++)
                for(int k = 0; k < 24; k++)
                    shiftContribution[i][j] += (shiftType[j][k] && demand[i][k]);
    }

    void assignShift(int num, int day, int shift, int contribution){
        assignedSchedule[num][day] = shift;
        targetFunction -= contribution;
        for(int i = 0; i < 24; i++) demand[day][i] -= (shiftType[shift][i] && demand[day][i]);

        for(int j = 1; j <= nK; j++){
            int tmp = 0;
            for(int k = 0; k < 24; k++)
                tmp += (shiftType[j][k] && demand[day][k]);
            shiftContribution[day][shift] = tmp;
        }
    }

    void output(){
        //cout << "target: " << targetFunction << '\n';
        for(vector<int>& v:assignedSchedule) for(int& i:v)
            cout << i << ",\n"[&i == &(v.back())];
    }
};

bool isNightShift(int shift, const vector<vector<int>>& shiftType){
    return (accumulate(begin(shiftType[shift]) + 18, end(shiftType[shift]), 0) > 0);
}

class Employee_General{
public:
    const int &nI, &nJ, &nK, &L, &w1, &w2;
    vector<vector<bool>>& request;
    vector<vector<pair<int,int>>> employeeBestContribution;

    Employee_General(int& _nI, int& _nJ, int& _nK, int& _L, int& _w1, int& _w2, vector<vector<bool>>& _request, Schedule schedule): nI(_nI), nJ(_nJ), nK(_nK), L(_L), w1(_w1), w2(_w2), request(_request){
        employeeBestContribution.resize(nI);
        for(vector<pair<int,int>>& v:employeeBestContribution) v.resize(nJ);
        for(int i = 0; i < nI; i++)
            for(int j = 0; j < nJ; j++)
                employeeBestContribution[i][j] = make_pair(*max_element(begin(schedule.shiftContribution[j]), end(schedule.shiftContribution[j])),
                                                            (max_element(begin(schedule.shiftContribution[j]), end(schedule.shiftContribution[j])) - begin(schedule.shiftContribution[j])) * (-1));
    }

    void update(int num, int day, Schedule& schedule){
        if(employeeBestContribution[num][day].first == -1) return;

        employeeBestContribution[num][day].first = -1;
        int daysWorking = 0;
        for(int& i:schedule.assignedSchedule[num]) daysWorking += (i != 0);
        if(daysWorking >= schedule.nJ - L){
            for(int i = 0; i < nJ; i++) employeeBestContribution[num][i].first = -1;
            return;
        }

        int consecWork = 1;
        for(int j = day - 1; j >= max(0, j - 6); j--){
            if(!schedule.assignedSchedule[num][j]) break;
            consecWork++;
        }
        for(int j = day + 1; j <= min(schedule.nJ - 1, day + 6); j++){
            if(!schedule.assignedSchedule[num][j]) break;
            consecWork++;
        }
        if(consecWork >= 7) return;

        if(schedule.assignedSchedule[num][day]) return;
        for(int j = 1; j <= schedule.nK; j++){
            int tmpCon = schedule.shiftContribution[day][j];
            if(isNightShift(j, schedule.shiftType))
                for(int k = max(0, day - 6); k <= min(schedule.nJ - 1, day + 6); k++)
                    if(isNightShift(schedule.assignedSchedule[num][k], schedule.shiftType)){
                        int Lday = min(k, day), Rday = max(k, day);
                        tmpCon -= w2 * (min(Lday - max(Rday - 6, 0), max(Lday + 6, schedule.nJ - 1) - Rday) + 1);
                    }
            
            if(request[num][day]) tmpCon -= w1;
            employeeBestContribution[num][day] = max(make_pair(tmpCon, -j), employeeBestContribution[num][day]);
        }
    }

    void getBestAssignment(int& bestEmp, int& bestDay, int& bestShift, int& bestCon){
        bestCon = 0;
        for(int i = 0; i < nI; i++) for(int j = 0; j < nJ; j++)
            if(employeeBestContribution[i][j].first > bestCon){
                bestCon = employeeBestContribution[i][j].first;
                bestEmp = i;
                bestDay = j;
                bestShift = -employeeBestContribution[i][j].second;
            }
    }
};

class Employee_Priority{
public:
    const int &nI, &nJ, &nK, &L, &w1, &w2;
    vector<vector<bool>>& request;
    vector<vector<tuple<int,bool,bool,int>>> employeeBestContribution;

    Employee_Priority(int& _nI, int& _nJ, int& _nK, int& _L, int& _w1, int& _w2, vector<vector<bool>>& _request, Schedule& schedule): nI(_nI), nJ(_nJ), nK(_nK), L(_L), w1(_w1), w2(_w2), request(_request){
        employeeBestContribution.resize(nI);
        for(auto& v:employeeBestContribution) v.resize(nJ, tuple<int,bool,bool,int>(0, 0, 0, 0));
    }

    void update(int num, int day, Schedule& schedule){
        if(get<0>(employeeBestContribution[num][day]) == -1) return;

        employeeBestContribution[num][day] = make_tuple(-1, 0, 0, 0);
        int daysWorking = 0;
        for(int& i:schedule.assignedSchedule[num]) daysWorking += (i != 0);
        if(daysWorking >= schedule.nJ - L){
            for(int i = 0; i < nJ; i++) get<0>(employeeBestContribution[num][i]) = -1;
            return;
        }

        int consecWork = 1;
        for(int j = day - 1; j >= max(0, j - 6); j--){
            if(!schedule.assignedSchedule[num][j]) break;
            consecWork++;
        }
        for(int j = day + 1; j <= min(schedule.nJ - 1, day + 6); j++){
            if(!schedule.assignedSchedule[num][j]) break;
            consecWork++;
        }
        if(consecWork >= 7) return;

        if(schedule.assignedSchedule[num][day]) return;
        for(int j = 1; j <= schedule.nK; j++){
            int tmpCon = schedule.shiftContribution[day][j];
            if(isNightShift(j, schedule.shiftType))
                for(int k = max(0, day - 6); k <= min(schedule.nJ - 1, day + 6); k++)
                    if(isNightShift(schedule.assignedSchedule[num][k], schedule.shiftType)){
                        int Lday = min(k, day), Rday = max(k, day);
                        tmpCon -= w2 * (min(Lday - max(Rday - 6, 0), max(Lday + 6, schedule.nJ - 1) - Rday) + 1);
                    }
            
            if(request[num][day]) tmpCon -= w1;
            employeeBestContribution[num][day] = max(employeeBestContribution[num][day], make_tuple(tmpCon, !isNightShift(j, schedule.shiftType), !request[num][day], -j));
        }
    }

    void getBestAssignment(int& bestEmp, int& bestDay, int& bestShift, int& bestCon){
        tuple<int,bool,bool,int> bestTp = make_tuple(-1, 0, 0, 0);
        for(int i = 0; i < nI; i++) for(int j = 0; j < nJ; j++){
            if(employeeBestContribution[i][j] > bestTp){
                bestTp = employeeBestContribution[i][j];
                bestEmp = i;
                bestDay = j;
                bestShift = -get<3>(bestTp);
                bestCon = get<0>(bestTp);
            }
        }
    }
};

/* function definitions */
int main(){
    cin.tie(0);
    ios_base::sync_with_stdio(0);

    int nI, nJ, nK, L, w1, w2, R;
    cin >> nI >> nJ >> nK >> L >> w1 >> w2 >> R;
    
    vector<vector<int> > shiftType;
    int temp;
    for(int i = 0; i < nK; i++)
    {
        vector<int> row;
        for(int j = 0; j < 24; j++)
        {
            cin >> temp;
            row.push_back(temp);
        }
        shiftType.push_back(row);
    }

    vector<int> row;
    for(int i = 0; i < 24; i++)
    {
        cin >> temp;
        row.push_back(temp);
    }
    shiftType.insert(begin(shiftType), row);
    
    vector<vector<int> > demand;
    for(int i = 0; i < nJ; i++)
    {
        vector<int> row;
        for(int j = 0; j < 24; j++)
        {
            cin >> temp;
            row.push_back(temp);
        }
        demand.push_back(row);
    }
    
    vector<vector<bool> > request(nI, vector<bool>(nJ, 0));
    vector<int> row2;
    for(int i = 0; i < R; i++)
    {
        cin >> temp;
        row2.push_back(temp);
    }
    for(int i = 0; i < R; i++){
        cin >> temp;
        request[row2[i] - 1][temp - 1] = 1;
    }

    Schedule schedule1(nI, nJ, nK, shiftType, demand), schedule2(nI, nJ, nK, shiftType, demand);
    Employee_General employee1(nI, nJ, nK, L, w1, w2, request, schedule1);
    Employee_Priority employee2(nI, nJ, nK, L, w1, w2, request, schedule2);
    for(int i = 0; i < nI; i++) for(int j = 0; j < nJ; j++) employee2.update(i, j, schedule2);

    while(true){
        int bestEmp = 0, bestDay = 0, bestShift = 0, bestCon = 0;
        employee1.getBestAssignment(bestEmp, bestDay, bestShift, bestCon);
        if(bestCon <= 0) break;
        else schedule1.assignShift(bestEmp, bestDay, bestShift, bestCon);
        
        for(int i = max(0, bestDay - 6); i <= min(bestDay + 6, nJ - 1); i++)
            employee1.update(bestEmp, i, schedule1);
        for(int i = 0; i < nI; i++) employee1.update(i, bestDay, schedule1);
    }

    while(true){
        int bestEmp = 0, bestDay = 0, bestShift = 0, bestCon = 0;
        employee2.getBestAssignment(bestEmp, bestDay, bestShift, bestCon);
        if(bestCon <= 0) break;
        else schedule2.assignShift(bestEmp, bestDay, bestShift, bestCon);
        
        for(int i = max(0, bestDay - 6); i <= min(bestDay + 6, nJ - 1); i++)
            employee2.update(bestEmp, i, schedule2);
        for(int i = 0; i < nI; i++) employee2.update(i, bestDay, schedule2);
    }

    if(schedule1.targetFunction < schedule2.targetFunction) schedule1.output();
    else schedule2.output();
    return 0;
}