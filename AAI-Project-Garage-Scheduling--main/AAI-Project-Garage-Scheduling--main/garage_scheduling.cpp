#include <bits/stdc++.h>
using namespace std;

#define new_task_threshold 0.7

// Making edge
struct Edge{
    pair<int,int> to;
    double prob;
};

// Task structure
struct Task{
    int car, id;
    vector<Edge> edges;
    int indegree = 0;
    int priority = -1;
    bool completed = false;
};

// Mechanic info...
struct Mechanic{
    int id;
    int  consecutive = 0;
    bool on_break = false;
};

struct Cmp{
    bool operator()(Task* a, Task* b) {
        return a->priority < b->priority;
    }
};

// making class scheduler 
class Scheduler {
private:
    map<pair<int,int>, Task> tasks;
    int globalTaskId = 100;

    struct Compare {
        bool operator()(Task* a, Task* b) {
            return a->priority < b->priority; 
        }
    };

    priority_queue<Task*, vector<Task*>, Compare> pq;

public:
    vector<Mechanic> mechanics;
    int K;

    // constructor
    Scheduler(int M, int k) : K(k) {
        for(int i = 0; i < M; i++)
            mechanics.push_back({i, 0, false});
    }

    // adding the new task
    void addTask(int car, int id) {
        pair<int,int> key = {car, id};
        if(!tasks.count(key))  tasks[key] = {car, id};
    }
    
    // adding the edge 
    void addEdge(int car, int u, int v, double p) {
        addTask(car, u);
        addTask(car, v);
        tasks[{car,u}].edges.push_back({{car,v}, p});
        tasks[{car,v}].indegree++;
    }

    // runnig dfs for the longest path using the dp method
    int dfs(pair<int,int> node, map<pair<int,int>,int>& dp) {
        if(dp.count(node)) return dp[node];
        if(tasks[node].completed) return dp[node] = 0;

        int mx = 0;
        for (auto& e : tasks[node].edges) {
            if(!tasks[e.to].completed) mx = max(mx, dfs(e.to, dp));
        }
        return dp[node] = 1 + mx;
    }

    void recomputePriority() {
        map<pair<int,int>,int> dp;
        for(auto& p : tasks)
            if(!p.second.completed)    p.second.priority = dfs(p.first, dp);
    }

    // Pushing all ready task into the queue 
        void initQueue() {
        for(auto& p : tasks)
            if(p.second.indegree == 0 && !p.second.completed)  pq.push(&p.second);
    }

    // printing initial schedule 
    /*
    void printInitialSchedule() {
        cout<<"Initial Optimal Schedule\n";

        map<pair<int,int>, int> tempIndegree;
        for(auto& p : tasks)
            tempIndegree[p.first] = p.second.indegree;

        auto cmp = [](Task* a, Task* b){ return a->priority < b->priority; };
        priority_queue<Task*, vector<Task*>, decltype(cmp)> localPQ(cmp);

        for(auto& p : tasks)
            if(tempIndegree[p.first] == 0) localPQ.push(&p.second);

        int time = 0;
        set<pair<int,int>> scheduled;

        while(!localPQ.empty()) {
            cout << "\n  [Time " << time << "]\n";
            int slots = (int)mechanics.size();
            int assigned = 0;

            vector<Task*> processing;
            priority_queue<Task*, vector<Task*>, decltype(cmp)> temp(cmp);

            // take out the m ready task form the queu
            while(!localPQ.empty() && assigned < slots) {
                Task* t = localPQ.top(); localPQ.pop();
                pair<int,int> key = {t->car, t->id};
                if(scheduled.count(key)) continue;

                cout<<"   Mechanic " << assigned << " -> Car " << t->car << " Task " << t->id<< ")\n";

                scheduled.insert(key);
                processing.push_back(t);
                assigned++;
            }

            while(!temp.empty()) { localPQ.push(temp.top()); temp.pop(); }

            for(Task* t : processing) {
                for(auto& e : t->edges) {
                    tempIndegree[e.to]--;
                    if(tempIndegree[e.to] == 0)    localPQ.push(&tasks[e.to]);
                }
            }
            time++;
        }
        cout<<"\n\n";
    }
    */

