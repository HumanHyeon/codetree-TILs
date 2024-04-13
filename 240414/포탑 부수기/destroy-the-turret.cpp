#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace std;
using pii = pair<int,int>;

#define DEBUG 0
#define DOUT DEBUG&&cout

#define MAX 11
#define IGNORE 0

typedef struct {
	int y;
	int x;
	int cnt;
} Pos;

const int dy[] = { 0, 1, 0, -1, -1, -1, 1, 1 };
const int dx[] = { 1, 0, -1, 0, -1, 1, -1, 1 };

int N;	//격자 Y (4~10)
int M;	//격자 X (4~10)
int K;	//턴 수 (1~1000)

int MAP[MAX][MAX];
int TIME[MAX][MAX];	//공격 했던 시간
bool REF[MAX][MAX]; //해당 턴에 공격과 관련 여부

int visited[MAX][MAX];

int answer;

bool printMAP(string s) {
	DOUT << " ===== " << s << " ======\n";
	for (int y = 0; y < N; ++y) {
		for (int x = 0; x < M; ++x)
			printf("%3d ", MAP[y][x]);
		DOUT << "\n";
	}
		
	DOUT << "\n";

	return (true);
}

bool checkFinish() {
	int cnt = 0;

	for (int y = 0; y < N; ++y)
		for (int x = 0; x < M; ++x) {
			if (MAP[y][x] > 0)	++cnt;
			if (cnt > 1) 	return (false);
		}

	if (cnt == 0) {
		DOUT << "[checkFinish] 남아 있는 포탑 수 0로 exit 호출\n";
		exit(1);
	}

	return (true);
}

void initREF() {	//TRUE인 경우는 공격과 관련 없음
	for (int y = 0; y < N; ++y)
		for (int x = 0; x < M; ++x) {
			if (MAP[y][x] == 0)	REF[y][x] = false;
			else 				REF[y][x] = true;	//공격 관련 없음
		}
}

void doRecovery() {
	for (int y = 0; y < N; ++y)
		for (int x = 0; x < M; ++x)	
			if (REF[y][x])
				++MAP[y][x];
}

pii getAttacker() {	//공격자로 선정되면 N+M 만큼 핸디캡을 더해야함
	int val = 1987654321;
	pii pos = { -1, -1 };

	for (int y = 0; y < N; ++y)
		for (int x = 0; x < M; ++x) {
			if (MAP[y][x] == IGNORE)	continue;

			if (MAP[y][x] < val) {
				val = MAP[y][x];
				pos = { y, x }; 
			}
			else if (MAP[y][x] == val) {
				int stdTime = TIME[pos.first][pos.second];
				int cmpTime = TIME[y][x];

				if (cmpTime > stdTime) {	//가장 최근에 공격한 포탑
					val = MAP[y][x];
					pos = { y, x }; 
				}
				else if (cmpTime == stdTime) {
					int stdPos = pos.first + pos.second;
					int cmpPos = y + x;

					if (cmpPos > stdPos) {
						val = MAP[y][x];
						pos = { y, x }; 
					}
					else if (cmpPos == stdPos) {
						if (x > pos.second) {
							val = MAP[y][x];
							pos = { y, x }; 
						}	
					}
				}
			}
		}

	if (pos.first == -1 || pos.second == -1) {
		DOUT << "[getAttacker] 공격자 좌표가 -1인 경우로 exit 호출\n";
		exit(1);
	}

	return (pos);
}

pii getTarget() {
	int val = 0;
	pii pos = { -1, -1 };

	for (int y = 0; y < N; ++y)
		for (int x = 0; x < M; ++x) {
			if (MAP[y][x] == IGNORE)	continue;

			if (MAP[y][x] > val) {
				val = MAP[y][x];
				pos = { y, x };
			}
			else if (MAP[y][x] == val) {
				int stdTime = TIME[pos.first][pos.second];
				int cmpTime = TIME[y][x];

				if (cmpTime < stdTime) {	//공격한지 오래 된 포탑
					val = MAP[y][x];
					pos = { y, x };
				}
				else if (cmpTime == stdTime) {
					int stdPos = pos.first + pos.second;
					int cmpPos = y + x;

					if (cmpPos < stdPos) {
						val = MAP[y][x];
						pos = { y, x };
					}
					else if (cmpPos == stdPos) {
						if (x < pos.second) {
							val = MAP[y][x];
							pos = { y, x };
						}
					}
				}
			}
		}

	if (pos.first == -1 || pos.second == -1) {
		DOUT << "[getTarget] 공격당할 타겟 좌표가 -1인 경우로 exit 호출\n";
		exit(1);
	}

	return (pos);
}

