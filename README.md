# Midterm Project
Pseudocode is here:
 
```
class Schedule{
	void assignShift
		Add shift to assignedSchedule 
		Update targetFunction, demand, shiftContribution
	
	void output
		Output the schedule
};

bool isNightShift
	return true if the given shift is a night shift

class Employee_General{
	void update
		Compute employeeBestContribution
	
	void getBestAssignment
		Get the best assignment from employeeBestContribution
};

class Employee_Random{
	void update
		Compute employeeBestContribution (with randomness)
	
	void getBestAssignment
		Get the best assignment from employeeBestContribution
};

int main(){
	cin >> nI >> nJ >> nK >> L >> w1 >> w2 >> R
	
	input shiftType, demand, (day-off)request
	
	Schedule schedule1(nI, nJ, nK, shiftType, demand), schedule2(nI, nJ, nK, shiftType, demand)
    
	for i to nI
		for j to nJ
			employee1.update(i, j, schedule1)
	
    	for i to nI
		for j to nJ
			employee2.update(i, j, schedule2)
	
	while(true){
        employee1.getBestAssignment(bestEmp, bestDay, bestShift, bestCon)
        
        if(bestCon <= 0)
			break;
        else
			schedule1.assignShift(bestEmp, bestDay, bestShift, bestCon)
        
        for i from max(0, bestDay - 6) to min(bestDay + 6, nJ - 1)
            employee1.update(bestEmp, i, schedule1)
        
        for i to nI
			employee1.update(i, bestDay, schedule1)
	}
	
	while(true){
		employee2.getBestAssignment(bestEmp, bestDay, bestShift, bestCon)
		
        if(bestCon <= 0)
			break;
        else
			schedule2.assignShift(bestEmp, bestDay, bestShift, bestCon)
        
        for i from max(0, bestDay - 6) to min(bestDay + 6, nJ - 1)
            employee2.update(bestEmp, i, schedule2)
        
        for i to nI
			employee2.update(i, bestDay, schedule2)
	}
	
	if schedule1.targetFunction < schedule2.targetFunction
		schedule1.output()
    	else
		schedule2.output()
}
```
