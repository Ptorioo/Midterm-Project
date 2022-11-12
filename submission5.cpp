#include<bits/stdc++.h>
using namespace std;
//#define DEBUG

/* constants */
const int INF = 0x3f3f3f3f;
const int maxnI = 105, maxnJ = 35, maxnK = 35, nPeriod = 25;

// TODO: Improve Messy Structure
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
                    shiftContribution[i][j] += (shiftType[j][k] && (demand[i][k] > 0));
    }

    void updateShiftCon(int day){
        for(int j = 1; j <= nK; j++){
            int tmp = 0;
            for(int k = 0; k < 24; k++)
                tmp += (shiftType[j][k] && (demand[day][k] > 0));
            shiftContribution[day][j] = tmp;
        }
    }
    
    void assignShift(int num, int day, int shift, int contribution){
        assignedSchedule[num][day] = shift;
        targetFunction -= contribution;
        for(int i = 0; i < 24; i++) demand[day][i] -= shiftType[shift][i];
        updateShiftCon(day);
    }

    void output(){
        for(vector<int>& v:assignedSchedule) for(int& i:v)
            cout << i << ",\n"[&i == &(v.back())];
    }
};

bool isNightShift(int shift, const vector<vector<int>>& shiftType){
    static vector<int> isNightShiftRes(35, -1);
    if(isNightShiftRes[shift] == -1)
        return isNightShiftRes[shift] = (accumulate(begin(shiftType[shift]) + 18, end(shiftType[shift]), 0) > 0);
    else return isNightShiftRes[shift];
}

class Employee{
public:
    const int &nI, &nJ, &nK, &L, &w1, &w2;
    vector<vector<bool>>& request;
    vector<vector<pair<int,int>>> employeeBestContribution;

    Employee(int& _nI, int& _nJ, int& _nK, int& _L, int& _w1, int& _w2, vector<vector<bool>>& _request): nI(_nI), nJ(_nJ), nK(_nK), L(_L), w1(_w1), w2(_w2), request(_request){
        employeeBestContribution.resize(nI);
        for(vector<pair<int,int>>& v:employeeBestContribution) v.resize(nJ, make_pair(0, 0));
    }

    void update(int num, int day, Schedule& schedule, bool notInitialize = 1){
        if(employeeBestContribution[num][day].first == -1) return;

        employeeBestContribution[num][day].first = -1;
        int daysWorking = 0;
        if(notInitialize){
            for(int& i:schedule.assignedSchedule[num]) daysWorking += (i != 0);
            if(daysWorking >= nJ - L){
                for(int i = 0; i < nJ; i++) employeeBestContribution[num][i].first = -1;
                return;
            }

            int consecWork = 1;
            for(int j = day - 1; j >= max(0, j - 6); j--){
                if(!schedule.assignedSchedule[num][j]) break;
                consecWork++;
            }
            for(int j = day + 1; j <= min(nJ - 1, day + 6); j++){
                if(!schedule.assignedSchedule[num][j]) break;
                consecWork++;
            }
            if(consecWork >= 7) return;
        }

        if(schedule.assignedSchedule[num][day]) return;
        for(int j = 1; j <= nK; j++){
            int tmpCon = schedule.shiftContribution[day][j] - request[num][day] * w1;
            if(notInitialize && isNightShift(j, schedule.shiftType)){
                int nightShiftCnt = 0;
                for(int start = max(0, day - 6); start <= min(day, nJ - 7); start++){
                    if(start == max(0, day - 6))
                        for(int k = start; k <= start + 6; k++) nightShiftCnt += isNightShift(schedule.assignedSchedule[num][k], schedule.shiftType);
                    else{
                        nightShiftCnt -= isNightShift(schedule.assignedSchedule[num][start - 1], schedule.shiftType);
                        nightShiftCnt += isNightShift(schedule.assignedSchedule[num][start + 6], schedule.shiftType);
                    }

                    if(nightShiftCnt) tmpCon -= w2;
                }
            }
            
            employeeBestContribution[num][day] = max(make_pair(tmpCon, -j), employeeBestContribution[num][day]);
        }
    }

    void getBestAssignment(int& bestEmp, int& bestDay, int& bestShift, int& bestCon, int numL, int numR, int dayL, int dayR){
        pair<int,int> bestAssi = make_pair(0,0);
        for(int i = numL; i < numR; i++) for(int j = dayL; j < dayR; j++){
            if(employeeBestContribution[i][j] > bestAssi){
                bestAssi = employeeBestContribution[i][j];
                bestCon = bestAssi.first;
                bestEmp = i;
                bestDay = j;
                bestShift = -employeeBestContribution[i][j].second;
            }
        }
    }

    void cancelShift(int num, int day, Schedule& schedule){
        int impact = 0, currShift = schedule.assignedSchedule[num][day];
        schedule.assignedSchedule[num][day] = 0;
        
        // reverse contribution (impact)
        for(int i = 0; i < 24; i++){
            impact += (schedule.shiftType[currShift][i] && schedule.demand[day][i] >= 0);
            schedule.demand[day][i] += schedule.shiftType[currShift][i];
        }
        impact -= (request[num][day] && currShift) * w1;
        if(isNightShift(currShift, schedule.shiftType)){
            int nightShiftCnt = 0;
            for(int start = max(0, day - 6); start <= min(day, nJ - 7); start++){
                if(start == max(0, day - 6))
                    for(int k = start; k <= start + 6; k++) nightShiftCnt += isNightShift(schedule.assignedSchedule[num][k], schedule.shiftType);
                else{
                    nightShiftCnt -= isNightShift(schedule.assignedSchedule[num][start - 1], schedule.shiftType);
                    nightShiftCnt += isNightShift(schedule.assignedSchedule[num][start + 6], schedule.shiftType);
                }

                if(nightShiftCnt) impact -= w2;
            }
        }
        schedule.targetFunction += impact;
        schedule.updateShiftCon(day);
    }
};

