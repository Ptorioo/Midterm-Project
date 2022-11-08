# Midterm Project
Pseudocode is here:
 
```
class Schedule{
	void assignShift
		Add shift to assignedSchedule 
		Update targetFunction, demand, shiftContribution
	void updateShiftCon
		update shiftContribution on a certain day
	void output
		Output the schedule
};

bool isNightShift
	return true if the given shift is a night shift

class Employee{
	void update
		Compute employeeBestContribution
	void getBestAssignment
		Get the best assignment from employeeBestContribution
	void redecideShift
		reset one's assigned shift on a certain day and find best assignment again
};

int main(){
	cin >> nI >> nJ >> nK >> L >> w1 >> w2 >> R
	
	input shiftType, demand, (day-off)request
	
	Schedule schedule(nI, nJ, nK, shiftType, demand)
    	Employee employee(nI, nJ, nK, L, w1, w2, request)
	
	for i from 0 to nI - 1
		for j from 0 to nJ - 1
			employee.update(i, j, schedule)
	
	while(true){
        	employee.getBestAssignment(bestEmp, bestDay, bestShift, bestCon)
        
        	if(bestCon <= 0)
			break;
        	else
			schedule.assignShift(bestEmp, bestDay, bestShift, bestCon)
        
        	for i from max(0, bestDay - 6) to min(bestDay + 6, nJ - 1)
            		employee.update(bestEmp, i, schedule)
        
        	for i from 0 to nI - 1
			employee.update(i, bestDay, schedule)
	}
	
	for k from 0 to 3{
		for i from nI - 1 to 0
			for j from nJ - 1 to 0
				 employee.redecideShift(i, j, schedule)
		for i from 0 to nI - 1
			for j from 0 to nJ - 1
				 employee.redecideShift(i, j, schedule)
	}
	
	schedule.output()
}
```