bool doLazerAttack(const pii attacker, const pii target) {
	queue<Pos> q;
	queue<pii> trace;
	int dist = -1;
	int damage = MAP[attacker.first][attacker.second];

	memset(visited, 0, sizeof(visited));

	visited[target.first][target.second] = 1;
	q.push({ target.first, target.second, 1 });
	while (!q.empty()) {
		int y = q.front().y;
		int x = q.front().x;
		int cnt = q.front().cnt;
		q.pop();

		if (y == attacker.first && x == attacker.second) {
			dist = cnt;
			break;
		}

		for (int flag = 0; flag < 4; ++flag) {
			int ny = y + dy[flag];
			int nx = x + dx[flag];

			if (ny < 0)	ny += N;
			if (nx < 0)	nx += M;
			ny %= N;
			nx %= M;

			if (MAP[ny][nx] == 0)	continue;	//죽은 포탑
			if (visited[ny][nx] != 0 && visited[ny][nx] <= cnt)	continue;

			visited[ny][nx] = cnt + 1;
			q.push({ ny, nx, cnt + 1 });
		}
	}

	DOUT << "------ Vistied 정보 ------\n";
	if (DEBUG) {
		for (int y = 0; y < N; ++y) {
			for (int x = 0; x < N; ++x) {
					printf("%3d ", visited[y][x]);
			}
			DOUT << "\n";
		}
	}
	DOUT << "\n";
		
	if (dist == -1) {	//갈 수 없는 상태
		DOUT << "[doLazerAttack] 레이저 공격 할 수 없음\n";
		return (false);	
	}	
	else {
		int y = attacker.first;
		int x = attacker.second;
		int cnt = dist;

		while (true) {
			for (int flag = 0; flag < 4; ++flag) {
				int ny = y + dy[flag];
				int nx = x + dx[flag];

				if (ny < 0)	ny += N;
				if (nx < 0)	nx += M;
				ny %= N;
				nx %= M;

				if (visited[ny][nx] == cnt - 1) {
					y = ny;
					x = nx;
					--cnt;
					break;
				}
			}

			if (y == target.first && x == target.second)	break;
			trace.push({ y, x });
		}
	}

	DOUT << "----- 경로 정보 ------\n";
	if (DEBUG) {
		queue<pii> tmp = trace;

		while (!tmp.empty()) {
			pii t = tmp.front();
			tmp.pop();

			DOUT << "(" << t.first << ", " << t.second << ") ";
		}
		DOUT << "\n";
	}
	DOUT << "\n";

	while (!trace.empty()) {
		pii t = trace.front();
		trace.pop();

		MAP[t.first][t.second] -= (damage / 2);
		if (MAP[t.first][t.second] < 0)
			MAP[t.first][t.second] = IGNORE;
		REF[t.first][t.second] = false;
	}
	MAP[target.first][target.second] -= damage;
	if (MAP[target.first][target.second] < 0)
		MAP[target.first][target.second] = IGNORE;

	REF[attacker.first][attacker.second] = REF[target.first][target.second] = false;

	return (true);
}

void doBombAttack(const pii attacker, const pii target) {
	int damage = MAP[attacker.first][attacker.second];
	int y = target.first;
	int x = target.second;

	MAP[target.first][target.second] -= damage;
	if (MAP[target.first][target.second] < 0)
		MAP[target.first][target.second] = IGNORE;
	REF[attacker.first][attacker.second] = REF[target.first][target.second] = false;
	
	for (int flag = 0; flag < 8; ++flag) {
		int ny = y + dy[flag];
		int nx = x + dx[flag];

		if (ny < 0)	ny += N;
		if (nx < 0)	nx += M;
		ny %= N;
		nx %= M;

		if (ny == attacker.first && nx == attacker.second)	//공격자는 영향 받지 않음
			continue;
		if (MAP[ny][nx] == IGNORE)	continue;

		MAP[ny][nx] -= (damage / 2);
		if (MAP[ny][nx] < 0)
			MAP[ny][nx] = IGNORE;
		REF[ny][nx] = false;
	}
}

void solution() {
	DEBUG && printMAP("초기 상태 맵");

	for (int turn = 1; turn <= K; ++turn) {
		DOUT << "\n*********** Turn: " << turn << " 시작 ***********\n";

		pii attacker, target;

		initREF();
		attacker = getAttacker();			DOUT << "공격자: (" << attacker.first << ", " << attacker.second << ")\n";
		target = getTarget();				DOUT << "타겟: (" << target.first << ", " << target.second << ")\n";
		MAP[attacker.first][attacker.second] += (N + M);	DEBUG && printMAP("공격 전 맵 상태");
		if (!doLazerAttack(attacker, target))
			doBombAttack(attacker, target);					DEBUG && printMAP("공격 후 맵 상태");
		doRecovery();										DEBUG && printMAP("회복 후 맵 상태");
		if (checkFinish())	break;
		TIME[attacker.first][attacker.second] = turn;
	}

	for (int y = 0; y < N; ++y)
		for (int x = 0; x < M; ++x)
			answer = max(answer, MAP[y][x]);
}

void input() {
	cin >> N >> M >> K;
	for (int y = 0; y < N; ++y)
		for (int x = 0; x < M; ++x)
			cin >> MAP[y][x];
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);	cout.tie(NULL);


	input();
	solution();

	cout << answer;
}