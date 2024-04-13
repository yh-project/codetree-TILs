#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <queue>

#define endl '\n'
#define MAX 51
#define SANTAMAX 31
#define INF 987654321

using namespace std;
using pii = pair<int, int>;

struct Santa {
    int r, c;
    int score;
    bool stateSanta;
    bool stateRudolph;
};
int N, M, P;
int C, D;
bool matrix[MAX][MAX];
pair<pii, int> r;
Santa s[SANTAMAX];
int dr[] = {-1, 0, 1, 0, -1, -1, 1, 1}; // 상 우 하 좌 좌상 우상 우하 좌하
int dc[] = {0, 1, 0, -1, -1, 1, 1, -1};

/* 입력 */
void input() {
    cin >> N >> M >> P >> C >> D;
    
    int rr, rc; cin >> rr >> rc;
    r = {{rr, rc}, -1};

    int idx, sr, sc;
    for(int i=0; i<P; i+=1) {
        cin >> idx >> sr >> sc;
        s[idx] = {sr, sc, 0, true, true}; 
        matrix[sr][sc] = true;
    }
}

/* 루돌프의 움직임 */
int getDir(int idx) {
    if(r.first.first > s[idx].r) {
        if(r.first.second > s[idx].c) return 4;
        if(r.first.second < s[idx].c) return 5;
        return 0;
    } else if(r.first.first < s[idx].r) {
        if(r.first.second > s[idx].c) return 7;
        if(r.first.second < s[idx].c) return 6;
        return 2;
    } else {
        if(r.first.second > s[idx].c) return 3;
        return 1;
    }
}

void moveRudolph() {
    int target = 0;
    int minDist = INF;

    for(int i=1; i<=P; i+=1) {
        if(s[i].r < 1 || s[i].r > N || s[i].c < 1 || s[i].c > N) continue; // 탈락x

        int tmpDist = pow(abs(r.first.first - s[i].r), 2) + pow(abs(r.first.second - s[i].c), 2); // 거리
        if(tmpDist < minDist) { // 최소거리 갱신
            minDist = tmpDist;
            target = i; 
        } else if(tmpDist == minDist) {
            if(s[target].r == s[i].r) {
                if(s[target].c < s[i].c) target = i;
            } else if(s[target].r < s[i].r) target = i;
        }
    }
    int d = getDir(target);
    r = {{r.first.first + dr[d], r.first.second + dc[d]}, d};
}


/* 루돌프 상호작용 */
void interactionRudolph(int target, bool isStart) {
    matrix[s[target].r][s[target].c] = false;
    s[target].r += (dr[r.second] * (isStart ? C : 1));
    s[target].c += (dc[r.second] * (isStart ? C : 1));
    
    if(s[target].r < 1 || s[target].r > N || s[target].c < 1 || s[target].c > N) return;
    if(matrix[s[target].r][s[target].c]) {
        for(int i=1; i<=P; i+=1) {
            if(target == i || s[target].r != s[i].r || s[target].c != s[i].c) continue;
            interactionRudolph(i, false);
            break;
        }
    }
    matrix[s[target].r][s[target].c] = true;
}

/* 루돌프 충돌 */
void crushRudolph() {
    int target;
    for(int i=1; i<=P; i+=1) {
        if(s[i].r != r.first.first || s[i].c != r.first.second) continue;
        target = i;
        break;
    }
    s[target].score += C;
    s[target].stateRudolph = false;
    s[target].stateSanta = false;
    interactionRudolph(target, true);
}

/* 산타 상호작용 */
void interactionSanta(int target, int d, bool isStart) {
    matrix[s[target].r][s[target].c] = false;
    s[target].r += (dr[d] * (isStart ? D : 1));
    s[target].c += (dc[d] * (isStart ? D : 1));
    
    if(s[target].r < 1 || s[target].r > N || s[target].c < 1 || s[target].c > N) return;
    if(matrix[s[target].r][s[target].c]) {
        for(int i=1; i<=P; i+=1) {
            if(target == i || s[target].r != s[i].r || s[target].c != s[i].c) continue;
            interactionSanta(i, d, false);
            break;
        }
    }
    matrix[s[target].r][s[target].c] = true;
}

/* 산타 충돌 */
void crushSanta(int target, int d) {
    s[target].score += D;
    s[target].stateSanta = false;
    interactionSanta(target, d, true);
}

/* 산타 상호작용 방향 */
int getCrushDir(int nextDir) {
    switch(nextDir) {
        case 0: return 2;
        case 1: return 3;
        case 2: return 0;
        case 3: return 1;
        case 4: return 6;
        case 5: return 7;
        case 6: return 4;
        case 7: return 5;
    }
}

/* 산타의 움직임 */
void moveSanta() {
    for(int i=1; i<=P; i+=1) {
        if(s[i].r < 1 || s[i].r > N || s[i].c < 1 || s[i].c > N) continue;
        if(!s[i].stateRudolph) {
            s[i].stateRudolph = true;
            continue;
        }
        if(!s[i].stateSanta) {
            s[i].stateSanta = true;
            continue;
        }

        priority_queue<pii, vector<pii>, greater<pii>> pq;
        int curDist = pow(abs(r.first.first - s[i].r), 2) + pow(abs(r.first.second - s[i].c), 2);
        int nr, nc;
        for(int d=0; d<4; d+=1) {
            nr = s[i].r + dr[d];
            nc = s[i].c + dc[d];
            int nextDist = pow(abs(r.first.first - nr), 2) + pow(abs(r.first.second - nc), 2);

            if(nr < 1 || nr > N || nc < 1 || nc > N || nextDist >= curDist || matrix[nr][nc]) continue;
            pq.push({nextDist, d});
        }

        if(pq.empty()) continue;

        pii nextDir = pq.top();
        nr = s[i].r + dr[nextDir.second];
        nc = s[i].c + dc[nextDir.second];

        matrix[s[i].r][s[i].c] = false;
        s[i] = {nr, nc, s[i].score, s[i].stateSanta, s[i].stateRudolph};
        matrix[s[i].r][s[i].c] = true;
        if(s[i].r == r.first.first && s[i].c == r.first.second) {
            crushSanta(i, getCrushDir(nextDir.second));
        }
    }
}

/* 남은 산타 확인 */
bool checkSanta() {
    for(int i=1; i<=P; i+=1) {
        if(s[i].r < 1 || s[i].r > N || s[i].c < 1 || s[i].c > N) continue;
        return true;
    }
    return false;
}


void solve() {
    for(int i=0; i<M; i+=1) {
        moveRudolph();
        if(matrix[r.first.first][r.first.second]) {
            crushRudolph();
        }
        moveSanta();
        if(!checkSanta()) break;
        for(int i=1; i<=P; i+=1) {
            if(s[i].r < 1 || s[i].r > N || s[i].c < 1 || s[i].c > N) continue;
            s[i].score += 1;
            
        }
    }
    
    for(int i=1; i<=P; i+=1) {
        cout << s[i].score << ' ';
    }
}

int main() {
    input();
    solve();
    return 0;
}