    void printInitialSchedule() {
        cout << "Initial Optimal Schedule\n";

        // Copy indegrees
        map<pair<int,int>, int> tempIndegree;
        for (auto& p : tasks)
            tempIndegree[p.first] = p.second.indegree;

        // Push all tasks with no dependencies
        priority_queue<Task*, vector<Task*>, Cmp> localPQ;
        for (auto& p : tasks)
            if (tempIndegree[p.first] == 0)
                localPQ.push(&p.second);

        int time = 0;
        set<pair<int,int>> scheduled;

        while (!localPQ.empty()) {
            cout << "\n  [Time " << time << "]\n";

            int slots = (int)mechanics.size();
            int assigned = 0;
            vector<Task*> processing;

            // Assign tasks to mechanics
            while (!localPQ.empty() && assigned < slots) {
                Task* t = localPQ.top(); localPQ.pop();
                pair<int,int> key = {t->car, t->id};

                if (scheduled.count(key)) continue;

                cout << "   Mechanic " << assigned << " -> Car " << t->car << " Task " << t->id << ")\n";
                scheduled.insert(key);
                processing.push_back(t);
                assigned++;
            }

            // Unlock next tasks
            for (Task* t : processing) {
                for (auto& e : t->edges) {
                    tempIndegree[e.to]--;
                    if (tempIndegree[e.to] == 0)
                        localPQ.push(&tasks[e.to]);
                }
            }

            time++;
        }

        cout << "\n\n";
    }

    //  Dynamic task creation 
    Task* createDynamicTask(int car){
        int newId = globalTaskId++;
        cout<<"Dynamic sub-task created: Car " << car<< " Task " << newId << "\n";

        addTask(car, newId);
        return &tasks[{car, newId}];
    }

    vector<Task*> completeTask(Task* t) {
        vector<Task*> activated;

        // Randomly producing the new task
        double r_new = (double)rand()/RAND_MAX;
        if(r_new > new_task_threshold){
            Task* newT = createDynamicTask(t->car);
            activated.push_back(newT);
        }

        for(auto& e : t->edges) {
            auto& child = tasks[e.to];
            child.indegree--;

            double r = (double)rand() / RAND_MAX;
            if(r > e.prob){
                if(child.indegree == 0 && !child.completed)    activated.push_back(&child);
            }
        }

        return activated;
    }

    // main logic
    void run(){
        int time = 0;

        while(!pq.empty()) {
            cout<<"\n===Time: " << time << " ===\n";

            // phase 1 :- mechanic on break
            for(auto& m : mechanics){
                if(m.on_break){
                    cout<<"Mechanic " << m.id << " finishes break, ready.\n";
                    m.on_break    = false;
                    m.consecutive = 0;   // reset the counter after a break
                }
            }

            // Phase 2 :- mechanics take breaks , they worked for the k consecutive unit time
            for(auto& m : mechanics){
                if(!m.on_break && m.consecutive == K){
                    cout<<"  Mechanic " << m.id   << " takes mandatory break (worked " << K << " in a row).\n";
                    m.on_break    = true;
                    m.consecutive = 0;
                }
            }

            // Phase 3 :- available mechanics
            vector<Mechanic*> avail;
            for(auto& m : mechanics)
                if(!m.on_break)    avail.push_back(&m);

            // Assign tasks to the available mechanics 
            vector<pair<Mechanic*, Task*>> assign;
            int i = 0;
            while(!pq.empty() && i < (int)avail.size()) {
                Task* t = pq.top(); pq.pop();
                if(t->completed) continue;
                assign.push_back({avail[i], t});
                i++;
            }

            // all mechanics are on break go ahead with the time
            if(assign.empty() && !pq.empty()) {
                cout<<"(all mechanics on break — advancing time)\n";
                time++;
                continue;
            }

            // Executing the assigned task
            for(auto& [m, t] : assign) {
                cout<<"  Mechanic "<< m->id << " executes Car " << t->car << " Task " << t->id<< ")\n";

                m->consecutive++;
                t->completed = true;

                auto newTasks = completeTask(t);
                for(Task* nt : newTasks){
                    cout<<"    -> Activated: Car " << nt->car << " Task " << nt->id << "\n";
                    pq.push(nt);
                }
            }

            // recomputing the priority after doing the changes 
            recomputePriority();

            vector<Task*> tmp;
            while(!pq.empty()) { tmp.push_back(pq.top()); pq.pop(); }
            for(Task* t : tmp)
                if(!t->completed)  pq.push(t);

            time++;
        }

        cout<<"\nAll tasks completed.\n";
    }
};

// main code
int main() {
    freopen("input.txt", "r", stdin);
    srand((unsigned)time(0));

    int N;
    cin>>N;

    int M, K;
    cin>>M>>K;

    Scheduler sch(M, K);

    for(int car = 1; car <= N; car++) {
        int E; 
        cin>>E;
        for(int i = 0; i < E; i++) {
            int u, v; double p;
            cin>>u>>v>>p;
            sch.addEdge(car, u, v, p);
        }
    }

    sch.recomputePriority();
    sch.printInitialSchedule();

    sch.initQueue();
    sch.run();

    return 0;
}