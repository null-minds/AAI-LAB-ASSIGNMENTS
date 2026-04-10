INPUT: N, K, m, assignments with (p_i, dependencies)

Initialize:
    result = []
    assignment_done[i] = false
    schedule[i] = (student, day)

FUNCTION is_valid(i, s, d):
    # Check prompt limit
    total_prompts = sum of prompts of assignments assigned to student s on day d
    if total_prompts + p[i] > K:
        return false

    # Check dependencies
    for each dep in dependencies[i]:
        if assignment_done[dep] == false:
            return false
        if day(dep) >= d:
            return false

    return true


FUNCTION backtrack(i):
    if all assignments are assigned:
        print schedule
        return

    for each assignment i not done:
        if dependencies satisfied:

            for s in 1 to N:
                for d in 1 to m:

                    if is_valid(i, s, d):

                        assign i → (s, d)
                        assignment_done[i] = true

                        backtrack(next)

                        # undo
                        assignment_done[i] = false
                        remove assignment i


MAIN:
    backtrack(0)