void greedyAssignment(Schedule& schedule, Employee& employee, int numL, int numR, int dayL, int dayR){
    while(true){
        int bestEmp = 0, bestDay = 0, bestShift = 0, bestCon = 0;
        employee.getBestAssignment(bestEmp, bestDay, bestShift, bestCon, numL, numR, dayL, dayR);
        if(bestCon <= 0) break;
        else schedule.assignShift(bestEmp, bestDay, bestShift, bestCon);
        
        employee.update(bestEmp, bestDay, schedule);
        if(numR - numL > 1)  // update other employees
            for(int i = 0; i < schedule.nI; i++)
                employee.update(i, bestDay, schedule);
        if(dayR - dayL > 1)  // update 13 days interval
            for(int i = max(0, bestDay - 6); i <= min(bestDay + 6, schedule.nJ - 1); i++)
                employee.update(bestEmp, i, schedule);
    }
}

void cancelAndUpdate(Schedule& schedule, Employee& employee, int numL, int numR, int dayL, int dayR){
    for(int i = numL; i < numR; i++) for(int j = dayL; j < dayR; j++)
        employee.cancelShift(i, j, schedule);
    for(int i = numL; i < numR; i++) for(int j = dayL; j < dayR; j++){
        employee.employeeBestContribution[i][j] = make_pair(0, 0);
        employee.update(i, j, schedule, (dayR - dayL != schedule.nJ));
    }
}

void saveOrRollback(Schedule& schedule, Employee& employee, int numL, int numR, int dayL, int dayR){
    static vector<vector<int>> oldAssignedSchedule(schedule.nI, vector<int>(schedule.nJ)),
        oldDemand(schedule.nJ, vector<int>(24));

    static int oldTgtFunc = INF;
    if(schedule.targetFunction > oldTgtFunc){
        for(int i = numL; i < numR; i++) for(int j = dayL; j < dayR; j++)
            schedule.assignedSchedule[i][j] = oldAssignedSchedule[i][j];
        for(int j = dayL; j < dayR; j++){
            schedule.demand[j] = oldDemand[j];
            schedule.updateShiftCon(j);
        }
        schedule.targetFunction = oldTgtFunc;
    }
    else{
        for(int i = numL; i < numR; i++) for(int j = dayL; j < dayR; j++)
            oldAssignedSchedule[i][j] = schedule.assignedSchedule[i][j];
        for(int j = dayL; j < dayR; j++)
            oldDemand[j] = schedule.demand[j];
        oldTgtFunc = schedule.targetFunction;
    }
}

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

    Schedule schedule(nI, nJ, nK, shiftType, demand);
    Employee employee(nI, nJ, nK, L, w1, w2, request);
    for(int i = 0; i < nI; i++) for(int j = 0; j < nJ; j++) employee.update(i, j, schedule, 0);

    /* greedy main proceedure */
    greedyAssignment(schedule, employee, 0, nI, 0, nJ);
    saveOrRollback(schedule, employee, 0, nI, 0, nJ);

    int attempts = 16;
    while(attempts--){
        /* employee erase attempt */
        for(int i = nI - 1; i >= 0; i--){
            cancelAndUpdate(schedule, employee, i, i + 1, 0, nJ);
            greedyAssignment(schedule, employee, i, i + 1, 0, nJ);
            saveOrRollback(schedule, employee, i, i + 1, 0, nJ);
        }

        /* day erase attempt */
        for(int i = nI - 1; i >= 0; i--) for(int j = nJ - 1; j >= 0; j--){
            cancelAndUpdate(schedule, employee, i, i + 1, j, j + 1);
            greedyAssignment(schedule, employee, i, i + 1, j, j + 1);
        }
        saveOrRollback(schedule, employee, 0, nI, 0, nJ);
    }
    
    /* just for target function debugging */
    #ifdef DEBUG
    int targetFuncTest = 0;
    for(vector<int>& v:schedule.demand) for(int& i:v) targetFuncTest += max(i, 0);
    
    for(int i = 0; i < nI; i++) for(int j = 0; j <= nJ - 7; j++){
        int nightShift = 0;
        for(int k = j; k <= j + 6; k++)
            if(isNightShift(schedule.assignedSchedule[i][k], shiftType)) nightShift++;
        targetFuncTest += w2 * max(0, nightShift - 1);
    }

    for(int i = 0; i < nI; i++)
        for(int j = 0; j < nJ; j++)
            if(schedule.assignedSchedule[i][j] && request[i][j]) targetFuncTest += w1;
    assert(targetFuncTest == schedule.targetFunction);
    #endif
    
    schedule.output();
    //cout << schedule.targetFunction << ' ' << targetFuncTest << '\n';

    return 0;
}