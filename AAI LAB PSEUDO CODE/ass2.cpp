// 

Input:
    A = set of assignments
    p[i] = prompts needed for assignment i
    Dep[i] = dependencies of assignment i
    N = number of students
    K = prompts per student per day
    m = number of days

Output:
    valid schedule

// 

Data Structures
schedule[i] = (student, day)
assigned[i] = false

usedPrompt[s][d] = prompts used by student s on day d
completionDay[i]

// 

CHECK VALID ASSIGNMENT
FUNCTION is_valid(i, s, d):

    if usedPrompt[s][d] + p[i] > K:
        return FALSE

    for each dep in Dep[i]:
        if assigned[dep] == FALSE:
            return FALSE

        if completionDay[dep] >= d:
            return FALSE

    return TRUE

// EARLIEST COMPLETION TIME

schedule_possible(N, K, days):
   FUNCTION schedule_possible(N, K, days):

    mark all assignments unassigned

    BACKTRACK()

    if all assignments assigned:
        return TRUE
    else:
        return FALSE

// backtrack

FUNCTION BACKTRACK():

    if all assignments assigned:
        return TRUE

    choose assignment i whose dependencies satisfied

    for s = 1 to N:
        for d = 1 to days:

            if is_valid(i, s, d):

                assign i to (s,d)
                assigned[i] = TRUE
                completionDay[i] = d
                usedPrompt[s][d] += p[i]

                if BACKTRACK() == TRUE:
                    return TRUE

                undo assignment
                assigned[i] = FALSE
                usedPrompt[s][d] -= p[i]

    return FALSE


// CASE 2
BEST SUBSCRIPTION SCHEME

low = 1
high = sum of all prompts
answer = high

WHILE low <= high:

    mid = (low + high) / 2

    if schedule_possible(N, mid, m):

        answer = mid
        high = mid - 1
    else:
        low = mid + 1

print "Minimum Subscription =", answer